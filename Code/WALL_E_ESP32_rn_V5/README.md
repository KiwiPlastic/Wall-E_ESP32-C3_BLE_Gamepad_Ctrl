/* * * * * * * * * * * * * * * * * * * * * * *
 *
 * Upgraded Code by:  Richard Nicholson
 * Email:    richn01@msn.com
 * based on the original project and code from Simon Bluett ( hello@chillibasket.com )
 * Version:  1.0
 * Date:     23 July 2026
 * Copyright (C) 2020, MIT License
 *
 * HOW TO USE:
 * 1. Install the following library's
 *    In the Arduino IDE, on the left, click the libary manager icon
 *    BLE-Game_Client by Tomaz bekas V0.12.1
 *    ESP_Software_Serial by Dirk Kaar SNDVAPS 8.3.1
 *    Adafruit SSD1306 by Adfruit v2.5.17
 *    Adafruit PWM Servo Driver lib by Adfruit v 3.03 
 *    Adafruit PCF8574 by Adfruit v 1.1.2
 *    DFRobotDFPlayerMini by DFRobot v1.0.6
 *    I2C Wire driver, Could be built in to IDE 
 *    
 * 2. Calibrate the servo motors, using the calibration sketch provided in the
 *    GitHub repository. Paste the calibrated values between lines 369 to 391.
 * 3. Upload the sketch to the micro-controller, and open the serial monitor 
 *    at a baud rate of 115200.
 * 4. Additional instructions and hints can be found at:
 *    https://wired.chillibasket.com/3d-printed-wall-e/
 *
 * * * * * * * * * * * * * * * * * * * * * * *

Summary of upgardes
 - Removed web server and associated libarys due to documented memory leak
 - Got Track motors working correctly
 - Got DFPlayer starting more reliably
 - Added I2C Oled display
 - Added I2C PCF8575 I/O Expander
 - Added BLE Gamepad controler libaray, note needs edit in c++ files to get working
 - Fixed Hardware crash coursed by BLE driver. Hardware serial for DFPlayer stopped working
 - Added ESP_Soft_Serial, DFPayer working with BLE driver (Xbox)
 - Expose all xbox buttons and vibration
 - Map xbox buttons to Wall-E CLI servos and controls
 - Added 4 x Push Buttons and LEDs
 - Added DFPlay Busy feed back via PCF8575
 - Added Bay Door Servo
 - Added Bay LED light
 - Added RGB mood LED on PWM module
 - Added different simple Oled display
 
NOTE: BLE Controler should be OFF on Wall-E Power up
 
  // BLE Gamepad - Xbox controler - button map
    Tracks turn left/right  Left Stick X (hoz)      
    Tracks forward/back     Left Stick Y (Vert) 
    Head rotation           Right Stick X
    Head Up/Down            Right Stick Y
    Neck Bottom             Button A + Dpad Up/Dwn
    Lhs Eye                 Button X + Dpad Up/Dwn
    Lhs Eyebrow             Button X + Dpad Left/Right
    Rhs Eye                 Button B + Dpad Up/Dwn
    Rhs Eyebrow             Button B + Dpad Left/Right
    Door Open/Close         Button Y + Dpad Left Right
    Left Arm                Left Trigger  + Dpad Up/Dwn
    Right Arm               Right Trigger + Dpad Up/Dwn
    Play Sound              Left Bumper + Dpad Left/Right - can crash
    Motor dead zone         Left Bumper + Dpad Up/Dwn - 0 to 250
    LEDS move               Right Bumper + Dpad Left/Right - loop x 4
    Steering Offset         Right Bumper + Dpad Up/Dwn -  -100 to 100
    Animation Number        ViewButton +  Dpad Up/Dwn
    Automonus Servo Mode    MenuButton On/Off toggel
    Dpad Up                 adjust up
    Dpad Down               adjust down
    Dpad Left               adjust left
    Dpad Right              adjust right
    Left Stick Button
    Right Stick Button
    ShareButton
    XboxButton

 // LED Push Buttons on Front
    RED_PB = Play Prev
    GRN_PB = PlaY Next
    ORG_PB = Autonomus Servos On/Off
    WHT_PB = Door Open/Close

 //  CLI
    Wn -100 to 100 Motor turn Left/Right Ctrl
    Yn -100 to 100 Motor Forward/Reverse
    sn -100 to 100 Steering Offset
    On 0 to 250 Motor dead zone
    An  Animation number 
    Mn  0 to 1 Autoomonus Servo Mode on/off
    Ln 0 to 100 Left Arm move 
    Rn 0 to 100 Right Arm Movement
    Bn 0 to 100 Neck Bottom
    Tn 0 to 100 Neck Top        
    Gn 0 to 100 Head Rotation
    En 0 to 100 Lhs Eye
    Un 0 to 100 Rhs Eye
    Cn 0 to 100 Lhs Eyebrow
    Vn 0 to 100 Rhs Eyebrow
    Dn 0 to 1 Door open/close
    w Move forwrad
    q Stop
    s Backward
    a Drive & look Left
    d Drive and look Right
    j Left head Tilt
    l Right Head tilt
    i Sad Head
    k Head Neutral
    f Head Up
    g Head Forward
    h Head Down
    b LHS Arm Low,RHS Arm up
    n Both Arms Natural
    m left arm high,right arm low
    !n play sound n (not 0 will crash) - oled
    vn Volume n 0-30? - ole

 // PWM Mux channel #
    0 Head Rotation
    1 Neck Top
    2 Neckk Bottom
    3 Eye Right
    4 Eye Left
    5 Arm Left
    6 Arm Right
    7 Eye Brow Right
    8 Eye Brow Left
    9 Door
    13 RED RGB LED
    14 GRN RGB LED
    15 BLU RGB LED

//---------------------------------------------------------------
 * @following shows calls into DFPlayer.  eg myDFPlayer.play(number); 
 * @file DFRobotDFPlayerMini.h 

  #define DFPLAYER_EQ_NORMAL 0
  #define DFPLAYER_EQ_POP 1
  #define DFPLAYER_EQ_ROCK 2
  #define DFPLAYER_EQ_JAZZ 3
  #define DFPLAYER_EQ_CLASSIC 4
  #define DFPLAYER_EQ_BASS 5

  #define DFPLAYER_DEVICE_U_DISK 1
  #define DFPLAYER_DEVICE_SD 2
  #define DFPLAYER_DEVICE_AUX 3
  #define DFPLAYER_DEVICE_SLEEP 4
  #define DFPLAYER_DEVICE_FLASH 5

  #define DFPlayerCardInserted 2
  #define DFPlayerCardRemoved 3
  #define DFPlayerCardOnline 4
  #define DFPlayerPlayFinished 5
  #define DFPlayerError 6
  #define DFPlayerUSBInserted 7
  #define DFPlayerUSBRemoved 8
  #define DFPlayerUSBOnline 9
  #define DFPlayerCardUSBOnline 10
  #define DFPlayerFeedBack 11
  
  //---- can call these 
  
  void setTimeOut(unsigned long timeOutDuration);
  
  void next();
  
  void previous();
  
  void play(int fileNumber=1);    // eg myDFPlayer.play(number); 
  
  void volumeUp();
  
  void volumeDown();
  
  void volume(uint8_t volume);
  
  void EQ(uint8_t eq);
  
  void loop(int fileNumber);
  
  void outputDevice(uint8_t device);
  
  void sleep();
  
  void reset();
  
  void start();
  
  void pause();
  
  void playFolder(uint8_t folderNumber, uint8_t fileNumber);
  
  void outputSetting(bool enable, uint8_t gain);
  
  void enableLoopAll();
  
  void disableLoopAll();
  
  void playMp3Folder(int fileNumber);
  
  void advertise(int fileNumber);
  
  void playLargeFolder(uint8_t folderNumber, uint16_t fileNumber);
  
  void stopAdvertise();
  
  void stop();
  
  void loopFolder(int folderNumber);
  
  void randomAll();
  
  void enableLoop();
  
  void disableLoop();
  
  void enableDAC();
  
  void disableDAC();
  
  int readState();
  
  int readVolume();

  int readEQ();
  
  int readFileCounts(uint8_t device);
  
  int readCurrentFileNumber(uint8_t device);
  
  int readFileCountsInFolder(int folderNumber);
  
  int readFileCounts();

  int readFolderCounts();
  
  int readCurrentFileNumber();
  




