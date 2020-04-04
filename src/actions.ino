int readButtons() {
  int GPIO_reason = esp_sleep_get_ext1_wakeup_status();
  int wakeup_button = (log(GPIO_reason))/log(2);

  if ((wakeup_button == button1_pin || wakeup_button == button2_pin) &&
      digitalRead(button1_pin) == HIGH && digitalRead(button2_pin) == HIGH) {
    Serial.println("Button 1 + Button 2 are HIGH");
    return 5;
  }
  if ((wakeup_button == button2_pin || wakeup_button == button3_pin) &&
      digitalRead(button2_pin) == HIGH && digitalRead(button3_pin) == HIGH) {
    Serial.println("Button 2 + Button 3 are HIGH");
    return 6;
  }
  if ((wakeup_button == button3_pin || wakeup_button == button4_pin) &&
      digitalRead(button3_pin) == HIGH && digitalRead(button4_pin) == HIGH) {
    Serial.println("Button 3 + Button 4 are HIGH");
    return 7;
  }
  if (digitalRead(button1_pin) == HIGH) {
    Serial.println("Button 1 is HIGH");
    return 1;
  } else if (digitalRead(button2_pin) == HIGH) {
    Serial.println("Button 2 is HIGH");
    return 2;
  } else if (digitalRead(button3_pin) == HIGH) {
    Serial.println("Button 3 is HIGH");
    return 3;
  } else if (digitalRead(button4_pin) == HIGH) {
    Serial.println("Button 4 is HIGH");
    return 4;
  }
  
  return 0;
}

void toggleConfigMode() {
  if (digitalRead(button1_pin) == HIGH && digitalRead(button4_pin) == HIGH) {
    int i = 0;
    while (digitalRead(button1_pin) == HIGH && digitalRead(button4_pin) == HIGH && i < 200) {
      delay(10);

      if (i > 100) {
        deviceMode = CONFIG_MODE;
        configTimer = millis(); // start counter
        return;
      }
      i++;
    }
  }
}

void toggleOTAMode() {
  if (digitalRead(button1_pin) == HIGH && digitalRead(button3_pin) == HIGH) {
    int i = 0;
    while (digitalRead(button1_pin) == HIGH && digitalRead(button3_pin) == HIGH && i < 200) {
      delay(10);

      if (i > 100) {
        deviceMode = OTA_MODE;
        otaTimer = millis(); // start counter
        return;
      }
      i++;
    }
  }
}

void goToSleep() {
  Serial.println("Going to sleep in 1 second");
  delay(1000);
  Serial.println("Starting sleep");
  Serial.flush();
     
  // To power down the DotStar for deep sleep you call this
  tp.DotStar_SetPower( false );
  delay(50);
    
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_deep_sleep_start();
}

void blinkResetReason() {
  uint32_t color;
  esp_reset_reason_t reset_reason;
  reset_reason = esp_reset_reason();

  switch(reset_reason)
  {
    case ESP_RST_UNKNOWN: color = tp.Color(255, 255, 255); break;  // white
    case ESP_RST_POWERON: color = tp.Color(0, 0, 255); break;  // blue
    case ESP_RST_SW: color = tp.Color(0, 255, 0); break;  // green (software reset via esp_restart)
    case ESP_RST_PANIC: color = tp.Color(255, 0, 0); break;  // red
    case ESP_RST_INT_WDT:
    case ESP_RST_TASK_WDT:
    case ESP_RST_WDT: color = tp.Color(255, 0, 255); break;  // purple (watchdog)
    case ESP_RST_BROWNOUT: color = tp.Color(255, 255, 0); break;  // yellow
    case ESP_RST_SDIO: color = tp.Color(150, 75, 0); break;
    default : color = tp.Color(100, 100, 100); break;
  }
  for (int i = 1; i <= 4; i++) {
    tp.DotStar_Clear();
    delay(100);
    tp.DotStar_SetPixelColor( color );
    delay(100);
  }
}


void printResetReason(){
  esp_reset_reason_t reset_reason;
  reset_reason = esp_reset_reason();

  switch(reset_reason)
  {
    case ESP_RST_UNKNOWN: Serial.println("Reset reason can not be determined."); break;
    case ESP_RST_POWERON: Serial.println("Reset due to power-on event."); break;
    case ESP_RST_EXT: Serial.println("Reset by external pin (not applicable for ESP32)"); break;
    case ESP_RST_SW: Serial.println("Software reset via esp_restart."); break;
    case ESP_RST_PANIC: Serial.println("Software reset due to exception/panic."); break;
    case ESP_RST_INT_WDT: Serial.println("Reset (software or hardware) due to interrupt watchdog."); break;
    case ESP_RST_TASK_WDT: Serial.println("Reset due to task watchdog."); break;
    case ESP_RST_WDT: Serial.println("Reset due to other watchdogs."); break;
    case ESP_RST_DEEPSLEEP: Serial.println("Reset after exiting deep sleep mode."); break;
    case ESP_RST_BROWNOUT: Serial.println("Brownout reset (software or hardware)"); break;
    case ESP_RST_SDIO: Serial.println("Reset over SDIO."); break;
    default : Serial.printf("Reset was not caused by a known reason: %d\n",reset_reason); break;
  }
}

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void printWakeupReason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

String macToStr(const uint8_t* mac) {
  String result;
  for (int i = 0; i < 6; ++i) {
    if (mac[i] < 0x10) result += "0";
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  result.toUpperCase();
  return result;
}

String macLastThreeSegments(const uint8_t* mac) {
  String result;
  for (int i = 3; i < 6; ++i) {
    if (mac[i] < 0x10) result += "0";
    result += String(mac[i], HEX);
  }
  result.toUpperCase();
  return result;
}

bool readConfig() {
  File stateFile = SPIFFS.open("/config.json");
  if (!stateFile) {
    Serial.println("Failed to read file, creating empty one.");
    json["id"] = json["pw"] = json["ip"] = json["d1"] = json["d2"] = json["gw"] = json["sn"] = json["b1"] = json["b2"] = json["b3"] = json["b4"] = json["b5"] = json["b6"] = json["b7"] = "";
    saveConfig();
  }
  DeserializationError error = deserializeJson(json, stateFile.readString());
  if (error) {
    Serial.println("Failed to read file, creating empty one.");
    json["id"] = json["pw"] = json["ip"] = json["d1"] = json["d2"] = json["gw"] = json["sn"] = json["b1"] = json["b2"] = json["b3"] = json["b4"] = json["b5"] = json["b6"] = json["b7"] = "";
    saveConfig();
  }
  stateFile.close();
  Serial.println("Reading config file:");
  serializeJson(json, Serial);
  Serial.println("");
  return true;
}

bool saveConfig() {
  File configFile = SPIFFS.open("/config.json", FILE_WRITE);
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  Serial.println("Writing config file:");
  serializeJson(json, configFile);
  serializeJson(json, Serial);
  Serial.println("");
  configFile.close();
  return true;
}

/* Battery percentage estimation, this is not very accurate but close enough */
uint8_t batteryPercentage() {
  float batteryVoltage = tp.GetBatteryVoltage();
  Serial.println("Battery voltage: " + ((String)batteryVoltage) + "V");  
  if(tp.IsChargingBattery() && batteryVoltage > 4.3f) {
    Serial.println("Battery is charging.");
    return 200;
  }
  
  if(batteryVoltage > 4.2f) return 100;
  if(batteryVoltage > 4.1f) return 90;
  if(batteryVoltage > 4.0f) return 80;
  if(batteryVoltage > 3.90f) return 70;
  if(batteryVoltage > 3.80f) return 60; // 3.8v ... 920
  if(batteryVoltage > 3.75f) return 50;
  if(batteryVoltage > 3.70f) return 40;
  if(batteryVoltage > 3.65f) return 30;
  if(batteryVoltage > 3.62f) return 20; // 3.65v ... 880
  if(batteryVoltage > 3.60f) return 10;
  return 0;
}

void sendHttpRequest(String buttonUrl) {
  int batteryPercent = batteryPercentage();
  if (batteryPercent > 100) batteryPercent = 100;

  buttonUrl.replace("[blvl]", (String)batteryPercent);
  buttonUrl.replace("[chrg]", (String)tp.GetBatteryVoltage());
  buttonUrl.replace("[mac]", macToStr(mac));

  if (buttonUrl.length() == 0 || buttonUrl == "null" || buttonUrl == NULL) {
    Serial.println("Button URL is not defined. Set it in config portal.");
    return;
  }

  std::unique_ptr<WiFiClientSecure>client(new WiFiClientSecure);
  HTTPClient http;

  if (buttonUrl.indexOf("https:") >= 0) {
    Serial.print("SSL mode...");
    http.begin(*client, buttonUrl);
  } else {
    Serial.print("HTTP mode...");
    http.begin(buttonUrl);
  }

  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.print("Request sucess, URL: ");
  } else {
    Serial.print("Request failed, URL: ");
  }
  http.end();
  Serial.println(buttonUrl);
}