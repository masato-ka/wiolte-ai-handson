#include <WioLTEforArduino.h>
#include <ADXL345.h>

#include <tfmicro_wrapper.h>
//#include <TensorFlowLite.h>
//#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
//#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
//#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
//#include "tensorflow/lite/schema/schema_generated.h"
//#include "tensorflow/lite/version.h"
//#include "wio_model.h"
#define INTERVAL    (10)
#define BUTTON_PIN (WIOLTE_D20)
#define BUZZER_PIN (WIOLTE_D38)
#define BUZZER_ON_TIME (10)
#define BUZZER_OFF_TIME (3000)




WioLTE Wio;
HardwareTimer Timer3(TIM3);

//Status
bool is_buzzer_on = false;
bool is_send = false;
short counter = 0;
char old_data[1]={0x00};

ADXL345 Accel;

tfmicro_wrapper::TfMicroWrapper tfMicroWrapper;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
float mean = 277.92817024;
float sigma = 73.88031294;

//Hardware interrupt.
void buzzer(){
  is_buzzer_on = !is_buzzer_on;
}

//Callback for Timer3
void send_event(HardwareTimer* HT){
  counter++;
  if(counter > 10){
    is_send = true;
    counter = 0;
  }
}

//Send to unified endpoint.
int send_unified(char* data){
  char rcv_data[1024];
  int connectId = Wio.SocketOpen("uni.soracom.io", 23080, WIOLTE_UDP);
  if ( connectId < 0){
    
    return connectId;
  }
  if(!Wio.SocketSend(connectId, data)){
    goto err_close;
  }
  int length;
  length = Wio.SocketReceive(connectId, rcv_data, sizeof (rcv_data), 10000);
  if (length < 0) {
    goto err_close;
  }
  if (length == 0) {
    goto err_close;
  }
err_close:
  if (!Wio.SocketClose(connectId)) {
    return -1;
  }
  
  return 1;
}

void setup() {
 
  //Initialzie WioLTE
  Wio.Init();
  Wio.PowerSupplyGrove(true);
  Wio.PowerSupplyLTE(true);
  delay(500);

  //Accel sensor startup.
  Accel.powerOn();
  
  //LTE connection
  Wio.TurnOnOrReset();
  Wio.Activate("soracom.io", "sora", "sora");
  
  //Initialize AI model.
  tfMicroWrapper.initialize();
  input = tfMicroWrapper.getInput();
  output = tfMicroWrapper.getOutput();

  // Start Intermittent transmission
  Timer3.setOverflow(1000 * 1000, MICROSEC_FORMAT); // = per 1 sec
  Timer3.attachInterrupt(&send_event);
  Timer3.resume();
  
  
  pinMode(BUZZER_PIN, OUTPUT);
  
  //Switch Interrupt
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(BUTTON_PIN, buzzer, RISING);

  delay(500);  
}

void loop() {
  char data[1];
  //加速度センサー処理
  // Place our calculated x value in the model's input tensor
  int x;
  int y;
  int z;
  for(int i = 0; i < 16; i++){
    Accel.readXYZ(&x, &y, &z);
    float l = pow(x,2)+pow(y,2)+pow(z,2);
    float l2 = sqrt(l);
    float norm_l2 = (l2-mean)/sigma;
    input->data.f[i] = norm_l2;
    delay(INTERVAL);
  }

  // Run inference, and report any error
  
  tfMicroWrapper.invoke();
  
  if(output->data.f[0] < output->data.f[1]){
    SerialUSB.println("Buzzer!");
    Wio.LedSetRGB(10,10,10);
    data[0]=0x01; //Buzzer ON
  }else{
    Wio.LedSetRGB(0,0,0);
    data[0]=0x02; //Buzzer OFF
  }

  //When change state and intermittent transmission
  if(old_data[0] != data[0] || is_send){    
    int i = send_unified(data);
    is_send =false;
  }
  old_data[0] = data[0];

  //Buzzer ON/OFF
  digitalWrite(BUZZER_PIN, is_buzzer_on);
  
}
