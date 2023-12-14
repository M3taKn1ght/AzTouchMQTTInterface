//-----------------------------------------------------
// Az-Touch Control to get infos from IoT-Devices and
// send data to IoT-Devices, all with MQTT
// Autor:   Joern Weise
// License: GNU GPl 3.0
// Created: 19. MAR 2023
// Update:  19. MAR 2023
//-----------------------------------------------------

//Start with needed included
#include <esp_task_wdt.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <TFT_eSPI.h>      // Hardware-specific library
#include "Configuration.h"
#include "WiFi.h"
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "ButtonWidget.h"

//Some needed definitions
#define POSYHEADER 6
#define POSXHEADER 6
#define SPACE 2
#define TFT_BGHEADER 0x9CD3
#define TFT_BGBUTTON 0xAD75
#define TFT_DARKORANGE 0xCB20
#define ERR_INIT 0
#define ERR_WIFI 1
#define ERR_MQTT 2
#define WEEKDAYS 7

//Functions
/*
* =================================================================
* Function:     setup
* Description:  Setup display and sensors   
* Returns:      void
* =================================================================
*/
void setup();

/*
* =================================================================
* Function:     loop
* Description:  Main loop to let program work   
* Returns:      void
* =================================================================
*/
void loop();

/*
* =================================================================
* Function:     touch_calibrate()
* Description:  Calibrate the touch-screen if needed or wanted
* Returns:      void
* =================================================================
*/
void touch_calibrate();

/*
* =================================================================
* Function:     setColorsForDisplay()
* Description:  Set the colors for display-ui
* Returns:      void
* =================================================================
*/
void setColorsForDisplay();

/*
* =================================================================
* Function:     initWifi()
* Description:  Init WiFi
* Returns:      bool
* =================================================================
*/
bool initWifi();

bool reconnectMQTT();

/*
* =================================================================
* Function:     writeTFTInfo()
* Description:  Write info of current task
* IN strInfo:   Short description which status
* Returns:      void
* =================================================================
*/
void writeTFTInfo(String strInfo = "", bool bForStatus = true);

/*
* =================================================================
* Function:     writeTFTStatus()
* Description:  Write on TFT if current status was success or not
* IN bStatus:   If false text will be red, otherwise green. Default is false
* Returns:      void
* =================================================================
*/
void writeTFTStatus(bool bStatus = false);

/*
* =================================================================
* Function:     redrawHeader()
* Description:  Redraw header
* Returns:      void
* =================================================================
*/
void redrawHeader(bool bForceRedraw = false);

/*
* =================================================================
* Function:     writeDeviceName()
* Description:  Write DeviceName to header
* Returns:      void
* =================================================================
*/
void writeDeviceName();

/*
* =================================================================
* Function:     updateTime()
* Description:  Update date/time from NTP-Server
* Returns:      void
* =================================================================
*/
void updateTime();

/*
* =================================================================
* Function:     showTime()
* Description:  Function to update time to header
* Returns:      void
* =================================================================
*/
void showTime();

/*
* =================================================================
* Function:     updateWiFi()
* Description:  Function to redraw WiFi-Signal-Symbol
* Returns:      void
* =================================================================
*/
void updateWiFi();

/*
* =================================================================
* Function:     GetDigits()
* Description:  Return given value as String, with leeding "0"
* IN iValue:    Value for the String
* Returns:      String
* =================================================================
*/
String GetDigits(int iValue);

/*
* =================================================================
* Function:     isSummerTime()
* IN year:      Current year
* IN month:     Current month
* IN day:       Current day
* IN hour:      Current hour
* IN tzHours:   0=UTC, 1=MEZ
* Description:  Check if its sommer or winter time
* Returns:      bool if sommertime, otherwise false
* =================================================================
*/
bool isSummerTime(int year, int month, int day, int hour, byte tzHours=1);

/*
* =================================================================
* Function:     get_signal_quality()
* Description:  Calc signal strength
* Returns:      uint8_t with signal strength
* =================================================================
*/
uint8_t get_signal_quality(int8_t rssi);

/*
* =================================================================
* Function:     initButtons()
* Description:  Init the buttons for the display
* Returns:      void
* =================================================================
*/
void initButtons();

/*
* =================================================================
* Function:     callback()
* Description:  Callback for MQTT to receive and further 
*               process messages
* POINT chTopic:    Topic from msg (char)
* POINT byPayload:  Message from topic
* IN iLength:       Msg-Length
* Returns:  void
* =================================================================
*/
void callback(char* chTopic, byte* byPayload, unsigned int iLength);

/*
* =================================================================
* Function:     checkTouched()
* Description:  Function to check if display was touched
* Returns:      void
* =================================================================
*/
void checkTouched();

/*
* =================================================================
* Function:     updateButton()
* Description:  Overwrite info for button and if needed init 
*               redraw of button
* IN iConfigPos:  Position in vector for all buttons
* IN iSubPos:     Position in Subscribe-Vector
* IN strMsg:      Message from MQTT
* Returns:      void
* =================================================================
*/
void updateButton(int iConfigPos = -1, int iSubPos = -1, String strMsg = "");

/*
* =================================================================
* Function:     updateButtonNewPage()
* Description:  Draw buttons for the current page
* Returns:      void
* =================================================================
*/
void updateButtonNewPage();

/*
* =================================================================
* Function:     getButtonStrings()
* Description:  Returns the available Msgs from element
* IN iConfigPos:  Position in m_vecMQTTConfig
* Returns:      std::vector<String> of saved infos
* =================================================================
*/
std::vector<String> getButtonStrings(int iConfigPos = -1);

/*
* =================================================================
* Function:       writeButtonStrings()
* Description:    Initiates the overwriting of the button text
* IN vecStrings:  Position in m_vecMQTTConfig
* IN byBtnIndex:  Index on display to update string(s)
* Returns:        If redraw of button is needed true, otherwise false
* =================================================================
*/
bool writeButtonStrings(std::vector<String> vecStrings, byte byBtnIndex = 255);

/*
* =================================================================
* Function:       writeButtonStrings()
* Description:    Overwrite the symbol-color for on and off
* REF iColorOn:   Referenz for ON-Color
* REF iColorOff:  Referent fo OFF-Color
* IN byType:      Needed type to get the corret color
* Returns:        void
* =================================================================
*/
void getColorForSymbols(uint16_t &iColorOn, uint16_t &iColorOff, byte byType = typeMax);

/*
* =================================================================
* Function:     buttonPressed()
* Description:  Gets pressed button and check if there needs to
*               publish data to MQTT
* IN iBtn:      Pressed button position
* Returns:      void
* =================================================================
*/
void buttonPressed(int iBtn = -1);

/*
* =================================================================
* Function:     buttonReleased()
* Description:  Check if button needs to publish data after 
                releasing to MQTT
* IN iBtn:      Pressed button position
* Returns:      void
* =================================================================
*/
void buttonReleased(int iBtn = -1);

/*
* =================================================================
* Function:     writeErrorMsg()
* Description:  Writes Error to screen
* IN byError:   Number of errors
* Returns:      void
* =================================================================
*/
void writeErrorMsg();

/*
* =================================================================
* Function:     drawClockFace   
* Returns:      String
* Description:  Draw clock face
* =================================================================
*/
void drawClockFace();

/*
* =================================================================
* Function:     drawClockTime   
* Returns:      void
* Description:  Basefunction to draw hour and minute hands
* =================================================================
*/
void drawClockTime();

/*
* =================================================================
* Function:     drawMinuteHand   
* Returns:      void
* Description:  Draw minute hand on clock face
* =================================================================
*/
void drawMinuteHand(int m, uint16_t color);

/*
* =================================================================
* Function:     drawHourHand   
* Returns:      void
* Description:  Draw hour hand on clock face
* =================================================================
*/
void drawHourHand(int h, int m, uint16_t color);

/*
* =================================================================
* Function:     drawCenterPoint   
* Returns:      void
* Description:  Draw centerpoint on clock face
* =================================================================
*/
void drawCenterPoint();

/*
* =================================================================
* Function:     drawDataTimeInfo   
* Returns:      void
* Description:  Write info about data and time in clock face
* =================================================================
*/
void drawDataTimeInfo();

/*
* =================================================================
* Function:     calcCalenderWeek   
* Returns:      byte
* Description:  Calcs the calender week
* Codesource from
* https://forum.arduino.cc/t/ds1302-clock-calendar-and-week-number/
* with some modifications
* =================================================================
*/
byte calcCalenderWeek();

/*
* -------------------
* ---- Variables ----
* -------------------
*/
Config m_config;
TFT_eSPI m_tft = TFT_eSPI();
std::vector<ButtonWidget> m_vecButtons;
ButtonWidget m_btnOne = ButtonWidget(&m_tft);
ButtonWidget m_btnTwo = ButtonWidget(&m_tft);
ButtonWidget m_btnThree = ButtonWidget(&m_tft);
ButtonWidget m_btnFour = ButtonWidget(&m_tft);
ButtonWidget m_btnFive = ButtonWidget(&m_tft);
ButtonWidget m_btnSix = ButtonWidget(&m_tft);
String daysOfTheWeek[WEEKDAYS] = {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
WiFiUDP m_ntpUDP;
WiFiClient m_espClient;
PubSubClient m_mqttClient(m_espClient);
NTPClient m_timeClient(m_ntpUDP, "europe.pool.ntp.org",3600);
bool m_bFirstRunSuccess = true;
bool m_bFirstrun = true;
bool m_bLastStateTouched = false;
bool m_bShowClock = false;
uint16_t m_calData[5];
int m_iButtonWidth = 104;
int m_iButtonHeight = 104;
byte m_byCurrentPage;
std::vector<MQTTInfos> m_vecMQTTConfig;
int m_iNumberPages;
byte m_byError;
int m_iLockMin = -1;
int m_iLockSec = -1;
uint16_t m_uiBackground;
//Clock
uint16_t m_uiClockOutline;
uint16_t m_uiHourMarks;
uint16_t m_uiMinuteMarks;
uint16_t m_uiClockCenter;
uint16_t m_uiClockHourHand;
uint16_t m_uiClockMinuteHand;
uint16_t m_uiClockText;
//Buttonview
uint16_t m_uiText;
uint16_t m_uiButtonOutline;
uint16_t m_uiButtonFill;
uint16_t m_uiButtonIconON;
uint16_t m_uiButtonICONOFF;
uint16_t m_uiButtonICONStatic;
uint16_t m_uiButtonICONNext;
uint16_t m_uiHeaderOutline;
uint16_t m_uiHeaderInfill;

