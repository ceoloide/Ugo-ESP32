void startConfigPortal() {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  IPAddress ap_ip(10, 10, 10, 1);
  WiFi.softAPConfig(ap_ip, ap_ip, IPAddress(255, 255, 255, 0));
  String ap_name = AP_NAME + macLastThreeSegments(mac);
  WiFi.softAP(ap_name.c_str());
  IPAddress ip = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.println(ip);

  if (MDNS.begin("hugo")) {
    Serial.println("mDNS responder is now active");
  }
  
  // Modify the TTL associated  with the domain name (in seconds)
  dnsServer.setTTL(300); // 5 minutes
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(DNS_PORT, "*", ip);
  
  server.on("/", HTTP_ANY, handleRoot);
  server.on("/common.css", HTTP_ANY, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/common.css", "text/css");
  });

  // Handle known captive portal URLs, returning a 302
  server.on("/mobile/status.php", HTTP_ANY, handleCaptivePortal); // Android 8.0 
  server.on("/generate_204", HTTP_ANY, handleCaptivePortal); // Android
  server.on("/gen_204", HTTP_ANY, handleCaptivePortal); // Android
  server.on("/ncsi.txt", HTTP_ANY, handleCaptivePortal); // Windows
  server.on("/hotspot-detect.html", HTTP_ANY, handleCaptivePortal); // iOS/OSX
  server.on("/hotspotdetect.html", HTTP_ANY, handleCaptivePortal); // iOS/OSX
  server.on("/library/test/success.html", HTTP_ANY, handleCaptivePortal); // iOS
  server.on("/success.txt", HTTP_ANY, handleCaptivePortal); // OSX

  server.on("/reset", HTTP_ANY, handleReset);
  
  server.onNotFound(notFound);

  server.begin();
  Serial.println("HTTP server starting...");

  // Looping until any button is pressed, mode changes, or timeout is 
  // reached
  delay(3000);
  while (deviceMode == CONFIG_MODE) {
    if (readButtons() > 0 || millis() - configTimer > CONFIG_TIMEOUT) {
      Serial.println("Stopping config mode and going to sleep.");
      dnsServer.stop();
      server.end();
      goToSleep();
      return;
    }
    dnsServer.processNextRequest();
  }
}

void handleCaptivePortal(AsyncWebServerRequest *request) {
  Serial.println("Captive Portal - Host: " + request->host());
  Serial.println("               - URL: " + request->url());

  String html = "<html>";
  html += "<head>";
  html += "<title>Redirecting to the configuration page.</title>";
  html += "<meta http-equiv='refresh' content='0; url=http://10.10.10.1/'>";
  html += "</head>";
  html += "<body>";
  html += "<p>If page does not refresh, click <a href='http://10.10.10.1'>here</a> to configure.</p>";
  html += "</body>";
  html += "</html>";

  request->send(302, "text/html", html);
}

void handleRoot(AsyncWebServerRequest *request) {
  if (request->args()) {
    char *keys[] = {
      "id",
      "pw",
      "ip",
      "d1",
      "d2",
      "gw",
      "sn",
      "b1",
      "b2",
      "b3",
      "b4",
      "b5",
      "b6",
      "b7",
      "mqttuser",
      "mqttpass"
    };
    for (int i = 0; i < 16; i++) {
      if (request->hasArg(keys[i])) {
        json[keys[i]] = request->arg(keys[i]);
        Serial.print("Received arg=");
        Serial.print(keys[i]);
        Serial.print(": ");
        Serial.println(request->arg(keys[i]));
      }
    }    
    saveConfig();
    Serial.println("Saved new config. Rebooting...");
    request->redirect("http://10.10.10.1/reset");
    return;
  }
  
  request->send(SPIFFS, "/webconfig.html", "text/html", false, processor);
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void handleReset(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/reset.html", "text/html", false, processor);
  Serial.print("Restarting in 2 seconds.");
  goToSleep();
}

String processor(const String& var)
{
  int batteryPercent = batteryPercentage();
  if(var == "BATTERY_CLASS") {
    return ((batteryPercent > 100) ? "charging" : (String)batteryPercent);
  } else if(var == "BATTERY_CHARGING_OR_PERCENT_HTML") {
    return ((batteryPercent > 100) ? "Charging" : (String)batteryPercent + "&percnt;");
  } else if(var == "BATTERY_VOLTAGE") {
    return ((String)tp.GetBatteryVoltage());
  } else if(var == "FIRMWARE_VERSION") {
    return FW_VERSION;
  } else if(var == "ID") {
    return json["id"].as<const char*>();
  } else if(var == "PW") {
    return json["pw"].as<const char*>();
  } else if(var == "IP") {
    return json["ip"].as<const char*>();
  } else if(var == "GW") {
    return json["gw"].as<const char*>();
  } else if(var == "SN") {
    return json["sn"].as<const char*>();
  } else if(var == "D1") {
    return json["d1"].as<const char*>();
  } else if(var == "D2") {
    return json["d2"].as<const char*>();
  } else if(var == "B1") {
    return json["b1"].as<const char*>();
  } else if(var == "B2") {
    return json["b2"].as<const char*>();
  } else if(var == "B3") {
    return json["b3"].as<const char*>();
  } else if(var == "B4") {
    return json["b4"].as<const char*>();
  } else if(var == "B5") {
    return json["b5"].as<const char*>();
  } else if(var == "B6") {
    return json["b6"].as<const char*>();
  } else if(var == "B7") {
    return json["b7"].as<const char*>();
  } else if(var == "MQTTUSER") {
    return json["mqttuser"].as<const char*>();
  } else if(var == "MQTTPASS") {
    return json["mqttpass"].as<const char*>();
  }
  
  // Return empty string as a default
  return String();
}
