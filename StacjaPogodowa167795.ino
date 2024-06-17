#include <M5Core2.h>
#include <ArduinoJson.h>
#include "M5_ENV.h"
#include "WiFi.h"
#include "math.h"
#include "PubSubClient.h"
#include <SD.h>

#define SSID "mikronapedy"
#define PASSWORD "12345678"


#define MAX_MEASUREMENTS 72 // Maksymalna liczba pomiarów do zachowania
int8_t curr_page = 1; 
int8_t curr_subpage = 1;
int8_t subpage_active = 0;
int8_t subpage_number = 1;
char disp_refresh = 1;
int TMax = 26;
int TMin = 15;
int TMinMaxSetter = 0;
int x = 0;
int HMax = 45;
int HMin = 10;
int HMinMaxSetter = 0;
int datePart = 1;
int timePart = 1;
int PMax = 1100;
int PMin = 900;
int PMinMaxSetter = 0;
int sDay = 1;
int sMonth = 1;
int sYear = 1970;
int sHour = 1;
int sMinute = 1;
int setMinMaxSetter = 1;
int change = 1;
int prev_minute = -1;
int liczba_linii = 0;
int Tcount = 0;
int Pcount = 0;
int Hcount = 0;
int MHcount = 0;
int paramVisibility = 1;
int CarouseleScreenMode = 1;
int CarouseleTime = 0;
int TapTime = 0;
TouchPoint_t touchPos;
TouchPoint_t pos; // Touch point position
bool touchPressed = false; // Is Touchpanel pressed?
bool upBtnOn = false;
bool dnBtnOn = false;
static unsigned long previousMillis = 0;
unsigned long currentMillis = 1;
char day_buf[10];
int day = 0;
char month_buf[10];
int month = 0;
char year_buf[10];
int year = 0;
char hour_buf[10];
int hour = 0;
char minute_buf[10];
int minute = 0;
char second_buf[10];
int second = 0;
HotZone UpBtn(235, 120, 315, 200); // TouchZone: (x1,y1,x2,y2)
HotZone DnBtn(5, 120, 85, 200);
HotZone AcceptBtn(120, 120, 200, 200);

HotZone TDDn(0, 120, 79, 200);
HotZone TDNx(81, 120, 159, 200);
HotZone TDUp(161, 120, 239, 200);
HotZone TDAccept(241, 120, 319, 200);

HotZone buttonA(5, 240, 100, 300); // Przycisk A
HotZone buttonB(110, 240, 210, 300); // Przycisk B
HotZone buttonC(220, 240, 310, 300); // Przycisk C

HotZone touchArea(0, 0, 320, 240);
int plusX, plusY, minusX, minusY;
SHT3X sht30;
QMP6988 qmp6988;
RTC_TimeTypeDef RTCTime;
RTC_DateTypeDef RTCDate;
bool wasPressedLastLoop = false;
float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;
unsigned long BtnCheckTime = 0;
unsigned long AlarmTime = 0;

float * temps;
float * hums;
float * pres;
float * hours;
int changeParam = 0;


const char* mqtt_server = "broker.mqttdashboard.com";
WiFiClient espClient;
void callback(char* topic, byte* payload, unsigned int length);
PubSubClient client(mqtt_server, 1883, callback, espClient);
bool connected;

void setupWifi();
void conMqtt();
void setDate(int y, int m, int d);
void setTime(int h, int m, int s);
int getDay();
int getMonth();
int getYear();
int getHour();
int getMinute();
int getSecond();
float getCurrTemp();
float getCurrHum();
float getCurrPres();
void saveParametersToFiles(float temperature, float humidity, float pressure);
int countLinesInFile(const char * filename);
String readLinesFromFile(const char * filename, int startLine);
float calculate_mean(float readings[], int n);
float find_maximum(float readings[], int n);
float find_minimum(float readings[], int n);
float * getTemps();
float * getHums();
float * getPres();
float * getMeasureHours();
bool isDateValid(int day, int month, int year);
void scr1();
void scr2();
void scr3();
void scr4();
void scr5();
void scr11();
void scr12();
void scr13();
void scr14();
void scr21();
void scr22();
void scr23();
void scr24();
void scr31();
void scr32();
void scr33();
void scr34();
void scr41();
void scr42();
void saveTMax(float v);
void saveTMin(float v);
void saveHMax(float v);
void saveHMin(float v);
void savePMax(float v);
void savePMin(float v);
void menuButtons();
void boundsButtons();
void dateButtons();
void tapButton();
void cycleMode();
void measureAndSaveForEachHour();
float readBound(const char* filename, float def);
void xd();
void setup() {
  Wire.begin(); 
  qmp6988.init();
  M5.begin();
  TMax = readBound("/tub.txt", TMax);
  TMin = readBound("/tlb.txt", TMin);
  HMax = readBound("/hub.txt", HMax);
  HMin = readBound("/hlb.txt", HMin);
  PMax = readBound("/pub.txt", PMax);
  PMin = readBound("/plb.txt", PMin);
  setDate(30, 5, 2024);
  setTime(14, 58, 25);
  previousMillis = millis();
  AlarmTime = millis();
  CarouseleTime = millis();
  TapTime = millis();
  
  setupWifi();
  conMqtt();

}

void loop() {
  M5.update();
  if (curr_page > 4) {
    CarouseleScreenMode == -1;
  }
  tapButton();
  cycleMode();
  currentMillis = millis();
  measureAndSaveForEachHour();
  if (!client.connected()) {
        conMqtt(); // Połącz się z serwerem MQTT, jeśli nie jesteś połączony
    }
    client.loop(); // Obsługa klienta MQTT

    
  switch (curr_page) {
  case 1:
    scr1();
    break;
  case 2:
    scr2();
    break;

  case 3:
    scr3();
    break;

  case 4:
    scr4();
    break;
  case 5:
    scr5();
    break;
  case 11:
    scr11();

    break;

  case 12:
    scr12();
    break;

  case 13:
    scr13();
    break;
  case 14:
    scr14();

    break;

  case 21:
    scr21();

    break;

  case 22:
    scr22();

    break;

  case 23:
    scr23();

    break;

  case 24:
    scr24();

    break;

  case 31:
    scr31();

    break;

  case 32:
    scr32();

    break;

  case 33:
    scr33();

    break;

  case 34:
    scr34();

    break;

  case 41:
    scr41();

    break;

  case 42:
    scr42();

    break;

  }

  touchPos = M5.Touch.getPressPoint();
  menuButtons();
  boundsButtons();
  dateButtons();

}



int getDay() {
  M5.Rtc.GetDate( & RTCDate);
  return RTCDate.Date;
}
int getMonth() {
  M5.Rtc.GetDate( & RTCDate);
  return RTCDate.Month;
}
int getYear() {
  M5.Rtc.GetDate( & RTCDate);
  return RTCDate.Year;
}
int getHour() {
  M5.Rtc.GetTime( & RTCTime);
  return RTCTime.Hours;
}
int getMinute() {
  M5.Rtc.GetTime( & RTCTime);
  return RTCTime.Minutes;
}
int getSecond() {
  M5.Rtc.GetTime( & RTCTime);
  return RTCTime.Seconds;
}

void saveParametersToFiles(float temperature, float humidity, float pressure) {

  File fileT = SD.open("/temperatures.txt", FILE_APPEND);
  File fileH = SD.open("/humidities.txt", FILE_APPEND);
  File fileP = SD.open("/pressures.txt", FILE_APPEND);
  File fileMH = SD.open("/measureHours.txt", FILE_APPEND);
  int hour = getHour();
  int minute = getMinute();

  fileT.print(temperature);
  fileT.print('\n');

  fileH.print(humidity);
  fileH.print('\n');

  fileP.print(pressure);
  fileP.print('\n');
  if(minute<30)fileMH.print(getHour());
  else fileMH.print(getHour()+1);
  fileMH.print('\n');


  fileT.close();
  fileH.close();
  fileP.close();
  fileMH.close();

  int linesT = countLinesInFile("/temperatures.txt");
  int linesH = countLinesInFile("/humidities.txt");
  int linesP = countLinesInFile("/pressures.txt");
  int linesMH = countLinesInFile("/measureHours.txt");
  if (linesT >= MAX_MEASUREMENTS) {
    String TContent = readLinesFromFile("/temperatures.txt", linesT - MAX_MEASUREMENTS);
    // Usuń plik
    SD.remove("/temperatures.txt");
    // Utwórz nowy plik i zapisz do niego odczytane linie
    File newFileT = SD.open("/temperatures.txt", FILE_WRITE);
    if (newFileT) {
      newFileT.print(TContent);
      newFileT.close();
    } 
  }

  if (linesH >= MAX_MEASUREMENTS) {
    String HContent = readLinesFromFile("/humidities.txt", linesH - MAX_MEASUREMENTS);
    // Usuń plik
    SD.remove("/humidities.txt");
    // Utwórz nowy plik i zapisz do niego odczytane linie
    File newFileH = SD.open("/humidities.txt", FILE_WRITE);
    if (newFileH) {
      newFileH.print(HContent);
      newFileH.close();
    } 
  }

  if (linesP >= MAX_MEASUREMENTS) {
    String PContent = readLinesFromFile("/pressures.txt", linesP - MAX_MEASUREMENTS);
    // Usuń plik
    SD.remove("/pressures.txt");
    // Utwórz nowy plik i zapisz do niego odczytane linie
    File newFileP = SD.open("/pressures.txt", FILE_WRITE);
    if (newFileP) {
      newFileP.print(PContent);
      newFileP.close();
    } 
  }

  if (linesMH >= MAX_MEASUREMENTS) {
    String MHContent = readLinesFromFile("/measureHours.txt", linesMH - MAX_MEASUREMENTS);
    // Usuń plik
    SD.remove("/measureHours.txt");
    // Utwórz nowy plik i zapisz do niego odczytane linie
    File newFileMH = SD.open("/measureHours.txt", FILE_WRITE);
    if (newFileMH) {
      newFileMH.print(MHContent);
      newFileMH.close();
    } 
  }

}

int countLinesInFile(const char * filename) {
  File file = SD.open(filename, FILE_READ);
  if (!file) {
    return -1; 
  }

  int lineCount = 0;
  while (file.available()) {
    if (file.read() == '\n') {
      lineCount++;
    }
  }
  file.close();
  return lineCount;
}

String readLinesFromFile(const char * filename, int startLine) {

  File file = SD.open(filename, FILE_READ);
  if (!file) {
    return "";
  }

  // Przesuń kursor do linii startowej
  for (int i = 0; i < startLine; i++) {
    file.readStringUntil('\n');
  }

  // Zapisz pozostałe linie do zmiennej
  String lines = "";
  while (file.available()) {
    lines += file.readStringUntil('\n');
    lines += "\n";
  }

  // Zamknij plik
  file.close();

  return lines;
}

void setDate(int d, int m, int y) {
  RTCDate.Year = y;
  RTCDate.Month = m;
  RTCDate.Date = d;
  M5.Rtc.SetDate( & RTCDate);
}
void setTime(int h, int m, int s) {
  RTCTime.Hours = h;
  RTCTime.Minutes = m;
  RTCTime.Seconds = s;
  M5.Rtc.SetTime( & RTCTime);
}

// Funkcja do obliczania średniej
float calculate_mean(float * readings, int n) {
  float sum = 0.0;
  for (int i = 0; i < n; ++i) {
    sum += readings[i];
  }
  if (sum!=0) return sum/n;
  else return 0;
  
}

// Funkcja do znajdowania maksimum
float find_maximum(float * readings, int n) {
  float maxVal = readings[0];
  for (int i = 1; i < n; ++i) {
    if (readings[i] > maxVal) {
      maxVal = readings[i];
    }
  }
  return maxVal;
}

// Funkcja do znajdowania minimum
float find_minimum(float * readings, int n) {
  float minVal = readings[0];
  for (int i = 1; i < n; ++i) {
    if (readings[i] < minVal) {
      minVal = readings[i];
    }
  }
  return minVal;
}

void scr1() {
  if (temperature > TMax || temperature < TMin) {

    if ((currentMillis - AlarmTime >= 1000)) {
      paramVisibility = paramVisibility * (-1);
      AlarmTime = currentMillis;
      change = 1;
    }
  }

  if (change) {
    if (sht30.get() == 0) {
      temperature = sht30.cTemp;

    } else {
      temperature = 0;
    }

    char buf_temp[8];
    sprintf(buf_temp, "%.1f", temperature);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(RED);
    if (CarouseleScreenMode == -1) {
      M5.Lcd.setTextSize(1);
      M5.Lcd.setTextDatum(TL_DATUM);
      M5.Lcd.drawString("||", 0, 0, 4);
    }
    M5.Lcd.setTextSize(1);

    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("Temperature [C]", 160, 20, 4);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf(" more");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");

    if (paramVisibility == 1 || (temperature > TMin && temperature < TMax)) {
      M5.Lcd.setTextSize(5);
      M5.Lcd.drawString(buf_temp, 160, 120, 4);
    } else {
      M5.Lcd.setTextSize(5);
      M5.Lcd.drawString("! ! !", 160, 120, 4);
    }

    change = 0;
  }
}
void scr2() {
  if (humidity > HMax || humidity < HMin) {

    if ((currentMillis - AlarmTime >= 1000)) {
      paramVisibility = paramVisibility * (-1);
      AlarmTime = currentMillis;
      change = 1;
    }
  }
  if (change) {
    if (sht30.get() == 0) {
      humidity = sht30.humidity;
    } else {
      humidity = 0;
    }

    char buf_hum[8];
    M5.Lcd.fillScreen(BLACK);
    sprintf(buf_hum, "%.1f", humidity);
    if (CarouseleScreenMode == -1) {
      M5.Lcd.setTextColor(BLUE);
      M5.Lcd.setTextSize(1);
      M5.Lcd.setTextDatum(TL_DATUM);
      M5.Lcd.drawString("||", 0, 0, 4);
    }

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(BLUE);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("Humidity [%]", 160, 20, 4);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf(" more");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");
    if (paramVisibility == 1 || (humidity > HMin && humidity < HMax)) {
      M5.Lcd.setTextSize(5);
      M5.Lcd.drawString(buf_hum, 160, 120, 4);
    } else {
      M5.Lcd.setTextSize(5);
      M5.Lcd.drawString("! ! !", 160, 120, 4);
    }
    change = 0;
  }
}
void scr3() {
  if (temperature > PMax || pressure < PMin) {

    if ((currentMillis - AlarmTime >= 1000)) {
      paramVisibility = paramVisibility * (-1);
      AlarmTime = currentMillis;
      change = 1;
    }
  }
  if (change) {
    pressure = round(qmp6988.calcPressure() / 100);
    char buf_press[8];
    sprintf(buf_press, "%g", pressure);
    M5.Lcd.fillScreen(BLACK);
    if (CarouseleScreenMode == -1) {
      M5.Lcd.setTextColor(YELLOW);
      M5.Lcd.setTextSize(1);
      M5.Lcd.setTextDatum(TL_DATUM);
      M5.Lcd.drawString("||", 0, 0, 4);
    }
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("Pressure [hPa]", 160, 20, 4);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf(" more");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");
    if (paramVisibility == 1 || (pressure > PMin && pressure < PMax)) {
      M5.Lcd.setTextSize(5);
      M5.Lcd.drawString(buf_press, 160, 120, 4);
    } else {
      M5.Lcd.setTextSize(5);
      M5.Lcd.drawString("! ! !", 160, 120, 4);
    }
    change = 0;
  }
}
void scr4() {

  if (change) {
    day = getDay();
    month = getMonth();
    year = getYear();
    hour = getHour();
    minute = getMinute();

    second = getSecond();
    char buf_time[10];
    char buf_date[15];
    M5.Lcd.setTextColor(WHITE);

    if (day < 10) {
      sprintf(day_buf, "0%d.", day);
    } else {
      sprintf(day_buf, "%d.", day);
    }
    if (month < 10) {
      sprintf(month_buf, "0%d.", month);
    } else {
      sprintf(month_buf, "%d.", month);
    }
    sprintf(year_buf, "%d", year);
    if (hour < 10) {
      sprintf(hour_buf, "0%d:", hour);
    } else {
      sprintf(hour_buf, "%d:", hour);
    }
    if (minute < 10) {
      sprintf(minute_buf, "0%d", minute);
    } else {
      sprintf(minute_buf, "%d", minute);
    }
    strcpy(buf_date, day_buf);
    strcat(buf_date, month_buf);
    strcat(buf_date, year_buf);
    strcpy(buf_time, hour_buf);
    strcat(buf_time, minute_buf);

    M5.Lcd.fillScreen(BLACK);
    if (CarouseleScreenMode == -1) {
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.setTextSize(1);
      M5.Lcd.setTextDatum(TL_DATUM);
      M5.Lcd.drawString("||", 0, 0, 4);
    }
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString(buf_date, 160, 40, 4);
    M5.Lcd.setTextSize(5);
    M5.Lcd.drawString(buf_time, 160, 140, 4);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf(" more");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");
    change = 0;
  }
}
void scr5(){
  if(change){
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("Use buttons below screen",5,5,4);
    M5.Lcd.drawString("to navigate between screens",5,40,4);
    M5.Lcd.drawString("To on/off params in cycle",5,75,4);
    M5.Lcd.drawString("mode tap the screen.",5,110,4);
    M5.Lcd.drawString("\"More\" button for show stats,",5,145,4);
    M5.Lcd.drawString("plots, set alarm bounds,",5,180,4); 
    M5.Lcd.drawString("and set date/time\n.",5,215,4);
    change=0;
  }
  
}

void scr13() {
  if (change) {
    if (setMinMaxSetter) {
      TMinMaxSetter = TMax;
    }
    setMinMaxSetter = 0;
    char t_buf[8];
    sprintf(t_buf, "%d", TMinMaxSetter);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawRect(235, 120, 80, 80, RED); // Przycisk prawy
    M5.Lcd.drawRect(5, 120, 80, 80, RED); //przycisk lewy
    M5.Lcd.drawRect(120, 120, 80, 80, RED); //przycisk lewy
    M5.Lcd.setTextSize(4);
    plusX = (280 - 200) / 2 + 200 - M5.Lcd.textWidth("+") / 2;
    plusY = (200 - 120) / 2 + 120 - M5.Lcd.fontHeight() / 2;

    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("-", 45, 160, 4);
    M5.Lcd.drawString("+", 275, 160, 4);
  
    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("Temp. max (alarm limit)", 5, 15, 4);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("3/4", 315, 15, 4);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("ok", 160, 160, 4);
    M5.Lcd.setTextSize(3);
    M5.Lcd.drawString(t_buf, 160, 80, 4);

    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");
    change = 0;
  }
}
void scr14() {
  if (change) {
    if (setMinMaxSetter) {
      TMinMaxSetter = TMin;
    }
    setMinMaxSetter = 0;
    char t_buf[8];
    sprintf(t_buf, "%d", TMinMaxSetter);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawRect(235, 120, 80, 80, RED); // Przycisk prawy
    M5.Lcd.drawRect(5, 120, 80, 80, RED); //przycisk lewy
    M5.Lcd.drawRect(120, 120, 80, 80, RED); //przycisk lewy
    M5.Lcd.setTextSize(4);
    plusX = (280 - 200) / 2 + 200 - M5.Lcd.textWidth("+") / 2;
    plusY = (200 - 120) / 2 + 120 - M5.Lcd.fontHeight() / 2;
    M5.Lcd.setTextSize(4);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("-", 45, 160, 4);
    M5.Lcd.drawString("+", 275, 160, 4);
    //M5.Lcd.setCursor(plusX, plusY);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("ok", 160, 160, 4);

    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("Temp. min (alarm limit)", 5, 15, 4);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("4/4", 315, 15, 4);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString(t_buf, 160, 80, 4);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");

    change = 0;
  }
}
void scr11() {
  if (change) {

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("1/4", 315, 15, 4);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.drawString("Temp. stats (last 3 days)", 5, 15, 4);
    char Tmean[10];
    char Tmax[10];
    char Tmin[10];

    float * temps = getTemps();
    float mean = 0;
    mean = calculate_mean(temps, Tcount);
    float maxVal = find_maximum(temps, Tcount);
    float minVal = find_minimum(temps, Tcount);
    sprintf(Tmean, "%.1f", mean);
    sprintf(Tmax, "%.1f", maxVal);
    sprintf(Tmin, "%.1f", minVal);
    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextSize(2);
    M5.Lcd.drawString("Avg: ", 20, 80, 4);
    M5.Lcd.drawString(Tmean, 140, 80, 4);

    M5.Lcd.drawString("Max: ", 20, 130, 4);
    M5.Lcd.drawString(Tmax, 140, 130, 4);

    M5.Lcd.drawString("Min: ", 20, 180, 4);
    M5.Lcd.drawString(Tmin, 140, 180, 4);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");
    delete[] temps;

    change = 0;
  }
}
void scr12() {
  if (change) {

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawLine(60, 20, 60, 219, WHITE); // Oś y
    M5.Lcd.drawLine(51, 200, 300, 200, WHITE); // Oś x

    // Strzałka na osi y
    M5.Lcd.drawLine(60, 20, 55, 25, WHITE);
    M5.Lcd.drawLine(60, 20, 65, 25, WHITE);

    // Strzałka na osi x
    M5.Lcd.drawLine(300, 200, 295, 195, WHITE);
    M5.Lcd.drawLine(300, 200, 295, 205, WHITE);

    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("2/4", 315, 15, 4);

    float * temps = getTemps();
    float * hours = getMeasureHours();
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(GREEN);
    for (int i = 1; i < 72; i++) {
      int x1 = map(i - 1, 0, 71, 60, 300); // Mapowanie indeksu poprzedniego punktu na osi x
      int y1 = map(round(temps[i - 1]), (TMin), (TMax), 180, 20); // Mapowanie temperatury poprzedniego punktu na osi y
      int x2 = map(i, 0, 71, 60, 300); // Mapowanie bieżącego indeksu na osi x
      int y2 = map(round(temps[i]), (TMin), (TMax), 180, 20); // Mapowanie bieżącej temperatury na osi y
      M5.Lcd.drawLine(x1, y1, x2, y2, YELLOW);
      // Rysowanie linii łączącej punkty
    }
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.drawString("T[C]", 0, 100, 2);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.drawString("Hour", 310, 215, 2);

    // Podpisy godzin co 12 godzin
    for (int i = 0; i < 72; i += 12) {
      int hourIndex = map(i, 0, 71, 60, 300); // Mapowanie indeksu godziny na osi x
      M5.Lcd.setTextDatum(MC_DATUM);
      M5.Lcd.drawString(String(int(hours[i])), hourIndex, 210, 2); // Wyświetlenie godziny
    }

    // Podpisy wartości na osi Y i kreski poziome
    int stepSize = (TMax - TMin) / 5; // Rozmiar kroku pomiędzy podpisami
    for (int i = 0; i <= 5; i++) {
      M5.Lcd.setTextColor(WHITE);
      float tempValue = TMin + i * stepSize; // Wartość temperatury dla danego podpisu
      int yValue = map(tempValue, TMin, TMax, 180, 20); // Mapowanie wartości temperatury na osi Y
      M5.Lcd.setTextDatum(MR_DATUM);
      M5.Lcd.drawString(String(int(tempValue)), 50, yValue, 2); // Wyświetlenie podpisu na osi Y
      M5.Lcd.drawLine(60, yValue, 300, yValue, WHITE); // Rysowanie kreski poziomej
    }


    int newYMin = 30; // Minimalna wartość Y na wykresie
    int newYMax = 170; // Maksymalna wartość Y na wykresie

    // Linie poziome reprezentujące poziomy ostrzegawcze TMin i TMax
    int yWarningMin = map(TMin, TMin, TMax, newYMax, newYMin);
    int yWarningMax = map(TMax, TMin, TMax, newYMax, newYMin);
    M5.Lcd.drawLine(60, yWarningMin, 300, yWarningMin, RED); // Linia ostrzeżenia dla TMin
    M5.Lcd.drawLine(60, yWarningMax, 300, yWarningMax, RED); // Linia ostrzeżenia dla TMax
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");

    delete[] temps;
    delete[] hours;
    change = 0;
  }
}
void scr21() {
  if (change) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(BLUE);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("1/4", 315, 15, 4);
    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("Hum. stats (last 3 days)", 5, 15, 4);
    char Hmean[10];
    char Hmax[10];
    char Hmin[10];

    float * hums = getHums();
    float mean = 0;
    mean = calculate_mean(hums, Hcount);
    float maxVal = find_maximum(hums, Hcount);
    float minVal = find_minimum(hums, Hcount);
    sprintf(Hmean, "%.1f", mean);
    sprintf(Hmax, "%.1f", maxVal);
    sprintf(Hmin, "%.1f", minVal);
    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextSize(2);
    M5.Lcd.drawString("Avg: ", 20, 80, 4);
    M5.Lcd.drawString(Hmean, 140, 80, 4);

    M5.Lcd.drawString("Max: ", 20, 130, 4);
    M5.Lcd.drawString(Hmax, 140, 130, 4);

    M5.Lcd.drawString("Min: ", 20, 180, 4);
    M5.Lcd.drawString(Hmin, 140, 180, 4);

   
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");
    delete[] hums;
    change = 0;
  }
}
void scr22() {
  if (change) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawLine(60, 20, 60, 219, WHITE); // Oś y
    M5.Lcd.drawLine(51, 200, 300, 200, WHITE); // Oś x

    // Strzałka na osi y
    M5.Lcd.drawLine(60, 20, 55, 25, WHITE);
    M5.Lcd.drawLine(60, 20, 65, 25, WHITE);

    // Strzałka na osi x
    M5.Lcd.drawLine(300, 200, 295, 195, WHITE);
    M5.Lcd.drawLine(300, 200, 295, 205, WHITE);

    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("2/4", 315, 15, 4);

    float * hums = getHums();
    float * hours = getMeasureHours();
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(GREEN);
    for (int i = 1; i < 72; i++) {
      int x1 = map(i - 1, 0, 71, 60, 300); // Mapowanie indeksu poprzedniego punktu na osi x
      int y1 = map(round(hums[i - 1]), (HMin), (HMax), 180, 20); // Mapowanie temperatury poprzedniego punktu na osi y
      int x2 = map(i, 0, 71, 60, 300); // Mapowanie bieżącego indeksu na osi x
      int y2 = map(round(hums[i]), (HMin), (HMax), 180, 20); // Mapowanie bieżącej temperatury na osi y
      M5.Lcd.drawLine(x1, y1, x2, y2, YELLOW);
      // Rysowanie linii łączącej punkty
    }
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.drawString("H[%]", 0, 100, 2);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.drawString("Hour", 310, 215, 2);

    // Podpisy godzin co 12 godzin
    for (int i = 0; i < 72; i += 12) {
      int hourIndex = map(i, 0, 71, 60, 300); // Mapowanie indeksu godziny na osi x
      M5.Lcd.setTextDatum(MC_DATUM);
      M5.Lcd.drawString(String(int(hours[i])), hourIndex, 210, 2); // Wyświetlenie godziny
    }

    // Podpisy wartości na osi Y i kreski poziome
    int stepSize = (HMax - HMin) / 5; // Rozmiar kroku pomiędzy podpisami
    for (int i = 0; i <= 5; i++) {
      M5.Lcd.setTextColor(WHITE);
      float humValue = HMin + i * stepSize; // Wartość temperatury dla danego podpisu
      int yValue = map(humValue, HMin, HMax, 180, 20); // Mapowanie wartości temperatury na osi Y
      M5.Lcd.setTextDatum(MR_DATUM);
      M5.Lcd.drawString(String(int(humValue)), 50, yValue, 2); // Wyświetlenie podpisu na osi Y
      M5.Lcd.drawLine(60, yValue, 300, yValue, WHITE); // Rysowanie kreski poziomej
    }

    
    int newYMin = 30; // Minimalna wartość Y na wykresie
    int newYMax = 170; // Maksymalna wartość Y na wykresie

    // Linie poziome reprezentujące poziomy ostrzegawcze TMin i TMax
    int yWarningMin = map(HMin, HMin, HMax, newYMax, newYMin);
    int yWarningMax = map(HMax, HMin, HMax, newYMax, newYMin);
    M5.Lcd.drawLine(60, yWarningMin, 300, yWarningMin, RED); // Linia ostrzeżenia dla TMin
    M5.Lcd.drawLine(60, yWarningMax, 300, yWarningMax, RED); // Linia ostrzeżenia dla TMax
    M5.Lcd.setTextColor(BLUE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");

    delete[] hums;
    delete[] hours;
    change = 0;
  }
}
void scr23() {
  if (change) {
    if (setMinMaxSetter) {
      HMinMaxSetter = HMax;
    }
    setMinMaxSetter = 0;
    char h_buf[8];
    sprintf(h_buf, "%d", HMinMaxSetter);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawRect(235, 120, 80, 80, BLUE); 
    M5.Lcd.drawRect(5, 120, 80, 80, BLUE); 
    M5.Lcd.drawRect(120, 120, 80, 80, BLUE); 
    M5.Lcd.setTextSize(4);
    plusX = (280 - 200) / 2 + 200 - M5.Lcd.textWidth("+") / 2;
    plusY = (200 - 120) / 2 + 120 - M5.Lcd.fontHeight() / 2;

    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("-", 45, 160, 4);
    M5.Lcd.drawString("+", 275, 160, 4);

    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("Hum. max (alarm limit)", 0, 20, 4);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("3/4", 315, 15, 4);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("ok", 160, 160, 4);
    M5.Lcd.setTextSize(3);
    M5.Lcd.drawString(h_buf, 160, 80, 4);

    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");
    change = 0;
  }
}
void scr24() {
  if (change) {
    if (setMinMaxSetter) {
      HMinMaxSetter = HMin;
    }
    setMinMaxSetter = 0;
    char h_buf[8];
    sprintf(h_buf, "%d", HMinMaxSetter);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawRect(235, 120, 80, 80, BLUE); 
    M5.Lcd.drawRect(5, 120, 80, 80, BLUE); 
    M5.Lcd.drawRect(120, 120, 80, 80, BLUE); 
    M5.Lcd.setTextSize(4);
    plusX = (280 - 200) / 2 + 200 - M5.Lcd.textWidth("+") / 2;
    plusY = (200 - 120) / 2 + 120 - M5.Lcd.fontHeight() / 2;

    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("-", 45, 160, 4);
    M5.Lcd.drawString("+", 275, 160, 4);

    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("Hum. min (alarm limit)", 0, 20, 4);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("4/4", 315, 15, 4);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("ok", 160, 160, 4);
    M5.Lcd.setTextSize(3);
    M5.Lcd.drawString(h_buf, 160, 80, 4);

    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");
    change = 0;
  }
}
void scr31() {
  if (change) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("1/4", 315, 15, 4);
    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("Pres. stats (last 3 days)", 5, 15, 4);
    char Pmean[10];
    char Pmax[10];
    char Pmin[10];
    float * pres = getPres();
    float mean = 0;
    mean = calculate_mean(pres, Pcount);
    float maxVal = find_maximum(pres, Pcount);
    float minVal = find_minimum(pres, Pcount);
    sprintf(Pmean, "%.1f", mean);
    sprintf(Pmax, "%.1f", maxVal);
    sprintf(Pmin, "%.1f", minVal);
    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextSize(2);
    M5.Lcd.drawString("Avg: ", 20, 80, 4);
    M5.Lcd.drawString(Pmean, 140, 80, 4);

    M5.Lcd.drawString("Max: ", 20, 130, 4);
    M5.Lcd.drawString(Pmax, 140, 130, 4);

    M5.Lcd.drawString("Min: ", 20, 180, 4);
    M5.Lcd.drawString(Pmin, 140, 180, 4);

    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");
    delete[] pres;
    change = 0;
  }
}
void scr32() {
  if (change) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawLine(60, 20, 60, 219, WHITE); // Oś y
    M5.Lcd.drawLine(51, 200, 300, 200, WHITE); // Oś x

    // Strzałka na osi y
    M5.Lcd.drawLine(60, 20, 55, 25, WHITE);
    M5.Lcd.drawLine(60, 20, 65, 25, WHITE);

    // Strzałka na osi x
    M5.Lcd.drawLine(300, 200, 295, 195, WHITE);
    M5.Lcd.drawLine(300, 200, 295, 205, WHITE);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("2/4", 315, 15, 4);

    float * pres = getPres();
    float * hours = getMeasureHours();
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(GREEN);
    for (int i = 1; i < 72; i++) {
      int x1 = map(i - 1, 0, 71, 60, 300); // Mapowanie indeksu poprzedniego punktu na osi x
      int y1 = map(round(pres[i - 1]), (PMin), (PMax), 180, 20); // Mapowanie temperatury poprzedniego punktu na osi y
      int x2 = map(i, 0, 71, 60, 300); // Mapowanie bieżącego indeksu na osi x
      int y2 = map(round(pres[i]), (PMin), (PMax), 180, 20); // Mapowanie bieżącej temperatury na osi y
      M5.Lcd.drawLine(x1, y1, x2, y2, YELLOW);
      // Rysowanie linii łączącej punkty
    }
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.drawString("P[hPa]", 0, 100, 2);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.drawString("Hour", 310, 215, 2);

    // Podpisy godzin co 12 godzin
    for (int i = 0; i < 72; i += 12) {
      int hourIndex = map(i, 0, 71, 60, 300); // Mapowanie indeksu godziny na osi x
      M5.Lcd.setTextDatum(MC_DATUM);
      M5.Lcd.drawString(String(int(hours[i])), hourIndex, 210, 2); // Wyświetlenie godziny
    }

    // Podpisy wartości na osi Y i kreski poziome
    int stepSize = (PMax - PMin) / 5; // Rozmiar kroku pomiędzy podpisami
    for (int i = 0; i <= 5; i++) {
      M5.Lcd.setTextColor(WHITE);
      float presValue = PMin + i * stepSize; // Wartość temperatury dla danego podpisu
      int yValue = map(presValue, PMin, PMax, 180, 20); // Mapowanie wartości temperatury na osi Y
      M5.Lcd.setTextDatum(MR_DATUM);
      M5.Lcd.drawString(String(int(presValue)), 50, yValue, 2); // Wyświetlenie podpisu na osi Y
      M5.Lcd.drawLine(60, yValue, 300, yValue, WHITE); // Rysowanie kreski poziomej
    }

   
    int newYMin = 30; // Minimalna wartość Y na wykresie
    int newYMax = 170; // Maksymalna wartość Y na wykresie

    // Linie poziome reprezentujące poziomy ostrzegawcze TMin i TMax
    int yWarningMin = map(PMin, PMin, PMax, newYMax, newYMin);
    int yWarningMax = map(PMax, PMin, PMax, newYMax, newYMin);
    M5.Lcd.drawLine(60, yWarningMin, 300, yWarningMin, RED); // Linia ostrzeżenia dla TMin
    M5.Lcd.drawLine(60, yWarningMax, 300, yWarningMax, RED); // Linia ostrzeżenia dla TMax
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");

    delete[] pres;
    delete[] hours;
    change = 0;
  }
}
void scr33() {
  if (change) {
    if (setMinMaxSetter) {
      PMinMaxSetter = PMax;
    }
    setMinMaxSetter = 0;
    char p_buf[8];
    sprintf(p_buf, "%d", PMinMaxSetter);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawRect(235, 120, 80, 80, YELLOW); // Przycisk prawy
    M5.Lcd.drawRect(5, 120, 80, 80, YELLOW); //przycisk lewy
    M5.Lcd.drawRect(120, 120, 80, 80, YELLOW); //przycisk lewy
    M5.Lcd.setTextSize(4);
    plusX = (280 - 200) / 2 + 200 - M5.Lcd.textWidth("+") / 2;
    plusY = (200 - 120) / 2 + 120 - M5.Lcd.fontHeight() / 2;

    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("-", 45, 160, 4);
    M5.Lcd.drawString("+", 275, 160, 4);

    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("Pres. max (alarm limit)", 5, 15, 4);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("3/4", 315, 15, 4);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("ok", 160, 160, 4);
    M5.Lcd.setTextSize(3);
    M5.Lcd.drawString(p_buf, 160, 80, 4);

    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");
    change = 0;
  }
}
void scr34() {
  if (change) {
    if (setMinMaxSetter) {
      PMinMaxSetter = PMin;
    }
    setMinMaxSetter = 0;
    char p_buf[8];
    sprintf(p_buf, "%d", PMinMaxSetter);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawRect(235, 120, 80, 80, YELLOW); // Przycisk prawy
    M5.Lcd.drawRect(5, 120, 80, 80, YELLOW); //przycisk lewy
    M5.Lcd.drawRect(120, 120, 80, 80, YELLOW); //przycisk lewy
    M5.Lcd.setTextSize(4);
    plusX = (280 - 200) / 2 + 200 - M5.Lcd.textWidth("+") / 2;
    plusY = (200 - 120) / 2 + 120 - M5.Lcd.fontHeight() / 2;

    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("-", 45, 160, 4);
    M5.Lcd.drawString("+", 275, 160, 4);

    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("Pres. min (alarm limit)", 5, 15, 4);
    M5.Lcd.setTextDatum(MR_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("4/4", 315, 15, 4);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.drawString("ok", 160, 160, 4);
    M5.Lcd.setTextSize(3);
    M5.Lcd.drawString(p_buf, 160, 80, 4);

    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");
    change = 0;
  }
}
void scr41() {
  if (change) {
    if (setMinMaxSetter == 1) {
      sDay = getDay();
      sMonth = getMonth();
      sYear = getYear();
    }
    setMinMaxSetter = 0;

    M5.Lcd.setTextColor(WHITE);

    char buf_date[15];
    if (sDay < 10) {
      sprintf(day_buf, "0%d", sDay);
    } else {
      sprintf(day_buf, "%d", sDay);
    }
    if (sMonth < 10) {
      sprintf(month_buf, "0%d", sMonth);
    } else {
      sprintf(month_buf, "%d", sMonth);
    }
    sprintf(year_buf, "%d", sYear);
    strcpy(buf_date, day_buf);
    strcat(buf_date, month_buf);
    strcat(buf_date, year_buf);
    M5.Lcd.setTextSize(2);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawRect(0, 120, 80, 80, WHITE); 
    M5.Lcd.drawRect(80, 120, 80, 80, WHITE); 
    M5.Lcd.drawRect(160, 120, 80, 80, WHITE); 
    M5.Lcd.drawRect(240, 120, 80, 80, WHITE); 
    M5.Lcd.setTextDatum(ML_DATUM);

    if (datePart == 1) M5.Lcd.setTextColor(GREEN);
    else M5.Lcd.setTextColor(WHITE);
    M5.Lcd.drawString(day_buf, 40, 80, 4);
    M5.Lcd.drawString(".", 96, 80, 4);
    if (datePart == 2) M5.Lcd.setTextColor(GREEN);
    else M5.Lcd.setTextColor(WHITE);
    M5.Lcd.drawString(month_buf, 108, 80, 4);
    M5.Lcd.drawString(".", 166, 80, 4);
    if (datePart == 3) M5.Lcd.setTextColor(GREEN);
    else M5.Lcd.setTextColor(WHITE);
    M5.Lcd.drawString(year_buf, 184, 80, 4);
    M5.Lcd.setTextColor(GREEN);

    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.drawString("Date",160, 15, 4);
    M5.Lcd.setTextSize(3);
    M5.Lcd.drawString("-", 40, 160, 4);
    M5.Lcd.drawString(">", 120, 160, 4);
    M5.Lcd.drawString("+", 200, 160, 4);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("save", 280, 160, 4);
    M5.Lcd.setTextSize(4);
    M5.Lcd.setCursor(8, 10);
    M5.Lcd.setTextColor(WHITE);

    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");

    change = 0;
  }
}
void scr42() {
  if (change) {
    if (setMinMaxSetter == 1) {
      sHour = getHour();
      sMinute = getMinute();
    }
    setMinMaxSetter = 0;

    char buf_time[15];
    if (sHour < 10) {
      sprintf(hour_buf, "0%d", sHour);
    } else {
      sprintf(hour_buf, "%d", sHour);
    }
    if (sMinute < 10) {
      sprintf(minute_buf, "0%d", sMinute);
    } else {
      sprintf(minute_buf, "%d", sMinute);
    }
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawRect(0, 120, 80, 80, WHITE); // Przycisk prawy
    M5.Lcd.drawRect(80, 120, 80, 80, WHITE); //przycisk lewy
    M5.Lcd.drawRect(160, 120, 80, 80, WHITE); //przycisk lewy
    M5.Lcd.drawRect(240, 120, 80, 80, WHITE); //przycisk lewy
    M5.Lcd.setTextDatum(ML_DATUM);

    if (timePart == 1) M5.Lcd.setTextColor(GREEN);
    else M5.Lcd.setTextColor(WHITE);
    M5.Lcd.drawString(hour_buf, 95, 80, 4);
    M5.Lcd.drawString(".", 151, 80, 4);
    if (timePart == 2) M5.Lcd.setTextColor(GREEN);
    else M5.Lcd.setTextColor(WHITE);
    M5.Lcd.drawString(minute_buf, 163, 80, 4);
    M5.Lcd.setTextColor(WHITE);

    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.setTextSize(2);
    M5.Lcd.drawString("Time",160, 15, 4);
    M5.Lcd.setTextSize(3);
    
    M5.Lcd.drawString("-", 40, 160, 4);
    M5.Lcd.drawString(">", 120, 160, 4);
    M5.Lcd.drawString("+", 200, 160, 4);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("save", 280, 160, 4);
    M5.Lcd.setTextSize(4);
    M5.Lcd.setCursor(8, 10);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(48, 220);
    M5.Lcd.printf("<");
    M5.Lcd.setCursor(126, 220);
    M5.Lcd.printf("return");
    M5.Lcd.setCursor(262, 220);
    M5.Lcd.printf(">");
    change = 0;
  }
}


float getCurrTemp(){
  if (sht30.get() == 0) {
        temperature = sht30.cTemp;

      } else {
        temperature = 0;
      }
      return temperature;
}
float getCurrHum(){
  if (sht30.get() == 0) {
        humidity = sht30.humidity;
      } else {
        humidity = 0;
      }
      return humidity;
}
float getCurrPres(){
  pressure = round(qmp6988.calcPressure() / 100);
  return pressure;
}

float * getTemps() {
  File Tfile = SD.open("/temperatures.txt");
  float * temperaturesList = new float[72]; 
  int Tcnt = 0;

  if (Tfile) { 
    while (Tfile.available() && Tcnt < 72) {
      String temperaturesString = Tfile.readStringUntil('\n');

      Tcnt++;
      temperaturesList[Tcnt - 1] = temperaturesString.toFloat();
    }
    Tfile.close();
  } else { 
    for (int i = 0; i < 72; i++) {
      temperaturesList[i] = 0.0;
    }
  }
  Tcount = Tcnt;

  return temperaturesList; 
}

float * getHums() {
  File Hfile = SD.open("/humidities.txt");
  float * humiditiesList = new float[72]; 
  int Hcnt = 0;
if (Hfile) { 
  while (Hfile.available() && Hcnt < 72) {
    String humiditiesString = Hfile.readStringUntil('\n');

    Hcnt++;
    humiditiesList[Hcnt - 1] = humiditiesString.toFloat();
  }Hfile.close();
  } else { 
    for (int i = 0; i < 72; i++) {
      humiditiesList[i] = 0.0;
    }
  }
  Hcount = Hcnt;

  return humiditiesList; 
}

float * getPres() {
  File Pfile = SD.open("/pressures.txt");
  float * pressuresList = new float[72]; 
  int Pcnt = 0;
if (Pfile) { 
  while (Pfile.available() && Pcnt < 72) {
    String pressuresString = Pfile.readStringUntil('\n');

    Pcnt++;
    pressuresList[Pcnt - 1] = pressuresString.toFloat();
  }Pfile.close();
  } else { 
    for (int i = 0; i < 72; i++) {
      pressuresList[i] = 0.0;
    }
  }
  Pcount = Pcnt;

  return pressuresList; 
}

float * getMeasureHours() {
  File MHfile = SD.open("/measureHours.txt");
  float * measureHoursList = new float[72]; 
  int MHcnt = 0;
if (MHfile) { 
  while (MHfile.available() && MHcnt < 72) {
    String measureHoursString = MHfile.readStringUntil('\n');

    MHcnt++;
    measureHoursList[MHcnt - 1] = measureHoursString.toFloat();
  }MHfile.close();
  } else { 
    for (int i = 0; i < 72; i++) {
      measureHoursList[i] = 0.0;
    }
  }
  MHcount = MHcnt;

  return measureHoursList; 
}

bool isDateValid(int day, int month, int year) {
  // Tablica z maksymalną liczbą dni dla każdego miesiąca
  int daysInMonth[] = {
    31,
    28,
    31,
    30,
    31,
    30,
    31,
    31,
    30,
    31,
    30,
    31
  };

  if (month == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)))
    daysInMonth[1] = 29;

  if (day >= 1 && day <= daysInMonth[month - 1] && month >= 1 && month <= 12 && year >= 0)
    return true;
  else
    sDay = daysInMonth[month - 1];

  return false;
}

void saveTMin(float v)
{
  char buf [10];
  File file = SD.open("/tlb.txt",FILE_WRITE);
  sprintf(buf, "%f",v);
  file.print(buf);
  file.close();
  TMin = v;
}
void saveTMax(float v)
{
  char buf [10];
  File file = SD.open("/tub.txt",FILE_WRITE);
  sprintf(buf, "%f",v);
  file.print(buf);
  file.close();
  TMax = v;
}
void saveHMin(float v)
{
  char buf [10];
  File file = SD.open("/hlb.txt",FILE_WRITE);
  sprintf(buf, "%f",v);
  file.print(buf);
  file.close();
  HMin = v;
}
void saveHMax(float v)
{
  char buf [10];
  File file = SD.open("/hub.txt",FILE_WRITE);
  sprintf(buf, "%f",v);
  file.print(buf);
  file.close();
  HMax = v;
}
void savePMin(float v)
{
  char buf [12];
  File file = SD.open("/plb.txt",FILE_WRITE);
  sprintf(buf, "%f",v);
  file.print(buf);
  file.close();
  PMin = v;
}
void savePMax(float v)
{
  char buf [12];
  File file = SD.open("/pub.txt",FILE_WRITE);
  sprintf(buf, "%f",v);
  file.print(buf);
  file.close();
  PMax = v;
}

float readBound(const char* filename, float def)
{
  File file = SD.open(filename, FILE_READ); 
  if (file) {
  String value_str = file.readStringUntil('\n');
  file.close(); 
  
  float value = value_str.toFloat();
  
  return value;  
    
      }
  else return def;
  
}
void menuButtons(){

if (buttonA.inHotZone(touchPos) && (currentMillis - BtnCheckTime >= 1000)) {
    BtnCheckTime = currentMillis;

    if (subpage_active == 0) {
      if (curr_page == 1) {
        curr_page = 5;
      } else {
        curr_page--;
      }
    } else if (subpage_active == 1) {
      if (subpage_number == 1 || subpage_number == 2 || subpage_number == 3) {
        if (curr_subpage == 1) {
          curr_page = subpage_number * 10 + 4;
          change = 1;
          curr_subpage = 4;
        } else {
          curr_page--;
          change = 1;
          curr_subpage--;
        }
      } else if (subpage_number == 4) {
        if (curr_page == 41) {
          curr_page = 42;
          change = 1;
        } else if (curr_page == 42) {
          curr_page = 41;
          change = 1;
        }
      }
    }
    setMinMaxSetter = 1;
    change = 1;
  }

  if (buttonB.inHotZone(touchPos) && (currentMillis - BtnCheckTime >= 1000)) {
    BtnCheckTime = currentMillis;

    if (subpage_active == 0) {
      subpage_active = 1;
      if (curr_page == 1) {
        curr_page = 11;
        subpage_number = 1;
      }
      if (curr_page == 2) {
        curr_page = 21;
        subpage_number = 2;
      }
      if (curr_page == 3) {
        curr_page = 31;
        subpage_number = 3;
      }
      if (curr_page == 4) {
        curr_page = 41;
        subpage_number = 4;
      }

      change = 1;

    } else {
      curr_page = 1;
      subpage_active = 0;
      curr_subpage = 1;
      change = 1;
      setMinMaxSetter = 1;
    }
  }

  if (buttonC.inHotZone(touchPos) && (currentMillis - BtnCheckTime >= 1000)) {
    BtnCheckTime = currentMillis;
    if (subpage_active == 0) {
      if (curr_page == 5) {
        curr_page = 1;
      } else {
        curr_page++;
      }
    } else if (subpage_active == 1) {
      if (subpage_number == 1 || subpage_number == 2 || subpage_number == 3) {
        if (curr_subpage == 4) {
          curr_page = subpage_number * 10 + 1;
          change = 1;
          curr_subpage = 1;
        } else {
          curr_page++;
          change = 1;
          curr_subpage++;
        }
      } else if (subpage_number == 4) {
        if (curr_page == 41) {
          curr_page = 42;
          change = 1;
        } else if (curr_page == 42) {
          curr_page = 41;
          change = 1;
        }
      }
    }
    setMinMaxSetter = 1;
    change = 1;
  }

}
void boundsButtons(){

  if (touchPos.x != -1 && touchPos.y != -1 && (curr_page == 13 || curr_page == 14 || curr_page == 23 || curr_page == 24 || curr_page == 33 || curr_page == 34)) {
    if (UpBtn.inHotZone(touchPos)) {
      if ((curr_page == 13 || curr_page == 14 && TMax - TMinMaxSetter > 10)&&TMinMaxSetter<120) TMinMaxSetter++;
      if ((curr_page == 23 || curr_page == 24 && HMax - HMinMaxSetter > 10)&&HMinMaxSetter<90) HMinMaxSetter++;
      if ((curr_page == 33 || curr_page == 34 && PMax - PMinMaxSetter > 20)&&PMinMaxSetter<1100) PMinMaxSetter++;
      change = 1;
    }

    if (DnBtn.inHotZone(touchPos)) {
      if ((curr_page == 13 && TMinMaxSetter - TMin > 10 || curr_page == 14)&&TMinMaxSetter>-40) TMinMaxSetter--;
      if ((curr_page == 23 && HMinMaxSetter - HMin > 10 || curr_page == 24)&&HMinMaxSetter>10) HMinMaxSetter--;
      if ((curr_page == 33 && PMinMaxSetter - PMin > 20 || curr_page == 34)&&PMinMaxSetter>300) PMinMaxSetter--;
      change = 1;
    }
    if (AcceptBtn.inHotZone(touchPos)) {
      if (curr_page == 13) {
        TMax = TMinMaxSetter;
        saveTMax(TMax);
      }
      if (curr_page == 14) {
        TMin = TMinMaxSetter;
        saveTMin(TMin);
      }
      if (curr_page == 23) {
        HMax = HMinMaxSetter;
        saveHMax(HMax);
      }
      if (curr_page == 24) {
        HMin = HMinMaxSetter;
        saveHMin(HMin);
      }
      if (curr_page == 33) {
        PMax = PMinMaxSetter;
        savePMax(PMax);
      }
      if (curr_page == 34) {
        PMin = PMinMaxSetter;
        savePMin(PMin);
      }
      curr_page = 1;
      curr_subpage = 1;
      change = 1;
      subpage_active = 0;
      setMinMaxSetter = 1;

    }
  }

}
void dateButtons(){

  if (touchPos.x != -1 && touchPos.y != -1 && (curr_page == 41 || curr_page == 42)) {
    if (TDUp.inHotZone(touchPos)) {
      x = 1;
      change = 1;
      if (curr_page == 41) {
        if (datePart == 1) {
          if (isDateValid(sDay + 1, sMonth, sYear)) {
            sDay++;
          }
        }
        if (datePart == 2) {
          if (isDateValid(sDay, sMonth + 1, sYear)) {
            sMonth++;
          } else {
            if (sMonth < 12) {
              sMonth++;
            } else {
              sMonth = 1;
            }
          }
        }
        if (datePart == 3) {
          if (isDateValid(sDay, sMonth, sYear + 1)) {
            sYear++;
          }
        }
      } else if (curr_page == 42) {
        if (timePart == 1) {
          if (sHour <= 22) {
            sHour++;
          }
        }
        if (timePart == 2) {
          if (sMinute <= 58) {
            sMinute++;
          }
        }
      }
      isDateValid(sDay, sMonth, sYear);
    }
    if (TDDn.inHotZone(touchPos)) {
      x = 2;
      change = 1;
      if (curr_page == 41) {
        if (datePart == 1) {
          if (isDateValid(sDay - 1, sMonth, sYear)) {
            sDay--;
          }
        }
        if (datePart == 2) {
          if (isDateValid(sDay, sMonth - 1, sYear)) {
            sMonth--;
          } else {
            if (sMonth > 1) {
              sMonth--;
            } else {
              sMonth = 12;
            }
          }
        }
        if (datePart == 3) {
          if (isDateValid(sDay, sMonth, sYear - 1)) {
            sYear--;
          }
        }
      } else if (curr_page == 42) {
        if (timePart == 1) {
          if (sHour >= 1) {
            sHour--;
          }
        }
        if (timePart == 2) {
          if (sMinute >= 1) {
            sMinute--;
          }
        }
      }
      isDateValid(sDay, sMonth, sYear);
    }
    if (TDNx.inHotZone(touchPos)) {
      x = 3;
      change = 1;
      if (curr_page == 41) datePart = (datePart) % 3 + 1;
      if (curr_page == 42) timePart = (timePart) % 2 + 1;
    }
    if (TDAccept.inHotZone(touchPos)) {
      x = 4;
      change = 1;
      if (curr_page == 41) {
        setDate(sDay, sMonth, sYear);
        curr_page = 1;
        curr_subpage = 1;
        subpage_active = 0;
      }
      if (curr_page == 42) {
        setTime(sHour, sMinute, getMinute());
        curr_page = 1;
        curr_subpage = 1;
        subpage_active = 0;
      }
    }

  }

}

void tapButton(){
  if (touchArea.inHotZone(touchPos)) {
    if (curr_page >= 1 && curr_page <= 4 && (currentMillis - TapTime >= 1000)) {
      TapTime = currentMillis;
      CarouseleScreenMode = CarouseleScreenMode * (-1);
      CarouseleTime = currentMillis;
      change = 1;
    }
  }
}
void cycleMode(){
  if (currentMillis - CarouseleTime >= 8000 && CarouseleScreenMode == 1 && (curr_page >= 1 && curr_page <= 4)) {
    CarouseleTime = currentMillis;
    curr_page = curr_page % 4 + 1;
    change = 1;
  } else if (currentMillis - CarouseleTime >= 8000 && CarouseleScreenMode == -1 && (curr_page >= 1 && curr_page <= 4)) {
    CarouseleTime = currentMillis;
    temperature = getCurrTemp();
    humidity = getCurrHum();
    pressure = getCurrPres();
    change = 1;

    
  }
}

void measureAndSaveForEachHour(){
  if (currentMillis - previousMillis >= 3600000) {
    previousMillis = currentMillis;
    
  temperature = getCurrTemp();
  humidity = getCurrHum();
  pressure = getCurrPres();

    saveParametersToFiles(temperature, humidity, pressure);
  }}

  void setupWifi() {
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.setTextSize(3);
    M5.Lcd.drawString("Connecting...",160,120,2);

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    int attempts = 0;
    const int maxRetries = 1000;
    while (WiFi.status() != WL_CONNECTED && attempts < maxRetries) {
        delay(20);
        attempts++;
    }
    connected = WiFi.status() == WL_CONNECTED;
}
void conMqtt()
{
  int attempts = 0;
  const int maxRetries = 1000;
  while (!client.connected() && attempts < maxRetries) {
    if (client.connect("AHPIR")) {
    client.subscribe("WEATHERSTATION/167795/#");
    client.setBufferSize(4096);
    } else {
      delay(20);
      attempts++;  }
  } 
}



void callback(char* topic, byte* payload, unsigned int length) 
{


  if (strcmp(topic, "WEATHERSTATION/167795/get/temperature")==0)
    {
client.publish("WEATHERSTATION/167795/temperature", String(getCurrTemp()).c_str());    }

if (strcmp(topic, "WEATHERSTATION/167795/get/temperatureLB")==0)
    {
client.publish("WEATHERSTATION/167795/temperatureLB", String(TMin).c_str());    }

if (strcmp(topic, "WEATHERSTATION/167795/get/temperatureUB")==0)
    {
client.publish("WEATHERSTATION/167795/temperatureUB", String(TMax).c_str());    }

if (strcmp(topic, "WEATHERSTATION/167795/get/humidity")==0)
    {
client.publish("WEATHERSTATION/167795/humidity", String(getCurrHum()).c_str());    }

if (strcmp(topic, "WEATHERSTATION/167795/get/humidityLB")==0)
    {
client.publish("WEATHERSTATION/167795/humidityLB", String(HMin).c_str());    }

if (strcmp(topic, "WEATHERSTATION/167795/get/humidityUB")==0)
    {
client.publish("WEATHERSTATION/167795/humidityUB", String(HMax).c_str());    }

if (strcmp(topic, "WEATHERSTATION/167795/get/pressure")==0)
    {
client.publish("WEATHERSTATION/167795/pressure", String(getCurrPres()).c_str());    }

if (strcmp(topic, "WEATHERSTATION/167795/get/pressureLB")==0)
    {
client.publish("WEATHERSTATION/167795/pressureLB", String(PMin).c_str());    }

if (strcmp(topic, "WEATHERSTATION/167795/get/pressureUB")==0)
    {
client.publish("WEATHERSTATION/167795/pressureUB", String(PMax).c_str());    }

if (strcmp(topic, "WEATHERSTATION/167795/get/temperaturesLast72")==0)
    {

  float *measureHours = getMeasureHours();
  float *temperatures = getTemps();

  // Sprawdzenie, czy liczba godzin i temperatur jest taka sama.
  if (MHcount == Tcount && MHcount > 0) {
    // Utworzenie obiektu JSON.
    StaticJsonDocument<1024> doc;
    JsonArray data = doc.createNestedArray("data");

    // Wypełnienie JSON-a danymi.
    for (int i = 0; i < MHcount; i++) {
      JsonObject dataObj = data.createNestedObject();
      dataObj["time"] = measureHours[i];
      dataObj["temperature"] = temperatures[i];
    }

    // Serializacja JSON-a do String.
    String jsonStr;
    serializeJson(doc, jsonStr);
    client.publish("WEATHERSTATION/167795/temperaturesLast72", jsonStr.c_str());   } 
}

if (strcmp(topic, "WEATHERSTATION/167795/get/humiditiesLast72")==0)
    {

  float *measureHours = getMeasureHours();
  float *humidities = getHums();

  // Sprawdzenie, czy liczba godzin i temperatur jest taka sama.
  if (MHcount == Hcount && MHcount > 0) {
    // Utworzenie obiektu JSON.
    StaticJsonDocument<1024> doc;
    JsonArray data = doc.createNestedArray("data");

    // Wypełnienie JSON-a danymi.
    for (int i = 0; i < MHcount; i++) {
      JsonObject dataObj = data.createNestedObject();
      dataObj["time"] = measureHours[i];
      dataObj["humidity"] = humidities[i];
    }

    // Serializacja JSON-a do String.
    String jsonStr;
    serializeJson(doc, jsonStr);
    client.publish("WEATHERSTATION/167795/humiditiesLast72", jsonStr.c_str());   } 
}

if (strcmp(topic, "WEATHERSTATION/167795/get/pressuresLast72")==0)
    {

  float *measureHours = getMeasureHours();
  float *pressures = getPres();

  // Sprawdzenie, czy liczba godzin i temperatur jest taka sama.
  if (MHcount == Pcount && MHcount > 0) {
    // Utworzenie obiektu JSON.
    StaticJsonDocument<1024> doc;
    JsonArray data = doc.createNestedArray("data");

    // Wypełnienie JSON-a danymi.
    for (int i = 0; i < MHcount; i++) {
      JsonObject dataObj = data.createNestedObject();
      dataObj["time"] = measureHours[i];
      dataObj["pressure"] = pressures[i];
    }

    // Serializacja JSON-a do String.
    String jsonStr;
    serializeJson(doc, jsonStr);
    client.publish("WEATHERSTATION/167795/pressuresLast72", jsonStr.c_str());   } 
}


 if (strcmp(topic, "WEATHERSTATION/167795/set/temperatureLB")==0)
    {      

      payload[length] = '\0'; 
      int val = atoi((char *)payload);
      saveTMin(val);
      change=1;
      setMinMaxSetter = 1;
    }
if (strcmp(topic, "WEATHERSTATION/167795/set/temperatureUB")==0)
    {
      payload[length] = '\0'; 
      int val = atoi((char *)payload);
      saveTMax(val);
      change=1;
      setMinMaxSetter = 1;
    }

if (strcmp(topic, "WEATHERSTATION/167795/set/humidityLB")==0)
    {
      payload[length] = '\0'; 
      int val = atoi((char *)payload);
      saveHMin(val);
      change=1;
      setMinMaxSetter = 1;
    }
if (strcmp(topic, "WEATHERSTATION/167795/set/humidityUB")==0)
    {
      payload[length] = '\0'; 
      int val = atoi((char *)payload);
      saveHMax(val);
      change=1;
      setMinMaxSetter = 1;
    }

if (strcmp(topic, "WEATHERSTATION/167795/set/pressureLB")==0)
    {
      payload[length] = '\0'; 
      int val = atoi((char *)payload);
      savePMin(val);
      change=1;
      setMinMaxSetter = 1;
    }
if (strcmp(topic, "WEATHERSTATION/167795/set/pressureUB")==0)
    {
      payload[length] = '\0'; 
      int val = atoi((char *)payload);
      savePMax(val);
      change=1;
      setMinMaxSetter = 1;
    }




}

