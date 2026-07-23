/* * * * * * * * * * * * * * * * * * * * * * *
 * Update BLE controller status
 *
 * Code by:  Richard Nicholson
 * Email:    richn01@msn.com
 * based on the original project from Simon Bluett ( hello@chillibasket.com )
 * Version:  1.0
 * Date:     23 July 2026
 * Copyright (C) 2020, MIT License
 * * * * * * * * * * * * * * * * * * * * * * */

void Controler() {  // read xbox buttons and sticks, 100ms interval

  int controllerAddress;
  float leftStickX;       // Left Stick X: Wall-E Motors turn left/right
  float leftStickY;       // Left Stick Y: Wall-E Motors forward/back
  float rightStickX;      // Right Stick X: Head rotation
  float rightStickY;      // Right Stick Y: Head Up/Down - servo Neck top
  bool leftStickButton;   //
  bool rightStickButton;  //
  bool dpadUp;            // adjust up
  bool dpadDown;          // adjust down
  bool dpadLeft;          // adjust left
  bool dpadRight;         // adjust right
  bool buttonA;           // Neck Bottom - A plus Dpad UP/Dwn
  bool buttonX;           // Lhs Eye - X plus Dpad  Up/Dwn
                          // Lhs Eyebrow - X plus Dpad Left/Right
  bool buttonB;           // Rhs Eye - B plus Dpad Up/Dwn
                          // Rhs Eyebrow - B plus Dpad Left/Right
  bool buttonY;           // Door Open/Close - Y
  bool leftTrigger;       // Left Arm move Up/Down - LT + Dpad Up/Down
  bool rightTrigger;      // Right Arm  move Up/Down - LT + Dpad Up/Dwn
  bool leftBumper;        // Motor dead zone - LB + Dpad Up/Dwn - oled, 0 to 250
                          // Play Sound LB + Dpad Left/Right - index up/Down
  bool rightBumper;       // Steering Offset - RB + Dpad Up/Dwn - oled,  -100 to 100
                          // LEDS move - RB + Dpad Left/Right - loop x 4
  bool menuButton;        // Autonomus Servo Mode on/off
  bool viewButton;        // Animation Number
  bool shareButton;       //
  bool xboxButton;        //

  if (controller.isConnected()) {
    if (!BLE_Conected_Flag) {
      BLE_Conected_Flag = true;
      Serial.println("controller CONNECTED");
      oledxbox(BLE_Conected_Flag);
    }
    XboxControlsState s;
    controller.read(&s);

    // asign local variable names
    controllerAddress = s.controllerAddress;
    leftStickX = s.leftStickX;
    leftStickY = s.leftStickY;
    rightStickX = s.rightStickX;
    rightStickY = s.rightStickY;
    leftStickButton = s.leftStickButton;
    rightStickButton = s.rightStickButton;
    dpadUp = s.dpadUp;
    dpadDown = s.dpadDown;
    dpadLeft = s.dpadLeft;
    dpadRight = s.dpadRight;
    buttonA = s.buttonA;
    buttonB = s.buttonB;
    buttonX = s.buttonX;
    buttonY = s.buttonY;
    leftBumper = s.leftBumper;
    rightBumper = s.rightBumper;
    leftTrigger = s.leftTrigger;
    rightTrigger = s.rightTrigger;
    shareButton = s.shareButton;
    menuButton = s.menuButton;
    viewButton = s.viewButton;
    xboxButton = s.xboxButton;

    // ------ Sticks ------
    // --- Left JS --- Motor turn Left/Right CLI: Xn -100 to 100
    if ((leftStickX >= LSTICK_DZ_Xmax) || (leftStickX <= LSTICK_DZ_Xmin)) {
      doEvaluateSerial('X', String(leftStickX * 100));
      neutral_LHS_JS_X = false;  // not in neutral zone
    } else {                     // is in now in neutral zone
      if (!neutral_LHS_JS_X) {
        doEvaluateSerial('X', String(0));
        neutral_LHS_JS_X = true;
      }
    }
    // --- Left JS --- Motor Forward/Reverse CLI: Yn -100 to 100
    if ((leftStickY >= LSTICK_DZ_Ymax) || (leftStickY <= LSTICK_DZ_Ymin)) {
      doEvaluateSerial('Y', String(leftStickY * 100));
      neutral_LHS_JS_Y = false;  // not in neutral zone
    } else {
      if (!neutral_LHS_JS_Y) {
        doEvaluateSerial('Y', String(0));
        neutral_LHS_JS_Y = true;
      }
    }
    // --- Right JS --- Head Rotation CLI: Gn 0 to 100
    if ((rightStickX >= RSTICK_DZ_Xmax) || (rightStickX <= RSTICK_DZ_Xmin)) {
      rightStickX = (rightStickX * 100);
      value = String(map(rightStickX, -100, 100, 0, 100));
      doEvaluateSerial('G', value);
      neutral_RHS_JS_X = false;
    } else {
      if (!neutral_RHS_JS_X) {
        doEvaluateSerial('G', String(50));
        neutral_RHS_JS_X = true;
      }
    }
    // --- Right JS --- Neck Top (up/Down) CLI: Tn 0 to 100
    if ((rightStickY >= RSTICK_DZ_Ymax) || (rightStickY <= RSTICK_DZ_Ymin)) {
      rightStickY = (rightStickY * 100);
      value = String(map(rightStickY, -100, 100, 0, 100));
      doEvaluateSerial('T', value);
      neutral_RHS_JS_Y = false;
    } else {
      if (!neutral_RHS_JS_Y) {
        doEvaluateSerial('T', String(50));
        neutral_RHS_JS_Y = true;
      }
    }
    
    // Display Both JS readings - De bug only
    // Serial.printf("lstick: %.2f,%.2f, rstick: %.2f,%.2f\n",
    //              s.leftStickX, s.leftStickY, s.rightStickX, s.rightStickY);

    // ---- Buttons ----
    // Neck Bottom, up - A + Dpad UP - CLI: Bn 0-100
    if (buttonA && dpadUp) {
      neckBottomIndex++;
      if (neckBottomIndex >= 101) { neckBottomIndex = 100; }
      value = String(neckBottomIndex);
      doEvaluateSerial('B', value);
    }
    // Neck Bottom, down - A + Dpad Dwn -  CLI: Bn 0-100
    if (buttonA && dpadDown) {
      neckBottomIndex--;
      if (neckBottomIndex == 255) { neckBottomIndex = 0; }
      value = String(neckBottomIndex);
      doEvaluateSerial('B', value);
    }
    // Rhs Eye up - B + Dpad Up - CLI: Un 0 to 100
    if (buttonB && dpadUp) {
      eyeRHSIndex++;
      if (eyeRHSIndex >= 101) { eyeRHSIndex = 100; }
      value = String(eyeRHSIndex);
      doEvaluateSerial('U', value);
    }
    // Rhs Eye  down - B + Dpad Dwn - CLI: Un 0 to 100
    if (buttonB && dpadDown) {
      eyeRHSIndex--;
      if (eyeRHSIndex == 255) { eyeRHSIndex = 0; }
      value = String(eyeRHSIndex);
      doEvaluateSerial('U', value);
    }
    // Rhs Eyebrow up - B + Dpad Left - CLI: Vn 0 to 100
    if (buttonB && dpadLeft) {
      eyebrowRHSIndex++;
      if (eyebrowRHSIndex >= 101) { eyebrowRHSIndex = 100; }
      value = String(eyebrowRHSIndex);
      doEvaluateSerial('V', value);
    }
    // Rhs Eyebrow down - B + Dpad Right - CLI: Vn 0 to 100
    if (buttonB && dpadRight) {
      eyebrowRHSIndex--;
      if (eyebrowRHSIndex == 255) { eyebrowRHSIndex = 0; }
      value = String(eyebrowRHSIndex);
      doEvaluateSerial('V', value);
    }
    // Lhs Eye up - X + Dpad  Up - CLI: En 0 to 100
    if (buttonX && dpadUp) {
      eyeLHSIndex++;
      if (eyeLHSIndex >= 101) { eyeLHSIndex = 100; }
      value = String(eyeLHSIndex);
      doEvaluateSerial('E', value);
    }
    // Lhs Eye down - X + Dpad Dwn - CLI: En 0 to 100
    if (buttonX && dpadDown) {
      eyeLHSIndex--;
      if (eyeLHSIndex == 255) { eyeLHSIndex = 0; }
      value = String(eyeLHSIndex);
      doEvaluateSerial('E', value);
    }
    // Lhs Eyebrow up - X + Dpad Left - CLI: Cn 0 to 100
    if (buttonX && dpadLeft) {
      eyebrowLHSIndex++;
      if (eyebrowLHSIndex >= 101) { eyebrowLHSIndex = 100; }
      value = String(eyebrowLHSIndex);
      doEvaluateSerial('C', value);
    }
    // Lhs Eyebrow down - X + Dpad Right - CLI: Cn 0 to 100
    if (buttonX && dpadRight) {
      eyebrowLHSIndex--;
      if (eyebrowLHSIndex == 255) { eyebrowLHSIndex = 0; }
      value = String(eyebrowLHSIndex);
      doEvaluateSerial('C', value);
    }
    // Door Open/Close - Y button - CLI: Dn 0 to 1
    if (buttonY && !doorFlag) {
      doorMode = !doorMode;
      value = String(doorMode);
      doEvaluateSerial('D', value);
    }
    // Left Arm move Up - LT + Dpad Up - CLI: Ln 0 to 100
    if (leftTrigger && dpadUp) {
      leftArmIndex++;
      if (leftArmIndex >= 101) { leftArmIndex = 100; }
      value = String(leftArmIndex);
      doEvaluateSerial('L', value);
    }
    // Left Arm move Down - LT + Dpad Down - CLI: Ln 0 to 100
    if (leftTrigger && dpadDown) {
      leftArmIndex--;
      if (leftArmIndex == 255) { leftArmIndex = 0; }
      value = String(leftArmIndex);
      doEvaluateSerial('L', value);
    }
    // Right Arm  move Up - RT + Dpad Up - CLI: Rn 0 to 100
    if (rightTrigger && dpadUp) {
      rightArmIndex++;
      if (rightArmIndex >= 101) { rightArmIndex = 100; }
      value = String(rightArmIndex);
      doEvaluateSerial('R', value);
    }
    // Right Arm  move down - RT + Dpad Down - CLI: Rn 0 to 100
    if (rightTrigger && dpadDown) {
      rightArmIndex--;
      if (rightArmIndex == 255) { rightArmIndex = 0; }
      value = String(rightArmIndex);
      doEvaluateSerial('R', value);
    }
    // play prev file  - Left Bumper + Dpad Left - CLI: !n 1-17 (Not zero, will crash)
    if (leftBumper && dpadLeft) {
      if (!soundPlaying) {
        playIndex--;
        if (playIndex == 255) { playIndex = 17; }
        if (playIndex == 0) { playIndex = 1; }
      }
    }
    // play next file  Left Bumper + Dpad Right - CLI: !n 1-17
    if (leftBumper && dpadRight) {
      if (!soundPlaying) {  // Play sound)
        playIndex++;
        if (playIndex >= 18) { playIndex = 1; }
        if (playIndex == 0) { playIndex = 1; }
      }
    }
    // Motor Dead zone Down - LB + Dpad Up - CLI: On 0 to 250
    if (leftBumper && dpadUp) {
      motorDeadZoneIndex++;
      if (motorDeadZoneIndex >= 251) { motorDeadZoneIndex = 250; }
      value = String(motorDeadZoneIndex);
      doEvaluateSerial('O', value);
    }
    // Motor Dead zone Down - LB + Dpad Down - CLI: On 0 to 250
    if (leftBumper && dpadDown) {
      motorDeadZoneIndex--;
      if (motorDeadZoneIndex == 255) { motorDeadZoneIndex = 0; }
      value = String(motorDeadZoneIndex);
      doEvaluateSerial('O', value);
    }
    // Steering Offset - RB + Dpad Up - CLI: Sn -100 to 100
    if (rightBumper && dpadUp) {
      steerOffsetIndex++;
      if (steerOffsetIndex >= 101) { steerOffsetIndex = 100; }
      value = String(steerOffsetIndex);
      doEvaluateSerial('S', value);
    }
    // Steering Offset - RB + Dpad Down - CLI: Sn -100 to 100
    if (rightBumper && dpadDown) {
      steerOffsetIndex--;
      if (steerOffsetIndex <= -101) { steerOffsetIndex = -100; }
      value = String(steerOffsetIndex);
      doEvaluateSerial('S', value);
    }
    // LED Move left - RB + Dpad left, 0 = off, else 1-4
    if (rightBumper && dpadLeft) {
      ledIndex--;
      if (ledIndex == 255) { ledIndex = 4; }
    }
    // LED Move Right  - RB + Dpad right
    if (rightBumper && dpadRight) {
      ledIndex++;
      if (ledIndex >= 5) { ledIndex = 0; }
    }
    // "Autonomous Servo Mode" - menuButton- CLI: M0 off, M1 = on
    if (menuButton && !autoModeFlag) {
      autonomousMode = !autonomousMode;
      value = String(autonomousMode);
      doEvaluateSerial('M', value);
    }
    // Animation number to play - viewButton + Dpad Up/Down -  CLI: An
    if (viewButton && dpadUp) {
      animationIndex++;
      if (animationIndex >= 101) { animationIndex = 100; }
      value = String(animationIndex);
      doEvaluateSerial('A', value);
    }
    if (viewButton && dpadDown) {
      animationIndex--;
      if (animationIndex == 255) { animationIndex = 0; }
      value = String(animationIndex);
      doEvaluateSerial('A', value);
    }

    if (shareButton) { Serial.println("shareButton"); }
    if (xboxButton) { Serial.println("xboxButton"); }
    if (leftStickButton) { Serial.println("LeftStickButton"); }
    if (rightStickButton) { Serial.println("RightStickButton"); }

    //if (leftTrigger) {Serial.println("leftTrigger");}
    //if (rightTrigger) {Serial.println("rightTrigger");}
    //if (dpadUp) { Serial.println("dpadUp"); }
    //if (dpadDown) { Serial.println("dpadDown"); }
    //if (dpadLeft) { Serial.println("dpadLeft"); }
    //if (dpadRight) { Serial.println("dpadRight"); }

    BLE_Conected_Flag = true;

  } else {                    // no longer connected
    if (BLE_Conected_Flag) {  // was connected
      Serial.println("controller NOT connected");
      BLE_Conected_Flag = false;
      oledxbox(BLE_Conected_Flag);
    }
  }
}

// vibration(0);     // xbox vibrate use 0-4 values

//-------------------------
void vibration(int in) {
  XboxVibrationsCommand cmd;
  switch (in % 4) {
    case 0: cmd.rightMotor = 1.0f; break;  // 1.0f = 100% power on the motor
    case 1: cmd.leftMotor = 1.0f; break;
    case 2: cmd.leftTriggerMotor = 1.0f; break;
    case 3: cmd.rightTriggerMotor = 1.0f; break;
  }
  cmd.durationMs = 500;
  controller.write(cmd);

  //Serial.printf("rmotor: %.2f, lmotor: %.2f, ltmotor: %.2f, rtmotor: %.2f\n",
  //              cmd.rightMotor, cmd.leftMotor, cmd.leftTriggerMotor, cmd.rightTriggerMotor);
}
