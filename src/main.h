#include <Arduino.h>
#include <WiFi.h>
#include <ArtnetnodeWifi.h>
#include <NeoPixelBus.h>


#define SSID        "someSSID"
#define PSK         "somePSK"
#ifndef HOSTNAME
# error Hostname not defined
#endif


#define LED_NUMBER  288 // MAX rgbw: 2048, rgb: 2730
#define LED_PIN     19

#define SK6812
// #define WS2812
