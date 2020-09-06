#include <WioLTEforArduino.h>
#include <ADXL345.h>          // https://github.com/Seeed-Studio/Accelerometer_ADXL345

#define INTERVAL    (10)

#define BUTTON_PIN (WIOLTE_D20)
#define BUZZER_PIN (WIOLTE_D38)

#define BUZZER_ON_TIME (10)
#define BUZZER_OFF_TIME (3000)

HardwareTimer Timer3(TIM3);
volatile int t3;

WioLTE Wio;
ADXL345 Accel;

int is_buzzer_on = 0;


void button(){
    if(is_buzzer_on == 1){
      is_buzzer_on = 0;
    }else{
      is_buzzer_on = 1;
    }
}

void setup()
{
  delay(200);

  SerialUSB.println("");
  SerialUSB.println("--- START ---------------------------------------------------");
  
  SerialUSB.println("### I/O Initialize.");
  Wio.Init();
  
  SerialUSB.println("### Power supply ON.");
  Wio.PowerSupplyGrove(true);
  delay(500);
  Accel.powerOn();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(BUTTON_PIN, button, RISING);
  delay(500);
  
  SerialUSB.println("### Setup completed.");
}

void loop()
{
  int x;
  int y;
  int z;
  Accel.readXYZ(&x, &y, &z);
  SerialUSB.print(is_buzzer_on);
  SerialUSB.print(' ');
  SerialUSB.print(x);
  SerialUSB.print(' ');
  SerialUSB.print(y);
  SerialUSB.print(' ');
  SerialUSB.println(z);
  
  digitalWrite(BUZZER_PIN, is_buzzer_on);

  delay(INTERVAL);
}
