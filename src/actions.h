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

void mqtt_connect(const char *mqtt_usr, const char *mqtt_pass, PubSubClient &mqttClient);

void publishTopic(String topic, String payload, bool retained, PubSubClient &mqttClient);

void publishTopic(String topic, String payload, PubSubClient &mqttClient);

void publishTopic(String topic, StaticJsonDocument<512> &payload, bool retained, PubSubClient &mqttClient);

void publishTopic(String topic, StaticJsonDocument<512> &payload, PubSubClient &mqttClient);

void publishDeviceState();

void publishButtonData(String buttonUrl);

void handleButtonAction();