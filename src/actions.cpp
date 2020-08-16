/*
Copyright 2020 Marco Massarelli

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "main.h"

// Converts separate R,G,B values to a single packed value
uint32_t packColor(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

void setLedColor(uint8_t r, uint8_t g, uint8_t b )
{
#ifdef ENABLE_PCB_LED
#ifdef ENABLE_V_0_2_PCB_LED_FIX
    digitalWrite(COMMON_ANODE_PIN, HIGH);  // Power on
    // The PWM outputs are remapped to account for the wrong resistance
    ledcWrite(RED_LED_PWM_CHANNEL, map(max(r, g), 0, 255, 0, 75));
    ledcWrite(BLUE_LED_PWM_CHANNEL, map(b, 0, 255, 0, 185));
#else
    ledcWrite(RED_LED_PWM_CHANNEL, r); 
    ledcWrite(GREEN_LED_PWM_CHANNEL, g); 
    ledcWrite(BLUE_LED_PWM_CHANNEL, b);
#endif
#endif
#ifdef ENABLE_TP_LED
    tp.DotStar_SetPixelColor(r, g, b);
#endif
    delay(LED_CHANGE_DELAY);
}

void setLedColor(uint32_t rgb)
{
    uint8_t r = (uint8_t)rgb;
    uint8_t g = (uint8_t)(rgb >>  8);
    uint8_t b = (uint8_t)(rgb >> 16);
    setLedColor(r, g, b);
}

void turnOffLed()
{
#ifdef ENABLE_PCB_LED
    setLedColor(0, 0, 0);
#ifdef ENABLE_V_0_2_PCB_LED_FIX
    digitalWrite(COMMON_ANODE_PIN, LOW);  // Power off
#endif
#endif
#ifdef ENABLE_TP_LED
    tp.DotStar_Clear();
    delay(LED_CHANGE_DELAY);
#endif
}

int readButtons()
{
    int GPIO_reason = esp_sleep_get_ext1_wakeup_status();
    int wakeup_button = (log(GPIO_reason)) / log(2);

    if ((wakeup_button == button1_pin || wakeup_button == button2_pin) &&
        digitalRead(button1_pin) == HIGH && digitalRead(button2_pin) == HIGH)
    {
        Serial.println("Button 1 + Button 2 are HIGH");
        return 5;
    }
    if ((wakeup_button == button2_pin || wakeup_button == button3_pin) &&
        digitalRead(button2_pin) == HIGH && digitalRead(button3_pin) == HIGH)
    {
        Serial.println("Button 2 + Button 3 are HIGH");
        return 6;
    }
    if ((wakeup_button == button3_pin || wakeup_button == button4_pin) &&
        digitalRead(button3_pin) == HIGH && digitalRead(button4_pin) == HIGH)
    {
        Serial.println("Button 3 + Button 4 are HIGH");
        return 7;
    }
    if (digitalRead(button1_pin) == HIGH)
    {
        Serial.println("Button 1 is HIGH");
        return 1;
    }
    else if (digitalRead(button2_pin) == HIGH)
    {
        Serial.println("Button 2 is HIGH");
        return 2;
    }
    else if (digitalRead(button3_pin) == HIGH)
    {
        Serial.println("Button 3 is HIGH");
        return 3;
    }
    else if (digitalRead(button4_pin) == HIGH)
    {
        Serial.println("Button 4 is HIGH");
        return 4;
    }

    return 0;
}

void toggleConfigMode()
{
    if (digitalRead(button1_pin) == HIGH && digitalRead(button4_pin) == HIGH)
    {
        int i = 0;
        while (digitalRead(button1_pin) == HIGH && digitalRead(button4_pin) == HIGH && i < 200)
        {
            delay(10);

            if (i > 100)
            {
                deviceMode = CONFIG_MODE;
                configTimer = millis(); // start counter
                return;
            }
            i++;
        }
    }
}

void toggleOTAMode()
{
    if (digitalRead(button1_pin) == HIGH && digitalRead(button3_pin) == HIGH)
    {
        int i = 0;
        while (digitalRead(button1_pin) == HIGH && digitalRead(button3_pin) == HIGH && i < 200)
        {
            delay(10);

            if (i > 100)
            {
                deviceMode = OTA_MODE;
                otaTimer = millis(); // start counter
                return;
            }
            i++;
        }
    }
}

void toggleHassRegisterMode()
{
    if (digitalRead(button2_pin) == HIGH && digitalRead(button4_pin) == HIGH)
    {
        int i = 0;
        while (digitalRead(button2_pin) == HIGH && digitalRead(button4_pin) == HIGH && i < 200)
        {
            delay(10);

            if (i > 100)
            {
                deviceMode = HASS_REGISTER_MODE;
                configTimer = millis(); // start counter
                return;
            }
            i++;
        }
    }
}

void goToSleep()
{
    Serial.println("Going to sleep in 1 second");
    delay(1000);
    Serial.println("Starting sleep");
    Serial.flush();

    // To power down the DotStar for deep sleep you call this
    tp.DotStar_SetPower(false);
    delay(LED_CHANGE_DELAY);

    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_deep_sleep_start();
}

void blinkResetReason()
{
    uint32_t color;
    esp_reset_reason_t reset_reason;
    reset_reason = esp_reset_reason();

    switch (reset_reason)
    {
    case ESP_RST_UNKNOWN:
        color = packColor(255, 255, 255);
        break; // white
    case ESP_RST_POWERON:
        color = packColor(0, 0, 255);
        break; // blue
    case ESP_RST_SW:
        color = packColor(0, 255, 0);
        break; // green (software reset via esp_restart)
    case ESP_RST_PANIC:
        color = packColor(255, 0, 0);
        break; // red
    case ESP_RST_INT_WDT:
    case ESP_RST_TASK_WDT:
    case ESP_RST_WDT:
        color = packColor(255, 0, 255);
        break; // purple (watchdog)
    case ESP_RST_BROWNOUT:
        color = packColor(255, 255, 0);
        break; // yellow
    case ESP_RST_SDIO:
        color = packColor(150, 75, 0);
        break;
    default:
        color = packColor(100, 100, 100);
        break;
    }
    for (int i = 1; i <= 4; i++)
    {
        turnOffLed();
        delay(100);
        setLedColor(color);
        delay(100);
    }
}

void printResetReason()
{
    esp_reset_reason_t reset_reason;
    reset_reason = esp_reset_reason();

    switch (reset_reason)
    {
    case ESP_RST_UNKNOWN:
        Serial.println("Reset reason can not be determined.");
        break;
    case ESP_RST_POWERON:
        Serial.println("Reset due to power-on event.");
        break;
    case ESP_RST_EXT:
        Serial.println("Reset by external pin (not applicable for ESP32)");
        break;
    case ESP_RST_SW:
        Serial.println("Software reset via esp_restart.");
        break;
    case ESP_RST_PANIC:
        Serial.println("Software reset due to exception/panic.");
        break;
    case ESP_RST_INT_WDT:
        Serial.println("Reset (software or hardware) due to interrupt watchdog.");
        break;
    case ESP_RST_TASK_WDT:
        Serial.println("Reset due to task watchdog.");
        break;
    case ESP_RST_WDT:
        Serial.println("Reset due to other watchdogs.");
        break;
    case ESP_RST_DEEPSLEEP:
        Serial.println("Reset after exiting deep sleep mode.");
        break;
    case ESP_RST_BROWNOUT:
        Serial.println("Brownout reset (software or hardware)");
        break;
    case ESP_RST_SDIO:
        Serial.println("Reset over SDIO.");
        break;
    default:
        Serial.printf("Reset was not caused by a known reason: %d\n", reset_reason);
        break;
    }
}

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void printWakeupReason()
{
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        Serial.println("Wakeup caused by external signal using RTC_IO");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        Serial.println("Wakeup caused by external signal using RTC_CNTL");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        Serial.println("Wakeup caused by timer");
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        Serial.println("Wakeup caused by touchpad");
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        Serial.println("Wakeup caused by ULP program");
        break;
    default:
        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
        break;
    }
}

String macToStr(const uint8_t *mac)
{
    String result;
    for (int i = 0; i < 6; ++i)
    {
        if (mac[i] < 0x10)
            result += "0";
        result += String(mac[i], 16);
        if (i < 5)
            result += ':';
    }
    result.toUpperCase();
    return result;
}

String macLastThreeSegments(const uint8_t *mac)
{
    String result;
    for (int i = 3; i < 6; ++i)
    {
        if (mac[i] < 0x10)
            result += "0";
        result += String(mac[i], HEX);
    }
    result.toUpperCase();
    return result;
}

void updateConfigWithDefaults()
{
    Serial.println("Updating the config with defaults.");
    // WiFi defaults
    json["id"] = json["pw"] = json["ip"] = json["d1"] = json["d2"] = "";
    json["gw"] = json["sn"] = "";
    // Button defaults
    json["b1"] = json["b2"] = json["b3"] = "";
    json["b4"] = json["b5"] = json["b6"] = json["b7"] = "";
    // Home Assistant integration defaults
    json["ha_enabled"] = false;
    json["ha_user"] = json["ha_password"] = "";
    json["ha_broker"] = "homeassistant.local";
    json["ha_port"] = 1883;
    json["ha_prefix"] = "homeassistant";
}

bool saveConfig()
{
    File configFile = SPIFFS.open("/config.json", FILE_WRITE);
    if (!configFile)
    {
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

bool readConfig()
{
    File stateFile = SPIFFS.open("/config.json");
    if (!stateFile)
    {
        Serial.println("Failed to read file.");
        updateConfigWithDefaults();
        saveConfig();
    }
    DeserializationError error = deserializeJson(json, stateFile.readString());
    if (error)
    {
        Serial.println("Failed to read file.");
        updateConfigWithDefaults();
        saveConfig();
    }
    stateFile.close();
    Serial.println("Reading config file:");
    serializeJson(json, Serial);
    Serial.println("");
    return true;
}

/* Battery percentage estimation, this is not very accurate but close enough */
uint8_t batteryPercentage()
{
    float batteryVoltage = tp.GetBatteryVoltage();
    //  Serial.println("Battery voltage: " + ((String)batteryVoltage) + "V");
    if (tp.IsChargingBattery() && batteryVoltage > 4.3f)
    {
        //    Serial.println("Battery is charging.");
        return 200;
    }

    // The values below are a rough estimate calibrated for my own battery
    if (batteryVoltage > 4.2f)
        return 100;
    if (batteryVoltage > 4.1f)
        return 90;
    if (batteryVoltage > 3.99f)
        return 80;
    if (batteryVoltage > 3.94f)
        return 70;
    if (batteryVoltage > 3.92f)
        return 60;
    if (batteryVoltage > 3.88f)
        return 50;
    if (batteryVoltage > 3.85f)
        return 40;
    if (batteryVoltage > 3.82f)
        return 30;
    if (batteryVoltage > 3.79f)
        return 20;
    if (batteryVoltage > 3.71f)
        return 10;
    return 0;
}

void sendHttpRequest(String buttonUri)
{
    int batteryPercent = batteryPercentage();
    if (batteryPercent > 100)
        batteryPercent = 100;

    buttonUri.replace("[blvl]", (String)batteryPercent);
    buttonUri.replace("[chrg]", (String)tp.GetBatteryVoltage());
    buttonUri.replace("[mac]", macToStr(mac));

    if (buttonUri.length() == 0 || buttonUri == "null" || buttonUri == NULL)
    {
        Serial.println("Button URL is not defined. Set it in config portal.");
        return;
    }

    std::unique_ptr<WiFiClientSecure> client(new WiFiClientSecure);
    HTTPClient http;

    if (buttonUri.indexOf("https:") >= 0)
    {
        Serial.print("SSL mode...");
        http.begin(*client, buttonUri);
    }
    else
    {
        Serial.print("HTTP mode...");
        http.begin(buttonUri);
    }

    int httpCode = http.GET();

    if (httpCode > 0)
    {
        Serial.print("Request sucess, URL: ");
    }
    else
    {
        Serial.print("Request failed, URL: ");
    }
    http.end();
    Serial.println(buttonUri);
}

void printPubSubClientState(PubSubClient &mqttClient)
{
    switch (mqttClient.state())
    {
    case MQTT_CONNECTION_TIMEOUT:
        Serial.println("the server didn't respond within the keepalive time");
        break;
    case MQTT_CONNECTION_LOST:
        Serial.println("the network connection was broken");
        break;
    case MQTT_CONNECT_FAILED:
        Serial.println("the network connection failed");
        break;
    case MQTT_DISCONNECTED:
        Serial.println("the client is disconnected cleanly");
        break;
    case MQTT_CONNECTED:
        Serial.println("the client is connected");
        break;
    case MQTT_CONNECT_BAD_PROTOCOL:
        Serial.println("the server doesn't support the requested version of MQTT");
        break;
    case MQTT_CONNECT_BAD_CLIENT_ID:
        Serial.println("the server rejected the client identifier");
        break;
    case MQTT_CONNECT_UNAVAILABLE:
        Serial.println("the server was unable to accept the connection");
        break;
    case MQTT_CONNECT_BAD_CREDENTIALS:
        Serial.println("the username/password were rejected");
        break;
    case MQTT_CONNECT_UNAUTHORIZED:
        Serial.println("the client was not authorized to connect");
        break;
    default:
        Serial.println("unknown state!");
        break;
    }
}

bool mqtt_connect(const char *username, const char *password, PubSubClient &mqttClient)
{
    if (mqttClient.connected())
    {
        mqttClient.disconnect();
    }
    String mqttClientId = "ugo_" + macLastThreeSegments(mac);
    Serial.println("MQTT Client ID: " + mqttClientId);
    int i = 0;
    while (!mqttClient.connected() && i < 5)
    {
        Serial.println("Attempting MQTT connection...");
        if (username[0] != '\0' && password[0] != '\0')
        {
            if (mqttClient.connect(mqttClientId.c_str(), username, password))
            {
                Serial.println("MQTT connected using credentials.");
                return true;
            }
        }
        else
        {
            if (mqttClient.connect(mqttClientId.c_str()))
            {
                Serial.println("MQTT connected anonymously.");
                return true;
            }
        }
        Serial.print("MQTT connection attempt failed: ");
        printPubSubClientState(mqttClient);
        // The following failure states should not be retried
        if (mqttClient.state() == MQTT_CONNECT_BAD_PROTOCOL ||
            mqttClient.state() == MQTT_CONNECT_BAD_CLIENT_ID ||
            mqttClient.state() == MQTT_CONNECT_BAD_CREDENTIALS ||
            mqttClient.state() == MQTT_CONNECT_UNAUTHORIZED)
        {
            return false;
        }
        ++i;
        delay(10);
    }
    return false;
}

void replacePlaceholders(String &original)
{
    original.replace("[id]", macLastThreeSegments(mac));
    original.replace("[blvl]", String(batteryPercentage()));
    original.replace("[chrg]", String(tp.GetBatteryVoltage()));
    original.replace("[mac]", macToStr(mac));
    original.replace("[btn]", String(button));
}

bool publishTopic(String topic, String payload, bool retained, PubSubClient &mqttClient)
{
    Serial.println("Original topic: " + topic);
    replacePlaceholders(topic);
    Serial.println("Compiled topic: " + topic);

    Serial.println("Original payload: " + payload);
    replacePlaceholders(payload);
    Serial.println("Compiled payload: " + payload);

    return mqttClient.publish(topic.c_str(), payload.c_str(), retained);
}

bool publishTopic(String topic, String payload, PubSubClient &mqttClient)
{
    bool retained = false;
    return publishTopic(topic, payload, retained, mqttClient);
}

bool publishTopic(String topic, StaticJsonDocument<512> &payload, bool retained, PubSubClient &mqttClient)
{
    char serializedPayload[512];
    serializeJson(payload, serializedPayload);
    Serial.println("Serialized payload" + String(serializedPayload));
    return publishTopic(topic, String(serializedPayload), retained, mqttClient);
}

bool publishTopic(String topic, StaticJsonDocument<512> &payload, PubSubClient &mqttClient)
{
    bool retained = false;
    return publishTopic(topic, payload, retained, mqttClient);
}

void publishDeviceState()
{
    const char *ha_user = json["ha_user"].as<const char *>();
    const char *ha_password = json["ha_password"].as<const char *>();
    const char *ha_broker = json["ha_broker"].as<const char *>();
    int ha_port = json["ha_port"].as<int>();
    const char *ha_prefix = json["ha_prefix"].as<const char *>();
    String stateTopic = String(ha_prefix) + "/sensor/ugo_[id]/state";

#if CORE_DEBUG_LEVEL == 5
    Serial.println("[ha_user]: " + String(ha_user));
    Serial.println("[ha_password]: " + String(ha_password));
    Serial.println("[ha_broker]: " + String(ha_broker));
    Serial.println("[ha_port]: " + String(ha_port));
    Serial.println("[ha_prefix]: " + String(ha_prefix));
    Serial.println("[stateTopic]: " + stateTopic);
#endif

    PubSubClient mqttClient;
    if (ha_port == 1883 || ha_port == 1884)
    {
        Serial.println("Using a non-secure client.");
        mqttClient = mqttClientInsecure;
    }
    else if (ha_port == 8883 || ha_port == 8884)
    {
        Serial.println("Using a secure client.");
        mqttClient = mqttClientSecure;
    }

    mqttClient.setServer(ha_broker, ha_port);
    if(mqtt_connect(ha_user, ha_password, mqttClient)) {
        String statePayload = "{\"battery\":[blvl],\"voltage\":[chrg],\"button\":[btn]}";
        Serial.println("Sending device state data...");
        for (int i = 0; i < MQTT_PUBLISH_TRIES && !publishTopic(stateTopic, statePayload, mqttClient); i++)
        {
            Serial.println("Failed to publish message.");
            Serial.print("Client state: ");
            printPubSubClientState(mqttClient);
            // Check if we are still connected
            if (mqttClient.state() != MQTT_CONNECTED)
            {
                mqtt_connect(ha_user, ha_password, mqttClient);
            }
            mqttClient.loop();
        }
        mqttClient.disconnect();
    }
}

void publishButtonData(String buttonUri)
{
    // The user can specify a MQTT URI as follows:
    //   mqtt[s]://[username]:[password]@[address]:[port]/[topic]?[payload]

    // Expected: mqtt[s]
    String protocol = buttonUri.substring(0, buttonUri.indexOf(":"));
    // Expected: [username]:[password]@[address]:[port]/[topic]?[payload]
    String uriWithoutProtocol = buttonUri.substring(protocol.length() + 3);
    // Expected: [username]:[password]@[address]:[port]
    String uriAddress = uriWithoutProtocol.substring(0, uriWithoutProtocol.indexOf("/"));
    // Expected: [topic]?[payload]
    String uriPath = uriWithoutProtocol.substring(uriWithoutProtocol.indexOf("/") + 1);

    int credentialsSeparatorIndex = uriAddress.indexOf("@");
    int portSeparatorIndex = uriAddress.indexOf(":", credentialsSeparatorIndex + 1);
    int usernameSeparatorIndex = min(uriAddress.indexOf(":"), uriAddress.indexOf("@"));
    int payloadSeparatorIndex = uriPath.indexOf("?");

    PubSubClient mqttClient;
    if (protocol.equals("mqtt"))
    {
        Serial.println("Using a non-secure client.");
        mqttClient = mqttClientInsecure;
    }
    else if (protocol.equals("mqtts"))
    {
        Serial.println("Using a secure client.");
        mqttClient = mqttClientSecure;
    }

    String brokerAddress = uriAddress.substring(credentialsSeparatorIndex + 1, portSeparatorIndex);
    int brokerPort;

    if (portSeparatorIndex >= 0)
    {
        brokerPort = uriAddress.substring(portSeparatorIndex + 1).toInt();
    }
    else
    {
        // No port was specified, defaulting to 1883 for mqtt and 8883 for mqtts
        if (protocol.equals("mqtt"))
        {
            brokerPort = 1883;
        }
        else
        {
            brokerPort = 8883;
        }
    }

    String username = "";
    String password = "";

    if (credentialsSeparatorIndex >= 0)
    {
        if (usernameSeparatorIndex < credentialsSeparatorIndex)
        {
            username = uriAddress.substring(0, usernameSeparatorIndex);
            password = uriAddress.substring(usernameSeparatorIndex + 1, credentialsSeparatorIndex);
        }
        else
        {
            username = uriAddress.substring(0, credentialsSeparatorIndex);
        }
    }

    String topic = uriPath;
    String payload = "";

    if (payloadSeparatorIndex >= 0)
    {
        topic = uriPath.substring(0, payloadSeparatorIndex);
        payload = uriPath.substring(payloadSeparatorIndex + 1);
    }

#if CORE_DEBUG_LEVEL == 5
    Serial.println("[buttonUri]: " + buttonUri);
    Serial.println("[protocol]: " + protocol);
    Serial.println("[uriWithoutProtocol]: " + uriWithoutProtocol);
    Serial.println("[uriAddress]: " + uriAddress);
    Serial.println("[uriPath]: " + uriPath);
    Serial.println("[credentialsSeparatorIndex]: " + String(credentialsSeparatorIndex));
    Serial.println("[portSeparatorIndex]: " + String(portSeparatorIndex));
    Serial.println("[usernameSeparatorIndex]: " + String(usernameSeparatorIndex));
    Serial.println("[payloadSeparatorIndex]: " + String(payloadSeparatorIndex));
#endif
#if CORE_DEBUG_LEVEL >= 4
    Serial.println("URI: " + buttonUri);
    Serial.println("Broker address: " + brokerAddress);
    Serial.println("Broker port: " + String(brokerPort));
    Serial.println("Topic: " + topic);
    Serial.println("Payload: " + payload);
    Serial.println("MQTT User: " + username);
    Serial.println("MQTT Pass: " + password);
#endif

    mqttClient.setServer(brokerAddress.c_str(), brokerPort);
    if(mqtt_connect(username.c_str(), password.c_str(), mqttClient)) {
        Serial.println("Publishing button message...");
        for (int i = 0; i < MQTT_PUBLISH_TRIES && !publishTopic(topic, payload, mqttClient); i++)
        {
            Serial.println("Failed to publish message.");
            Serial.print("Client state: ");
            printPubSubClientState(mqttClient);
            // Check if we are still connected
            if (mqttClient.state() != MQTT_CONNECTED)
            {
                mqtt_connect(username.c_str(), password.c_str(), mqttClient);
            }
            mqttClient.loop();
        }
        mqttClient.disconnect();
    }
}

void handleButtonAction()
{
    if (button < 1 || button > 7)
    {
        Serial.println("Unknown button...");
        return;
    }
    Serial.println("Button " + String(button) + " was pressed!");
    String buttonUri = json["b" + String(button)].as<String>();
    if (buttonUri.indexOf("http") >= 0)
    {
        sendHttpRequest(buttonUri);
    }
    else if (buttonUri.indexOf("mqtt") >= 0)
    {
        publishButtonData(buttonUri);
    }
}