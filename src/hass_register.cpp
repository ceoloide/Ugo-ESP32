#include "main.h"
#include "actions.h"

void startHassRegister()
{
    if (deviceMode != HASS_REGISTER_MODE)
        return;

    client.setServer(IPAddress(192, 168, 1, 202), 1883);
    const char *mqtt_usr = json["mqttuser"].as<const char *>();
    const char *mqtt_pass = json["mqttpass"].as<const char *>();
    mqtt_connect(mqtt_usr, mqtt_pass);

    Serial.println("Sending MQTT Discovery data...");
    StaticJsonDocument<512> payload;

    String configTopic = "homeassistant/sensor/ugo_[id]/config";
    String stateTopic = "homeassistant/sensor/ugo_[id]/state";

    payload["uniq_id"] = "ugo_[id]";
    payload["name"] = "Ugo [id] (battery)";
    payload["stat_t"] = stateTopic;
    payload["dev_cla"] = "battery";
    payload["unit_of_meas"] = "%";
    JsonObject device = payload.createNestedObject("device");
    device["ids"] = "ugo_[id]";
    device["name"] = "Ugo-ESP32 ([id])";
    device["mf"] = "https://github.com/ceoloide/Ugo-ESP32";
    device["mdl"] = "Ugo-TinyPICO";
    device["sw"] = FW_VERSION;

    publishTopic(configTopic, payload, true);
    client.loop();
    Serial.println("Sending state data...");
    publishTopic(stateTopic, String(batteryPercentage()));
    client.loop();
    client.disconnect();

    Serial.println("Ugo should now be discovered by Home Assistant. Use following topic to update values:");
    Serial.println(stateTopic);
    delay(1000);
    goToSleep();
}