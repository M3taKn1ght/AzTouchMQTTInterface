#include "ButtonWidget.h"

ButtonWidget::ButtonWidget(TFT_eSPI *tft)
{
    //Set all member-vars to default
    m_ptft  = tft;
    m_iX    = 0;
    m_iY    = 0;
    m_uiWidth = 0;
    m_uiHeight = 0;
    m_iRadiusButtonEdge = 4;
    m_uiTextsize = 1;
    m_byTextLength = 9;
    m_FirstText.clear();
    m_SecondText.clear();
    m_ThirdText.clear();
    m_bON = false;
    m_bCurrstate = false;
    m_bHideButton = false;
}

void ButtonWidget::initButton(  int16_t iXPos, int16_t iYPos, uint16_t iWidth, uint16_t iHeight, uint16_t uiOutlineColor, 
                                uint16_t uiFillColor, uint16_t uiTextColor, uint16_t uiRadiusEdge, uint16_t uiOutlineWidth)
{
    m_iX = iXPos;
    m_iY = iYPos;
    m_uiWidth = iWidth;
    m_uiHeight = iHeight;
    m_outlinewidth = uiOutlineWidth;
    setRadius(uiRadiusEdge);
    m_outlinecolor = uiOutlineColor;
    m_fillcolor = uiFillColor;
    m_textcolor = uiTextColor;
    m_FirstText.clear();
    m_SecondText.clear();
    m_ThirdText.clear();
    m_byType = typeMax;
}

void ButtonWidget::setNewTextLength(byte byTextLength)
{
    m_byTextLength = byTextLength;
}

void ButtonWidget::setNewType(byte byNewType, uint16_t uiON, uint16_t uiOFF)
{
    if(byNewType > typeMax)
    {
        m_byType = typeMax;
    }
    else
    {
        m_byType = byNewType;
    }
    m_SymbolON = uiON;
    m_SymbolOFF = uiOFF;
}

void ButtonWidget::setOnStatus(bool bSetStatus, bool bRedraw)
{
    m_bON = bSetStatus;
    if(bRedraw)
    {
        drawButton();
    }

}

bool ButtonWidget::setButtonText(String firstText, String secondText, String thirdText, bool bRedraw)
{
    bool bNeedsUpdate = false;
    bNeedsUpdate = bNeedsUpdate || (firstText != m_FirstText);
    if(firstText.length() != 0)
    {
        m_FirstText = firstText;
    }
    else
    {
        m_FirstText.clear();
    }

    bNeedsUpdate = bNeedsUpdate || (secondText != m_SecondText);
    if(secondText.length() != 0)
    {
        m_SecondText = secondText;
    }
    else
    {
        m_SecondText.clear();
    }

    bNeedsUpdate = bNeedsUpdate || (thirdText != m_ThirdText);
    if(thirdText.length() != 0)
    {
        m_ThirdText = thirdText;
    }
    else
    {
        m_ThirdText.clear();
    }
    return bNeedsUpdate;
}

void ButtonWidget::setPressed(bool pressed)
{
    m_bCurrstate = pressed;
}

void ButtonWidget::setRadius(uint16_t uiRadius)
{
    if(min(m_uiWidth, m_uiHeight) / 4 > uiRadius)
    {
        m_iRadiusButtonEdge = min(m_uiWidth, m_uiHeight) / 4;
    }
    else
    {
        m_iRadiusButtonEdge = uiRadius;
    }
}

void ButtonWidget::drawButton()
{
    if(m_bHideButton)
    {
        return;
    }

    if(m_outlinewidth > 0)
    {
        m_ptft->fillSmoothRoundRect(m_iX, m_iY, m_uiWidth, m_uiHeight, m_iRadiusButtonEdge, m_outlinecolor, m_fillcolor);
    }

    if(m_iRadiusButtonEdge > 0)
    {
        m_ptft->fillSmoothRoundRect(m_iX + m_outlinewidth, m_iY + m_outlinewidth, m_uiWidth - (2 *m_outlinewidth), m_uiHeight - (2 * m_outlinewidth),
                                    m_iRadiusButtonEdge - m_outlinewidth, m_fillcolor, m_outlinecolor);
    }
    else
    {
        m_ptft->fillSmoothRoundRect(m_iX, m_iY, m_uiWidth, m_uiHeight, m_iRadiusButtonEdge, m_fillcolor, m_outlinecolor);
    }

    drawSymbol();

    drawText();


}

bool ButtonWidget::contains(int16_t iX, int16_t iY)
{
    return ((iX >= m_iX) && (iX < (m_iX + m_uiWidth)) &&
          (iY >= m_iY) && (iY < (m_iY + m_uiHeight)));
}

void ButtonWidget::hideButton(uint16_t iBgColor)
{
    m_ptft->fillRect(m_iX, m_iY, m_uiWidth, m_uiHeight, iBgColor);
    m_bHideButton = true;
}

void ButtonWidget::showButton()
{
    m_bHideButton = false;
    drawButton();
}

void ButtonWidget::redrawButton(uint16_t iBgColor)
{
    m_ptft->fillRect(m_iX, m_iY, m_uiWidth, m_uiHeight, iBgColor);
    drawButton();
}

void ButtonWidget::drawSymbol()
{
    if(m_byType == typeMax)
    {
        return;
    }
    
    //Calculate upper left corner for image
    int16_t iXStart, iYStart, iColor;
    iXStart = m_iX + (m_uiWidth / 2) - (SIZE_LARGE_IMAGE / 2);
    iYStart = m_iY + (m_uiHeight / 2) - (SIZE_LARGE_IMAGE / 2);
    
    //Get correct color for image
    if(m_bON)
    {
        iColor = m_SymbolON;
    }
    else
    {
        iColor = m_SymbolOFF;
    }

    if(m_byType == typeInfo)
    {
        m_ptft->drawBitmap(iXStart, iYStart, info_large, SIZE_LARGE_IMAGE, SIZE_LARGE_IMAGE, iColor);
    }
    else if(m_byType == typeSwitch)
    {
        m_ptft->drawBitmap(iXStart, iYStart, switch_large, SIZE_LARGE_IMAGE, SIZE_LARGE_IMAGE, iColor);
    }
    else if(m_byType == typeSocket)
    {
        m_ptft->drawBitmap(iXStart, iYStart, socket_large, SIZE_LARGE_IMAGE, SIZE_LARGE_IMAGE, iColor);
    }
    else if(m_byType == typeShutter)
    {
        m_ptft->drawBitmap(iXStart, iYStart, shutter_large, SIZE_LARGE_IMAGE, SIZE_LARGE_IMAGE, iColor);
    }
    else if(m_byType == typeLight)
    {
        m_ptft->drawBitmap(iXStart, iYStart, light_large, SIZE_LARGE_IMAGE, SIZE_LARGE_IMAGE, iColor);
    }
    else if(m_byType == typeMeasur)
    {
        m_ptft->drawBitmap(iXStart, iYStart, measurement_large, SIZE_LARGE_IMAGE, SIZE_LARGE_IMAGE, iColor);
    }
    else if(m_byType == typeNext)
    {
        m_ptft->drawBitmap(iXStart, iYStart, next_large, SIZE_LARGE_IMAGE, SIZE_LARGE_IMAGE, iColor);
    }
}

void ButtonWidget::drawText()
{
    int iXStart = m_iX + (m_uiWidth / 2);
    int iFontHeight = m_ptft->fontHeight(GFXFF);
    int iYStart = m_iY + (m_uiHeight / 2) - int( iFontHeight / 2);
    m_ptft->setTextColor(m_textcolor);
    if(!m_SecondText.isEmpty())
    {
        m_ptft->drawCentreString(m_SecondText, iXStart, iYStart, GFXFF);
    }

    if(!m_FirstText.isEmpty())
    {
        iYStart = m_iY + int(iFontHeight / 2);
        m_ptft->drawCentreString(m_FirstText, iXStart, iYStart, GFXFF);
    }

    if(!m_ThirdText.isEmpty())
    {
        iYStart = m_iY + m_uiHeight - int(m_ptft->fontHeight(GFXFF) * 1.5);
        m_ptft->drawCentreString(m_ThirdText, iXStart, iYStart, GFXFF);
    }
}