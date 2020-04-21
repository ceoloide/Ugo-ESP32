#ifndef CONFIG_PORTAL_H
#define CONFIG_PORTAL_H

String processor(const String &var);

void handleCaptivePortal(AsyncWebServerRequest *request);

void handleRoot(AsyncWebServerRequest *request);

void notFound(AsyncWebServerRequest *request);

void handleReset(AsyncWebServerRequest *request);

void startConfigPortal();

#endif