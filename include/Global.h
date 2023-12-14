#ifndef _GlobalH_
#define _GlobalH_
#include <Arduino.h>
#include <vector>
#include "bitmaps.h"
#include "Free_Fonts.h"

enum eType
{
    typeInfo = 0,
    typeLight,
    typeSwitch,
    typeShutter,
    typeMeasur,
    typeSocket,
    typeNext,
    typeMax
};

struct MQTTInfos
{
    byte byType                 = typeMax;  //ENum to detect which of element is set
    String strName              = "Unset";  //Name of device
    byte byPos                  = 0;        //Wanted position on display
    byte byPage                 = 0;        //Page on display
    String strMQTTPub           = "";
    std::vector<String> vecMQTTSub;
    std::vector<String> vecSubUnit;
    std::vector<String> vecSubValue;
};

#endif