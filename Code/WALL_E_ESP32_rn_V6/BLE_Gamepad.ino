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
  int leftStickX;         // Left Stick X: Head rotation
  int leftStickY;         // Left Stick Y: Head Up/Down - Neck Bottom servo
  int rightStickX;        // Right Stick X: Wall-E Motors turn left/right
  int rightStickY;        // Right Stick Y: Wall-E Motors forward/back
  bool leftStickButton;   //
  bool rightStickButton;  // Autonomus Mode on/off
  bool buttonA;           // Neck Top - A + L JS
  bool buttonX;           // Lhs Eye - X + L JS
  bool buttonB;           // Rhs Eye - B + L JS
  bool buttonY;           // Door Open/Close toggle - Y
  bool leftBumper;        // LHS Eye Brrow Flap open/close
  bool rightBumper;       // RHS Eye Brow Flap open/close
  bool leftTrigger;       // Left Arm move Up/Down - LT + L JS
  bool rightTrigger;      // Right Arm  move Up/Down - RT + L JS
  bool dpadUp;            // adjust up
  bool dpadDown;          // adjust down
  bool dpadLeft;          // adjust left
  bool dpadRight;         // adjust right
  bool menuButton;        // Animation Number #
  bool viewButton;        // Dead Zone #
  bool shareButton;       // Steering Offset #
  bool xboxButton;        // 

  // Is controller connected
  if (controller.isConnected()) {
    if (!BLE_Conected_Flag) {
      BLE_Conected_Flag = true;
      Serial.println("controller CONNECTED");
      oledxbox(BLE_Conected_Flag);
    }
    XboxControlsState s;
    controller.read(&s);

    // read current values to local variable names
    controllerAddress = s.controllerAddress;
    leftStickX = (s.leftStickX * 100);
    leftStickY = (s.leftStickY * 100);
    rightStickX = (s.rightStickX * 100);
    rightStickY = (s.rightStickY * 100);
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

    //----------------------------------------------------
    // Play File  - Left Trigger & right Trigger + Dpad up/down - CLI: !n 1-17 (playindextotal)
    // need to do this here to stop serial conflick with DFP = crash
    if (leftTrigger && rightTrigger) {
      playFilePBFlag = true;
      if ((playIndex == lastplayIndex) && (displaytime == 0)) {  // display current index if not already
        OledPlayFile(playIndex);
      }
      if (dpadUp) {
        playIndex++;
        if (playIndex >= (playIndexTotal + 1)) { playIndex = 1; }
        if (playIndex == 0) { playIndex = 1; }
        OledPlayFile(playIndex);
      }
      if (dpadDown) {
        playIndex--;
        if (playIndex == 255) { playIndex = playIndexTotal; }
        if (playIndex == 0) { playIndex = 1; }
        OledPlayFile(playIndex);
      }
      goto bailout;
    }
    if ((!leftTrigger && !rightTrigger) && playFilePBFlag) {
      playFilePBFlag = false;
      //lastplayIndex = playIndex;
      if (!soundPlaying) {    
        OledPlayFile(playIndex);   
        playSoundTrigger = true;
      }
      //value = String(playIndex);
      //doEvaluateSerial('!', value);  // send new value
      goto bailout;
    }


    //----------------------------------------------------------------------------
    // Display Both JS readings - De bug only
    //
    //Serial.printf("lstick: %.2f,%.2f, rstick: %.2f,%.2f\n",
    //              s.leftStickX, s.leftStickY, s.rightStickX, s.rightStickY);
    //----------------------------------------------------------------------------

    //----------------------------------------------------------------
    // TRACKS - Right Joy Stick X --- Motor turn Left/Right CLI: Xn -100 to 100
    if ((rightStickX >= RSTICK_DZ_Xmax) || (rightStickX <= RSTICK_DZ_Xmin)) {
      value = String(map(rightStickX, -100, 100, 100, -100));  //convert signal to correct way around
      doEvaluateSerial('X', value);
      neutral_RHS_JS_X = false;
    } else {
      if (!neutral_RHS_JS_X) {
        doEvaluateSerial('X', String(0));
        neutral_RHS_JS_X = true;
      }
    }

    //----------------------------------------------------------------
    // TRACKS Right Joy Stick Y --- Motor Forward/Reverse CLI: Yn -100 to 100
    if ((rightStickY >= RSTICK_DZ_Ymax) || (rightStickY <= RSTICK_DZ_Ymin)) {
      doEvaluateSerial('Y', String(rightStickY));
      neutral_RHS_JS_Y = false;
    } else {
      if (!neutral_RHS_JS_Y) {
        doEvaluateSerial('Y', String(0));
        neutral_RHS_JS_Y = true;
      }
    }

    //---------------------------------------
    // Neck Top - A + Left JS - CLI: Tn 0-100
    if (buttonA) {
      if ((leftStickY != lastnecktop) && ((leftStickY >= LSTICK_DZ_Ymax) || (leftStickY <= LSTICK_DZ_Ymin))) {
        lastnecktop = leftStickY;
        value = String(map(leftStickY, -100, 100, 0, 100));
        doEvaluateSerial('T', value);
        neutral_LHS_JS_Y = false;  // not in neutral zone
        goto bailout;
      } else {  // is in now in neutral zone
        if ((!neutral_LHS_JS_Y) && (leftStickY != lastnecktop)) {
          doEvaluateSerial('T', String(50));
          neutral_LHS_JS_Y = true;
          lastnecktop = leftStickY;
          goto bailout;
        }
        goto bailout;
      }
      goto bailout;
    }

    //---------------------------------------
    // Lhs Eye - X + JS - CLI: En 0 to 100
    else if (buttonX) {
      if ((leftStickY != lasteyeLHS) && ((leftStickY >= LSTICK_DZ_Ymax) || (leftStickY <= LSTICK_DZ_Ymin))) {
        lasteyeLHS = leftStickY;
        value = String(map(leftStickY, -100, 100, 0, 100));
        doEvaluateSerial('E', value);
        neutral_LHS_JS_Y = false;  // not in neutral zone
        goto bailout;
      } else {  // is now in neutral zone or holding
        if ((!neutral_LHS_JS_Y) && (leftStickY != lasteyeLHS)) {
          doEvaluateSerial('E', String(50));
          neutral_LHS_JS_Y = true;
          lasteyeLHS = leftStickY;
          goto bailout;
        }
        goto bailout;
      }
      goto bailout;
    }

    //--------------------------------------
    // Rhs Eye - B + JS - CLI: Un 0 to 100
    else if (buttonB) {
      if ((leftStickY != lasteyeRHS) && ((leftStickY >= LSTICK_DZ_Ymax) || (leftStickY <= LSTICK_DZ_Ymin))) {
        lasteyeRHS = leftStickY;
        value = String(map(leftStickY, -100, 100, 0, 100));
        doEvaluateSerial('U', value);
        neutral_LHS_JS_Y = false;  // not in neutral zone
        goto bailout;
      } else {  // is in now in neutral zone
        if ((!neutral_LHS_JS_Y) && (leftStickY != lasteyeRHS)) {
          doEvaluateSerial('U', String(50));
          neutral_LHS_JS_Y = true;
          lasteyeRHS = leftStickY;
          goto bailout;
        }
        goto bailout;
      }
      goto bailout;
    }

    //-----------------------------------------------
    // Left Arm move - LT + JS - CLI: Ln 0 to 100
    else if (leftTrigger) {
      if ((leftStickY != lastleftArm) && ((leftStickY >= LSTICK_DZ_Ymax) || (leftStickY <= LSTICK_DZ_Ymin))) {
        lastleftArm = leftStickY;
        value = String(map(leftStickY, -100, 100, 0, 100));
        doEvaluateSerial('L', value);
        neutral_LHS_JS_Y = false;  // not in neutral zone
        goto bailout;
      } else {  // is now in neutral zone
        if ((!neutral_LHS_JS_Y) && (leftStickY != lastleftArm)) {
          doEvaluateSerial('L', String(50));
          neutral_LHS_JS_Y = true;
          lastleftArm = leftStickY;
          goto bailout;
        }
        goto bailout;
      }
      goto bailout;
    }

    //-----------------------------------------------
    // Right Arm  move - RT + JS - CLI: Rn 0 to 100
    else if (rightTrigger) {
      if ((leftStickY != lastrightArm) && ((leftStickY >= LSTICK_DZ_Ymax) || (leftStickY <= LSTICK_DZ_Ymin))) {
        lastrightArm = leftStickY;
        value = String(map(leftStickY, -100, 100, 0, 100));
        doEvaluateSerial('R', value);
        neutral_LHS_JS_Y = false;  // not in neutral zone
        goto bailout;
      } else {  // is in now in neutral zone
        if ((!neutral_LHS_JS_Y) && (leftStickY != lastrightArm)) {
          doEvaluateSerial('R', String(50));
          neutral_LHS_JS_Y = true;
          lastrightArm = leftStickY;
          goto bailout;
        }
        goto bailout;
      }
      goto bailout;
    }

    //---------------------------------------------------------
    // Left Joy Stick X --- Head Rotation CLI: Gn 0 to
    if (leftStickX != lastleftStickX) {
      if ((leftStickX >= LSTICK_DZ_Xmax) || (leftStickX <= LSTICK_DZ_Xmin)) {
        lastleftStickX = leftStickX;
        value = String(map(leftStickX, -100, 100, 0, 100));
        doEvaluateSerial('G', value);
        neutral_LHS_JS_X = false;  // flag not in neutral zone
        goto bailout;
      } else {  // is in neutral zone or holding position
        if (!neutral_LHS_JS_X) {
          doEvaluateSerial('G', String(50));
          neutral_LHS_JS_X = true;
          lastleftStickX = leftStickX;
          goto bailout;
        }
      }
    }

    //----------------------------------------------------------------
    // Left Joy Stick Y --- Neck Bottom (up/Down) CLI: Bn 0 to 100
    if (leftStickY != lastleftStickY) {
      if ((leftStickY >= LSTICK_DZ_Ymax) || (leftStickY <= LSTICK_DZ_Ymin)) {  //outside nutural zone
        lastleftStickY = leftStickY;
        value = String(map(leftStickY, -100, 100, 100, 0));
        doEvaluateSerial('B', value);
        neutral_LHS_JS_Y = false;  // not in neutral zone
        goto bailout;
      } else {
        if (!neutral_LHS_JS_Y) {
          doEvaluateSerial('B', String(50));
          neutral_LHS_JS_Y = true;
          lastleftStickY = leftStickY;
         goto bailout;
        }
      }
    }

    // non joy stick stuff...

    //------------------------------------------------
    // LHS Eyebrow - Left Bump - CLI: Cn 0 to 100

    if (leftBumper && !leftBrowPB) {  //button down open eye brrow
      leftBrowPB = true;
      value = String(100);
      doEvaluateSerial('C', value);
      goto bailout;
    } else if (!leftBumper && leftBrowPB) {  // button up close eye brrow
      leftBrowPB = false;
      value = String(0);
      doEvaluateSerial('C', value);
      goto bailout;
    }

    //------------------------------------------------
    // RHS Eyebrow up - R bumper  - CLI: Vn 0 to 100
    if (rightBumper && !rightBrowPB) {
      rightBrowPB = true;
      value = String(100);
      doEvaluateSerial('V', value);
      goto bailout;
    } else if (!rightBumper && rightBrowPB) {
      rightBrowPB = false;
      value = String(0);
      doEvaluateSerial('V', value);
      goto bailout;
    }

    //------------------------------------------
    // Door Open/Close - Y PB - CLI: Dn 0 to 1
    if (buttonY && !doorFlag) {
      doorMode = !doorMode;
      value = String(doorMode);
      doEvaluateSerial('D', value);
      goto bailout;
    }

    //------------------------------------------------------------
    // Autonomous Servo Mode - xboxButton - CLI: M0 off, M1 = on
    if (rightStickButton && !autoModeFlag) {
      autonomousMode = !autonomousMode;
      value = String(autonomousMode);
      doEvaluateSerial('M', value);
      goto bailout;
    }

    //----------------------------------------------------------------
    // Animation number to play - menuButton + Dpad Up/Down -  CLI: An
    if (menuButton) {
      animationPBFlag = true;
      if ((animationIndex == LastAnimationIndex) && (displaytime == 0)) {  // display current index if not already
        oledAnimation(animationIndex);
      }
      if (dpadUp) {
        animationIndex++;
        if (animationIndex >= 101) { animationIndex = 100; }
        oledAnimation(animationIndex);
      }
      if (dpadDown) {
        animationIndex--;
        if (animationIndex == 255) { animationIndex = 0; }
        oledAnimation(animationIndex);
      }
      goto bailout;
    }
    if ((!menuButton && animationPBFlag) && (animationIndex != LastAnimationIndex)) {
      animationPBFlag = false;
      LastAnimationIndex = animationIndex;
      value = String(animationIndex);
      doEvaluateSerial('A', value);  // send new value
      goto bailout;
    }

    //------------------------------------------------------------------
    // Steering offset - shareButton + Dpad Up/Down -  CLI: Sn -100 to 100
    if (shareButton) {
      steeringoffsetPBFlag = true;
      if ((steeringIndex == LaststeeringIndex) && (displaytime == 0)) {  // display current index if not already
        OledSteeringOffset(steeringIndex);
      }
      if (dpadUp) {
        steeringIndex++;
        if (steeringIndex >= 101) { steeringIndex = 100; }
        OledSteeringOffset(steeringIndex);
      }
      if (dpadDown) {
        steeringIndex--;
        if (steeringIndex <= -101) { steeringIndex = -100; }
        OledSteeringOffset(steeringIndex);
      }
      goto bailout;
    }
    if ((!shareButton && steeringoffsetPBFlag) && (steeringIndex != LaststeeringIndex)) {
      steeringoffsetPBFlag = false;
      LaststeeringIndex = steeringIndex;
      value = String(steeringIndex);
      doEvaluateSerial('S', value);  // send new value
      goto bailout;
    }

    //-------------------------------------------------------
    // Motor Dead Zone - viewButton + Dpad Up/Down - CLI: On 0 to 250
    if (viewButton) {
      motorDeadZonePBFlag = true;
      if ((motorDeadZoneIndex == LastmotorDeadZoneIndex) && (displaytime == 0)) {  // display current index if not already
        OledMotorDeadZone(motorDeadZoneIndex);
      }
      if (dpadUp) {
        motorDeadZoneIndex++;
        if (motorDeadZoneIndex >= 251) { motorDeadZoneIndex = 250; }
        OledMotorDeadZone(motorDeadZoneIndex);
      }
      if (dpadDown) {
        motorDeadZoneIndex--;
        if (motorDeadZoneIndex == 255) { motorDeadZoneIndex = 0; }
        OledMotorDeadZone(motorDeadZoneIndex);
      }
      goto bailout;
    }
    if ((!viewButton && motorDeadZonePBFlag) && (motorDeadZoneIndex != LastmotorDeadZoneIndex)) {
      motorDeadZonePBFlag = false;
      LastmotorDeadZoneIndex = motorDeadZoneIndex;
      value = String(motorDeadZoneIndex);
      doEvaluateSerial('O', value);  // send new value
      goto bailout;
    }

   
    //if (leftStickButton) { Serial.println("LeftStickButton"); }
    //if (rightStickButton) { Serial.println("RightStickButton"); }

    //if (leftTrigger) {Serial.println("leftTrigger");}
    //if (rightTrigger) {Serial.println("rightTrigger");}

    //if (dpadUp) { Serial.println("dpadUp"); }
    //if (dpadDown) { Serial.println("dpadDown"); }
    //if (dpadLeft) { Serial.println("dpadLeft"); }
    //if (dpadRight) { Serial.println("dpadRight"); }

    bailout:
    BLE_Conected_Flag = true;

  } else {                    // no longer connected
    if (BLE_Conected_Flag) {  // was connected
      Serial.println("controller NOT connected");
      BLE_Conected_Flag = false;
      oledxbox(BLE_Conected_Flag);
    }
  }
}

//-------------------------
// usage: vibration(0);     // xbox vibrate use 0-4 values
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

//--------------------------
