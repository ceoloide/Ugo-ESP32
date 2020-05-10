# Ugo-ESP32

A 4-button Wi-Fi connected remote, based on the original [Hugo-ESP8266 concept by mcer12](https://github.com/mcer12/Hugo-ESP8266) (see picture below).

![Hugo-ESP8266 concept by mcer12](https://raw.githubusercontent.com/mcer12/Hugo-ESP8266/master/Images/hugo_numbered.png)

## Features

-   4 physical buttons that can trigger up to 7 different actions, one for each button and three for 2-button combinations (B1+B2, B2+B3, B3+B4).
-   Support for Home Assistant auto-discovery, optionally sending device state (button, battery percentage, voltage) after every button push.
-   Support for HTTPS and MQTTS endpoints, although certificate validation is planned but not yet supported.
-   Web UI to configure WiFi and button actions.

## TinyPICO

![Image of a TinyPICO](https://images.squarespace-cdn.com/content/v1/5c85d89877b903606126e6df/1554765183744-8257OPZRW59FK1BKT7MG/ke17ZwdGBToddI8pDm48kPubfIArWU6SqtYLllvlJnd7gQa3H78H3Y0txjaiv_0fDoOvxcdMmMKkDsyUqMSsMWxHk725yiiHCCLfrh8O1z4YTzHvnKhyp6Da-NYroOW3ZGjoBKy3azqku80C789l0sofvP-RiTb638-KOMjny0u3C70MUIFojZm1DzSb_fNIUUjRX0gf4zJhWpz755r9bA/TinyPICO_Hero_Black.jpg?format=250w)

The code currently depends on a [TinyPICO](https://tinypico.com), since I'm using this board for development. TinyPICO has been selected for its very good low-power draw on deep sleep. Check out Unexpected Maker's [Discord server](https://discord.gg/83Nr7rz) for updates and questions on TinyPICO or this project.

### Ugo-ESP32 TinyPICO Shield

The hardware folder contains the PCB and schematics for a TinyPICO shield version of the Ugo-ESP32, featuring similar overall dimensions (60 mm width, 32 mm height) and button layout as the [Philips Hue Dimmer Switch](https://www2.meethue.com/en-us/p/hue-dimmer-switch/046677473372), as well as an RGB led for status notifications.

The board houses the TinyPICO on the back, and can comfortably fit a 60x30x5mm (503060 / 053060) LiPo battery, usually available with a 1300mAh capacity. Early and untested estimates indicate a battery life of roughly 1 year with up to 2 button presses per day.

![PCB front](https://raw.githubusercontent.com/ceoloide/Ugo-ESP32/master/hardware/Ugo-ESP32%20(TinyPICO)/PCB%20Render%20(Front).svg)

![PCB back](https://raw.githubusercontent.com/ceoloide/Ugo-ESP32/master/hardware/Ugo-ESP32%20(TinyPICO)/PCB%20Render%20(Back).svg)

## Credits

A huge thanks to:
- [@mcer12](https://github.com/mcer12) for the original [Hugo-ESP8266 concept](https://github.com/mcer12/Hugo-ESP8266)

Shoutout to everyone working on the libraries used for this project:
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [PubSubClient](https://github.com/knolleary/pubsubclient)
- [TinyPICO-Helper](https://github.com/)

## License 

This software is licensed under the APACHE 2.0 License. See the [license file](LICENSE) for details.
