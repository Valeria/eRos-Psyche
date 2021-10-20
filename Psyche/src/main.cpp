/********************************************************
  
  Based on code examples built by Khoi Hoang
  https://github.com/khoih-prog/Blynk_Async_ESP32_BT_WF
  which are licensed under MIT license.
     
 *******************************************************/

#include "defines.h"
#include "Credentials.h"
#include "dynamicParams.h"
#include "Arduino.h"
#include "OneButton.h"
#include "ArduinoJson.h"
#include "FS.h"
#include "SPIFFS.h"
#include "Adafruit_BMP280.h"
#include "Wire.h"
//#include "NewPing.h"
//#include <HCSR04.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Setup Buttons
OneButton button1(14, true, true);
OneButton button2(13, true, true);

// LEDs
const int led[3] = {5, 19, 23};

// Sensors
const int sensor[4] = {21, 22, 32, 33};

// Ven: low battery signal

#define vEnableEnabled false // Enables the usage of the undervoltage detector
const int Ven = 27;
const int trytry = 0;

// PWM integers
int freq = 10000; // 10 Khz
int motorChannel[3] = {0, 1, 2};
int resolution = 8;                // 2^8 = 256 possible values
const int motor[3] = {16, 17, 18}; // Motors GPIO

// Pattern integers
int next = 0;
int lastNext = 1;
int vmax = 200;
int vmin = 30;
int step = 25;
char vminC[3], vmaxC[3], nextC[3], stepC[3], periodC[3]; // Will be required for Blynk LCD Widget
unsigned long period = 250;
unsigned long currentTime = 0;
unsigned long lastTime = 0;
int seq = 0;
int seq2 = 0;
int now = 0;

// Blynk custom pattern integers
int customMode = 0;
unsigned long lastTimeCustom[3] = {0, 0, 0};
unsigned long currentTimeCustom[3] = {0, 0, 0};
unsigned long stepCustom = 25;
unsigned long periodCustom = 200;
int vminCustom[3] = {30, 30, 30};
int vmaxCustom[3] = {255, 255, 255};
int nowCustom[3] = {0, 0, 0};
int nextCustom[3] = {0, 0, 0};
int seqCustom[3] = {0, 0, 0};
int seq2Custom[3] = {0, 0, 0};
int kCustom[3] = {1, 1, 1};

// Blynk sensor mode integers
int sensorMode = 0; // Are we in sensor mode?
int x, y, z = 0;    // We be floating in space (Gravity sensor integers)

// Blynk direct control integers
int directControl = 0; // Are we being direct controlled? (Can't assume this)
int sliderjoy = 1;     // Assume direct control via sliders (1) or joystick (2).
int joyX = 0;
int joyY = 0;

// Patterns list
#include <patterns.h>
int maxpatterns = 6; // Number of actual patterns used

// Function to set motor PWM settings to correct pattern values
// also a list of patterns currently in use
void callPattern(int k)
{
  switch (k)
  {
  case 0:
    fixedAll(0);
    break;
  case 1:
    fixedAll(vmax);
    break;
  case 2:
    pulse(vmin, vmax);
    break;
  case 3:
    ramp(vmin, vmax, step);
    break;
  case 4:
    rampBoth(vmin, vmax, step);
    break;
  case 5:
    sharpRamp(seq);
    break;
  case 6:
    randi(vmin, vmax);
    break;
  default:
    break;
  }
}

// Will be called when Button1 is clicked a single time
void Click1()
{
  next += 1;
  if (next > maxpatterns)
    next = 1; // Loop back if over range
} // Click1

// Will be called when Button2 is clicked a single time.
void Click2()
{
  next -= 1;
  if (next < 1)
    next = maxpatterns; // Loop back if over range
} // Click2

// Will be called when Button1 is held.
void HODL1()
{
  if (digitalRead(13) == LOW)
    next = 0;
} // HODL1

// Will be called when Button2 is held.
void HODL2()
{
  if (digitalRead(14) == LOW)
    next = 0;
} // HODL2

// Will be called when Button1 is clicked twice.
void ClickDouble1()
{
  vmax += 25;
  vmax = constrain(vmax, vmin, 255);
  //callPattern(next);
} // ClickDouble1

// Will be called when Button2 is clicked twice.
void ClickDouble2()
{
  vmax -= 25;
  vmax = constrain(vmax, vmin, 255);
  //callPattern(next);
} // ClickDouble2

bool USE_BLE = true;
long timePreviousMeassure = 0;

#include "blynk.h"
#include "table.h"

#define WIFI_BLE_SELECTION_PIN 14 //Pin D14 mapped to pin GPIO14/HSPI_SCK/ADC16/TOUCH6/TMS of ESP32

BlynkTimer timer;

#include <Ticker.h>
Ticker led_ticker;

#define BLYNK_PIN_FORCED_CONFIG V100 // Pin to access web configuration portal

#include <asyncSetup.h>
#include "sensor.h"

void setup()
{
  // Initialize PWM
  ledcSetup(motorChannel[0], freq, resolution);
  ledcAttachPin(motor[0], motorChannel[0]);
  ledcSetup(motorChannel[1], freq, resolution);
  ledcAttachPin(motor[1], motorChannel[1]);
  ledcSetup(motorChannel[2], freq, resolution);
  ledcAttachPin(motor[2], motorChannel[2]);

  ledcWrite(motorChannel[0], 0);
  ledcWrite(motorChannel[1], 0);
  ledcWrite(motorChannel[2], 0);

  // Initialize serial console
  Serial.begin(115200);
  /*while (!Serial)
    ;

  //delay(200);
  */

  setCpuFrequencyMhz(80); // To lower power usage, default 240Mhz isn't required for this application

  SPIFFS.begin();

  pinMode(WIFI_BLE_SELECTION_PIN, INPUT_PULLUP);
  //pinMode(WIFI_BLE_SELECTION_PIN, INPUT);

  // Set Timer to update gauges
  gaugeTimer.setInterval(1000L, gauges);

  // LEDs
  //pinMode(led[1], OUTPUT);
  //pinMode(led[2], OUTPUT);
  //pinMode(led[3], OUTPUT);

  // Sensors
  //pinMode(sensor[1], OUTPUT);
  //pinMode(sensor[2], INPUT);
  //pinMode(sensor[3], INPUT);
  //pinMode(sensor[4], INPUT); //Commented out for usage as capacitive pin

  // Ven: low battery signal
  pinMode(Ven, INPUT);

  // Attach button clicks to functions
  button1.attachClick(Click1);
  button1.attachDoubleClick(ClickDouble1);
  button1.attachLongPressStart(HODL1);
  button2.attachClick(Click2);
  button2.attachDoubleClick(ClickDouble2);
  button2.attachLongPressStart(HODL2);

// Pressure sensor
/*#if sensorsEnabled
  bmpSetup();
#endif
*/

#if BLYNK_USE_BLE_ONLY
  Blynk_BLE.setDeviceName(BLE_Device_Name);

#if ESP32_BLE_WF_DEBUG
  Serial.println(F("Blynk_BLE begin"));
#endif

  Blynk_BLE.begin(auth);
#else
  //if (digitalRead(WIFI_BLE_SELECTION_PIN) == HIGH)
    if (trytry == 1)
  {
    USE_BLE = false;
    Serial.println(F("Using Wi-Fi"));
#if USE_BLYNK_WM
    // Set config portal SSID and Password
    Blynk.setConfigPortal("eRos-ConfigPortal", "123456789");
    // Set config portal IP address
    Blynk.setConfigPortalIP(IPAddress(192, 168, 232, 1));

    // Set config portal channel, default = 1. Use 0 => random channel from 1-13 to avoid conflict
    Blynk_WF.setConfigPortalChannel(0);

    // Set static IP + DNS
    Blynk.setSTAStaticIPConfig(IPAddress(192, 168, 2, 232), IPAddress(192, 168, 2, 1), IPAddress(255, 255, 255, 0));

#if USING_CUSTOMS_STYLE
    Blynk.setCustomsStyle(NewCustomsStyle);
#endif

#if USING_CUSTOMS_HEAD_ELEMENT
    Blynk.setCustomsHeadElement("<style>html{filter: invert(10%);}</style>");
#endif

#if USING_CORS_FEATURE
    Blynk.setCORSHeader("Your Access-Control-Allow-Origin");
#endif

    //////////////////////////////////////////////

    // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
    //Blynk.begin();
    // Use this to personalize DHCP hostname (RFC952 conformed)
    // 24 chars max,- only a..z A..Z 0..9 '-' and no '-' as last char
    //Blynk.begin("ESP32-BLE-WM");

    Blynk_WF.begin(BLE_Device_Name);
#else
    //Blynk_WF.begin(auth, ssid, pass);
#if ESP32_BLE_WF_DEBUG
    Serial.println(F("Not USE_BLYNK_WM: Blynk_WF begin"));
#endif
    Blynk_WF.begin(auth, ssid, pass, cloudBlynkServer.c_str(), BLYNK_SERVER_HARDWARE_PORT);
#endif
  }
  else
  {
    USE_BLE = true;
    Serial.println(F("Using BLE"));
    Blynk_BLE.setDeviceName(BLE_Device_Name);
#if USE_BLYNK_WM
    if (Blynk_WF.getBlynkBLEToken() == NO_CONFIG) //String("blank"))
    {
      Serial.println(F("No valid stored BLE auth. Must run WiFi and enter config portal"));
      USE_BLE = false;

#if ESP32_BLE_WF_DEBUG
      Serial.println(F("USE_BLYNK_WM: No BLE Token. Blynk_WF begin"));
#endif

      Blynk_WF.begin(BLE_Device_Name);
    }
    String BLE_auth = Blynk_WF.getBlynkBLEToken();
#else
    String BLE_auth = auth;
#endif

    if (USE_BLE)
    {
      Serial.print(F("Connecting Blynk via BLE, using auth = "));
      Serial.println(BLE_auth);

#if ESP32_BLE_WF_DEBUG
      Serial.println(F("USE_BLE: Blynk_BLE begin"));
#endif

      Blynk_BLE.begin(BLE_auth.c_str());
    }
  }
#endif

  // Important, need to keep constant communication to Blynk Server at least once per ~25s
  // Or Blynk will lost and have to (auto)reconnect
  timer.setInterval(10000L, noticeAlive);

  /*
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
  }
  */
 Wire.begin(32, 33);
}

#if (USE_BLYNK_WM && USE_DYNAMIC_PARAMETERS)
void displayCredentials()
{
  Serial.println(F("\nYour stored Credentials :"));

  for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
  {
    Serial.print(myMenuItems[i].displayName);
    Serial.print(F(" = "));
    Serial.println(myMenuItems[i].pdata);
  }
}

void displayCredentialsInLoop()
{
  static bool displayedCredentials = false;

  if (!displayedCredentials)
  {
    for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
    {
      if (!strlen(myMenuItems[i].pdata))
      {
        break;
      }

      if (i == (NUM_MENU_ITEMS - 1))
      {
        displayedCredentials = true;
        displayCredentials();
      }
    }
  }
}

#endif

void loop()
{
#if vEnableEnabled
  while (digitalRead(Ven) == LOW) // Do nothing until we get high voltage signal
    ;
#endif
  // Keep checking the buttons
  //button1.tick();
  //button2.tick();

#if BLYNK_USE_BLE_ONLY
  Blynk_BLE.run();
#else
  if (USE_BLE)
    Blynk_BLE.run();
  else
    Blynk_WF.run();
#endif

  // Call function to update PWM values
  if (directControl)
  {
    // We do nothin'!
  }
  else if (customMode)
  {
    callPatternCustom(kCustom[0], 0);
    callPatternCustom(kCustom[1], 1);
    callPatternCustom(kCustom[2], 2);
  }
  else if (tableMode)
  {
    nextTable();                            // Checks if we must move to the next pattern in the list
    callTable(patternList1[indexTable], 0); // Sets motors
    callTable(patternList2[indexTable], 1);
    callTable(patternList3[indexTable], 2);
  }
  else if (sensorMode)
  {
    if (sensorSelect == 1)
      sonarPattern();
    else if (sensorSelect == 2)
      bmpPattern();
    else if (sensorSelect == 3)
      capacitive();
  }
  else
    callPattern(next);

  timer.run();
  checkStatus();
  gaugeTimer.run();

#if (USE_BLYNK_WM && USE_DYNAMIC_PARAMETERS)
  displayCredentialsInLoop();
#endif
}