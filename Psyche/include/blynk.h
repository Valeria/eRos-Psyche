bool virginLCD = true;

BlynkTimer gaugeTimer;       // Announcing the timer
WidgetTerminal terminal(V1); // Attach virtual serial terminal to Virtual Pin V1
WidgetLCD lcd(V2);
WidgetLCD lcd2(V3); // Announce LCD widgets
int motor1Enabled = 0;
int motor2Enabled = 0;
int motor3Enabled = 0;


// updateTime for custom Blynk pattern
int updateTimeCustom(unsigned long per, int index)
{
  currentTimeCustom[index] = millis();
  if (currentTimeCustom[index] - lastTimeCustom[index] >= per)
  {
    lastTimeCustom[index] = currentTimeCustom[index];
    return 1;
  }
  else
    return 0;
}

// Patterns for Blynk custom mode

// Send pulses to a motor
void pulseCustom(int vmin, int vmax, int index, unsigned long per)
{
  if (updateTimeCustom(per, index))
  {
    if (ledcRead(motorChannel[index]) == vmin)
    {
      ledcWrite(motorChannel[index], vmax);
    }
    else
    {
      ledcWrite(motorChannel[index], vmin);
    }
  }
}

// Ramp motor upwards
void rampCustom(int vmin, int vmax, int step, int index, unsigned long per)
{
  if (updateTimeCustom(per, index))
  {
    if (nowCustom[index] < vmax)
    {
      nowCustom[index] += step;
      nowCustom[index] = constrain(nowCustom[index], vmin, vmax);
      ledcWrite(motorChannel[index], nowCustom[index]);
    }
    else
    {
      nowCustom[index] = vmin;
      ledcWrite(motorChannel[index], nowCustom[index]);
    }
  }
}

// Ramp motors upwards then to the bottom
void rampBothCustom(int vmin, int vmax, int step, int index, unsigned long per)
{
  if (updateTimeCustom(per, index)) // Checks if the period time has passed
  {
    if (seqCustom[index] == 0) // Sequence going upwards
    {
      nowCustom[index] += step;                                   // Increases power
      nowCustom[index] = constrain(nowCustom[index], vmin, vmax); // Constrains PWM value between vmin and vmax
      if (nowCustom[index] == vmax)                               // This is the final step
        seqCustom[index] = !seqCustom[index];                     // So it reverses the sequence
      ledcWrite(motorChannel[index], nowCustom[index]);
    }
    else
    {
      nowCustom[index] -= step;
      nowCustom[index] = constrain(nowCustom[index], vmin, vmax);
      if (nowCustom[index] == vmin)
        seqCustom[index] = !seqCustom[index];
      ledcWrite(motorChannel[index], nowCustom[index]);
    }
  }
}

// Exponential sequence
// Credited to: github/Comingle & github/jgeisler0303
void sharpRampCustom(int index)
{
  const uint8_t fadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255};
  if (updateTimeCustom(12, index))
  {
    seq2Custom[index]++;
    if (seq2Custom[index] >= 32)
      seq2Custom[index] = 0;
    ledcWrite(motorChannel[index], fadeTable[seq2Custom[index]]);
  }
}

// Set a motor to a random value
void randiCustom(int vmin, int vmax, int index, unsigned long per)
{
  if (updateTimeCustom(per, index))
  {
    ledcWrite(motorChannel[index], random(vmin, vmax + 1));
  }
}

// Function to set motor PWM settings to correct pattern values
// also a list of patterns currently in use
void callPatternCustom(int k, int index)
{
  switch (k)
  {
  case 1:
    fixedSingle(vmaxCustom[index], index);
    break;
  case 2:
    pulseCustom(vminCustom[index], vmaxCustom[index], index, periodCustom);
    break;
  case 3:
    rampCustom(vminCustom[index], vmaxCustom[index], stepCustom, index, periodCustom);
    break;
  case 4:
    rampBothCustom(vminCustom[index], vmaxCustom[index], stepCustom, index, periodCustom);
    break;
  case 5:
    sharpRampCustom(index);
    break;
  case 6:
    randiCustom(vminCustom[index], vmaxCustom[index], index, periodCustom);
    break;
  case 7:
    fixedSingle(0, index);
    break;
  default:
    break;
  }
}

// Will run once when connected to server
BLYNK_CONNECTED()
{
  lcd.clear();
  lcd.print(5, 0, "Hello");
  lcd.print(5, 1, "eRos!");
  if (USE_BLE)
    Blynk_BLE.syncAll();
  else
    Blynk_WF.syncAll();
}

// Will run once when disconnected to server
BLYNK_DISCONNECTED()
{
  //delay(300); //Otherwise dumb Blynk complains that "packet is too big"
  lcd.clear(); // Clear the widget
  lcd.print(4, 0, "Goodbye");
  lcd.print(4, 1, "eRos :(");
}

// Updates motor gauges
void gauges()
{
  if (USE_BLE)
  {
    Blynk_BLE.virtualWrite(V97, ledcRead(motorChannel[0]));
    Blynk_BLE.virtualWrite(V98, ledcRead(motorChannel[1]));
    Blynk_BLE.virtualWrite(V99, ledcRead(motorChannel[2]));
  }
  else
  {
    Blynk_WF.virtualWrite(V97, ledcRead(motorChannel[0]));
    Blynk_WF.virtualWrite(V98, ledcRead(motorChannel[1]));
    Blynk_WF.virtualWrite(V99, ledcRead(motorChannel[2]));
  }
}

// Attached to player widget, works like physical buttons
BLYNK_WRITE(V1)
{
  String action = param.asStr();

  if (action == "play")
  {
    lcd.clear();
    lcd.print(2, 0, "Now running:");
    lcd.print(2, 1, "Default mode");
    if (next == 0)
      next = lastNext;
  }
  if (action == "stop")
  {
    lcd.clear();
    lcd.print(2, 0, "Stopped.");
    lastNext = next; // Saves last used pattern for later
    next = 0;
  }
  if (action == "next")
  {
    next += 1;
    if (next > maxpatterns)
      next = 1; // Loop back if over range
  }
  if (action == "prev")
  {
    next -= 1;
    if (next < 1)
      next = maxpatterns; // Loop back if over range
  }
}

// Sets Vmin value
BLYNK_WRITE(V10)
{
  vmin = param.asInt();
}

// Sets Vmax value
BLYNK_WRITE(V11)
{
  vmax = param.asInt();
}

// Sets step value
BLYNK_WRITE(V12)
{
  step = param.asInt();
}

// Sets period value
BLYNK_WRITE(V13)
{
  period = param.asLong();
}

// Enables Direct Control over default pattern settings
BLYNK_WRITE(V20)
{
  directControl = param.asInt();
  if (directControl)
  {
    lcd.clear();
    lcd.print(2, 0, "Now running:");
    lcd.print(2, 1, "Direct mode");
    Serial.println("ASSUMING DIRECT CONTROL.");
  }
  else
  {
    lcd.clear();
    Serial.println("RELEASING CONTROL OF THIS FORM.");
  }
}

// Chooses between slider or joystick Direct Control
BLYNK_WRITE(V21)
{
  motor1Enabled = param.asInt();
}

// The following ones all change PWM values according to Direct Control Sliders
BLYNK_WRITE(V22)
{
  motor2Enabled = param.asInt();
}

BLYNK_WRITE(V23)
{
  motor3Enabled = param.asInt();
}

// This one's for Joystick
BLYNK_WRITE(V24)
{
  if (directControl)
  {
    joyX = param[0].asInt();
    joyY = param[1].asInt();
    // Cool function that maxes out 1 and 3 motors at 255 and 0
    // Could also simply set motors to read value from joystick
    /*
    if (x <= 255 && x < 128)
    {
      if (motor1Enabled)
        ledcWrite(motorChannel[0], constrain(round(1.81 * joyX - 206.81), 0, 255));
      if (motor2Enabled)
        ledcWrite(motorChannel[1], constrain(round(-1.81 * joyX + 486.81), 0, 255));
      if (motor3Enabled)
        ledcWrite(motorChannel[2], constrain(round(-0.2 * joyX + 50.2), 0, 255));
    }
    else if (x <= 128 && x >= 0)
    {
      if (motor1Enabled)
        ledcWrite(motorChannel[0], constrain(round(0.2 * joyX), 0, 255));
      if (motor2Enabled)
        ledcWrite(motorChannel[1], constrain(round(1.8 * joyX + 25), 0, 255));
      if (motor3Enabled)
        ledcWrite(motorChannel[2], constrain(round(-1.8 * joyX + 255), 0, 255));
    }
    */
  }
  if (joyX <= 128 && joyX >= 0)
  {
    if (motor1Enabled)
        ledcWrite(motorChannel[1], constrain(joyY, 0, 255));
    if (motor2Enabled)
        ledcWrite(motorChannel[0], constrain(((128 - joyX) * 2), 0, 255));
    if (motor3Enabled)
      ledcWrite(motorChannel[2], 0);
    
  }
  else if (joyX > 128 && joyX <= 255)
  {
    if (motor3Enabled)
        ledcWrite(motorChannel[1], constrain(joyY, 0, 255));
    if (motor2Enabled)
        ledcWrite(motorChannel[2], constrain(((joyX - 128) * 2), 0, 255));
    if (motor1Enabled)
      ledcWrite(motorChannel[0], 0);
  }
}

// The following ones are for custom mode
BLYNK_WRITE(V30)
{
  customMode = param.asInt();
  if (customMode)
  {
    lcd.clear();
    lcd.print(2, 0, "Now running:");
    lcd.print(2, 1, "Custom mode");
  }
  else
    lcd.clear();
}
BLYNK_WRITE(V31)
{
  kCustom[0] = param.asInt();
}
BLYNK_WRITE(V32)
{
  vminCustom[0] = param.asInt();
}
BLYNK_WRITE(V33)
{
  vmaxCustom[0] = param.asInt();
}
BLYNK_WRITE(V34)
{
  kCustom[1] = param.asInt();
}
BLYNK_WRITE(V35)
{
  vminCustom[1] = param.asInt();
}
BLYNK_WRITE(V36)
{
  vmaxCustom[1] = param.asInt();
}
BLYNK_WRITE(V37)
{
  kCustom[2] = param.asInt();
}
BLYNK_WRITE(V38)
{
  vminCustom[2] = param.asInt();
}
BLYNK_WRITE(V39)
{
  vmaxCustom[2] = param.asInt();
}
BLYNK_WRITE(V58)
{
  stepCustom = param.asInt();
}
BLYNK_WRITE(V59)
{
  periodCustom = param.asLong();
}

// These are for sensor mode
BLYNK_WRITE(V80)
{
  sensorMode = param.asInt();
}

// LCD1 widget
// LCD2 widget
void printLcd2()
{
  lcd2.clear(); // Clean the widget
  lcd2.print(0, 0, "Min");
  itoa(vmin, vminC, 10);
  lcd2.print(4, 0, vminC);
  lcd2.print(9, 0, "Max");
  itoa(vmax, vmaxC, 10);
  lcd2.print(13, 0, vmaxC);
  lcd2.print(0, 1, "Stp");
  itoa(step, stepC, 10);
  lcd2.print(4, 1, stepC);
  lcd2.print(9, 1, "Per");
  itoa(period, periodC, 10);
  lcd2.print(13, 1, periodC);
}