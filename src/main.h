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

#pragma once

#include <Arduino.h>
#include <TinyPICO.h>

// APA102 Dotstar
#define DOTSTAR_PWR 13
#define DOTSTAR_DATA 2
#define DOTSTAR_CLK 12

// Battery
#define BAT_CHARGE 34
#define BAT_VOLTAGE 35

#ifdef ESP32
#include <WiFiClientSecure.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#else
#error Platform not supported
#endif
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <PubSubClient.h>

#define OTA_NAME "Ugo_" // Last 6 MAC address characters will be appended at the end of the OTA name, "Ugo_XXXXXX" by default
#define AP_NAME "Ugo_"  // Last 6 MAC address characters will be appended at the end of the AP name, "Ugo_XXXXXX" by default

// Button PINs
#define button1_pin 25
#define button2_pin 4
#define button3_pin 27
#define button4_pin 26

// On the Ugo-ESP32 (TinyPICO) v0.2 PCB, the RGB led has been rotated 180 degrees
// This means the Green LED pin is now the common anode, the 3V3 pin is the Green LED, which is now always off
// And the Red and Blue pins are reversed. It's also not possible to turn on both Red and Blue leds at the same time.
#ifdef ENABLE_V_0_2_PCB_LED_FIX
#define RED_LED_PIN 18
#define COMMON_ANODE_PIN 19
#define BLUE_LED_PIN 23
#else
#define RED_LED_PIN 23
#define GREEN_LED_PIN 19
#define BLUE_LED_PIN 18
#endif

#define RED_LED_PWM_CHANNEL 0
#define GREEN_LED_PWM_CHANNEL 1
#define BLUE_LED_PWM_CHANNEL 2

#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8

#if !defined(RGB_COMMON_ANODE) && !defined(RGB_COMMON_CATHODE)
// Defaulting to RGB_COMMON_ANODE since that's the RGB LED on the Ugo-ESP32 (TinyPICO) PCB v0.2
#define RGB_COMMON_ANODE
#endif

#ifdef RGB_COMMON_CATHODE
#define RGB_LED_ON HIGH
#define RGB_LED_OFF LOW
#define INVERT_LED_PWM_SENSE 0
#endif

#if (defined(RGB_COMMON_ANODE) && defined(RGB_COMMON_CATHODE)) || defined(RGB_COMMON_ANODE)
#define RGB_LED_ON LOW
#define RGB_LED_OFF HIGH
#define INVERT_LED_PWM_SENSE 1
#endif

#define LED_CHANGE_DELAY 50  // Amount of time (ms) to wait after LED color / status change 

#define OTA_TIMEOUT 300000    // 5 minutes
#define CONFIG_TIMEOUT 300000 // 5 minutes

// DO NOT CHANGE DEFINES BELOW
#define NORMAL_MODE 0
#define OTA_MODE 1
#define CONFIG_MODE 2
#define HASS_REGISTER_MODE 3

// If the max message size is too small, throw an error at compile time. See PubSubClient.cpp line 359
#if MQTT_MAX_PACKET_SIZE < 512
#error "MQTT_MAX_PACKET_SIZE is too small in libraries/PubSubClient/src/PubSubClient.h, increase it to 512"
#endif

#define MQTT_PUBLISH_TRIES 3

// Initialise the TinyPICO library
extern TinyPICO tp;

extern uint8_t deviceMode;

extern uint8_t button;

extern bool otaModeStarted;
extern volatile bool ledState;

// TIMERS
extern unsigned long otaMillis, ledMillis, configTimer, otaTimer;

extern byte mac[6];
extern AsyncWebServer server;

extern byte dnsPort;
extern DNSServer dnsServer;

extern WiFiClient espClientInsecure;
extern PubSubClient mqttClientInsecure;
extern WiFiClientSecure espClientSecure;
extern PubSubClient mqttClientSecure;

extern DynamicJsonDocument json; // config buffer

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED false // false

//#define BUTTON_PIN_BITMASK 0x300000000 // 2^33 in hex
//                                  10 // Pin  4
//                                4000 // Pin 14
//                                8000 // Pin 15
//                             2000000 // Pin 25
//                             4000000 // Pin 26
//                             8000000 // Pin 27
//                           100000000 // Pin 32
//                           200000000 // Pin 33
//#define BUTTON_PIN_BITMASK 0x00800C010 // 4 + 14 + 15 + 27
//#define BUTTON_PIN_BITMASK 0x308000010 // 4 + 33 + 32 + 27
#define BUTTON_PIN_BITMASK 0x00E000010 // 4 + 25 + 26 + 27

void setup();

void loop();