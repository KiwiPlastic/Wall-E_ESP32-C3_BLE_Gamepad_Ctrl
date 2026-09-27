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
 
// 100ms hit cycle, read inputs 
void PCF8575_Update() {       
 
  bool red_pd_stat = pcf.digitalRead(PIN_RED_PB);
  bool grn_pd_stat = pcf.digitalRead(PIN_GRN_PB);
  bool org_pd_stat = pcf.digitalRead(PIN_ORG_PB);
  bool wht_pd_stat = pcf.digitalRead(PIN_WHT_PB);
  bool dfp_busy_stat = pcf.digitalRead(PIN_DFP_BUSY);

  // Process Inputs - Active Low
  //--------------------------------
  
  // Red PB - Play Previous
  if (!red_pd_stat && !autoModeFlag) {  
    alloff();                             // turn leds off
    pcf.digitalWrite(PIN_RED_LED, LOW);  // turn LED on 
    Serial.println("Play Previous");
    if (!soundPlaying) {   
      playIndex --;
      //if (playIndex == 1) { playIndex = 18; }
      if (playIndex <= 0) { playIndex = 1; }
      OledPlayFile(playIndex); 
      //playSoundTrigger = true;
    }
  }

  // Green PB - Play Current
  if (!grn_pd_stat) {  
    alloff();
    pcf.digitalWrite(PIN_GRN_LED, LOW);  // turn LED on 
    Serial.println("Play Current");
    if (!soundPlaying) {    
      OledPlayFile(playIndex);   
      playSoundTrigger = true;
    }
  }
  
  // Orange PB - Play Next
  if (!org_pd_stat) {
    alloff();
    pcf.digitalWrite(PIN_ORG_LED, LOW);  // turn LED on 
    Serial.println("Play Next");
    if (!soundPlaying) {
      playIndex ++;
      if (playIndex >= playIndexTotal) { playIndex = playIndexTotal; }
      if (playIndex == 0) { playIndex = playIndexTotal; }
      OledPlayFile(playIndex); 
      //playSoundTrigger = true;
    }
  }
  
  // White PB - Open/close Door
  if (!wht_pd_stat && !doorFlag) {
    alloff();
    pcf.digitalWrite(PIN_WHT_LED, LOW);  // turn PB LED on 
    ledIndex = 4;
    pcf.digitalWrite(PIN_BAY_LED, doorMode);  // turn bay light on/off (active High)
    doorMode = !doorMode;
    value = String(doorMode);
    doEvaluateSerial('D', value);   // CLI: Dn 0 to 1 
  }

  // DFPlayer finished playing. Busy line gone high active low
  if (dfp_busy_stat && soundPlaying) {    
    soundPlaying = false;
    Serial.println ("DFP Busy signal - OFF");
    alloff();
  }  
}

void alloff() {
  pcf.digitalWrite(PIN_ORG_LED, HIGH);
  pcf.digitalWrite(PIN_GRN_LED, HIGH);
  pcf.digitalWrite(PIN_RED_LED, HIGH);
  pcf.digitalWrite(PIN_WHT_LED, HIGH);
  //pcf.digitalWrite(PIN_BAY_LED, LOW); // active high
}
