// Aaron's Filer Clock
// aaron ( at ) brodney ( dot ) net

// May 19 2016
// Designed for Arduino Uno/Leonardo + Adafruit RGB shield
// DS1307 RTC

// June 1 2016 -- Version 1.1.1

#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include "RTClib.h"
#include <avr/wdt.h> // access watch-dog timer stuff


#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

boolean rtcFailed = false;

int eventCount = 0;

byte defaultColor = WHITE;

char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

const long refreshDelay = 900;

RTC_DS1307 rtc;

boolean displayMessage = 0;

// instantiate Adafruit RGB LCD shield

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void countDown(int from)
{
  for ( int i = from ; i > 0 ; i-- )
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("REBOOT IN:");
    lcd.setCursor(0, 1);
    lcd.print(i);
    delay(1000);
  }
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("BOOOOOOOM!");
  delay(1000);

}

void triggerReboot()
{
  Serial.println("triggerReboot called");
  Serial.flush();
  delay(1000);
  cli();               // clear interrupts
  wdt_enable(WDTO_15MS);     // enable WDT
  while (1);        // enter infinite loop, watch-dog saves our ass
}


void helloWorld()
{
  lcd.clear();

  if ( displayMessage == 0 )
  {
    lcd.setCursor(0, 0);
    lcd.print("Aaron's NetApp");
    lcd.setCursor(0, 1);
    lcd.print("FilerClock");
    displayMessage = 1;
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Nutanix Rules!");
    lcd.setCursor(0, 1);
    lcd.print("NetApp Drools!");
    displayMessage = 0;
  }
  delay(3000);
}

void printCurrentTime()
{
  DateTime now = rtc.now();
  long xYear = 0;
  int xSecond = 0, xMinute = 0, xMonth = 0, xDay = 0;

  lcd.clear();

  lcd.setCursor(0, 0);

  lcd.print(now.hour(), DEC);
  lcd.print(":");

  xMinute = now.minute();

  if ( xMinute < 10 )
  {
    lcd.print("0");
    lcd.print(xMinute, DEC);
  } else {
    lcd.print(xMinute, DEC);
  }
  lcd.print(":");

  xSecond = now.second();
  if ( xSecond < 10 )
  {
    lcd.print("0");
    lcd.print(xSecond, DEC);
  } else {
    lcd.print(xSecond, DEC);
  }
  lcd.print(" EDT");
  lcd.setCursor(0, 1);
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.print(" ");
  lcd.print(now.month(), DEC);
  lcd.print("/");
  lcd.print(now.day(), DEC);
  lcd.print("/");

  xYear = now.year();
  xYear -= 2000;

  lcd.print(xYear);

}

void printTimeNow()
{
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

}

void setup() {
  // Debugging output
  Wire.begin();

  Serial.begin(9600);

  // instantiate our lcd screen
  lcd.begin(16, 2);

  lcd.clear();
  lcd.setBacklight(defaultColor);
  lcd.print("DATA ONCRAP");
  lcd.setCursor(0, 1);
  lcd.print("BOOT");
  delay(2000);

  Serial.println("FILERCLOCK BOOT!");

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    lcd.clear();
    lcd.print("RTC missing!");
    while (1);
  }

  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");

    rtcFailed = true;

    // set date-time to when the sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // From docs:
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    lcd.clear();
    lcd.print("RTC FAILURE");
    delay(15000);

  }

  helloWorld();
  delay(3000);

}

void loop() {

  long currentTime = millis();

  if ( currentTime > 43200000 )
  {
    Serial.println( currentTime );
    Serial.println("Timer indicates system has been running 12 hours. Reboot.");
    countDown(9);
    triggerReboot();
  }

  if ( eventCount == 60 ) // cycle between datetime and messages
  {
    helloWorld();

    eventCount = 0;

  } else {
    printCurrentTime();

    // Print the current time over Serial. use for debugging
    //  printTimeNow();

    delay(refreshDelay);

    eventCount++;
  }
}
