# WASP Control Interface

Interface for the multi stepper driver box.  
Currently setup for a 2 material extruder with a active mixer

# Control interface
The motor controller (mega 2560) has 4 digital inputs configured, enable and three option switches.
Currently these are set to:  
Enable : 7  
Switch 1 : 8  
Switch 2 : 9  
Switch 3 : 10  

These can be changed in 'pin_config.h'  

When high enable activates the stepper motor drivers.

The motor velocities can then be modified through the option switches as follows:  
|Switch | Mix Ratio|
|----------|------------|
|1 1 1 | 50:50|
|1 0 0 | 100:0|
|1 1 0 | 75:25|
|0 0 1 | 0:100|
|0 1 1 | 25:75|
|1 0 1 |0:0|
|0 0 0 | 0:0|

The velocities for the motors can then be set through 'extruder_settings.h'  

# Arduino Cloud
One potential interface is through arduino cloud using an esp32. Log on to [arduino cloud](https://cloud.arduino.cc/home/) with the bmade robots login and open the MultiStepperInterface dashboard.