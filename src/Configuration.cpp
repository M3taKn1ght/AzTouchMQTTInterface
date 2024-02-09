#include "Configuration.h"
#include "mesh_util.h"
using namespace tinyxml2;
bool Config::initSD()
{
    if(!SD.begin(SD_PIN))
    {
#if DEBUG
        Serial.println("Unable to init SD-Card");
#endif
        return false;
    }
    return getSDType();
}

bool Config::checkTouchFileExists()
{
    bool bReturn = true;
    File calibration = SD.open(CALIBRATION_FILE, FILE_READ);
    if(!calibration)
    {
#if DEBUG
        Serial.println("File " + String(CALIBRATION_FILE) + " not found");
#endif
        bReturn = false;
    }
    else
    {
        calibration.close();
    }
    return bReturn;
}

void Config::writeTouchCalibration(uint16_t valOne, uint16_t valTwo, uint16_t valThree, uint16_t valFour, uint16_t valFive)
{
    String strContent = String(valOne) + "," + String(valTwo) + "," + String(valThree) + "," + String(valFour) + "," + String(valFive);
    File calibration = SD.open(CALIBRATION_FILE, FILE_WRITE);
    if(calibration)
    {
#if DEBUG
        Serial.println("Write to file: " + strContent);
#endif
        calibration.print(strContent);
    }
    calibration.close();
}

bool Config::readTouchCalibration(uint16_t &valOne, uint16_t &valTwo, uint16_t &valThree, uint16_t &valFour, uint16_t &valFive)
{
    bool bReturn = true;
    File calibration = SD.open(CALIBRATION_FILE, FILE_READ);
    if(!calibration)
    {
#if DEBUG
        Serial.println("File " + String(CALIBRATION_FILE) + " not found");
#endif
        bReturn = false;
    }
    else
    {
        String strCalibration = "";
        while(calibration.available())
        {
            strCalibration = calibration.readString();
        }
#if DEBUG
        Serial.println("Filecontent: " + strCalibration);
#endif
        calibration.close();
        String strs[5];
        int StringCount = 0;
        while (strCalibration.length() > 0 && StringCount < 5)
        {
            int index = strCalibration.indexOf(',');
            if (index == -1) // No space found
            {
                strs[StringCount++] = strCalibration;
#if DEBUG
                Serial.println("Value " + String(StringCount-1) + ": " + String(strs[StringCount-1]));
#endif
                break;
            }
            else
            {
                strs[StringCount++] = strCalibration.substring(0, index);
                strCalibration = strCalibration.substring(index+1);
#if DEBUG
                Serial.println("Value " + String(StringCount-1) + ": " + String(strs[StringCount-1]));
#endif
            }
        }
        valOne      = strs[0].toInt();
        valTwo      = strs[1].toInt();
        valThree    = strs[2].toInt();
        valFour     = strs[3].toInt();
        valFive     = strs[4].toInt();
    }
    
    return bReturn;
}

bool Config::readGeneralConfig(String filepath)
{
#if DEBUG
    Serial.println("Try to read file: " + filepath);
#endif
    File configFile = SD.open(filepath,"r");
    if(!configFile)
    {
#if DEBUG
        Serial.println("No file found");
#endif
        return false;
    }
#if DEBUG
    size_t size = configFile.size();
    Serial.println("Config file size is:" + String(size) + " Bytes");
#endif
    if(!configFile.available())
    {
#if DEBUG
    Serial.println("Config file is not available!");
#endif
        return false;
    }
    String fileContent = configFile.readString();
 #if DEBUG
    Serial.println("Content of file:");
    Serial.print(fileContent);
    Serial.println("");
#endif   
    configFile.close();

    return storeGeneralInfos(fileContent);
}

bool Config::readMQTTConfig(std::vector<MQTTInfos>& vecMQTTConfig, int& iNumberpages, String filepath)
{
#if DEBUG
    Serial.println("Try to read file: " + filepath);
#endif
    File configFile = SD.open(filepath,"r");
    if(!configFile)
    {
#if DEBUG
        Serial.println("No file found");
#endif
        return false;
    }
#if DEBUG
    size_t size = configFile.size();
    Serial.println("Config file size is:" + String(size) + " Bytes");
#endif
    if(!configFile.available())
    {
#if DEBUG
    Serial.println("Config file is not available!");
#endif
        return false;
    }
    String fileContent = configFile.readString();
 #if DEBUG
    Serial.println("Content of file:");
    Serial.print(fileContent);
    Serial.println("");
#endif   
    configFile.close();
    return storeMQTTInfos(vecMQTTConfig, iNumberpages, fileContent);
}

bool Config::getSDType()
{

    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) 
    {
#if DEBUG
        Serial.println("No SD card attached");
#endif
        return false;
    }
    
    if (cardType == CARD_MMC)
    {
#if DEBUG
        Serial.println("SD-Type: MMC");
#endif
        return true;
    } 
    else if (cardType == CARD_SD)
    {
#if DEBUG
        Serial.println("SD-Type: SDSC");
#endif
        return true;
    } 
    else if (cardType == CARD_SDHC)
    {
#if DEBUG
        Serial.println("SD-Type: SDHC");
#endif
        return true;
    }
    else
    {
#if DEBUG
        Serial.println("SD-Type: UNKNOWN");
#endif
        return false;
    }
}

bool Config::storeGeneralInfos(String content)
{
    const char * charValue;
    charValue="";
    String strReturn = "";
    if(content.length() == 0)
    {
        return false;
    }
    XMLDocument xmlDocument;
    if(xmlDocument.Parse(content.c_str()) != XML_SUCCESS)
    {
#if DEBUG
        Serial.println("Unable to parse xml to document!");
#endif    
        return false;
    }

    XMLNode * root = xmlDocument.RootElement();
    if(root == NULL)
    {
#if DEBUG
        Serial.println("Unable to get root-element!");
#endif 
        return false;       
    }
    //Now search for all needed general information
    //Starting with WiFi
#if DEBUG
    Serial.println("------ GUI ------");
#endif
    XMLElement * element = root->FirstChildElement("GUI");
    if(element == NULL)
    {
#if DEBUG
        Serial.println("Unable to get GUI-element!");
#endif 
        return false;
    }
    getBoolFromElement(element, "darkMode", bDarkMode);
    getBoolFromElement(element, "showTime",bShowTime);
    if(!getStringFromElement(element, "name", strReturn))
    {
#if DEBUG
        Serial.println("Unable to get Devicename!");
#endif
        strDeviceName="";
    }
    else
    {
#if DEBUG
        Serial.println("DeviceName is: " + strReturn);
        Serial.println("Length of name is: " + String(strReturn.length()));
#endif
        strDeviceName=strReturn;
    }

    element = root->FirstChildElement("WiFi");
    if(element == NULL)
    {
#if DEBUG
        Serial.println("Unable to get Wifi-element!");
#endif 
        return false;
    }

    //Get SSID from network
#if DEBUG
    Serial.println("------ WIFI ------");
#endif
    if(!getStringFromElement(element, "SSID", strReturn))
    {
#if DEBUG
        Serial.println("Unable to get WiFi-SSID!");
#endif
        return false;
    }
    else
    {
        strWiFiSSID=strReturn;
    }   
#if DEBUG
        Serial.println("WiFi-SSID is: " + strWiFiSSID);
#endif
    //Get Wifi-Pass
    if(!getStringFromElement(element, "Pass", strReturn))
    {
#if DEBUG
        Serial.println("Unable to get WiFi-Pass!");
#endif
        strWiFiPass="";
    }
    else
    {
        strWiFiPass=strReturn;
    }
#if DEBUG
    Serial.println("WiFi-Pass is: " + strWiFiPass);
    Serial.println("------ Secure ------");
#endif
    //Nex step, get info to secure Az-Touch
    element = root->FirstChildElement("Secure");
    if(element == NULL)
    {
#if DEBUG
        Serial.println("Can't find element \"Secure\"!");
#endif    
    }
    getBoolFromElement(element, "active", bSecure);
#if DEBUG
        Serial.println("Flag active is: " + String(bSecure));
#endif
    //Get Secure-Pass
    if(!getStringFromElement(element, "pass", strReturn))
    {
#if DEBUG
        Serial.println("Unable to get Secure-Pass!");
#endif
        strSecureCode="";
    }
    else
    {
        strSecureCode=strReturn;
    }
#if DEBUG
    Serial.println("Secure-Pass is: " + strSecureCode);
#endif
    if(!getIntFromElement(element, "timeout", iTimeout))
    {
        iTimeout = TIMEOUTCLOCK;
    }
    else
    {
        if(iTimeout <= 0)
        {
            iTimeout = TIMEOUTCLOCK;
        }
    }
#if DEBUG
    Serial.println("Timeout is: " + String(iTimeout));
#endif

#if DEBUG
    Serial.println("------ MQTT ------");
#endif

    //Next all infos for MQTT
    element = root->FirstChildElement("MQTT");
    if(element == NULL)
    {
#if DEBUG
        Serial.println("Can't find element \"MQTT\"!");
#endif
        return false;
    }
    //Get broker IP
    if(!getStringFromElement(element, "broker", strReturn))
    {
#if DEBUG
        Serial.println("Unable to get Broker!");
#endif
        return false;
        strBrokerIP="";
    }
    else
    {
        strBrokerIP=strReturn;
        bBrokerisIP = ipBroker.fromString(strReturn);
    }
#if DEBUG
    Serial.println("Broker is: " + strBrokerIP);
    Serial.println("Broker is IP: " + String(bBrokerisIP));
    Serial.println("IP is: " + ipBroker.toString());
#endif

    //Get broker Port
    if(!getIntFromElement(element, "port", iMQTTPort))
    {
#if DEBUG
        Serial.println("Port set to default.");
#endif
        iMQTTPort=1883;
    }
#if DEBUG
    Serial.println("Port is: " + String(iMQTTPort));
#endif
    //Get name for this system
    if(!getStringFromElement(element, "clientName", strReturn))
    {
#if DEBUG
        Serial.println("Unable to get Clientname, use default \""+strMQTTClientName+"\"!");
#endif
    }
    else
    {
        strMQTTClientName=strReturn;
#if DEBUG
    Serial.println("Clientname is: " + strMQTTClientName);
#endif
    }

    //Get Login-user
    if(!getStringFromElement(element, "user", strReturn))
    {
        strMQTTUser = "";
#if DEBUG
        Serial.println("Unable to get MQTT-User");
#endif
    }
    else
    {
        strMQTTUser=strReturn;
#if DEBUG
        Serial.println("MQTTUser is: " + strMQTTUser);
#endif
    }
    //Get Login-Pass
   if(!getStringFromElement(element, "pass", strReturn))
   {
        strMQTTPass = "";
#if DEBUG
        Serial.println("Unable to get MQTT-Pass");
#endif
    }
    else
    {
        strMQTTPass=strReturn;
#if DEBUG
        Serial.println("MQTTUser is: " + strMQTTPass);
#endif
    }

#if DEBUG
    Serial.println("------ Watchdog ------");
#endif
    //Next all infos for Watchdog
    element = root->FirstChildElement("Watchdog");
    if(element == NULL)
    {
#if DEBUG
        Serial.println("Can't find element \"Watchdog\"!");
#endif
    }
    //Check Login-Data
    getBoolFromElement(element, "active", bWatchdog);
#if DEBUG
    Serial.println("Flag active is: " + String(bWatchdog));
#endif
    if(bWatchdog)
    {
        if(!getIntFromElement(element, "time", iWatchdogIntervall))
        {
            bWatchdog = false;
#if DEBUG
    Serial.println("Flag active set to: " + String(bWatchdog));
#endif
        }
#if DEBUG
    Serial.println("Watchdog-time is: " + String(iWatchdogIntervall));
#endif
        //Prevent a boot-loop       
        if(iWatchdogIntervall < WDT_TIMEOUT)
        {
            iWatchdogIntervall = WDT_TIMEOUT;
#if DEBUG
            Serial.println("Set watchdog-time to : " + String(iWatchdogIntervall));
#endif
        }
    }
    return true;
}

bool Config::storeMQTTInfos(std::vector<MQTTInfos>& vecMQTTConfig, int& iNumberpages, String content)
{
    const char * charValue;
    charValue="";
    String strReturn = "";
    int iReturn = 0;
    int iNumElements = 0;
    std::vector<MQTTInfos> vecMQTTOnHold;
    if(content.length() == 0)
    {
        return false;
    }
    XMLDocument xmlDocument;
    if(xmlDocument.Parse(content.c_str()) != XML_SUCCESS)
    {
#if DEBUG
        Serial.println("Unable to parse xml to document!");
#endif    
        return false;
    }
    XMLNode * root = xmlDocument.RootElement();
    if(root == NULL)
    {
#if DEBUG
        Serial.println("Unable to get root-element!");
#endif 
        return false;       
    }
    vecMQTTConfig.clear();
    //First check how many elements are stored and pages are needed
    XMLElement * element = root->FirstChildElement("Element");
    while(element != nullptr)
    {      
        String strReturn;
        if(getStringFromElement(element, "type", strReturn))
        {
            if(getTypeEnum(strReturn) == typeShutter)
            {
                iNumElements += 2;
            }
            else
            {
                iNumElements++;
            }  
        }
        element = element->NextSiblingElement("Element");
    }

    if(iNumElements <= 6)
    {
        iNumberpages = 1;
    }
    else
    {
        if(iNumElements % 5 != 0)
        {
            iNumberpages = iNumElements / 5 + 1; 
        }
        else
        {
            iNumberpages = iNumElements / 5;
        }
    }
#if DEBUG
        Serial.println("Number of elements: " + String(iNumElements));
        Serial.println("Number of calc. pages: " + String(iNumberpages));
#endif 
    //Now parse through configuration
    element = root->FirstChildElement("Element");
    while(element != nullptr)
    {
        MQTTInfos newMQTTInfoElement;
        if(!getStringFromElement(element, "name", newMQTTInfoElement.strName))
        {
#if DEBUG
        Serial.println("Internal error to read attribute \"name\" from element");
#endif 
            return false;
        }
        if(!getStringFromElement(element, "type", strReturn))
        {
#if DEBUG
        Serial.println("Internal error to read attribute \"type\" from element");
#endif 
            return false;            
        }
        else
        {
            newMQTTInfoElement.byType = getTypeEnum(strReturn);
        }
        int iPage = 0;
        int iPos = 0;
        if(!getIntFromElement(element, "pos", iPos))
        {
#if DEBUG
        Serial.println("Internal error to read attribute \"pos\" from element");
#endif 
            iPos = -1;            
        }

        //Correct Position if to high
        if(iPos > BUTTON_COLUMNS * BUTTON_ROWS)
        {
            iPos = BUTTON_COLUMNS * BUTTON_ROWS;
        }
        else if(iPos < -1)  //or to low
        {
            iPos = -1;  //This means any free position later
        }
        newMQTTInfoElement.byPos = lowByte(iReturn);


        if(!getIntFromElement(element, "page", iPage))
        {
#if DEBUG
        Serial.println("Internal error to read attribute \"page\" from element");
#endif 
            iPage = -1;            
        }
        
        int iMaxSub = 0;
        XMLElement * item = element->FirstChildElement("pub");
        while(item != nullptr && iMaxSub < MAXSUBCMD)
        {
            const char *charText = item->GetText();
            if(strlen(charText) != 0)
            {
                newMQTTInfoElement.strMQTTPub = String(charText);
            }
            item = item->NextSiblingElement("pub");
            iMaxSub++;
        }

        iMaxSub = 0;
        item = element->FirstChildElement("sub");
        while(item != nullptr && iMaxSub < MAXSUBSTAT)
        {
            const char *charText = item->GetText();
            if(String(charText).length() == 0)
            {
                continue;
            }
            if(!getStringFromElement(item, "unit", strReturn))
            {
                strReturn = "";
            }
            if(strlen(charText) != 0)
            {
                newMQTTInfoElement.vecMQTTSub.push_back(String(charText));
                newMQTTInfoElement.vecSubUnit.push_back(strReturn);
                newMQTTInfoElement.vecSubValue.push_back("");
            }
            item = item->NextSiblingElement("sub");
            iMaxSub++;
        }

        //Check if page an position is blocked allready
        if(checkPositionAvailable(vecMQTTConfig, iPage, iPos, iNumberpages))
        {
            //Position on page not blocked (first element), so write it fix in vector
            newMQTTInfoElement.byPage = iPage;
            newMQTTInfoElement.byPos = iPos;
            vecMQTTConfig.push_back(newMQTTInfoElement);
        }
        else
        {
#if DEBUG
            Serial.println("Element " + newMQTTInfoElement.strName + " on hold.");
#endif 
            //Page and position blocked (missconfiguration xml?), so searching later for free position
            vecMQTTOnHold.push_back(newMQTTInfoElement);
        }
        
        element = element->NextSiblingElement("Element");
    }

    //Add next button to all pages, if needed
    if(iNumberpages != 1)
    {
        for(int iCnt = 0; iCnt < iNumberpages; iCnt++)
        {
            MQTTInfos newMQTTInfoElement;
            newMQTTInfoElement.byPage = iCnt + 1;
            newMQTTInfoElement.byPos = 6;
            newMQTTInfoElement.byType = typeNext;
            vecMQTTConfig.push_back(newMQTTInfoElement);
        }
    }

#if DEBUG
    Serial.println("Size of on hold vector: " + String(vecMQTTOnHold.size()));
#endif 
    //Insert all on hold elements to free places on pages
    for(int iPage = 0; iPage < iNumberpages; iPage++)
    {
#if DEBUG
    Serial.println("Start searching with page: " + String(iPage + 1));
#endif
        if(vecMQTTOnHold.size() <= 0)
        {
            break;
        }
        bool myArray[] = {false, false, false, false, false, false};
        for(int iCnt = 0; iCnt < vecMQTTConfig.size(); iCnt++)
        {
            if(vecMQTTConfig[iCnt].byPage == iPage + 1)
            {
                myArray[vecMQTTConfig[iCnt].byPos - 1] = true;
            }
        }
        for(int iArray = 0; iArray < ARRAY_SIZE(myArray); iArray++)
        {
            if(!myArray[iArray] && vecMQTTOnHold.size() != 0)
            {
                MQTTInfos newMQTTInfoElement = vecMQTTOnHold.front();
                newMQTTInfoElement.byPage = iPage + 1;
                newMQTTInfoElement.byPos = iArray + 1;
                vecMQTTConfig.push_back(newMQTTInfoElement);
                vecMQTTOnHold.erase(vecMQTTOnHold.begin());
            }
        }

    }
    return true;
}

bool Config::getStringFromElement(XMLElement* element, String strAttribute, String& strReturn)
{
    const char * charValue;
    charValue="";
    strReturn="";
    if(element == NULL || strAttribute.length() == 0)
    {
#if DEBUG
        Serial.println("Given element-pointer is NULL or Attribute to search for!");
        return false;
#endif        
    }
        if(element->QueryStringAttribute(strAttribute.c_str(), &charValue) != XML_SUCCESS)
    {
#if DEBUG
        Serial.println("Unable to get attribute: \"" + strAttribute +"\"");
#endif
        strReturn = "";
        return false;
    }
    if(strlen(charValue) != 0)
    {
        strReturn=charValue;
    }
    else
    {
        strReturn="";
    }
    return true;
}

bool Config::getBoolFromElement(tinyxml2::XMLElement* element, String strAttribute, bool& bReturn)
{
    bReturn = false;
    if(element == NULL || strAttribute.length() == 0)
    {
#if DEBUG
        Serial.println("Given element-pointer is NULL or Attribute to search for!");
        return false;
#endif        
    }
    if(element->QueryBoolAttribute(strAttribute.c_str(), &bReturn) != XML_SUCCESS)
    {
#if DEBUG
        Serial.println("Unable to get Attribute \""+strAttribute+"\"!");
#endif
        bReturn = false;
        return false;
    }
    return true;
}

bool Config::getIntFromElement(tinyxml2::XMLElement* element, String strAttribute, int& iReturn)
{
    iReturn = -1;
    if(element == NULL || strAttribute.length() == 0)
    {
#if DEBUG
        Serial.println("Given element-pointer is NULL or Attribute to search for!");
        return false;
#endif        
    }
    if(element->QueryIntAttribute(strAttribute.c_str(), &iReturn) != XML_SUCCESS)
    {
#if DEBUG
        Serial.println("Unable to get Attribute \""+strAttribute+"\"!");
#endif
        iReturn = -1;
        return false;
    }
    return true;
}

byte Config::getTypeEnum(String strType)
{
    if(strType.length() == 0)
    {
        return typeMax;
    }
    strType.toLowerCase();
    if(strType == "switch")
    {
        return typeSwitch;
    }
    else if(strType == "info")
    {
        return typeInfo;
    }
    else if(strType == "light")
    {
        return typeLight;
    }
    else if(strType == "shutter")
    {
        return typeShutter;
    }
    else if(strType == "measurement")
    {
        return typeMeasur;
    }
    else if(strType == "socket")
    {
        return typeSocket;
    }
    return typeMax;
}

bool Config::checkPositionAvailable(std::vector<MQTTInfos>& vecMQTTConfig, int iPage, int iPos, int iNumCalcPages)
{
    //Special case
    if(iPage == -1 || iPos == -1)
    {
        return false;
    }

    //Special if there are more than on page, than button 6 is blocked for "NEXT"-Button
    if(iNumCalcPages > 1 && iPos >= BUTTON_COLUMNS * BUTTON_ROWS)
    {
        return false;
    }

    if(iPage > iNumCalcPages)
    {
        iPage = iNumCalcPages;
    }
    else if(iPage <= 0)
    {
        iPage = iNumCalcPages;
    }
    
    for(int iCnt = 0; iCnt < vecMQTTConfig.size(); iCnt++)
    {
        if(vecMQTTConfig[iCnt].byPage == iPage && vecMQTTConfig[iCnt].byPos == iPos)
        {
            return false;
        }
    }
    return true;
}