/* * * * * * * * * * * * * * * * * * * * * * *
 * For the PCF8575 I/O Multiplexer
 *
 * Code by:  Richard Nicholson
 * Email:    richn01@msn.com
 * based on the original project from Simon Bluett ( hello@chillibasket.com )
 * Version:  1.0
 * Date:     23 July 2026
 * Copyright (C) 2020, MIT License
 * * * * * * * * * * * * * * * * * * * * * * */
 
void PCF8575_Update() {       // 100ms hit cycle, read inputs
  bool org_pd_stat = pcf.digitalRead(PIN_ORG_PB);
  bool grn_pd_stat = pcf.digitalRead(PIN_GRN_PB);
  bool red_pd_stat = pcf.digitalRead(PIN_RED_PB);
  bool wht_pd_stat = pcf.digitalRead(PIN_WHT_PB);
  bool dfp_busy_stat = pcf.digitalRead(PIN_DFP_BUSY);

  // Process Inputs - Active Low
  //--------------------------------
  // Orange PB - Play Previous
  if (!org_pd_stat) {  
    alloff();
    pcf.digitalWrite(PIN_ORG_LED, LOW);  // turn LED on 
    ledIndex = 1;
    if (!soundPlaying) {    
      if (playIndex == 1) { playIndex = 18; }
      playIndex --;
      if (playIndex == 0) { playIndex = 1; }
    }
  }
  // Green PB - Play Next
  if (!grn_pd_stat) {  
    alloff();
    pcf.digitalWrite(PIN_GRN_LED, LOW);  // turn LED on 
    ledIndex = 2;
    if (!soundPlaying) {      
      playIndex ++;
      if (playIndex >= 18) { playIndex = 1; }
      if (playIndex == 0) { playIndex = 1; }
    }
  }
  // Red PB - Autonomuus Servo Mode on/off
  if (!red_pd_stat && !autoModeFlag) {  
    alloff();
    pcf.digitalWrite(PIN_RED_LED, LOW);  // turn LED on 
    ledIndex = 3;
    autonomousMode = !autonomousMode;
    value = String(autonomousMode);
    doEvaluateSerial('M', value); // CLI: Mn 0 to 1 
  }
  // White PB - Open/close Door
  if (!wht_pd_stat && !doorFlag) {
    alloff();
    pcf.digitalWrite(PIN_WHT_LED, LOW);  // turn PB LED on 
    ledIndex = 4;
    doorMode = !doorMode;
    value = String(doorMode);
    doEvaluateSerial('D', value);   // CLI: Dn 0 to 1 
    pcf.digitalWrite(PIN_BAY_LED, doorMode);  // turn bay light on/off (active High)
  }

  // DFPlayer finished playing. Busy line gone high active low
  if (dfp_busy_stat && soundPlaying) {    
    soundPlaying = false;
    alloff();
  }  

  // outputs - Led chaser
  if (ledIndex != lastledIndex) {
    alloff();
    if (ledIndex == 0) { alloff(); }
    if (ledIndex == 1) { pcf.digitalWrite(PIN_ORG_LED, LOW); }  // turn LED on by sinking current to ground
    if (ledIndex == 2) { pcf.digitalWrite(PIN_GRN_LED, LOW); }  
    if (ledIndex == 3) { pcf.digitalWrite(PIN_RED_LED, LOW); }  
    if (ledIndex == 4) { pcf.digitalWrite(PIN_WHT_LED, LOW); }  
    lastledIndex = ledIndex;
  }
}

void alloff() {
  pcf.digitalWrite(PIN_ORG_LED, HIGH);
  pcf.digitalWrite(PIN_GRN_LED, HIGH);
  pcf.digitalWrite(PIN_RED_LED, HIGH);
  pcf.digitalWrite(PIN_WHT_LED, HIGH);
  //pcf.digitalWrite(PIN_BAY_LED, LOW); // active high
}
