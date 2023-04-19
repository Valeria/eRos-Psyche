# eRos & Psyche
### **Open Source Dildo running on the ESP32 Platform**

![PCB!](Images/eROS.png?raw=true "PCB")
![Hello!](Images/Dong.jpeg?raw=true "Hi!")
![Licenses](Images/LICENSE-INFO.png?raw=true "License Information")

V1.0 Attaches to a **ESP32-DevKitC-V4**, **check pinout to make sure it coincides**.

It's meant to drive three (or less) **3.3v motors, max 1A per PWM output**.
Use with Li-Po or Li-Ion **3.7v nominal voltage batteries**.

Connects to a **Blynk.io** server, which can be self-hosted, otherwise, use the free Blynk Cloud server or contact me for a spot on mine.

Mobile app can be recreated by scanning the following QR code on the Blynk phone app:

![Blynk Project QR](Images/blynkQR.png?raw=true "Scan this on Blynk app")

Hardware files are in the **eRos** directory, meant to be edited on **KiCad v5.0 or higher**.

Firmware files are in the **Psyche** directory, if you open the project on PlatformIO it should load all required Arduino libraries automatically, else, here's a list:

 - shaggydog/OneButton@^1.5.0
 - ayushsharma82/AsyncElegantOTA@^2.2.5
 - ottowinter/AsyncTCP-esphome@^1.2.1
 - ottowinter/ESPAsyncWebServer-esphome@^1.2.7
 - bblanchon/ArduinoJson@^6.17.3
 - s00500/ESPUI@^2.0.0
 - khoih.prog/Blynk_Async_ESP32_BT_WF@^1.2.1
 - adafruit/Adafruit BMP280 Library@^2.4.1
 - teckel12/NewPing@^1.9.1
 - gamegine/HCSR04 ultrasonic sensor@^2.0.2
 - adafruit/Adafruit_VL53L0X@^1.1.1

Firmware.bin must be built with the **huge_app.csv** partition scheme.

Have fun!
