#include "main.h"
#include "actions.h"

void startHassRegister()
{
    if (deviceMode != HASS_REGISTER_MODE)
        return;

    const char *ha_user = json["ha_user"].as<const char *>();
    const char *ha_password = json["ha_password"].as<const char *>();
    const char *ha_broker = json["ha_broker"].as<const char *>();
    int ha_port = json["ha_port"].as<int>();
    const char *ha_prefix = json["ha_prefix"].as<const char *>();

#if CORE_DEBUG_LEVEL == 5
    Serial.println("[ha_user]: " + String(ha_user));
    Serial.println("[ha_password]: " + String(ha_password));
    Serial.println("[ha_broker]: " + String(ha_broker));
    Serial.println("[ha_port]: " + String(ha_port));
    Serial.println("[ha_prefix]: " + String(ha_prefix));
#endif

    client.setServer(ha_broker, ha_port);
    mqtt_connect(ha_user, ha_password);

    Serial.println("Sending MQTT Discovery data...");
    StaticJsonDocument<512> payload;

    String configTopic = String(ha_prefix) + "/sensor/ugo_[id]/config";
    String stateTopic = String(ha_prefix) + "/sensor/ugo_[id]/state";

    payload["uniq_id"] = "ugo_[id]";
    payload["name"] = "Ugo [id] (battery)";
    payload["stat_t"] = stateTopic;
    payload["dev_cla"] = "battery";
    payload["unit_of_meas"] = "%";
    payload["frc_upd"] = true;
    JsonObject device = payload.createNestedObject("device");
    device["ids"] = "ugo_[id]";
    device["name"] = "Ugo-ESP32 ([id])";
    device["mf"] = "https://github.com/ceoloide/Ugo-ESP32";
    device["mdl"] = "Ugo-TinyPICO";
    device["sw"] = FW_VERSION;
    JsonArray connections = device.createNestedArray("connections");
    JsonArray macInformation = connections.createNestedArray();
    macInformation.add("mac");
    macInformation.add(macToStr(mac));

    publishTopic(configTopic, payload, true);
    client.loop();
    delay(1000);
    Serial.println("Sending initial state data...");
    publishTopic(stateTopic, "[blvl]");
    client.loop();
    client.disconnect();

    Serial.println("Ugo should now be discovered by Home Assistant. Use following topic to update values:");
    Serial.println(stateTopic);
    delay(1000);
    goToSleep();
}