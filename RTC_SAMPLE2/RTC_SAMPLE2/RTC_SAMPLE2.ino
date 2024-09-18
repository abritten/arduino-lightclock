#include "RTC.h"
#include "Arduino_LED_Matrix.h"
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "arduino_secrets.h"  // Include the secrets header

ArduinoLEDMatrix matrix;
WiFiUDP ntpUDP;

// Set time offset for Pacific Standard Time (PST)
// -28800 = UTC - 8 hours
// -25200 = UTC - 7 hours
int timeZoneOffset = -25200;

NTPClient timeClient(ntpUDP, "pool.ntp.org", timeZoneOffset, 60000);  // Sync every 60 seconds

byte Time[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

byte Digits[5][30] = {
  { 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1 },
  { 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1 },
  { 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 }
};
int currentSecond;
boolean secondsON_OFF = 1;
int hours, minutes, seconds, year, dayofMon;
String dayofWeek;
String monthNames[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
String month;

void displayDigit(int d, int s_x, int s_y) {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 5; j++)
      Time[i + s_x][11 - j - s_y] = Digits[j][i + d * 3];

  matrix.renderBitmap(Time, 8, 12);
}

// Function to convert epoch time to date and time
void convertEpochToDateTime(unsigned long epoch) {
  // Calculate basic time elements from epoch
  seconds = epoch % 60;
  epoch /= 60;
  minutes = epoch % 60;
  epoch /= 60;
  hours = epoch % 24;
  epoch /= 24;

  // Days since epoch
  int daysSinceEpoch = epoch;

  // Calculate year
  year = 1970;
  while (daysSinceEpoch >= 365) {
    if (year % 4 == 0) {  // Leap year
      if (daysSinceEpoch >= 366) {
        daysSinceEpoch -= 366;
        year++;
      } else {
        break;
      }
    } else {
      daysSinceEpoch -= 365;
      year++;
    }
  }

  // Calculate month and day of the month
  int daysInMonth[] = {31, (year % 4 == 0) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int monthIndex = 0;
  for (int i = 0; i < 12; i++) {
    if (daysSinceEpoch < daysInMonth[i]) {
      dayofMon = daysSinceEpoch + 1;
      monthIndex = i;
      break;
    } else {
      daysSinceEpoch -= daysInMonth[i];
    }
  }

  // Set the month name from monthNames array
  month = monthNames[monthIndex];
}

void setup() {
  Serial.begin(9600);
  matrix.begin();
  
  // Connect to WiFi using credentials from arduino_secrets.h
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Initialize NTP client with time zone offset
  timeClient.begin();
  timeClient.update();
}

void loop() {
  timeClient.update();  // Sync NTP periodically
  
  // Get the current epoch time and convert it to human-readable format
  unsigned long epochTime = timeClient.getEpochTime();
  convertEpochToDateTime(epochTime);

  // Display time on matrix
  if (seconds != currentSecond) {
    secondsON_OFF = !secondsON_OFF;
    displayDigit(hours / 10, 0, 0);
    displayDigit(hours % 10, 4, 0);
    displayDigit(minutes / 10, 1, 6);
    displayDigit(minutes % 10, 5, 6);
    
    Time[0][2] = secondsON_OFF;
    Time[0][4] = secondsON_OFF;
    currentSecond = seconds;
    matrix.renderBitmap(Time, 8, 12);
  }
  
  delay(1000);  // Refresh every second
}