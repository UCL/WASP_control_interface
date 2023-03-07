#include <Arduino.h>
#include <arduino-timer.h>
#include "pin_config.h"
#include "extruder_settings.h"
#include <AccelStepper.h>
#include <MultiStepper.h>


//TODO: enable the use of the 4th driver
AccelStepper stepper_a(AccelStepper::DRIVER, AXIS_A_STP, AXIS_A_DIR),
             stepper_b(AccelStepper::DRIVER, AXIS_B_STP, AXIS_B_DIR),
             stepper_c(AccelStepper::DRIVER, AXIS_C_STP, AXIS_C_DIR);

MultiStepper multi_stepper; 

int set_speed_a = 0, set_speed_b = 0, set_speed_c = 0;
int status_enable = 0, status_sw1 = 0, status_sw2 = 0, status_sw3 = 0;
bool running = false, changed = false;
byte previous_state = 0b000;

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

  stepper_a.setEnablePin(AXIS_A_ENA);
  stepper_b.setEnablePin(AXIS_B_ENA);
  stepper_c.setEnablePin(AXIS_C_ENA);

  stepper_a.setMaxSpeed(500);
  stepper_b.setMaxSpeed(500);
  stepper_c.setMaxSpeed(500);

  multi_stepper.addStepper(stepper_a);
  multi_stepper.addStepper(stepper_b);
  multi_stepper.addStepper(stepper_c);

  if (Serial)
  {
    // If the serial is connected then send the output
    timer.every(1000, serialStatus);
  }
  
}

bool calculateSpeedFromIO(int sw1, int sw2, int sw3){
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

  // Return true if state has changed;
  if(state != previous_state){
    previous_state = state;
    return true;
  } else{
    previous_state = state;
    return false;
  }
}

float rpmToStep(float rpm){
  return rpm * (MICROSTEPS * MOTOR_STEPS)/60.0;
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
    // Check if the state (switches) are different
    if(calculateSpeedFromIO(status_sw1, status_sw2, status_sw3) || !running){
      stepper_a.setSpeed(rpmToStep(set_speed_a));
      stepper_b.setSpeed(rpmToStep(set_speed_b));
      stepper_c.setSpeed(rpmToStep(set_speed_c));
    }

    if(!running){
      stepper_a.enableOutputs();
      stepper_b.enableOutputs();
      stepper_c.enableOutputs();

      running = true;
    }
    
    stepper_a.runSpeed();
    stepper_b.runSpeed();
    stepper_c.runSpeed();
    
  } else
  {
    if(running){
      stepper_a.stop();
      stepper_b.stop();
      stepper_c.stop();

      stepper_a.disableOutputs();
      stepper_b.disableOutputs();
      stepper_c.disableOutputs();

      running = false;
    }
  }
}

