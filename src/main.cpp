#include <Arduino.h>
#include <BasicStepperDriver.h>
#include <MultiDriver.h>
#include <arduino-timer.h>
#include "pin_config.h"
#include "extruder_settings.h"

#define MOTOR_STEPS 200
#define MICROSTEPS 1

//TODO: enable the use of the 4th driver
BasicStepperDriver stepper_a(MOTOR_STEPS, AXIS_A_DIR, AXIS_A_STP, AXIS_A_ENA),
                  stepper_b(MOTOR_STEPS, AXIS_B_DIR, AXIS_B_STP, AXIS_B_ENA),
                  stepper_c(MOTOR_STEPS, AXIS_C_DIR, AXIS_C_STP, AXIS_C_ENA);
                  // stepper_d(MOTOR_STEPS, AXIS_D_DIR, AXIS_D_STP, AXIS_D_ENA);

MultiDriver multi_driver(stepper_a, stepper_b, stepper_c); 

int set_speed_a = 0, set_speed_b = 0, set_speed_c = 0;

int status_enable = 0, status_sw1 = 0, status_sw2 = 0, status_sw3 = 0;

auto timer = timer_create_default();
char buff[50];

bool serialStatus(void *){
  sprintf(buff, "System enable: %d\tSW1: %d\tSW2: %d\tSW3: %d\n", status_enable, status_sw1, status_sw2, status_sw3);
  Serial.print(buff);
  return true;
}

void setup() { 
  Serial.begin(9600);
  delay(1500); 

  pinMode(ENABLE, INPUT);
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);

  // multi_driver.enable();
  multi_driver.begin(0, MICROSTEPS);

  if (Serial)
  {
    // If the serial is connected then send the output
    timer.every(1000, serialStatus);
  }
  
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
    set_speed_a = VEL_A_50;
    set_speed_b = VEL_B_50;
    set_speed_c = VEL_MIXER;
    break;
  
  case 0b110: // 1 1 0
    // 75:25 split
    set_speed_a = VEL_A_75;
    set_speed_b = VEL_B_25;
    set_speed_c = VEL_MIXER;
    break;

  case 0b100: // 1 0 0
    // 100:0 split
    set_speed_a = VEL_A_100;
    set_speed_b = 0;
    set_speed_c = VEL_MIXER;
    break;

  case 0b000: // 0 0 0
    // off
    set_speed_a = 0;
    set_speed_b = 0;
    set_speed_c = 0;
    break;

  case 0b001: // 0 0 1
    // 0:100 split
    set_speed_a = 0;
    set_speed_b = VEL_B_100;
    set_speed_c = VEL_MIXER;
    break;

  case 0b011: // 0 1 1
    // 25:75 split
    set_speed_a = VEL_A_25;
    set_speed_b = VEL_B_75;
    set_speed_c = VEL_MIXER;
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
 */

void loop() {
  //refresh status
  status_enable = digitalRead(ENABLE);
  status_sw1 = digitalRead(SW1);
  status_sw2 = digitalRead(SW2);
  status_sw3 = digitalRead(SW3);

  timer.tick();

  if(status_enable){
    multi_driver.enable();
    calculateSpeedFromIO(status_sw1, status_sw2, status_sw3);

    multi_driver.setRPM(set_speed_a, set_speed_b, set_speed_c);
    multi_driver.startMove(set_speed_a > 0? 100 : 0, set_speed_b > 0? 100 : 0, set_speed_c > 0? 100 : 0);
    multi_driver.nextAction();
  } else
  {
    multi_driver.stop();
    multi_driver.disable();
  }
  
}

