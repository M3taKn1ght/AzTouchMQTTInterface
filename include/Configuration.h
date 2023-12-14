#ifndef _ConfigurationH_
#define _ConfigurationH_

//#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include "tinyxml2.h"
#include "IPAddress.h"
#include "Global.h"

class Config
{
public:
    /*--------------
      ---- Init ----
      --------------*/
    /*
    Function:       initSD
    Description:    Try to get a connection to SD-Card
    Return:         true if mount successfull, otherwise false
    */
    bool initSD();

    /*
    Function:       checkTouchFileExists
    Description:    Try to open a specific folder with touch-calibration
    Return:         true if file available, otherwise false
    */
    bool checkTouchFileExists();

    void writeTouchCalibration(uint16_t valOne, uint16_t valTwo, uint16_t valThree, uint16_t valFour, uint16_t valFive);
    
    bool readTouchCalibration(uint16_t &valOne, uint16_t &valTwo, uint16_t &valThree, uint16_t &valFour, uint16_t &valFive);
    /*
    Function:       readGeneralConfig
    Description:    Read the basic information from xml and store it to private variables
    IN filepath:    Absolute filepath on SD to configuration-xml, default is /general.xml
    Return:         true if reading successfull, otherwise false
    */
    bool readGeneralConfig(String filepath="/general.xml");

    /*
    Function:       readMQTTConfig
    Description:    Read the MQTT information from xml and store it to private variables
    IN filepath:    Absolute filepath on SD to configuration-xml, default is /MATT.xml
    Return:         true if reading successfull, otherwise false
    */
    bool readMQTTConfig(std::vector<MQTTInfos>& vecMQTTConfig, int& iNumberpages, String filepath="/MQTT.xml");

    /*----------------
      ---- Getter ----
      ----------------*/
    /*
    Function:       getDeviceName
    Description:    Returns stored Devicename
    Return:         String
    */
    String getDeviceName()
    { 
        return strDeviceName;
    };
    
    /*
    Function:       darkModeactive
    Description:    Returns if darkmode is selected
    Return:         bool
    */
    bool darkModeactive()
    { 
        return bDarkMode;
    };

    /*
    Function:       getShowTime
    Description:    Returns id time should shown
    Return:         bool
    */
    bool getShowTime()
    { 
        return bShowTime;
    };

    /*
    Function:       getWifiSSID
    Description:    Returns stored Wifi-SSID
    Return:         String
    */
    String getWifiSSID()
    { 
        return strWiFiSSID;
    };
    
    /*
    Function:       getWiFiPass
    Description:    Returns stored WiFi-Pass
    Return:         String
    */
    String getWiFiPass()
    {
        return strWiFiPass;
    }
    
    /*
    Function:       getSecure
    Description:    Returns if secure system with pass is active
    Return:         bool
    */
    bool getSecure()
    {
        return bSecure;
    }

    /*
    Function:       getSecureCode
    Description:    Get securecode to unlock device
    Return:         String
    */
    String getSecureCode()
    {
        return strSecureCode;
    }

    /*
    Function:       getClockTimeout
    Description:    Get timeout to show clock
    Return:         int
    */
    int getClockTimeout()
    {
        return iTimeout;
    }


    /*
    Function:       getBrokerIP
    Description:    Returns broker IP or local name
    Return:         bool
    */
    bool brokerIsIP()
    {
        return bBrokerisIP;
    }

    /*
    Function:       getBrokerString
    Description:    Returns broker IP or local name
    Return:         String
    */
    String getBrokerString()
    {
        return strBrokerIP;
    }

    /*
    Function:       getBrokerIP
    Description:    Returns broker IP or local name
    Return:         String
    */
    IPAddress getBrokerIP()
    {
        return ipBroker;
    }

    /*
    Function:       getBrokerPort
    Description:    Get Port to broker
    Return:         int
    */
    int getBrokerPort()
    {
        return iMQTTPort;
    }

    /*
    Function:       getClientName
    Description:    Get name for this client to use in MQTT
    Return:         String
    */
    String getClientName()
    {
        return strMQTTClientName;
    }

    /*
    Function:       getMQTTUser
    Description:    Returns MQTT-User to login
    Return:         String
    */
    String getMQTTUser()
    {
        return strMQTTUser;
    }

    /*
    Function:       getMQTTPass
    Description:    Returns MQTT-Pass to login
    Return:         String
    */
    String getMQTTPass()
    {
        return strMQTTPass;
    }

    /*
    Function:       getWatchdogActive
    Description:    Return if watchdog should be use
    Return:         bool
    */
    bool getWatchdogActive()
    {
        return bWatchdog;
    }

    /*
    Function:       getWatchdogIntervall
    Description:    Returns intervall-time for watchdog
    Return:         int
    */
    int getWatchdogIntervall()
    {
        return iWatchdogIntervall;
    }

private:
    /*
    Function:       getSDType
    Description:    Try to get the SD-Card-Type
    Return:         true if known type found, otherwise false
    */
    bool getSDType();

    /*
    Function:       storeGeneralInfos
    Description:    Function to store the content for general configuration in private Vars
                    This function will be called, if getting content from SD was successful.
                    Note that some variables have default parameter and will no return false, if
                    the attribute or node-element wasn't found!!
    IN content:     Content from read xml-file, default empty
    Return:         true needed content successfully stored in vars, otherwise false
    */
    bool storeGeneralInfos(String content = "");

    /*
    Function:       storeMQTTInfos
    Description:    Function to store the content for MQTT configuration in private Vars
                    This function will be called, if getting content from SD was successful.
                    Note that some variables have default parameter and will no return false, if
                    the attribute or node-element wasn't found!!
    IN content:     Content from read xml-file, default empty
    Return:         true needed content successfully stored in vars, otherwise false
    */
    bool storeMQTTInfos(std::vector<MQTTInfos>& vecMQTTConfig, int& iNumberpages, String content = "");

    /*
    Function:           getStringFromElement
    Description:        Get from given element the attribute as string
    PN element:         Pointer to element
    IN strAttribute:    Attributename to search for
    REF strReturn:      Value to store found attribute
    Return:             true needed content successfully found, otherwise false and REF strReturn is empty
    */
    bool getStringFromElement(tinyxml2::XMLElement* element, String strAttribute, String& strReturn);

    /*
    Function:           getBoolFromElement
    Description:        Get from given element the attribute as bool
    PN element:         Pointer to element
    IN strAttribute:    Attributename to search for
    REF bReturn:        Value to store found attribute
    Return:             true needed content successfully found, otherwise false and REF bReturn is false
    */
    bool getBoolFromElement(tinyxml2::XMLElement* element, String strAttribute, bool& bReturn);

    /*
    Function:           getIntFromElement
    Description:        Get from given element the attribute as int
    PN element:         Pointer to element
    IN strAttribute:    Attributename to search for
    REF strReturn:      Value to store found attribute
    Return:             true needed content successfully found, otherwise false and REF iReturn is -1
    */
    bool getIntFromElement(tinyxml2::XMLElement* element, String strAttribute, int& iReturn);

    /*
    Function:           getTypeEnum
    Description:        Return correct byte eNum for given StringType
    IN strType:         Type as String
    Return:             correct eNum eType otherwise typeMax
    */
    byte getTypeEnum(String strType);

    /*
    Function:           checkPositionAvailable
    Description:        Check if the selected position on display is available or not.
    REF vecMQTTConfig:  Referenze to current config vector
    IN iPage:           Wanted page for button
    IN iPos:            Wanted pos for button
    IN iNumCalcPages:   Calculated pages
    Return:             true if page and position is free, otherwise false
    */
    bool checkPositionAvailable(std::vector<MQTTInfos>& vecMQTTConfig, int iPage = -1, int iPos = -1, int iNumCalcPages = 1);
    /*-------------------
    * ---- Variables ----
      -------------------*/
    bool bDarkMode = false;
    bool bShowTime = false;
    String strDeviceName = "AzTouch openHab";
    String strWiFiSSID = "";
    String strWiFiPass = "";
    bool bSecure = false;
    String strSecureCode = "";
    int iTimeout = 5;
    bool bBrokerisIP = false;
    String strBrokerIP = "";
    IPAddress ipBroker;
    int iMQTTPort = 1883;
    String strMQTTUser = "";
    String strMQTTPass = "";
    String strMQTTClientName = "AzTouchmod";
    bool bWatchdog = false;
    int iWatchdogIntervall = -1;
};

#endif