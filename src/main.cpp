#include "main.h"

void setup()
{
  Serial.begin(DEBUG_BAUDRATE);
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, TFT_LED_STAT);
  m_tft.begin();
  m_tft.setRotation(1);
  m_tft.fillScreen(TFT_BLACK);
  m_tft.setCursor(20, 0);
  m_tft.setTextFont(2);
  m_tft.setTextColor(TFT_RED, TFT_BLACK);
  m_tft.drawCentreString("AzTouchMQTTInterface",m_tft.width()/2, m_tft.getCursorY(), 2);
  m_tft.println();
  m_tft.setTextColor(TFT_GREEN, TFT_BLACK);
  m_tft.setCursor(20, m_tft.getCursorY());
  m_tft.drawCentreString("Create by Joern Weise",m_tft.width()/2, m_tft.getCursorY(), 2);
  m_tft.println();
  m_tft.println();
  m_tft.setTextSize(1);
#if DEBUG
  writeTFTInfo("Buildversion: " + String(BUILDVERSION) + " - DEBUG",false);
#else
  writeTFTInfo("Buildversion: " + String(BUILDVERSION) + " - RELEASE",false);
#endif
  writeTFTInfo("Builddate: " + String(__DATE__) + " - " + String(__TIME__), false);
  m_tft.setTextColor(TFT_WHITE, TFT_BLACK);
  writeTFTInfo("TFT-Init");
  writeTFTStatus(true);
  writeTFTInfo("Mount SD");
  if(!m_config.initSD())
  {
    m_tft.setTextColor(TFT_RED, TFT_BLACK);
    writeTFTStatus();
    m_tft.println();
    m_tft.setFreeFont(FSSB9);
    m_tft.drawCentreString("Please reboot device",m_tft.width()/2, m_tft.getCursorY(), GFXFF);
    m_tft.println();
    m_tft.drawCentreString("or check SD-card!",m_tft.width()/2, m_tft.getCursorY(), GFXFF);
#if DEBUG
    Serial.println("Unable to make any more actions");
#endif
    while(true){};
  }
  writeTFTStatus(true);
  writeTFTInfo("Read calibration");
  if(m_config.checkTouchFileExists())
  {
    if(!m_config.readTouchCalibration(m_calData[0], m_calData[1], m_calData[2], m_calData[3], m_calData[4]))
    {
      writeTFTStatus();
    }
    m_tft.setTouch(m_calData);
    writeTFTStatus(true);
  }
  else
  {
    touch_calibrate();
    m_config.writeTouchCalibration(m_calData[0], m_calData[1], m_calData[2], m_calData[3], m_calData[4]);
    m_tft.fillScreen(TFT_BLACK);
    m_tft.setCursor(20, 0);
    m_tft.setTextFont(2);
    m_tft.setTextColor(TFT_RED, TFT_BLACK);
    m_tft.drawCentreString("Az-Touch Control",m_tft.width()/2, m_tft.getCursorY(), 2);
    m_tft.println();
    m_tft.setTextColor(TFT_GREEN, TFT_BLACK);
    m_tft.setCursor(20, m_tft.getCursorY());
    m_tft.drawCentreString("Create by Joern Weise",m_tft.width()/2, m_tft.getCursorY(), 2);
    m_tft.println();
    m_tft.println();
    m_tft.setTextSize(1);
#if DEBUG
  writeTFTInfo("Buildversion: " + String(BUILDVERSION) + " - DEBUG",false);
#else
  writeTFTInfo("Buildversion: " + String(BUILDVERSION) + " - RELEASE",false);
#endif
    writeTFTInfo("Builddate: " + String(__DATE__) + " - " + String(__TIME__), false);
    m_tft.setTextColor(TFT_WHITE, TFT_BLACK);
    writeTFTInfo("TFT-Init");
    writeTFTStatus(true);
    writeTFTInfo("Mount SD");
    writeTFTStatus(true);
    writeTFTInfo("Write calibration");
    writeTFTStatus(true);
  }
  writeTFTInfo("Read General conf");
  if(!m_config.readGeneralConfig())
  {
    m_tft.setTextColor(TFT_RED, TFT_BLACK);
    writeTFTStatus();
    m_bFirstRunSuccess = false;
    while(true){};
  }
  writeTFTStatus(true);
  setColorsForDisplay();
  writeTFTInfo("Read MQTT conf");
  if(!m_config.readMQTTConfig(m_vecMQTTConfig, m_iNumberPages))
  {
    m_tft.setTextColor(TFT_RED, TFT_BLACK);
    writeTFTStatus();
    m_bFirstRunSuccess = false;
    while(true){};
  }
  writeTFTStatus(true);
#if DEBUG
  Serial.println("----------MQTT-Config vec read----------");
  for(int iCnt = 0; iCnt < m_vecMQTTConfig.size(); iCnt++)
  {
    Serial.println("---Vec-Pos " + String(iCnt) + " ---");
    Serial.println("Read name: " + m_vecMQTTConfig[iCnt].strName);
    Serial.println("Read type: " + String(m_vecMQTTConfig[iCnt].byType));
    Serial.println("Read pos: " + String(m_vecMQTTConfig[iCnt].byPos));
    Serial.println("Read page: " + String(m_vecMQTTConfig[iCnt].byPage));
    Serial.println("Read Pub: " + m_vecMQTTConfig[iCnt].strMQTTPub);

    for(int iSub = 0; iSub < m_vecMQTTConfig[iCnt].vecMQTTSub.size(); iSub++)
    {
      Serial.println("Sub topic " + String(iSub) + ": " + m_vecMQTTConfig[iCnt].vecMQTTSub[iSub]);
      Serial.println("Sub Unit " + String(iSub) + ": " + m_vecMQTTConfig[iCnt].vecSubUnit[iSub]);
    }
  }
#endif
  writeTFTInfo("Init WiFi");
  if(!initWifi())
  {
    writeTFTStatus();
    m_bFirstRunSuccess = false;
    return;
  }
  writeTFTStatus(true);
  m_timeClient.begin();

  if(m_config.brokerIsIP())
  {
    m_mqttClient.setServer(m_config.getBrokerIP(), m_config.getBrokerPort());
  }
  else
  {
    m_mqttClient.setServer(m_config.getBrokerString().c_str(), m_config.getBrokerPort());
  }
  m_mqttClient.setCallback(callback);
  if(m_config.getWatchdogActive())
  {
    Serial.println("Watchdog activated");
    writeTFTInfo("Watchdog activated",false);
    esp_task_wdt_init(m_config.getWatchdogIntervall(),true);
    esp_task_wdt_add(NULL);
    writeTFTInfo("Watchdogtimer: " + String(m_config.getWatchdogIntervall()) + " sec",false);
#if DEBUG
    Serial.println("Watchdogtimer: " + String(m_config.getWatchdogIntervall()) + " sec");
#endif
  }
  else
  {
    writeTFTInfo("Watchdog deactivated",false);
#if DEBUG
    Serial.println("Watchdog deactivated");
#endif
  }
  writeTFTInfo("Initialisation");
  writeTFTStatus(true);
#if DEBUG
  Serial.println("initialisation done.");
#endif
  //Reset watchdog only needed, if active
  if(m_config.getWatchdogActive())
  {
    esp_task_wdt_reset();
  }
  delay(5000);
  m_tft.setFreeFont(FSSB9);
  m_iButtonHeight = (TFT_WIDTH - (4 * SPACE) - (m_tft.fontHeight(GFXFF) + SPACE)) / BUTTON_COLUMNS;
  m_iButtonWidth = (TFT_HEIGHT - (4 * SPACE)) / BUTTON_ROWS;
#if DEBUG    
  Serial.println("Fontsize: " + String(m_tft.fontHeight(GFXFF)));
  Serial.println("Button widht: " + String(m_iButtonWidth));
  Serial.println("Button height: " + String(m_iButtonHeight));
#endif
  m_byCurrentPage = 1;
  if(m_config.getShowTime())
  {
    m_bShowClock = true;
  }
#if DEBUG
  Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " bytes");
#endif
}

void loop()
{
  m_byError = 0b00000000;

  //First run, so draw overview on display
  if(m_bFirstrun)
  {
    m_tft.setFreeFont(FSSB9);
    m_tft.fillScreen(m_uiBackground);
    initButtons();
  }

  //Here the system will stuck forever, not the best deal
  if(!m_bFirstRunSuccess)
  {
    bitWrite(m_byError, ERR_INIT, 1);
  }
  else if(WiFi.status() != WL_CONNECTED)
  {
    if(!initWifi())
    {
      bitWrite(m_byError, ERR_WIFI, 1);
    }
  }

  //Connection check for MQTT, should not be done every loop
  static unsigned ulLastConCheck = millis();
  if(m_bFirstrun || (millis() - ulLastConCheck) >= 100)
  {
    ulLastConCheck = millis();
    if(!m_mqttClient.connected())
    {
      if(!reconnectMQTT())
      {
        bitWrite(m_byError, ERR_MQTT, 1);
      }
    }
  }

  m_mqttClient.loop();  //Needed to get the info
  
  if(m_config.getShowTime())
  {
    //Lock screen if calculate min and sec reached AND clock is not shown AND no errors
    if(minute() == m_iLockMin && m_iLockSec <= second() && !m_bShowClock && m_byError == 0b00000000)
    {
      m_bShowClock = true;
    }
  }
  else
  {
    m_bShowClock = false;
  }

  if(m_byError == 0b00000000)
  {
    checkTouched(); //Did someone touch me?
    updateTime();   //Check if there needs to be an NTP-Update
  }
  
  //Interesting for later
  static byte byLastPage = m_byCurrentPage;
  static bool bLastShowClock = m_bShowClock;
  if((byLastPage != m_byCurrentPage || (m_bFirstrun && !m_bShowClock)) && m_byError == 0b00000000)
  {
    updateButtonNewPage();
    byLastPage = m_byCurrentPage;
  }
  else if((bLastShowClock != m_bShowClock) || ((m_bFirstrun && m_bShowClock)) && m_byError == 0b00000000)
  {
    if(m_bShowClock)
    {
      drawClockFace();
    }
    bLastShowClock = m_bShowClock;
  }

  if(m_bShowClock && m_byError == 0b00000000)
  {
    drawClockTime();
  }
  static byte byLastError = 0b00000000;
  if(m_byError != byLastError)
  {
      if(m_byError != 0b00000000)
      {
        writeErrorMsg();
      }
      else
      {
        if(!m_bShowClock)
        {
          m_tft.fillScreen(m_uiBackground);
          updateButtonNewPage();
          redrawHeader(true);
        }
        else
        {
          drawClockFace();
          drawClockTime();
        }
      }
      byLastError = m_byError;
  }
  
  redrawHeader(); //Header if needed

  if(m_bFirstrun)
  {
    m_bFirstrun = false;
  }

  //Reset watchdog only needed, if active
  if(m_config.getWatchdogActive())
  {
    esp_task_wdt_reset(); //Reset for the watchdog
  }
}

void writeTFTInfo(String strInfo, bool bForStatus)
{
  m_tft.setTextColor(TFT_WHITE, TFT_BLACK);
  m_tft.setCursor(20,m_tft.getCursorY());
  if(bForStatus)
  {
    m_tft.print(strInfo+ ": ");
  }
  else
  {
    m_tft.println(strInfo);
  }
}

void writeTFTStatus(bool bStatus)
{
  if(bStatus)
  {
    m_tft.setTextColor(TFT_GREEN, TFT_BLACK);
    m_tft.drawCentreString("DONE",m_tft.width()/2  + 20, m_tft.getCursorY(),2);
    m_tft.println();
  }
  else
  {
    m_tft.setTextColor(TFT_RED, TFT_BLACK);
    m_tft.drawCentreString("FAILED",m_tft.width()/2 + 20, m_tft.getCursorY(),2);
    m_bFirstRunSuccess = false;
    m_tft.println();
  }
}

bool initWifi()
{
  unsigned long ulMillis = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin(m_config.getWifiSSID().c_str(), m_config.getWiFiPass().c_str());
#if DEBUG
  Serial.print("Connection to WiFi .");
#endif
  while(WiFi.status() != WL_CONNECTED)
  {
#if DEBUG
  Serial.print(" .");
#endif
    if (WiFi.status() == WL_CONNECT_FAILED)
    {
#if DEBUG
      Serial.print(" FAILED");
#endif
      return false;
    }
    //After a defined time, cancel connection
    if(millis() - ulMillis  > WLANTIMEOUT)
    {
#if DEBUG
      Serial.println(" FAILED");
      Serial.println("WiFi timeout.");
#endif
      return false;
    }
    delay(500);
  }
#if DEBUG
    Serial.println(" DONE");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
#endif
  return true;
}

bool reconnectMQTT()
{
  unsigned long ulCancel = millis() + int(WLANTIMEOUT);
  unsigned long ulLastTime;
  bool bFirstRun = true;

  //Reset watchdog only needed, if active
  if(m_config.getWatchdogActive())
  {
    esp_task_wdt_reset();
  }

  while (!m_mqttClient.connected()) 
  {
    if(millis() >= ulCancel)
    {
      break;
    }
    if(((millis() - ulLastTime) > 2000) || bFirstRun)
    {
      Serial.println("Try to connect.....");
#if DEBUG
      Serial.println("MQTT-User: " + m_config.getMQTTUser());
      Serial.println("MQTT-Pass: " + m_config.getMQTTPass());
#endif     
      if(m_config.getMQTTUser().length() != 0 && m_config.getMQTTPass().length() != 0)
      {
        Serial.println("Try with User and pass");
        m_mqttClient.connect(m_config.getClientName().c_str(), m_config.getMQTTUser().c_str(), m_config.getMQTTPass().c_str());
      }
      else
      {
#if DEBUG
        Serial.println("Try without User and pass");
#endif
        m_mqttClient.connect(m_config.getClientName().c_str());
      }
#if DEBUG
      if(!m_mqttClient.connected())
      {
        Serial.print("failed, rc=");
        Serial.println(m_mqttClient.state());
      }
#endif
      ulLastTime = millis();
    }

    //Reset watchdog only needed, if active
    if(m_config.getWatchdogActive())
    {
      esp_task_wdt_reset();
    }

    bFirstRun = false;
  }
  static std::vector<String> vecSubTopics;
  if(vecSubTopics.size() == 0)
  {
    for(int iCnt = 0; iCnt < m_vecMQTTConfig.size(); iCnt++)
    {
      for(int iSub = 0; iSub < m_vecMQTTConfig[iCnt].vecMQTTSub.size(); iSub++)
      {
        vecSubTopics.push_back(m_vecMQTTConfig[iCnt].vecMQTTSub[iSub]);
      }
    }
  }
  if(m_mqttClient.connected())
  {
    for(int iSubs = 0; iSubs < vecSubTopics.size(); iSubs++)
    {
#if DEBUG
      Serial.println("Subscribe to channel: " + vecSubTopics[iSubs]);
#endif
      bool bConfirmed = m_mqttClient.subscribe(vecSubTopics[iSubs].c_str());
#if DEBUG
      Serial.println("Confirmed: " + String(bConfirmed));
#endif      
    }
  }
#if DEBUG
      Serial.print("Status MQTT: ");
      if(m_mqttClient.connected())
      {
        Serial.println("Connected");
      }
      else
      {
        Serial.println("NOT Connected");
      }
#endif
  return m_mqttClient.connected();
}

void touch_calibrate()
{
  uint8_t calDataOK = 0;

  // Calibrate
  m_tft.fillScreen(TFT_BLACK);
  m_tft.setCursor(20, 0);
  m_tft.setTextFont(2);
  m_tft.setTextSize(1);
  m_tft.setTextColor(TFT_WHITE, TFT_BLACK);

  m_tft.println("Touch corners as indicated");

  m_tft.setTextFont(1);
  m_tft.println();

  m_tft.calibrateTouch(m_calData, TFT_MAGENTA, TFT_BLACK, 15);
  m_tft.setTouch(m_calData);
  m_tft.fillScreen(TFT_BLACK);
  m_tft.setTextColor(TFT_GREEN, TFT_BLACK);
}

void setColorsForDisplay()
{
  //Definition for clock
  
  m_uiClockHourHand   = TFT_RED;
  if(m_config.darkModeactive())
  {
    //Definition clock
    m_uiBackground      = TFT_BLACK;
    m_uiHourMarks       = TFT_SILVER;
    m_uiMinuteMarks     = TFT_SILVER;
    m_uiClockCenter     = TFT_SILVER;
    m_uiClockMinuteHand = TFT_SILVER;
    m_uiClockOutline    = TFT_GOLD;
    m_uiClockText       = TFT_GOLD;
    //Buttonview
    m_uiText              = TFT_SILVER;
    m_uiButtonOutline     = TFT_SILVER;
    m_uiButtonFill        = 0x39C7;
    m_uiButtonIconON      = TFT_GREEN;
    m_uiButtonICONOFF     = TFT_RED;
    m_uiButtonICONStatic  = TFT_BLACK;
    m_uiButtonICONNext    = TFT_SILVER;
    m_uiHeaderOutline     = TFT_SILVER;
    m_uiHeaderInfill      = 0x4A49;
  }
  else
  {
    //Definition clock
    m_uiBackground      = TFT_SILVER;
    m_uiHourMarks       = TFT_BLACK;
    m_uiMinuteMarks     = TFT_BLACK;
    m_uiClockCenter     = TFT_BLACK;
    m_uiClockMinuteHand = TFT_BLACK;
    m_uiClockOutline    = 0xFA20;
    m_uiClockText       = 0xFA20;
    //Buttonview
    m_uiText              = TFT_BLACK;
    m_uiButtonOutline     = TFT_BLACK;
    m_uiButtonFill        = 0xAD75;
    m_uiButtonIconON      = TFT_GREEN;
    m_uiButtonICONOFF     = TFT_RED;
    m_uiButtonICONStatic  = 0x8C71;
    m_uiButtonICONNext    = TFT_BLACK;
    m_uiHeaderOutline     = TFT_BLACK;
    m_uiHeaderInfill      = 0x9CD3;
  }
}

void writeDeviceName()
{
  m_tft.setTextColor(m_uiText, m_uiHeaderInfill);
  if(m_config.getDeviceName().length() <= 18)
  {
    m_tft.drawCentreString(m_config.getDeviceName(),m_tft.width()/2 ,POSYHEADER, GFXFF);
  }
  else
  {
    String strName = m_config.getDeviceName().substring(0,15);
    strName+= " ..";
    m_tft.drawCentreString(strName,m_tft.width()/2 ,POSYHEADER, GFXFF);
  }
}

void redrawHeader(bool bForceRedraw)
{
  static int iLastMinute = -1;
  if(m_bShowClock)
  {
    return;
  }
  if((iLastMinute != minute() || m_bFirstrun || bForceRedraw)) //&& !m_bShowClock)
  {
    m_tft.fillRoundRect(SPACE, SPACE, m_tft.width() - (SPACE * 2), m_tft.fontHeight(GFXFF) + SPACE, 4, m_uiHeaderInfill);
    m_tft.drawRoundRect(SPACE, SPACE, m_tft.width() - (SPACE * 2), m_tft.fontHeight(GFXFF) + SPACE, 4, m_uiHeaderOutline);
    writeDeviceName();
    showTime();
    updateWiFi();
    iLastMinute = minute();
  }
}

void showTime()
{
  if(!m_config.getShowTime())
  {
    return;
  }

  m_tft.setTextColor(m_uiText, m_uiHeaderInfill);
  if(isSummerTime(year(),month(),day(),hour(),1))
  {     
    m_tft.drawString(GetDigits(hour() + 1) + " : " + GetDigits(minute()), POSXHEADER, POSYHEADER, GFXFF);
  }
  else
  {
    m_tft.drawString(GetDigits(hour()) + " : " + GetDigits(minute()), POSXHEADER, POSYHEADER, GFXFF);
  }  
}

void updateWiFi()
{
  uint8_t iquality = get_signal_quality(WiFi.RSSI());
  uint16_t color = TFT_RED;
#if DEBUG
  Serial.println("Signalquality: " + String(iquality));
#endif
  if(iquality>=50)
  {
    color = TFT_DARKGREEN;
  }
  else if(iquality > 20 && iquality < 50)
  {
    color = TFT_DARKORANGE;
  }
  m_tft.drawBitmap(m_tft.width() - (SPACE * 2)- POSXHEADER - SIZE_SMALL_IMAGE , SPACE, wifi_icon, SIZE_SMALL_IMAGE, SIZE_SMALL_IMAGE, color);
}

void updateTime()
{
  if(!m_config.getShowTime())
  {
    return;
  }

  static bool bUpdateDone;
  static unsigned long ulLastUpdate;
  if(m_bFirstrun || (hour() == 1 && !bUpdateDone))
  {
    if(!m_bFirstrun && (millis() - ulLastUpdate) < 120000)
    {
#if DEBUG
      Serial.println("Skip update");
#endif
      bUpdateDone = true;
      return;
    }
    ulLastUpdate = millis();
    
    unsigned long ulUpdateTimeout = millis() + (unsigned long)10000;  //This could be a problem
    while(!m_timeClient.update() && ulUpdateTimeout > millis()) 
    {
      m_timeClient.forceUpdate();

      //Reset watchdog only needed, if active
      if(m_config.getWatchdogActive())
      {
        esp_task_wdt_reset();
      }

    }

    if(ulUpdateTimeout <= millis())
    {
#if DEBUG
      Serial.println("Timeout during ntp-Update");
      bUpdateDone = true;
#endif
      return;
    }

    setTime(m_timeClient.getEpochTime());
#if DEBUG
    Serial.println("Update time");
    Serial.println("Current time: " + GetDigits(hour()) + ":" + GetDigits(minute()) + ":" + GetDigits(second()));
    Serial.println("Current date: " + GetDigits(day()) + "/" + GetDigits(month()) + "/" + String(year()));
#endif
    bUpdateDone = true;
  }
  if(hour() != 1)
  {
    bUpdateDone = false;
  }
  
}

String GetDigits(int iValue)
{
  String strValue = "";
  if(iValue < 10)
  {
    strValue += "0";
  }
  strValue += String(iValue);
  return strValue;
}

bool isSummerTime(int year, int month, int day, int hour, byte tzHours)
{ 
  static int x1,x2, lastyear; // Zur Beschleunigung des Codes ein Cache für einige statische Variablen
  static byte lasttzHours;
  int x3;
  if (month < 3 || month > 10)
  {
    return false; // No sommertime in Jan, Feb, Nov, Dez
  }
  if (month > 3 && month < 10)
  {
    return true; // Sommertime in Apr, Mai, Jun, Jul, Aug, Sep
  }

  // Changeover start und -end
  if (year != lastyear || tzHours != lasttzHours) 
  { 
    x1= 1 + tzHours + 24*(31 - (5 * year /4 + 4) % 7);  
    x2= 1 + tzHours + 24*(31 - (5 * year /4 + 1) % 7);
    lastyear = year;
    lasttzHours = tzHours;
  }

  x3= hour + 24 * day;
  if (month == 3 && x3 >= x1 || month == 10 && x3 < x2)
  {
    return true;
  }
  else 
  {
    return false;
  }
}

uint8_t get_signal_quality(int8_t rssi)
{
    if (rssi < -100)
    {
      return 0;
    }
    else if (rssi > -50)
    {
      return 100;
    }
    else
    {
      return 2 * (rssi + 100);
    }
}

void initButtons()
{
  int iCnt = 0;
  bool invert = false;
  m_vecButtons.push_back(m_btnOne);
  m_vecButtons.push_back(m_btnTwo);
  m_vecButtons.push_back(m_btnThree);
  m_vecButtons.push_back(m_btnFour);
  m_vecButtons.push_back(m_btnFive);
  m_vecButtons.push_back(m_btnSix);
  for(int iColumns = 0; iColumns < BUTTON_COLUMNS; iColumns++)
  {
    for(int iRows = 0; iRows < BUTTON_ROWS; iRows++)
    {
      m_vecButtons[iCnt].initButton((iRows + 1) * SPACE + (iRows * m_iButtonWidth), (3 * SPACE + m_tft.fontHeight(GFXFF)) + (iColumns * SPACE) + (iColumns * m_iButtonHeight), m_iButtonWidth, m_iButtonHeight, m_uiButtonOutline, m_uiButtonFill, m_uiText, 6, SPACE);
      iCnt++;
    }
  }
}

void callback(char* chTopic, byte* byPayload, unsigned int iLength)
{
#if DEBUG
  Serial.print("Message arrived [");
  Serial.print(chTopic);
  Serial.print("] ");
  for (int iCnt = 0; iCnt < iLength; iCnt++) 
  {
    Serial.print((char)byPayload[iCnt]);
  }
  Serial.println();
#endif
  String strMsg = "";
  for (int iCnt = 0  ;iCnt  < iLength; iCnt++) 
  {
    strMsg += String((char)byPayload[iCnt]);
  }
 #if DEBUG
  Serial.println("Newest message: " + strMsg);
 #endif 
  for(int iCnt = 0; iCnt < m_vecMQTTConfig.size(); iCnt++)
  {
    for(int iSubCnt = 0; iSubCnt < m_vecMQTTConfig[iCnt].vecMQTTSub.size(); iSubCnt++)
    {
      if(m_vecMQTTConfig[iCnt].vecMQTTSub[iSubCnt] == String(chTopic))
      {
        if(m_byError == 0)
        {
          updateButton(iCnt, iSubCnt, strMsg);
        }
        break;
      }
    }
  }
}

void checkTouched()
{
  uint16_t t_x = 9999, t_y = 9999;
  bool bTouched = m_tft.getTouch(&t_x, &t_y);
  static unsigned long ulLastTouch = millis();
  if(millis() - ulLastTouch >= 50)
  {
    if(m_bLastStateTouched != bTouched)
    {
      m_bLastStateTouched = bTouched;
      m_iLockMin = minute() + m_config.getClockTimeout();
      if(m_iLockMin > 59)
      {
        m_iLockMin -= 60;
      }
      m_iLockSec = second();
 #if DEBUG
        Serial.println("Timeout min: " + String(m_iLockMin));
        Serial.println("Timeout sec: " + String(m_iLockSec));
#endif       
      if(m_bShowClock)
      {
        m_bShowClock = false;
        m_tft.fillScreen(m_uiBackground);
        updateButtonNewPage();
        redrawHeader(true);
        return;
      }
      static int iTouchIndex = -1;
      if(bTouched)
      {
#if DEBUG
        Serial.println("Touchcoord - x: " + String(t_x) + " y: " + String(t_y));
#endif      
        for(int iCnt = 0; iCnt < m_vecButtons.size(); iCnt++)
        {
          if(m_vecButtons[iCnt].contains(t_x, t_y))
          {       
            buttonPressed(iCnt);
            if(iTouchIndex != -1)
            {
              buttonReleased(iTouchIndex);
            }
            iTouchIndex = iCnt;
            break;
          }
        }
      }
      else if(!bTouched && iTouchIndex != -1)
      {       
        buttonReleased(iTouchIndex);
        iTouchIndex = -1;
      }
    }
  }
}

void updateButton(int iConfigPos, int iSubPos, String strMsg)
{
  //This should not happen xo
  if(iConfigPos == -1 || iSubPos == -1 || strMsg.length() == 0)
  { 
    return;
  }
  byte byType = m_vecMQTTConfig[iConfigPos].byType;
  byte iBtn = m_vecMQTTConfig[iConfigPos].byPos - 1;
  if(iBtn > 5)
  {
    return;
  }
  if(byType == typeInfo || byType == typeMeasur)
  {
    int iIndex = strMsg.indexOf(".");
    String strSubString = strMsg.substring(0, iIndex);  
    m_vecMQTTConfig[iConfigPos].vecSubValue[iSubPos] = strSubString;
    if(m_bShowClock)
    {
      return;
    }
    if(m_vecMQTTConfig[iConfigPos].byPage == m_byCurrentPage)
    {
      std::vector<String> vecButtonTexts = getButtonStrings(iConfigPos);
      if(writeButtonStrings(vecButtonTexts, iBtn) )
      {
        m_vecButtons[iBtn].redrawButton(m_uiBackground);
      }
    }
  }
  else if(byType == typeLight || byType == typeSwitch || byType == typeSocket)
  {
    strMsg.toLowerCase();
    if(strMsg == "on")
    {
      m_vecMQTTConfig[iConfigPos].vecSubValue[iSubPos] = "on";
      if(!m_vecButtons[iBtn].getState())
      {
        m_vecButtons[iBtn].setOnStatus(true, !m_bShowClock);
      }
    }
    else
    {
      m_vecMQTTConfig[iConfigPos].vecSubValue[iSubPos] = "off";
      if(m_vecButtons[iBtn].getState())
      {
        m_vecButtons[iBtn].setOnStatus(false, !m_bShowClock);
      }
    }
  }
  else if(byType == typeShutter)
  {
    //Here needs to be some code!!
  }
}

void updateButtonNewPage()
{
  //First erase all buttons
#if DEBUG
  Serial.println("Update buttons for new page - " + String(m_byCurrentPage));
#endif
  for(int iCnt = 0; iCnt < m_vecButtons.size(); iCnt++)
  {
    m_vecButtons[iCnt].setButtonText();
    m_vecButtons[iCnt].setNewType();
    m_vecButtons[iCnt].redrawButton(m_uiBackground);
  }
  
  for(int iCntVec = 0; iCntVec < m_vecMQTTConfig.size(); iCntVec++)
  {
    if(m_vecMQTTConfig[iCntVec].byPage != m_byCurrentPage)
    {
      continue;
    }
    byte byBtnIndex = m_vecMQTTConfig[iCntVec].byPos - 1;
    if(byBtnIndex > m_vecButtons.size())
    {
      continue;
    }
#if DEBUG
  Serial.println("New setting for button " + m_vecMQTTConfig[iCntVec].strName);
  Serial.print("Get new colors....");
#endif   
    byte byType = m_vecMQTTConfig[iCntVec].byType;
    uint16_t iColorOn, iColorOff;
    getColorForSymbols(iColorOn, iColorOff, byType);
#if DEBUG
    Serial.println("DONE");
    Serial.print("Set text and status .... ");
#endif 
    if(byType == typeNext)
    {
      m_vecButtons[byBtnIndex].setButtonText();
      m_vecButtons[byBtnIndex].setOnStatus();
    }
    else if(byType != typeInfo && byType != typeMeasur)
    {
      m_vecButtons[byBtnIndex].setButtonText(m_vecMQTTConfig[iCntVec].strName);
      m_vecButtons[byBtnIndex].setOnStatus(m_vecMQTTConfig[iCntVec].vecSubValue[0] == "on", false);
    }
    else
    {
      std::vector<String> vecButtonTexts = getButtonStrings(iCntVec);
      writeButtonStrings(vecButtonTexts, byBtnIndex);
      m_vecButtons[byBtnIndex].setOnStatus(false, false);
    }
#if DEBUG
  Serial.println("DONE");
#endif
    m_vecButtons[byBtnIndex].setNewType(byType, iColorOn, iColorOff);
#if DEBUG
  Serial.println("Redraw button " + m_vecMQTTConfig[iCntVec].strName);
#endif    
    m_vecButtons[byBtnIndex].redrawButton(m_uiBackground);
#if DEBUG
  Serial.println("Redraw button done.");
#endif  
  }
}

std::vector<String> getButtonStrings(int iConfigPos)
{
  std::vector<String> vecReturn;
  vecReturn.clear();

  if(iConfigPos == -1)
  {
    return vecReturn;
  }

  for(int iCnt = 0; iCnt < m_vecMQTTConfig[iConfigPos].vecSubValue.size(); iCnt++)
  {
    vecReturn.push_back(m_vecMQTTConfig[iConfigPos].vecSubValue[iCnt] + " " + m_vecMQTTConfig[iConfigPos].vecSubUnit[iCnt]);
  }

  return vecReturn;
}

bool writeButtonStrings(std::vector<String> vecStrings, byte byBtnIndex )
{
  if(byBtnIndex == 255 || byBtnIndex > m_vecButtons.size())
  {
    return false;
  }

  if(vecStrings.size() == 1)
  {
    return m_vecButtons[byBtnIndex].setButtonText(vecStrings[0]);
  }
  else if(vecStrings.size() == 2)
  {
    return m_vecButtons[byBtnIndex].setButtonText(vecStrings[0], vecStrings[1]);
  }
  else if(vecStrings.size() == 3)
  {
    return m_vecButtons[byBtnIndex].setButtonText(vecStrings[0], vecStrings[1], vecStrings[2]);
  }
  return false;
}

void getColorForSymbols(uint16_t &iColorOn, uint16_t &iColorOff, byte byType)
{
  if(byType == typeInfo || byType == typeMeasur || byType == typeShutter)
  {
    iColorOn = m_uiButtonICONStatic;
    iColorOff = m_uiButtonICONStatic;
  }
  else if(byType == typeLight || byType == typeSwitch || byType == typeSocket)
  {
    iColorOn = m_uiButtonIconON;
    iColorOff = m_uiButtonICONOFF;
  }
  else if(byType == typeNext)
  {
    iColorOn = m_uiButtonICONNext;
    iColorOff = m_uiButtonICONNext;
  }
  else
  {
    iColorOn = m_uiButtonFill;
    iColorOff = m_uiButtonFill;
  }
}

void buttonPressed(int iBtn)
{
  if(iBtn == -1)
  {
    return;
  }
#if DEBUG
  Serial.println("Button " + String(iBtn + 1) + " was pressed!");
#endif 
  for(int iCnt = 0; iCnt < m_vecMQTTConfig.size(); iCnt++)
  {
    if(m_vecMQTTConfig[iCnt].byPage == m_byCurrentPage && m_vecMQTTConfig[iCnt].byPos == (iBtn + 1))
    {
      if(m_vecMQTTConfig[iCnt].byType == typeLight || m_vecMQTTConfig[iCnt].byType == typeSwitch || m_vecMQTTConfig[iCnt].byType == typeSocket)
      {
        bool bCurrentStat = m_vecButtons[iBtn].getState();
        if(m_vecMQTTConfig[iCnt].strMQTTPub.length() != 0)
        {
          if(bCurrentStat)
          {
            m_mqttClient.publish(m_vecMQTTConfig[iCnt].strMQTTPub.c_str(), "off");
          }
          else
          {
            m_mqttClient.publish(m_vecMQTTConfig[iCnt].strMQTTPub.c_str(), "on");
          }
        }
      }
      else if(m_vecMQTTConfig[iCnt].byType == typeNext)
      {
        m_byCurrentPage++;
        if(m_byCurrentPage > m_iNumberPages)
          m_byCurrentPage = 1;
      }
      break;
    }
  }
}

void buttonReleased(int iBtn)
{
    if(iBtn == -1)
  {
    return;
  }
#if DEBUG
  Serial.println("Button " + String(iBtn + 1) + " was released!");
#endif   
  for(int iCnt = 0; iCnt < m_vecMQTTConfig.size(); iCnt++)
  {
    if(m_vecMQTTConfig[iCnt].byPage == m_byCurrentPage && m_vecMQTTConfig[iCnt].byPos == (iBtn + 1))
    {
      if(m_vecMQTTConfig[iCnt].byType == typeShutter)
      {
        bool bCurrentStat = m_vecButtons[iBtn].getState();
        if(m_vecMQTTConfig[iCnt].strMQTTPub.length() != 0)
        {
          if(bCurrentStat)
          {
            m_mqttClient.publish(m_vecMQTTConfig[iCnt].strMQTTPub.c_str(), "off");
          }
          else
          {
            m_mqttClient.publish(m_vecMQTTConfig[iCnt].strMQTTPub.c_str(), "on");
          }
        }
      }
      break;
    }
  }
}

void writeErrorMsg()
{
  bool bFound = false;
  String strError = "";
  if(bitRead(m_byError, ERR_MQTT) == 1)
  {
    strError = "Error connection MQTT...";
    bFound = true;
  }
  else if(bitRead(m_byError, ERR_WIFI) == 1)
  {
    strError = "Error WiFi-Connection...";
    bFound = true;
  }
  else if(bitRead(m_byError, ERR_INIT) == 1)
  {
    strError = "Error during init...";
    bFound = true;
  }
  else if(!bFound)
  {
    strError = "Unknown error detected...";
  }
  m_tft.fillRoundRect(SPACE, (int)(m_tft.height() / 2 - m_tft.fontHeight(GFXFF) / 2), m_tft.width() - (SPACE * 2), m_tft.fontHeight(GFXFF) + SPACE, 4, TFT_RED);
  m_tft.drawRoundRect(SPACE, (int)(m_tft.height() / 2 - m_tft.fontHeight(GFXFF) / 2), m_tft.width() - (SPACE * 2), m_tft.fontHeight(GFXFF) + SPACE, 4, TFT_YELLOW);
  m_tft.setTextColor(TFT_BLACK);
  m_tft.drawCentreString(strError,m_tft.width()/2 ,(int)(m_tft.height() / 2 - m_tft.fontHeight(GFXFF) / 2 + 3), GFXFF);
}

void drawClockFace()
{
  int i = 0, angle = 0;
  int Xo = int(TFT_HEIGHT/2);
  int Yo = int(TFT_WIDTH/2);
  int RADIUS = (Yo - 20);

  m_tft.fillScreen(m_uiBackground);
  // Draw inner frame
  m_tft.fillCircle(Xo, Yo, RADIUS + 8, m_uiClockOutline);
  m_tft.fillCircle(Xo, Yo, RADIUS + 6, m_uiBackground);

  //Draw minute marks
  for (i = 0; i < 360; i += 6)
  {
    m_tft.drawPixel(Xo + round(RADIUS * cos(i * M_PI / 180)), Yo + round(RADIUS * sin(i * M_PI / 180)), m_uiMinuteMarks);
  }

  //Draw Numeric point
  for (i = 0; i <= 12; i++)
  {
    int x = Xo + round(RADIUS * cos(angle * M_PI / 180) );
    int y = Yo + round(RADIUS * sin(angle * M_PI / 180) );
    m_tft.fillCircle(x, y, 2, m_uiHourMarks);
    angle += 30;
  }

}

void drawClockTime()
{
  static int iPrevMin = -1;
  static int iPrevHour = -1;
  if(iPrevMin != minute() || iPrevHour != hour())
  {
    if(iPrevMin >= 0)
    {
      drawMinuteHand(iPrevMin, m_uiBackground);
    }
    if(iPrevHour >= 0 && iPrevMin >= 0)
    {
      drawHourHand(iPrevHour, iPrevMin, m_uiBackground);
    }
    drawMinuteHand(minute(), m_uiClockMinuteHand);
    drawHourHand(hour(), minute(), m_uiClockHourHand);
    drawDataTimeInfo();
    drawCenterPoint();
  }
  iPrevMin = minute();
  iPrevHour = hour();
}

void drawMinuteHand(int m, uint16_t color)
{
  
  float angle=-90;
  float Xa, Ya;
  int Xo = int(TFT_HEIGHT/2);
  int Yo = int(TFT_WIDTH/2);
  int RADIUS = (Yo - 10);
  Xa=Xo+(RADIUS-20)*cos((angle+m*6)*M_PI/180);
  Ya=Yo+(RADIUS-20)*sin((angle+m*6)*M_PI/180);
  for(int iCnt = -1; iCnt <= 1; iCnt++)
  {
    m_tft.drawLine(Xo, Yo+iCnt, Xa, Ya+iCnt, color);
  }
  
}

void drawHourHand(int h, int m, uint16_t color)
{
  float angle=-90;
  float Xa, Ya;
  int Xo = int(TFT_HEIGHT/2);
  int Yo = int(TFT_WIDTH/2);
  int RADIUS = (Yo - 20);
  Xa=Xo+(RADIUS-(RADIUS/2))*cos((angle+h*30+(m*30/60))*M_PI/180);
  Ya=Yo+(RADIUS-(RADIUS/2))*sin((angle+h*30+(m*30/60))*M_PI/180);
  for(int iCnt = -1; iCnt <= 1; iCnt++)
  {
   m_tft.drawLine(Xo, Yo+iCnt, Xa, Ya+iCnt, color);
  }
}

void drawCenterPoint()
{
  m_tft.fillCircle(int(TFT_HEIGHT/2), int(TFT_WIDTH/2), 3, m_uiClockCenter);
}

void drawDataTimeInfo()
{
  int Xo = int(TFT_HEIGHT/2);
  int Yo = int(TFT_WIDTH/2) - 40;
  static int iLastWeekday = WEEKDAYS;
  static int iLastDay = -1;
  static int iLastMonth = -1;
  static int iLastYear = -1;
  static int iLastHour = -1;
  static int iLastMinute = -1;
  static byte byLastWeek = -1;

  byte byCalcWeek = calcCalenderWeek();
  if(iLastWeekday != WEEKDAYS || byLastWeek != byCalcWeek)
  {
    m_tft.setTextColor(m_uiBackground, m_uiBackground);
    m_tft.drawCentreString("KW " + String(byLastWeek) + "  " + daysOfTheWeek[iLastWeekday] + ".",Xo ,Yo, GFXFF);
  }

  byLastWeek = byCalcWeek;

#if DEBUG
  Serial.println("Number weekday: " + String(weekday()));
  Serial.println("Weekday found: " + daysOfTheWeek[weekday() - 1]);
  Serial.println("KW: " + String(byLastWeek));
#endif
  
  m_tft.setTextColor(m_uiClockText, m_uiBackground);
  m_tft.drawCentreString("KW " + String(byLastWeek) + "  " + daysOfTheWeek[weekday() - 1] + ".",Xo ,Yo, GFXFF);
  iLastWeekday = weekday();

  Yo += 80;
  if(iLastDay != day() || iLastMonth != month() || iLastYear != year()|| iLastMinute != minute())
  {
    m_tft.setTextColor(m_uiBackground, m_uiBackground);
    m_tft.drawCentreString(GetDigits(iLastDay) + " / " + GetDigits(iLastMonth) + " / " + GetDigits(iLastYear), Xo, Yo, GFXFF);
  }

  m_tft.setTextColor(m_uiClockText, m_uiBackground);
  m_tft.drawCentreString(GetDigits(day()) + " / " + GetDigits(month()) + " / " + GetDigits(year()), Xo, Yo, GFXFF);
  iLastDay = day();
  iLastMonth = month();
  iLastYear = year();

  Yo += 20;
  if(iLastHour != hour() || iLastMinute != minute())
  {
    m_tft.setTextColor(m_uiBackground, m_uiBackground);
    m_tft.drawCentreString(GetDigits(iLastHour) + " : " + GetDigits(iLastMinute),Xo, Yo, GFXFF);
    m_tft.setTextColor(m_uiClockText, m_uiBackground);
    m_tft.drawCentreString(GetDigits(hour()) + " : " + GetDigits(minute()),Xo, Yo, GFXFF);
    iLastHour = hour();
    iLastMinute = minute();
  }
}

byte calcCalenderWeek()
{
  int iYear   = year();
  int iMonth  = month();
  int iDay    = day();

  if (iYear < 1901 || iYear > 2099 || iMonth < 1 || iMonth > 12)
  {
    return 0;
  }

  // take advantage of 28-year cycle
  while (iYear >= 1929)
  {
     iYear -= 28;
  }
  // compute adjustment for dates within the year
  //     If Jan. 1 falls on: Mo Tu We Th Fr Sa Su
  // then the adjustment is:  6  7  8  9  3  4  5
  int adj = (iYear - 1873) + ((iYear - 1873) >> 2);
  while (adj > 9)
  {
    adj -= 7;
  }
  // compute day of the year (in range 1-366)
  int doy = iDay;
  for (int i = 1; i < iMonth; i++)
  {
    doy += (30 + ((0x15AA >> i) & 1));
  }

  if (iMonth > 2)
  {
    doy -= ((iYear & 3) ? 2 : 1);
  }
  // compute the adjusted day number
  int dnum = adj + doy;
  // compute week number
  int wknum = dnum >> 3;
  dnum -= ((wknum << 3) - wknum);
  while (dnum >= 7)
  {
    dnum -= 7;
    wknum++;
  }
  // check for boundary conditions
  if (wknum < 1) {
    // last week of the previous year
    // check to see whether that year had 52 or 53 weeks
    // re-compute adjustment, this time for previous year
    adj = (iYear - 1874) + ((iYear - 1874) >> 2);
    while (adj > 9)
    {
      adj -= 7;
    }
    // all years beginning on Thursday have 53 weeks
    if (adj==9)
    {
      return 53;
    }
    // leap years beginning on Wednesday have 53 weeks
    if ((adj == 8) && ((iYear & 3) == 1))
    {
      return 53;
    }
    // other years have 52 weeks
    return 52;
  }

  if (wknum > 52)
  {
    // check to see whether week 53 exists in this year
    // all years beginning on Thursday have 53 weeks
    if (adj == 9)
    {
      return 53;
    }
    // leap years beginning on Wednesday have 53 weeks
    if ((adj == 8) && ((iYear & 3) == 0))
    {
      return 53;
    }
    // other years have 52 weeks
    return 1;
  }
  return byte(wknum & 0x00FF);
}
