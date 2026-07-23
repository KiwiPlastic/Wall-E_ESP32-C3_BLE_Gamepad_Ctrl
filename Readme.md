# Wall-E ESP32-C3 using BLE Game Controller
Thie is an upgrade on the Wall-E project developed by by Chillbasket

![Walle-E Chillibasket](https://github.com/KiwiPlastic/Wall-E_ESP32-C3_BLE_Gamepad_Ctrl/blob/main/Images/Chillibasket1.jpg?raw=true)

## 🚀 Upgrades and Features
  -	Web Server removed
  -	Added BLE Gamepad controller – Xbox controler 2026
  -	Added working Top buttons
  -	Added Bay Door Servo
  -	Controllable Bay LED light
  -	Electronics Platform – frees up Bay so can have a boot and plant etc
  -	RGB mood LED
  -	1.44 Oled feedback display
  -	All existing avaliable upgrade prints have been used and are included.
  - The only changed one is the top cover to accomidate pushbuttons
  
•	10 DOF:-
  -	Arms (2)
  -	Neck (2)
  -	Head Rotation (1)
  -	Eyes (2) 
  -	Eye Borrows (2)
  -	Bay Door (1)

•	Size:	H = 338		W = 291	D = 186

## 📦 Build & Installation

•	See this link for Wall-E Assembly Instructions and Videos by Chillbasket (https://wired.chillibasket.com/3d-printed-wall-e/) 

### Hardware: -
    1 x ESP32-C3
    1 x I2C PCA9685 16-Channel 12-bit PWM Servo Motor Driver Module
    1 x I2C PCF 8575 I/O Multiplexer
    1 x Audio Module - Mini MP3 Player Audio Module (DFPLayer)
    1 x mini-Speaker 8 or 4 ohm 3watt
    1 x 1.44 I2C OLED Display
    1 x 10mm RGB LED – common cathode 
    4 x Tactile Pushbutton with LED
    4 x 150R resistor
    1 x 4K7 Resistor
    3 x Logic Level Shifter 3v to 5v 
    1 x L298N Dual H Bridge DC Motor Driver
    10 x MG90S metal gear servos
    2 x 37mm motor with gearbox, offset shaft, 25kg, 150 rpm (Not cheap ones)
    1 x LR7843 Mosfet module
    1 x MP1482 min buck set to 2v
    1 x 1watt or 3watt LED (torch head light LED)
    2 x Buck 5amp Set to 5v and 6v
    1 x 10amp auto reset fuse
    1 x XT60 Male Connector
    16 x Bearings 10 x 15 x 4
    Misc. M3 bolts, nuts, heat inserts, heat shrink

circuit


```bash
git clone https://github.com
cd project-name
npm install
```

## 🛠️ Usage
Provide a quick example or screenshot showing how to run the project:
```bash
npm start
```

## 📜 License
Distributed under the MIT License. See `LICENSE` for more information.
