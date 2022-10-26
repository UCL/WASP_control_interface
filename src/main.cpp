#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Encoder.h>
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
Encoder encoder(ENCODER_A, ENCODER_B);

BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP, MOTOR_ENABLE);

long encoder_diff, set_speed = RPM_DEFAULT;
unsigned wait_time_micros;
bool prev_motor_state=false, encoder_changed=false;
 
void setup() { 
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.write("WASP Extruder");
  lcd.setCursor(0, 1);
  lcd.write("Speed:     DRV:0");

  pinMode(KUKA_ENABLE, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);
  stepper.setEnableActiveState(LOW);

  stepper.begin(set_speed, 2);
  stepper.enable();
}

void loop() {

  if(!digitalRead(BUTTON) || !digitalRead(KUKA_ENABLE)){
    if (!prev_motor_state)
    {
      lcd.setCursor(15, 1);
      lcd.print("1");
    }
    stepper.enable();
    stepper.setRPM(set_speed);
    stepper.startMove(100);
    wait_time_micros = stepper.nextAction();
    prev_motor_state = true;
    
  } else
  {
    if (prev_motor_state)
    {
      lcd.setCursor(15, 1);
      lcd.print("0");
    }
    stepper.stop();
    stepper.disable();
    wait_time_micros = 200;
    prev_motor_state = false;
  }

  if(wait_time_micros > 100){
    encoder_diff = encoder.readAndReset();

    // If changed
    if(abs(encoder_diff) > 0){
      if (set_speed + encoder_diff > RPM_MAX )
      {
        set_speed = RPM_MIN;
      }
      else if (set_speed + encoder_diff < RPM_MIN )
      {
        set_speed = RPM_MAX;
      }
      else
      {
        set_speed += encoder_diff;
      }
      
      lcd.setCursor(6, 1);
      lcd.print(set_speed);
    }
  }
}

