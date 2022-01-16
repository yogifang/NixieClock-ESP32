/*
    WifiSettings basic example

    Source and further documentation available at
    https://github.com/Juerd/ESP-WiFiSettings

    Note: this example is written for ESP32.
    For ESP8266, use LittleFS.begin() instead of SPIFFS.begin(true).
*/

#include "Adafruit_MCP23017.h"
#include <Arduino.h>
#include <NTPClient.h>
#include <SPIFFS.h>
#include <WiFiSettings.h>
#include <WiFiUdp.h>
#include <Wire.h>
void dspMinSec(bool bMin , byte byTime);


#define NEW_BOARD  1

#define  NIXIE_IN14  1
//#define  NIXIE_IN8   1


#define MCPAddr1 0x20
#define MCPAddr2 0x21

#define I2C_SDA 4
#define I2C_SCL 5

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String formattedDate;
String dayStamp;
String timeStamp;

Adafruit_MCP23017 mcpSec;
Adafruit_MCP23017 mcpMin;
volatile int interruptCounter;
int totalInterruptCounter;

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

bool bForceResetMcp = false;
bool bTimeFlish = false;
bool bSecChanged = true;
bool bMinChanged = true;
bool bHourChanged = true;
int iCnt = 0;
int iSec = 0;
int iMin = 0;
int iHour = 0;

const int iI2CRESET = 32;

const int iSW1 = 36;
const int iSW2 = 39;
int iCntSW1 = 0 ;
int iCntSW2 = 0 ;


#ifdef NEW_BOARD 
const int iH10 = 27;
const int iH11 = 25;
const int iH12 = 26;
const int iH1R = 34;
#endif
#ifndef NEW_BOARD
const int iH10 = 23;
const int iH11 = 25;
const int iH12 = 26;
const int iH1R = 34;
#endif

const int iH00 = 12;
const int iH01 = 13;
const int iH02 = 14;
const int iH03 = 15;
const int iH04 = 16;
const int iH05 = 17;
const int iH06 = 18;
const int iH07 = 19;
const int iH08 = 21;
const int iH09 = 22;
const int iH0R = 33;


#ifdef NIXIE_IN14 
const int iH1[] = {iH10, iH11, iH12, iH1R};

const int iH0[] = {iH00, iH01, iH02, iH03, iH04, iH05,
                   iH06, iH07, iH08, iH09, iH0R};

const uint16_t iMS0[10] = {BIT1, BIT0, BIT3, BIT2, BIT5,
                           BIT4, BIT7, BIT6, BIT8, BIT9};
const uint16_t iMS1[6] = {BIT15, BIT14, BIT13, BIT12, BIT11, BIT10};
#endif

#ifdef NIXIE_IN8
const int iH1[] = {iH10, iH11, iH12, iH1R};

const int iH0[] = {iH00, iH01, iH02, iH03, iH04, iH05,
                   iH06, iH07, iH08, iH09, iH0R};

const uint16_t iMS0[10] = {BIT8, BIT9, BIT1, BIT7, BIT4,
                           BIT5, BIT2, BIT3, BIT1, BIT2};
const uint16_t iMS1[6] = {BIT15, BIT14, BIT13, BIT12, BIT11, BIT10};
#endif



void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  if (bTimeFlish == true) {
    iSec++;
    bSecChanged = true;
    if (iSec > 59) {
      iSec = 0;
      iMin++;
      bMinChanged = true;
      if (iMin > 59) {
        iMin = 0;
        iHour++;
        bHourChanged = true;
        if (iHour > 23)
          iHour = 0;
      }
    }
  }

  portEXIT_CRITICAL_ISR(&timerMux);
}

void initMcp23017() {
  digitalWrite(iI2CRESET, LOW);
  delay(1);
  digitalWrite(iI2CRESET, HIGH);

  mcpSec.begin(0x0, &Wire); // use default address 0
  mcpMin.begin(0x1, &Wire);

  Wire.beginTransmission(MCPAddr1);
  Wire.write(0x00); // address port A direction
  Wire.write(0x00); // value to send
  Wire.endTransmission();
  Wire.beginTransmission(MCPAddr1);
  Wire.write(0x01); // address port B direction
  Wire.write(0x00); // value to send output
  Wire.endTransmission();
  Wire.beginTransmission(MCPAddr2);
  Wire.write(0x00); // address port A direction
  Wire.write(0x00); // value to send
  Wire.endTransmission();
  Wire.beginTransmission(MCPAddr2);
  Wire.write(0x01); // address port B direction
  Wire.write(0x00); // value to send output
  Wire.endTransmission();

  mcpSec.writeGPIOAB(0x0000);
  mcpMin.writeGPIOAB(0x0000);
}

void setup() {
  Serial.begin(115200);
   Serial.println("Nixie Clock Controller Ver1.5");

   Wire.begin(I2C_SDA, I2C_SCL, 400000); // wake up I2C bus

  pinMode(iI2CRESET, OUTPUT);
  pinMode(iSW1, INPUT_PULLUP);
  pinMode(iSW2, INPUT_PULLUP);

  for (iCnt = 0; iCnt < sizeof(iH1) / 4; iCnt++) {
    pinMode(iH1[iCnt], OUTPUT);
    digitalWrite(iH1[iCnt], LOW);
  }

  for (iCnt = 0; iCnt < sizeof(iH0) / 4; iCnt++) {
    pinMode(iH0[iCnt], OUTPUT);
    digitalWrite(iH0[iCnt], LOW);
  }
  initMcp23017();
  
  bSecChanged = true ;
  dspMinSec(false , 0) ;

   SPIFFS.begin(true); // Will format on the first run after failing to mount
   delay(500) ;
  // Use stored credentials to connect to your WiFi access point.
  // If no credentials are stored or if the access point is out of reach,
  // an access point will be started with a captive portal to configure WiFi.
  bSecChanged = true ;
  dspMinSec(false , 1) ;
  WiFiSettings.connect();

  String strTZ = WiFiSettings.timezone ;
  int iOffset = 0 ;
  if(strTZ.length() > 0) {
   iOffset = 3600 * strTZ.toInt() ;
  }
  
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(iOffset);

 
 
  // set I/O pins to outputs

  Serial.println("MCP init.........");
  interrupts();
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  // timerAlarmWrite(timer, 1000000, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
}
//================================================================================================


void dspMinSec(bool bMin, byte byTime) {
  Adafruit_MCP23017 *pMcp;
  if (bMin == true) {
    if (bMinChanged == false)
      return;
    pMcp = &mcpMin;
    bMinChanged = false;
    // Serial.print("Min.........");
  } else {
    if (bSecChanged == false)
      return;
    pMcp = &mcpSec;
    bSecChanged = false;
    // Serial.print("Sec.........");
  }

  byte by10 = byTime / 10;
  byte by0 = byTime % 10;
  // Serial.println("dspMinSec ........");
  uint16_t u16Data = 0;
  u16Data = iMS0[by0] | iMS1[by10];
  //u16Data = BIT10 ;
  noInterrupts();

   
  pMcp->writeGPIOAB(u16Data);
  if (u16Data != pMcp->readGPIOAB()) {
    bForceResetMcp = true;
    bMinChanged = true;
    bSecChanged = true;
  }
  interrupts();
}


void dspHour(byte byTime) {

  if (bHourChanged == false)
    return;
  if (byTime > 24)
    byTime = 0;

  byte by10 = byTime / 10;
  byte by0 = byTime % 10;

  for (iCnt = 0; iCnt < sizeof(iH1) / 4; iCnt++) {
    pinMode(iH1[iCnt], OUTPUT);
    digitalWrite(iH1[iCnt], LOW);
  }

  for (iCnt = 0; iCnt < sizeof(iH0) / 4; iCnt++) {
    pinMode(iH0[iCnt], OUTPUT);
    digitalWrite(iH0[iCnt], LOW);
  }

  digitalWrite(iH0[by0], HIGH);
  digitalWrite(iH1[by10], HIGH);
  bHourChanged = false;
}
void getInternetTime() {

  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // initMcp23017();
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  // Serial.println(formattedDate);

  // Extract date
  int splitT = formattedDate.indexOf("T");
  // dayStamp = formattedDate.substring(0, splitT);
  // Serial.print("DATE: ");
  // Serial.println(dayStamp);
  // Extract time
  timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
  sscanf(timeStamp.c_str(), "%d:%d:%d", &iHour, &iMin, &iSec);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);
  bMinChanged = true;
  bSecChanged = true;
  bHourChanged = true;
  dspMinSec(false, (byte)iSec);
  dspMinSec(true, (byte)iMin);
  dspHour((byte)iHour);
  delay(900);
}
//================================================================================================
void loop() {


  if (interruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    dspMinSec(false, (byte)iSec);
    dspMinSec(true, (byte)iMin);
    dspHour((byte)iHour);
    return;
  }
  if (bForceResetMcp == true) {
    initMcp23017();
    bForceResetMcp = false;
    bSecChanged = true;
    bMinChanged = true;
    bHourChanged = true;
    dspMinSec(false, (byte)iSec);
    dspMinSec(true, (byte)iMin);
    dspHour((byte)iHour);
  }

  int iZeroTime = iHour + iMin + iSec;
  if (iZeroTime == 0)
    bTimeFlish = false;

  if (bTimeFlish == false) {
    getInternetTime();
    bTimeFlish = true;
  }
  if (digitalRead(iSW1) == false) {
    iCntSW1++ ;
    if(iCntSW1 > 10000) {
      Serial.println("Switch 1 pressed.......") ;
      iCntSW1 = 10000 ;
      while (digitalRead(iSW1) != true) {
          iCntSW1 = 0 ;
      };
      WiFiSettings.portal() ;
    }
  } else {
    iCntSW1 = 0 ;
  }
}
