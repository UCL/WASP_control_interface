#include <Arduino.h>
#include <BasicStepperDriver.h>
#include <MultiDriver.h>
#include <pin_config.h>

#define MOTOR_STEPS 200
#define MICROSTEPS 1
#define RPM_MIN 0
#define RPM_MAX 300
#define RPM_DEFAULT 100

//TODO: enable the use of the 4th driver
BasicStepperDriver stepper_a(MOTOR_STEPS, AXIS_A_DIR, AXIS_A_STP, AXIS_A_ENA),
                  stepper_b(MOTOR_STEPS, AXIS_B_DIR, AXIS_B_STP, AXIS_B_ENA),
                  stepper_c(MOTOR_STEPS, AXIS_C_DIR, AXIS_C_STP, AXIS_C_ENA);
                  // stepper_d(MOTOR_STEPS, AXIS_D_DIR, AXIS_D_STP, AXIS_D_ENA);

MultiDriver multi_driver(stepper_a, stepper_b, stepper_c); 

long encoder_diff;
int set_speed_a = RPM_DEFAULT, set_speed_b = RPM_DEFAULT, set_speed_c = RPM_DEFAULT;
unsigned wait_time_micros;
bool prev_motor_state=false, encoder_changed=false;
 
void setup() { 
  pinMode(ENABLE, INPUT_PULLUP);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);

  multi_driver.enable();
  multi_driver.begin(RPM_DEFAULT, MICROSTEPS);
}

void calculateSpeedFromIO(int sw1, int sw2, int sw3){
  byte state = 0b000;
  bitWrite(state, 0, sw1);
  bitWrite(state, 1, sw2);
  bitWrite(state, 2, sw3);
  switch (state)
  {
  case 0b111: // 1 1 1
    // 50:50 split
    set_speed_a = 50;
    set_speed_b = 50;
    set_speed_c = 100;
    break;
  
  case 0b110: // 1 1 0
    // 75:25 split
    set_speed_a = 75;
    set_speed_b = 25;
    set_speed_c = 100;
    break;

  case 0b100: // 1 0 0
    // 100:0 split
    set_speed_a = 100;
    set_speed_b = 0;
    set_speed_c = 100;
    break;

  case 0b000: // 0 0 0
    // off
    set_speed_a = 0;
    set_speed_b = 0;
    set_speed_c = 0;
    break;

  case 0b001: // 0 0 1
    // 0:100 split
    set_speed_a = 100;
    set_speed_b = 0;
    set_speed_c = 100;
    break;

  case 0b011: // 0 1 1
    // 25:75 split
    set_speed_a = 25;
    set_speed_b = 75;
    set_speed_c = 100;
    break;
  
  default:
    set_speed_a = 0;
    set_speed_b = 0;
    set_speed_c = 0;
    break;
  }
}

/**
 * @brief loop function to check the inputs and run the motor
 * 
 * 
 */

void loop() {
  if(!digitalRead(ENABLE)){
    calculateSpeedFromIO(digitalRead(SW1), digitalRead(SW2), digitalRead(SW3));

    multi_driver.enable();
    multi_driver.setRPM(set_speed_a, set_speed_b, set_speed_c);
    multi_driver.startMove(100, 100, 100);
    wait_time_micros = multi_driver.nextAction();
  } else
  {
    multi_driver.stop();
    multi_driver.disable();
    wait_time_micros = 200;
    prev_motor_state = false;
  }
}

