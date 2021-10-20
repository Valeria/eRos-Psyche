#define sensorsEnabled true
unsigned int sensorSelect = 0;
int capacitiveVal = 100;
int capacitiveThreshold = 2;
int sensorDone = 0;

// ESP32 Capacitive touch sensor

void capacitive()
{
currentTime = millis();
    if (currentTime - lastTime >= 100)
    {
        lastTime = currentTime;
        capacitiveVal = touchRead(T8); // GPIO 33
        if (capacitiveVal < capacitiveThreshold)
        {
            ledcWrite(motorChannel[0], vmax);
            ledcWrite(motorChannel[1], vmax);
            ledcWrite(motorChannel[2], vmax);
        }
        else
        {
            ledcWrite(motorChannel[0], vmin);
            ledcWrite(motorChannel[1], vmin);
            ledcWrite(motorChannel[2], vmin);
        }
        lcd.print(4, 0, capacitiveVal);
    }
}
// BMP280 Pressure, temperature sensor
Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();
float pressure = 0;
float pressureOffset = 0;
float pressureDelta = 500;

void bmpSetup()
{
    bmp.begin();

    /* Default settings from datasheet. */
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void bmpPattern()
{
    pressure = bmp.readPressure();
    if (pressure - pressureOffset >= pressureDelta)
    {
        ledcWrite(motorChannel[0], vmax);
        ledcWrite(motorChannel[1], vmax);
        ledcWrite(motorChannel[2], vmax);
    }
    else
    {
        ledcWrite(motorChannel[0], vmin);
        ledcWrite(motorChannel[1], vmin);
        ledcWrite(motorChannel[2], vmin);
    }
}

// HC-SR04 Ultrasonic distance sensor
int maxDistance = 50;
//NewPing sonar(32, 33, maxDistance); // Trigger GPIO 21, Echo GPIO 22, 50cm max distance
//HCSR04 hc(32, 33);
int distance = 0;
VL53L0X_RangingMeasurementData_t measure;

void sonarPattern()
{
    delay(100); // Wait some time between pings
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

    if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    distance = measure.RangeMilliMeter;
    }
    //distance = sonar.ping_cm();
    //distance = hc.dist();
    
    if (distance > 500 || distance == 0)
    {
        ledcWrite(motorChannel[0], 0);
        ledcWrite(motorChannel[1], 0);
        ledcWrite(motorChannel[2], 0);
    }
    if (distance >= 400 && distance <= 500)
    {
        ledcWrite(motorChannel[0], 50);
        ledcWrite(motorChannel[1], 50);
        ledcWrite(motorChannel[2], 50);
    }
    else if (distance >= 300 && distance < 400)
    {
        ledcWrite(motorChannel[0], 100);
        ledcWrite(motorChannel[1], 100);
        ledcWrite(motorChannel[2], 100);
    }
    else if (distance >= 200 && distance < 300)
    {
        ledcWrite(motorChannel[0], 150);
        ledcWrite(motorChannel[1], 150);
        ledcWrite(motorChannel[2], 150);
    }
    else if (distance >= 100 && distance < 200)
    {
        ledcWrite(motorChannel[0], 200);
        ledcWrite(motorChannel[1], 200);
        ledcWrite(motorChannel[2], 200);
    }
    else if (distance < 100 && distance > 10)
    {
        ledcWrite(motorChannel[0], 255);
        ledcWrite(motorChannel[1], 255);
        ledcWrite(motorChannel[2], 255);
    }
    Serial.println(distance);
}

BLYNK_WRITE(V60)
{
    sensorMode = param.asInt();
    if (sensorMode == 1 && sensorDone == 0)
    {
    if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    sensorDone = 1;
    }
    }
}
BLYNK_WRITE(V61)
{
    sensorSelect = param.asInt();
}
BLYNK_WRITE(V62)
{
    if (param.asInt())
        pressureOffset = bmp.readPressure();
}
BLYNK_WRITE(V63)
{
    pressureDelta = param.asInt();
}