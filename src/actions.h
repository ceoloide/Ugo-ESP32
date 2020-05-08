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

int readButtons();

void toggleConfigMode();

void toggleOTAMode();

void toggleHassRegisterMode();

void goToSleep();

void blinkResetReason();

void printResetReason();

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void printWakeupReason();

String macToStr(const uint8_t *mac);

String macLastThreeSegments(const uint8_t *mac);

void updateConfigWithDefaults();

bool saveConfig();

bool readConfig();

/* Battery percentage estimation, this is not very accurate but close enough */
uint8_t batteryPercentage();

void sendHttpRequest(String buttonUrl);

void printPubSubClientState(PubSubClient &mqttClient);

bool mqtt_connect(const char *mqtt_usr, const char *mqtt_pass, PubSubClient &mqttClient);

bool publishTopic(String topic, String payload, bool retained, PubSubClient &mqttClient);

bool publishTopic(String topic, String payload, PubSubClient &mqttClient);

bool publishTopic(String topic, StaticJsonDocument<512> &payload, bool retained, PubSubClient &mqttClient);

bool publishTopic(String topic, StaticJsonDocument<512> &payload, PubSubClient &mqttClient);

void publishDeviceState();

void publishButtonData(String buttonUrl);

void handleButtonAction();