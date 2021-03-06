/***************************************************************************
  defines.h
  For ESP32 boards
  
  Blynk_Async_ESP32_BT_WF is a library, using AsyncWebServer instead of
  (ESP8266)WebServer for inclusion of both ESP32 
  Blynk BT/BLE and WiFi libraries. Then select either one or both at runtime.
  
  Based on and modified from Blynk library v0.6.1
  https://github.com/blynkkk/blynk-library/releases
  Built by Khoi Hoang https://github.com/khoih-prog/Blynk_Async_ESP32_BT_WF
  Licensed under MIT license
 *************************************************************************/

#ifndef defines_h
#define defines_h

#ifndef ESP32
#error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif

#define BLYNK_PRINT Serial

#define ESP32_BLE_WF_DEBUG true

#define DOUBLERESETDETECTOR_DEBUG true
#define BLYNK_WM_DEBUG 1

// Not use #define USE_LITTLEFS and #define USE_SPIFFS  => using SPIFFS for configuration data in WiFiManager
// (USE_LITTLEFS == false) and (USE_SPIFFS == false)    => using EEPROM for configuration data in WiFiManager
// (USE_LITTLEFS == true) and (USE_SPIFFS == false)     => using LITTLEFS for configuration data in WiFiManager
// (USE_LITTLEFS == true) and (USE_SPIFFS == true)      => using LITTLEFS for configuration data in WiFiManager
// (USE_LITTLEFS == false) and (USE_SPIFFS == true)     => using SPIFFS for configuration data in WiFiManager
// Those above #define's must be placed before #include <BlynkSimpleEsp32_WFM.h>

#define USE_LITTLEFS false
#define USE_SPIFFS true

#if !(USE_SPIFFS || USE_LITTLEFS)
// EEPROM_SIZE must be <= 2048 and >= CONFIG_DATA_SIZE (currently 172 bytes)
#define EEPROM_SIZE (2 * 1024)
// EEPROM_START + CONFIG_DATA_SIZE must be <= EEPROM_SIZE
#define EEPROM_START 0
#endif

/////////////////////////////////////////////

// Add customs headers from v1.2.0
#define USING_CUSTOMS_STYLE true
#define USING_CUSTOMS_HEAD_ELEMENT true
#define USING_CORS_FEATURE true

/////////////////////////////////////////////

// Force some params in Blynk, only valid for library version 1.0.1 and later
#define TIMEOUT_RECONNECT_WIFI 10000L
#define RESET_IF_CONFIG_TIMEOUT true
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET 5
// Those above #define's must be placed before #include <BlynkSimpleESP32_Async_WFM.h>

//#define BLYNK_USE_BLE_ONLY      true
#define BLYNK_USE_BLE_ONLY false

#include <BlynkSimpleEsp32_Async_BLE_WF.h>
#include <BLEDevice.h>
#include <BLEServer.h>

//#define USE_BLYNK_WM          true
#define USE_BLYNK_WM false

#if !BLYNK_USE_BLE_ONLY
#if USE_BLYNK_WM
#define USE_DYNAMIC_PARAMETERS false

/////////////////////////////////////////////

#define REQUIRE_ONE_SET_SSID_PW true

#define SCAN_WIFI_NETWORKS false

// To be able to manually input SSID, not from a scanned SSID lists
#define MANUAL_SSID_INPUT_ALLOWED true

// From 2-15
#define MAX_SSID_IN_LIST 10

/////////////////////////////////////////////

#warning Please select 1.3MB+ for APP (Minimal SPIFFS (1.9MB APP, OTA), HugeAPP(3MB APP, NoOTA) or NoOA(2MB APP)
#include <BlynkSimpleEsp32_Async_WFM.h>
#else
#include <BlynkSimpleEsp32_Async_WF.h>

String cloudBlynkServer = "SERVERHERE";
//String cloudBlynkServer = "192.168.2.110";
#define BLYNK_SERVER_HARDWARE_PORT 80
char ssid[] = "SSIDHERE";
char pass[] = "PASSWD HERE";
#endif
#endif

#if (BLYNK_USE_BLE_ONLY || !USE_BLYNK_WM)
// Blynk token shared between BLE and WiFi
char auth[] = "TOKENHERE";
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
#endif

#define HOST_NAME "eRos Board V1.0"

#endif //defines_h
