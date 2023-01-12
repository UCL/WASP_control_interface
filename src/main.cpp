#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <BasicStepperDriver.h>

// Pins Configuration
// Motor Driver
#define DIR 8
#define STEP 9
#define MOTOR_ENABLE 10

// LCD
#define ADDR 0x20
#define N_COL 16
#define N_ROWS 2

// Encoder
#define BUTTON 5
#define ENCODER_DT 3
#define ENCODER_CLK 2

// Kuka IO
#define KUKA_ENABLE 13

#define MOTOR_STEPS 200
#define RPM_MIN 100
#define RPM_MAX 250
#define RPM_DEFAULT 120

LiquidCrystal_I2C lcd(ADDR, N_COL, N_ROWS);
Encoder encoder(ENCODER_DT, ENCODER_CLK);

BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP, MOTOR_ENABLE);

long encoder_diff, set_speed = RPM_DEFAULT;
unsigned wait_time_micros;
bool prev_motor_state=false, encoder_changed=false;
 
void setup() { 
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("WASP Extruder");
  lcd.setCursor(0, 1);
  lcd.print("Speed:     DRV:0");

  lcd.setCursor(6, 1);
  lcd.print(set_speed);
  
  pinMode(KUKA_ENABLE, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);
  stepper.setEnableActiveState(LOW);

  stepper.begin(set_speed, 2);
  stepper.enable();
}

/**
 * @brief loop function to check the inputs and run the motor
 * 
 * 
 */

void loop() {

  if(!digitalRead(BUTTON) || digitalRead(KUKA_ENABLE)){
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

