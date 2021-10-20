// Checks if it's necessary to update values
int updateTime()
{
    currentTime = millis();
    if (currentTime - lastTime >= period)
    {
        lastTime = currentTime;
        return 1;
    }
    else
        return 0;
}

// Same but you input a fixed period value (for certain patterns)
int updateTimeValued(unsigned long per)
{
    currentTime = millis();
    if (currentTime - lastTime >= per)
    {
        lastTime = currentTime;
        return 1;
    }
    else
        return 0;
}

// Set fixed PWM value to a single motor
void fixedSingle(int v, int m)
{
    ledcWrite(motorChannel[m], v);
}

// Set fixed PWM value to all motors
void fixedAll(int v)
{
    ledcWrite(motorChannel[0], v);
    ledcWrite(motorChannel[1], v);
    ledcWrite(motorChannel[2], v);
}

// Send pulses to all motors
void pulse(int vmin, int vmax)
{
    if (updateTime())
    {
        if (ledcRead(motorChannel[0]) == vmin)
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
}

// Ramp motors upwards
void ramp(int vmin, int vmax, int step)
{
    if (updateTime())
    {
        if (now < vmax)
        {
            now += step;
            now = constrain(now, vmin, vmax);
            ledcWrite(motorChannel[0], now);
            ledcWrite(motorChannel[1], now);
            ledcWrite(motorChannel[2], now);
        }
        else
        {
            now = vmin;
            ledcWrite(motorChannel[0], now);
            ledcWrite(motorChannel[1], now);
            ledcWrite(motorChannel[2], now);
        }
    }
}

// Ramp motors upwards then to the bottom
void rampBoth(int vmin, int vmax, int step)
{
    if (updateTime()) // Checks if the period time has passed
    {
        if (seq == 0) // Sequence going upwards
        {
            now += step;                      // Increases power
            now = constrain(now, vmin, vmax); // Constrains PWM value between vmin and vmax
            if (now == vmax)                  // This is the final step
                seq = !seq;                   // So it reverses the sequence
            ledcWrite(motorChannel[0], now);
            ledcWrite(motorChannel[1], now);
            ledcWrite(motorChannel[2], now);
        }
        else
        {
            now -= step;
            now = constrain(now, vmin, vmax);
            if (now == vmin)
                seq = !seq;
            ledcWrite(motorChannel[0], now);
            ledcWrite(motorChannel[1], now);
            ledcWrite(motorChannel[2], now);
        }
    }
}

// Set a random motor to a random value
void randi(int vmin, int vmax)
{
    if (updateTime())
    {
        ledcWrite(motorChannel[random(0, 3)], random(vmin, vmax + 1));
    }
}

// Exponential sequence
// Credited to: github/Comingle & github/jgeisler0303
void sharpRamp(int seq)
{
    const uint8_t fadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255};
    if (updateTimeValued(12))
    {
        seq++;
        if (seq >= 32)
            seq = 0;
        ledcWrite(motorChannel[0], fadeTable[seq]);
        ledcWrite(motorChannel[1], fadeTable[seq]);
        ledcWrite(motorChannel[2], fadeTable[seq]);
    }
}
