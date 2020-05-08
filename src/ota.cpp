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
#include "actions.h"

void startOTA()
{
    ArduinoOTA.begin();
    delay(3000);

    while (deviceMode == OTA_MODE)
    {
        if (readButtons() > 0 || millis() - configTimer > OTA_TIMEOUT)
        {
            goToSleep();
            return;
        }
        ArduinoOTA.handle();
        delay(20);
    }
    goToSleep();
}