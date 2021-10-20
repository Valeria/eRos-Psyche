int patternList1[10], patternList2[10], patternList3[10];
int vminList[10], vmaxList[10], stepList[10];
int patternTemp1, patternTemp2, patternTemp3, vminTemp, vmaxTemp, stepTemp;
unsigned long durationTemp, periodTemp;
unsigned long periodList[10];
unsigned long durationList[10];
int used = 0;
int i;
unsigned long timetime;         // No good names left :(
unsigned long nowtimetime;      // Even worse name :((
unsigned long lasttimetime = 0; // Jesus.
int indexTable = 0;
int tableMode = 0;
int indexBlynkTable = 0;
int fileindex = 0;
String stringy, stringy2; // Stringy strings getting defined as strings.

WidgetTable table;
BLYNK_ATTACH_WIDGET(table, V40);

const char *filename = "/patternDefault.json";

// Loads the configuration from a file
void loadConfiguration(const char *filename)
{
  // Open file for reading
  File file = SPIFFS.open(filename);

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<4096> doc; // Probably overkill but let's be safe

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  //Copy values
  used = doc["used"];
  for (i = 0; i < used; i++)
  {
    patternList1[i] = doc["patterns1"][i];
    patternList2[i] = doc["patterns2"][i];
    patternList3[i] = doc["patterns3"][i];
    durationList[i] = doc["duration"][i];
    vminList[i] = doc["vmin"][i];
    vmaxList[i] = doc["vmax"][i];
    stepList[i] = doc["step"][i];
    periodList[i] = doc["period"][i];
  }

  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
}

// Saves the configuration to a file
void saveConfiguration(const char *filename)
{
  // Delete existing file, otherwise the configuration is appended to the file
  SPIFFS.remove(filename);

  // Open file for writing
  File file = SPIFFS.open(filename, "w");
  if (!file)
  {
    Serial.println(F("Failed to create file"));
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<4096> doc;

  // Set the values in the document
  doc["used"] = used;
  for (i = 0; i < used; i++)
  {
    doc["patterns1"][i] = patternList1[i];
    doc["patterns2"][i] = patternList2[i];
    doc["patterns3"][i] = patternList3[i];
    doc["duration"][i] = durationList[i];
    doc["vmin"][i] = vminList[i];
    doc["vmax"][i] = vmaxList[i];
    doc["step"][i] = stepList[i];
    doc["period"][i] = periodList[i];
  }

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0)
  {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}

// Checks if it's time to move to the next pattern in the table
int nextTable()
{
  nowtimetime = millis();
  if (lasttimetime == 0)
    lasttimetime = nowtimetime;                               // Because the first time this is called it shouldnt update index.
  if (nowtimetime - lasttimetime >= durationList[indexTable]) // OOPSIE!
  {
    lasttimetime = nowtimetime;
    if (indexTable < used)
      indexTable++;
    else
      indexTable = 0;
    return 1;
  }
  else
    return 0;
}

// Names patterns to improve user experience (amazing experience!)
String namePattern(int a)
{
  stringy.clear();
  switch (a)
  {
  case 1:
    stringy = "None";
    break;
  case 2:
    stringy = "Fixed";
    break;
  case 3:
    stringy = "Pulse";
    break;
  case 4:
    stringy = "Ramp";
    break;
  case 5:
    stringy = "RampBoth";
    break;
  case 6:
    stringy = "Exponential";
    break;
  case 7:
    stringy = "Random";
    break;
  default:
    break;
  }
  return stringy;
}

// Calls patterns from table
void callTable(int pattern, int motor)
{
  switch (pattern)
  {
  case 1:
    fixedSingle(0, motor);
    break;
  case 2:
    fixedSingle(vmaxList[indexTable], motor);
    break;
  case 3:
    pulseCustom(vminList[indexTable], vmaxList[indexTable], motor, periodList[indexTable]);
    break;
  case 4:
    rampCustom(vminList[indexTable], vmaxList[indexTable], stepList[indexTable], motor, periodList[indexTable]);
    break;
  case 5:
    rampBothCustom(vminList[indexTable], vmaxList[indexTable], stepList[indexTable], motor, periodList[indexTable]);
    break;
  case 6:
    sharpRampCustom(motor);
    break;
  case 7:
    randiCustom(vminList[indexTable], vmaxList[indexTable], motor, periodList[indexTable]);
    break;
  default:
    break;
  }
}

BLYNK_WRITE(V41)
{
  switch (param.asInt())
  {
  case 1:
    filename = "/pattern1.json";
    break;
  case 2:
    filename = "/pattern2.json";
    break;
  case 3:
    filename = "/pattern3.json";
    break;
  case 4:
    filename = "/pattern4.json";
    break;
  }
}

BLYNK_WRITE(V42)
{
  durationTemp = param.asInt();
}
BLYNK_WRITE(V43)
{
  vminTemp = param.asInt();
}
BLYNK_WRITE(V44)
{
  vmaxTemp = param.asInt();
}
BLYNK_WRITE(V45)
{
  stepTemp = param.asInt();
}
BLYNK_WRITE(V46)
{
  periodTemp = param.asInt();
}
BLYNK_WRITE(V47)
{
  if (param.asInt() && indexBlynkTable < 10)
  {
    stringy2 = "";
    stringy2 = namePattern(patternTemp1) + "  " + namePattern(patternTemp2) + "  " + namePattern(patternTemp3);
    table.addRow(indexBlynkTable, stringy2, durationTemp); // Good luck understanding this!
    patternList1[indexBlynkTable] = patternTemp1;
    patternList2[indexBlynkTable] = patternTemp2;
    patternList3[indexBlynkTable] = patternTemp3;
    durationList[indexBlynkTable] = durationTemp * 1000;
    vminList[indexBlynkTable] = vminTemp;
    vmaxList[indexBlynkTable] = vmaxTemp;
    stepList[indexBlynkTable] = stepTemp;
    periodList[indexBlynkTable] = periodTemp;
    used = indexBlynkTable + 1;
    indexBlynkTable++;
  }
}
BLYNK_WRITE(V48)
{
  if (param.asInt())
  {
    table.clear();
    indexBlynkTable = 0;
    used = 0;
  }
}
BLYNK_WRITE(V49)
{
  if (param.asInt())
  {
    saveConfiguration(filename);
  }
}
BLYNK_WRITE(V50)
{
  if (param.asInt())
  {
    loadConfiguration(filename);
    table.clear();
    for (i = 0; i < used; i++)
    {
      //table.addRow(i, patternList1[i], durationList[i]/1000);
      stringy2.clear();
      stringy2 = namePattern(patternList1[i]) + "  " + namePattern(patternList2[i]) + "  " + namePattern(patternList3[i]);
      table.addRow(i, stringy2, durationList[i] / 1000); // This one's worse! Ha!
    }
  }
}
BLYNK_WRITE(V51)
{
  patternTemp1 = param.asInt();
}
BLYNK_WRITE(V52)
{
  patternTemp2 = param.asInt();
}
BLYNK_WRITE(V53)
{
  patternTemp3 = param.asInt();
}
BLYNK_WRITE(V54)
{
  tableMode = param.asInt();
  if (tableMode)
  {
    lasttimetime = 0;
    indexTable = 0;
    lcd.clear();
    lcd.print(2, 0, "Now running:");
    lcd.print(2, 1, "Program mode");
  }
  else
    lcd.clear();
}
