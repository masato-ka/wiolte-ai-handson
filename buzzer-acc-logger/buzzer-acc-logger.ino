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

bool is_buzzer_on = true;

void buzzer(HardwareTimer *HT){
  digitalWrite(BUZZER_PIN, is_buzzer_on);
  is_buzzer_on = !is_buzzer_on;
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

  Timer3.setOverflow(1000 * 1000, MICROSEC_FORMAT); // = per 1 sec
  Timer3.attachInterrupt(&buzzer);
  //Timer3.resume();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  
  delay(500);
  
  SerialUSB.println("### Setup completed.");
}

void loop()
{
  int buttonState = digitalRead(BUTTON_PIN);
  if(buttonState){
    digitalWrite(BUZZER_PIN, HIGH);
  }else{
    digitalWrite(BUZZER_PIN, LOW);
  }
  int x;
  int y;
  int z;
  Accel.readXYZ(&x, &y, &z);
  SerialUSB.print(buttonState);
  SerialUSB.print(' ');
  SerialUSB.print(x);
  SerialUSB.print(' ');
  SerialUSB.print(y);
  SerialUSB.print(' ');
  SerialUSB.println(z);
  
  
  delay(INTERVAL);
}
