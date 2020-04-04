void startOTA() {
  ArduinoOTA.begin();
  delay(3000);
  
  while (deviceMode == OTA_MODE) {
    if (readButtons() > 0 || millis() - configTimer > OTA_TIMEOUT) {
      goToSleep();
      return;
    }
    ArduinoOTA.handle();
    delay(20);
  }
  goToSleep();
}
