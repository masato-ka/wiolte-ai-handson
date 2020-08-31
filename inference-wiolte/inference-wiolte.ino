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
ADXL345 Accel;
bool is_buzzer_on = false;

tfmicro_wrapper::TfMicroWrapper tfMicroWrapper;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
float min_val=23.388031127053;
float max_val=686.9454126784748;

//Callback for Timer3(Buzzer controll)
void buzzer(HardwareTimer *HT){
  digitalWrite(BUZZER_PIN, is_buzzer_on);
  is_buzzer_on = !is_buzzer_on;
}

//Send to unified endpoint.
int send_unified(char* data){
  int connectId = Wio.SocketOpen("uni.soracom.io", 23080, WIOLTE_UDP);
  if ( connectId < 0){
    return connectId;
  }

  if(!Wio.SocketSend(connectId, data)){
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

  // Start BUZZER
  Timer3.setOverflow(1000 * 1000, MICROSEC_FORMAT); // = per 1 sec
  Timer3.attachInterrupt(&buzzer);
  Timer3.resume();
  pinMode(BUZZER_PIN, OUTPUT);
  
  delay(500);
  
}

void loop() {

  char data[1024];
  //加速度センサー処理
  // Place our calculated x value in the model's input tensor
  int x;
  int y;
  int z;
  for(int i = 0; i < 16; i++){
    Accel.readXYZ(&x, &y, &z);
    float l = pow(x,2)+pow(y,2)+pow(z,2);
    float l2 = sqrt(l);
    float norm_l2 = (l2-min_val)/(max_val-min_val);
    input->data.f[i] = norm_l2;
    delay(INTERVAL);
  }

  // Run inference, and report any error
  
  tfMicroWrapper.invoke();
  
  if(output->data.f[0] < output->data.f[1]){
    SerialUSB.println("Buzzer!");
    Wio.LedSetRGB(10,10,10);
    sprintf(data, "{\"\":%s}", "BUZZER");
    send_unified(data);
  }else{
    Wio.LedSetRGB(0,0,0);
  }
   
}
