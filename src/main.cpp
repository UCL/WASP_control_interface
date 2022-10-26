#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SimpleEncoder.h>
#include <BasicStepperDriver.h>

// Pins Configuration
// Motor Driver
#define DIR 8
#define STEP 9
#define MOTOR_ENABLE 10

// LCD
#define RS 51
#define ENABLE 49
#define D4 47
#define D5 45
#define D6 43
#define D7 41

// Encoder
#define BUTTON 4
#define ENCODER_A 3
#define ENCODER_B 2

// Kuka IO
#define KUKA_ENABLE 7

#define MOTOR_STEPS 200
#define RPM_MIN 100
#define RPM_MAX 250
#define RPM_DEFAULT 120

LiquidCrystal lcd(RS, ENABLE, D4, D5, D6, D7);
SimpleEncoder encoder(BUTTON, ENCODER_A, ENCODER_B, RPM_DEFAULT, RPM_MIN, RPM_MAX);

BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP, MOTOR_ENABLE);

long encoder_current;
 
void setup() { 
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.write("WASP Extruder");
  lcd.setCursor(0, 1);
  lcd.write("Speed: ");

  pinMode(KUKA_ENABLE, INPUT_PULLUP);
  stepper.setEnableActiveState(LOW);

  stepper.begin(encoder.getValue(), 2);
  stepper.enable();
}

unsigned wait_time_micros;

void loop() {

  if(encoder.buttonPressed() || !digitalRead(KUKA_ENABLE)){
    lcd.setCursor(11, 1);
    lcd.print("DRV:1");
    stepper.enable();
    stepper.setRPM(encoder_current);
    stepper.startMove(100);
    wait_time_micros = stepper.nextAction();
    
  } else
  {
    lcd.setCursor(11, 1);
    lcd.print("DRV:0");
    stepper.stop();
    stepper.disable();
    wait_time_micros = 200;
  }

  if(wait_time_micros > 100){
    if (encoder.clockwise() && encoder_current == RPM_MAX )
    {
      encoder.setValue(RPM_MIN);
      encoder_current = RPM_MIN;
    }
    else if (encoder.counterClockwise() && encoder_current == RPM_MIN )
    {
      encoder.setValue(RPM_MAX);
      encoder_current = RPM_MAX;
    }
    else
    {
      encoder_current = encoder.getValue();
    }
    
    lcd.setCursor(6, 1);
    lcd.print(encoder_current);
  }
  
}

