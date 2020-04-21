#ifndef ACTIONS_H
#define ACTIONS_H

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

bool saveConfig();

bool readConfig();

/* Battery percentage estimation, this is not very accurate but close enough */
uint8_t batteryPercentage();

void sendHttpRequest(String buttonUrl);

void printPubSubClientState();

void mqtt_connect(const char *mqtt_usr, const char *mqtt_pass);

void publishTopic(String topic, String payload);

void publishTopic(String topic, StaticJsonDocument<512> &payload);

void publishBatteryLevel();

void publishButtonData(String buttonUrl);

void handleButtonAction();

#endif