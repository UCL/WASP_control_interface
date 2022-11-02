# WASP Control Interface

Control interface for the WASP extruder mounted to a robot.  
A LCD shows the set RPM for the extruder and if the extruder is running.

## Changing the limits
The RPM limits are set in `main.cpp` line 29-31, after changing these the program will need to be reuploaded. 
Use the following steps to reupload the program:  
  1. Open this project in Visual Studio Code
  1. Install the platformIO extension
  1. Go to the platform IO tab in the task bar on the left side of the screen
  1. Under project tasks -> Default, select build
  1. If there are no errors, switch off the control box, and plug in the usb from the control box to the computer
  1. Under project tasks -> Default, select upload

## Wiring
**TODO**: Wiring diagram

### Arduino Mega Pin Connections  

Connection | Arduino Pin 
---|---
Motor driver direction | 8
Motor driver step | 9
Motor driver Enable | 10
LCD RS | 51
LCD Enable | 49
LCD D4 | 47
LCD D5 | 45
LCD D6 | 43
LCD D7 | 41
Push button (encoder) | 4
Encoder A | 3
Encoder B | 2
Kuka IO | 7

### LCD Screen Ribbon
LCD Pin | Colour | Connection 
---|---|---
VSS | Black | GND
VDD | White | 5V
V0 | Grey | Potentiometer Output
RS | Purple | Arduino 51
RW | Blue | GND
E | Green | Arduino 49
D0 | NC | NC
D1 | NC | NC
D2 | NC | NC
D3 | NC | NC
D4 | Yellow | Arduino 47
D5 | Orange | Arduino 45
D6 | Red | Arduino 43
D7 | Brown | Arduino 41
A | White | 5V 
K | Black | GND

