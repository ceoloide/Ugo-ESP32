#pragma once

String processor(const String &var);

void handleCaptivePortal(AsyncWebServerRequest *request);

void handleRoot(AsyncWebServerRequest *request);

void notFound(AsyncWebServerRequest *request);

void handleReset(AsyncWebServerRequest *request);

void startConfigPortal();