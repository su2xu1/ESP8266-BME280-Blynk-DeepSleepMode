//ESP8266 BME280 Blynk in Deep Sleep Mode
//release 2017.3.25
//momma@nifty.com
//Pragmas
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#define BLYNK_DEBUG //For debug

// Auth Token in the Blynk App and Wi-Fi-Setting
const char* auth    = "blink-auth-token";
const char* ssid    = "wifi-ssid";
const char* pass    = "wifi-password";

//Include Headers
#include <ESP8266WiFi.h>
#include <Wire.h> 
#include <BlynkSimpleEsp8266.h>
#include <BME280_MOD-1022.h>
#include <SimpleTimer.h>


//add include using ESP8266 extend library
extern "C" {
#include "user_interface.h"
}


//Variables
SimpleTimer timer;//Timer valuables 2016/12/20
unsigned long lastCheck = 0;
double tempMostAccurate, humidityMostAccurate, pressureMostAccurate;
char buff[50];

// Arduino needs this to bring pretty numbers
void printFormattedFloat(float x, uint8_t precision) {
  char buffer[10];
  dtostrf(x, 7, precision, buffer);
  Serial.print(buffer);
}
void formattedFloat(float x, uint8_t precision, char *buff) {
  dtostrf(x, 7, precision, buff);
}


//BME280 Setup
// need to read the NVM compensation parameters 
void SetupBME280()
{
  BME280.readCompensationParams();
  BME280.writeStandbyTime(tsb_0p5ms);        // tsb = 0.5ms
  BME280.writeFilterCoefficient(fc_16);      // IIR Filter coefficient 16
  BME280.writeOversamplingPressure(os16x);    // pressure x16
  BME280.writeOversamplingTemperature(os2x);  // temperature x2
  BME280.writeOversamplingHumidity(os1x);    // humidity x1
  BME280.writeMode(smNormal);
}
 
 //Function to read BME280 value and go to deep sleep                                          
 void ReadSensor()
 {
 // read out the data - must do this before calling the getxxxxx routines
 BME280.readMeasurements();
//For Debugging, Serial Print Temp+Humid+Pressure
#ifdef BLYNK_DEBUG
    tempMostAccurate = BME280.getTemperatureMostAccurate();
    Serial.print("Temp:");
    printFormattedFloat(tempMostAccurate, 2);
    humidityMostAccurate = BME280.getHumidityMostAccurate();
    Serial.print(" Humid:");
    printFormattedFloat(humidityMostAccurate, 2);
    pressureMostAccurate = BME280.getPressureMostAccurate();
    Serial.print(" Pressure:");
    printFormattedFloat(pressureMostAccurate, 2);
    Serial.println();
#endif
//send sensed data to blink server
  //Temperature
  tempMostAccurate = BME280.getTemperatureMostAccurate();
  char buff1[6];  
  dtostrf(tempMostAccurate, 3, 2, buff1);
  Blynk.virtualWrite(V0, String(buff1));
  //Humidity
  humidityMostAccurate = BME280.getHumidityMostAccurate();
  char buf[6];
  dtostrf(humidityMostAccurate, 3, 2, buf);
  Blynk.virtualWrite(V1, String(buf));
  //Pressure
  pressureMostAccurate = BME280.getPressureMostAccurate();
  char buf8[8];
  dtostrf(pressureMostAccurate, 5, 2, buf8);
  Blynk.virtualWrite(V2, String(buf8));
//Dive to Deep Sleep Mode
Serial.println("Dive to Deep Sleep Mode");
ESP.deepSleep(60000000);//deep sleep per every 60 sec
delay(100);

}

//set up
 void setup()
 {
#ifdef BLYNK_DEBUG
  Serial.println("Begin Serial");
  Serial.begin(115200);
  Serial.println("Begin BME280");
#endif
  Wire.begin();
  SetupBME280();
#ifdef BLYNK_DEBUG
  Serial.println("Begin Wifi");
  Blynk.begin(auth, "ssid", "password");//ssid,pass
#endif
  timer.setInterval(1000L, ReadSensor);
 }

//loop, is this necessary?
void loop()
{
  Blynk.run();
  timer.run();
}

