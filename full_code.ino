
//This code uses a normal Arduino, a FONA 808 and a DS1307 RTC
//Make sure that the correct libraries are downloaded before using this code

#include "Adafruit_FONA.h"
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

tmElements_t tm;

String currentHour;
String currentMinute;
String currentTime;
String currentLat;
String currentLong;
String currentLoc;
String fullMessage;

char gpsdata[120];
int led = 13;
int button = 7;
bool sent = false;
int buttonState;

char sendto1[] = "xxxxxxxxxx";
char sendto2[] = "xxxxxxxxxx";
char sendto3[] = "xxxxxxxxxx";
char message[100];

void setup() {
  pinMode(led, OUTPUT);
  pinMode(button, INPUT);
  
  while (!Serial);

  Serial.begin(115200);
  Serial.println(F("FONA basic test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }

  //turn on GPS
  if (!fona.enableGPS(true)){
          Serial.println(F("Failed to turn on"));
  } else {
    Serial.println(F("GPS is on"));
    }

  //wait for a fix
  bool fixReady = false;
  while (fixReady == false) {
    int8_t stat;
    // check GPS fix
    stat = fona.GPSstatus();
    if (stat == 3){
      fixReady = true;
      Serial.println(F("GPS ready"));
      
      digitalWrite(led, HIGH);
      delay(5000);
      digitalWrite(led, LOW);
      }  
    }
    
  Serial.print(F("First #"));
  Serial.println(sendto1);

  Serial.print(F("Second #"));
  Serial.println(sendto2);

  Serial.print(F("Third #"));
  Serial.println(sendto3);
}

void loop() {
  buttonState = digitalRead(button);
  
  if (buttonState == LOW) {       //if the pen is removed
    sent = false;                  //text hasn't been sent yet
    if (sent == false) {          //if the text hasn't been sent yet
      sendSMS();                  //send the text
      sent = true;                //don't send it again
      }
    else {
      delay (5000);
      }
    }
}

void sendSMS(){
  getCurrentTime();
  Serial.print("Current time: ");
  Serial.println(currentTime);
  
  getCurrentLoc();
  Serial.print("Current location: ");
  Serial.println(currentLoc);
  
  combine();
  Serial.print("Full message: ");
  Serial.println(fullMessage);
  
  fullMessage.toCharArray(message, 100); 
  Serial.print("Message: ");
  Serial.println(message);
  
        if (!fona.sendSMS(sendto1, message)) {
          Serial.println(F("Failed (1)"));
        } else {
          Serial.println(F("Sent! (1)"));
        }

        if (!fona.sendSMS(sendto2, message)) {
          Serial.println(F("Failed (2)"));
        } else {
          Serial.println(F("Sent! (2)"));
        }

        if (!fona.sendSMS(sendto3, message)) {
          Serial.println(F("Failed (3)"));
        } else {
          Serial.println(F("Sent! (3)"));
        }
  }

void getCurrentTime() {
  if (RTC.read(tm)){
    //hour
    currentHour = String(tm.Hour, DEC);
    Serial.print("currentHour is: ");
    Serial.println(currentHour); 

    //minute
    currentMinute = String(tm.Minute, DEC);
    Serial.print("currentMinute is: ");
    Serial.println(currentMinute);

     //putting it together
     currentTime = "Time: " + currentHour + ':' + currentMinute;
  }
  }

void getCurrentLoc() {
  fona.getGPS(0, gpsdata, 120);
  
  char* mode = strtok(gpsdata, ",");
  char* fixstatus = strtok(NULL, ",");
  char* timeDate = strtok(NULL, ",");
  
  char* latitude = strtok(NULL, ",");
  currentLat = String(latitude);

  char* longitude = strtok(NULL, ",");
  currentLong = String(longitude);

  currentLoc = "location (lat,long): " + currentLat + ", " + currentLong;
  
  }

void combine() {
  fullMessage = "An epi-pen was used. " + currentTime + ", " + currentLoc;
  }

void flushSerial() {
  while (Serial.available())
    Serial.read();
}
