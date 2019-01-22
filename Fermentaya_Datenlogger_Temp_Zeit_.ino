#include "SD.h"
#include"SPI.h"
//#include <Wire.h>
#include "RTClibExtended.h"
//#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Timer.h>

RTC_DS3231 rtc;
Timer timer;

#define displayLength 16.0

//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// Data wire temp sensors is connected to GPIO2
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

DeviceAddress sensor1 = { 0x28, 0x27, 0xCA, 0x77, 0x91, 0x11, 0x2, 0x8B };
DeviceAddress sensor2 = { 0x28, 0xE0, 0xBE, 0x77, 0x91, 0x11, 0x2, 0xE3 };
DeviceAddress sensor3 = { 0x28, 0xE3, 0xDF, 0x77, 0x91, 0x16, 0x2, 0x8 };
DeviceAddress sensor4 = { 0x28, 0xFD, 0x5F, 0x77, 0x91, 0xB, 0x2, 0xEE };
DeviceAddress sensor5 = { 0x28, 0x5D, 0x16, 0x88, 0x6, 0x0, 0x0, 0x8E };
DeviceAddress sensor6 = { 0x28, 0x44, 0xD, 0x88, 0x6, 0x0, 0x0, 0x4F };
DeviceAddress sensor7 = { 0x28, 0xFC, 0xC0, 0xB0, 0x6, 0x0, 0x0, 0x73 };
DeviceAddress sensor8 = { 0x28, 0xFF, 0x23, 0x8C, 0x52, 0x15, 0x1, 0x4C };

const int CSpin = 10;
//float tempBokashi;
//float tempZelt;
String dataString = ""; // holds the data to be written to the SD card


File sensorData;

long id = 1;                //Use this to store the id # of our reading.

void setup() {

  /*
    #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
    #endif
  */
  Serial.begin(9600);
  //lcd.begin(16, 2);
  //lcd.clear();
  sensors.begin();

  Serial.print("Initializing SD card...");
  pinMode(CSpin, OUTPUT);

  delay(500); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
//    lcd.print("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
//    lcd.print("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // see if the card is present and can be initialized:
  if (!SD.begin(CSpin)) {
    Serial.println("Card failed, or not present");
    /*
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("SD-Karte nicht");
    lcd.setCursor(1, 0);
    lcd.println("gefunden.");
    */
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  /*
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("SD-Karte");
  lcd.setCursor(1, 0);
  lcd.println("initialisiert.");
  */
  CSVheader();
  delay(2000);
  temp();
  timer.every(20000, temp);
  timer.every(5000, saveData);
}


void loop() {
  // build the data string
  timer.update();
  //dataString = String(tempBokashi) + "," + String(tempZelt); // convert to CSV
}


void temp() {

  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");

  // tempZelt = sensors.getTempC(sensor1);
  // tempBokashi = sensors.getTempC(sensor2);


  //tempBokashi = 0.00;
  //tempZelt = 0.00;
  /*

    if (tempZelt && tempBokashi == -127.00) {
      //sensorData.print("No value,No value,");
      tempZelt = 0.0;
      tempBokashi = 0.0;

    } else if (tempZelt == -127.00) {
      //sensorData.print("No value," + String(tempBokashi) + ",");
      tempZelt = 0.0;
      } else if (tempBokashi == -127.00) {
        //sensorData.print(String(tempZelt) + String(",No value,");
        tempBokashi = 0.0;
      } else {
  */
  Serial.print("Sensor 1(*C): ");
  Serial.print(sensors.getTempC(sensor1));

  Serial.print("Sensor 2(*C): ");
  Serial.print(sensors.getTempC(sensor2));
/*
  lcd.setCursor(0, 0);
  lcd.print(String("Zelt: ") + sensors.getTempC(sensor1) + String(" C"));
  //lcd.print(sensors.getTempC(sensor1));
  lcd.setCursor(0, 1);
  //lcd.print(sensors.getTempC(sensor2));
  lcd.print(String("Bokashi: ") + sensors.getTempC(sensor2) + String(" C"));
*/

}

void CSVheader() {
  //Write Log File Header
  sensorData = SD.open("data.csv", FILE_WRITE);
  if (sensorData) {
    //sensorData.println(", , , ,"); //Just a leading blank line, incase there was previous data
    String header = "Temp Zelt, Temp Bokashi, Datum, Uhrzeit";
    sensorData.println(header);
    sensorData.close();
    Serial.println(header);
  }
  else
  {
    Serial.println("Couldn't open log file");
  }
}


void saveData() {

  //dataString = String(tempZelt) + "," + String(tempBokashi); // convert to CSV
  DateTime now = rtc.now();
  sensors.requestTemperatures();
  sensorData = SD.open("data.csv", FILE_WRITE);

  //delay(500);
  //if (sensorData) {
  //dataString = String(0.00) + "," + String(0.00) + ","; //+ String(now.year() + now.month() + now.day() + (", ") + now.hour() + now.minute() + now.second());
  dataString = String(sensors.getTempC(sensor1)) + "," + String(sensors.getTempC(sensor2)) + "," + String(sensors.getTempC(sensor3)) + "," + String(sensors.getTempC(sensor4)) + "," + String(sensors.getTempC(sensor5)) + "," + String(sensors.getTempC(sensor6)) + "," + String(sensors.getTempC(sensor7)) + "," + String(sensors.getTempC(sensor8)) + ","; //+ String(now.year() + now.month() + now.day() + (", ") + now.hour() + now.minute() + now.second());
  sensorData.print(dataString);
  Serial.println(dataString);

  //sensorData.println(now.year() + now.month() + now.day() + (", ") + now.hour() + now.minute() + now.second());
  //Serial.println(String(now.year() + now.month() + now.day() + (", ") + now.hour() + now.minute() + now.second()));

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(",");

  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);

  sensorData.print(now.year(), DEC);
  sensorData.print('/');
  sensorData.print(now.month(), DEC);
  sensorData.print('/');
  sensorData.print(now.day(), DEC);
  sensorData.print(",");
  sensorData.print(now.hour(), DEC);
  sensorData.print(':');
  sensorData.print(now.minute(), DEC);
  sensorData.print(':');
  sensorData.println(now.second(), DEC);

  sensorData.close();
  //}
  //}
  /*
    else {
    Serial.println("Error writing to file !");
    }
  */
}
