/***************************************************************************************
* This is a button-class to create a button for the AzTouch Control - Project
* 
* Creator: M3taKn1ght
* Creation date: Mar 13, 2023
* Base idea creator: Bodmer (creator TFT_eSPI)
* Base idea link: https://github.com/Bodmer/TFT_eWidget
***************************************************************************************/
#ifndef _ButtonWidgetH_
#define _ButtonWidgetH_

//Standard-includes
#include "Global.h"
#include <TFT_eSPI.h>


class ButtonWidget : public TFT_eSPI
{

public:
    /*
    * =================================================================
    * Function:     ButtonWidget()
    * Description:  Basic init for this class
    * POINT tft:    Pointer to tft-class
    * Returns:  void
    * =================================================================
    */
    ButtonWidget(TFT_eSPI *tft);

    /*
    * =================================================================
    * Function:     initButton()
    * Description:  Init button with all needed values
    * IN iXPos:     X-Pos for button
    * IN iYPos:     Y-Pos for button
    * IN iWidth:    Width of button
    * IN iHeight:   Height of button
    * IN uiOutlineColor: Color for outline frame of button
    * IN uiFillColor: Fill color for button
    * IN uiTextColor: Text color
    * IN uiRadiusEdge: Edge radius for button
    * IN uiOutlineWidth: Outline frame width
    * Returns:  void
    * =================================================================
    */
    void initButton(int16_t iXPos, int16_t iYPos, uint16_t iWidth, uint16_t iHeight, uint16_t uiOutlineColor,
                    uint16_t uiFillColor, uint16_t uiTextColor, uint16_t uiRadiusEdge = 0, uint16_t uiOutlineWidth = 0);

    /***********************
    //  Setter-Functions    
    ***********************/
    /*
    * =================================================================
    * Function:     setNewTextLength()
    * Description:  (Re-)Define max. text length
    * IN setNewTextLength:   Allow max. textlength for each line
    * Returns:  void
    * =================================================================
    */
    void setNewTextLength(byte byTextLength = 9);

    /*
    * =================================================================
    * Function:     setNewType()
    * Description:  (Re-)Define the current type of button, needed for
    *               correct symbol
    * IN byNewType:   New type for button
    * IN uiON:  Color for on
    * IN uiOFF: Color for off
    * Returns:  void
    * =================================================================
    */
    void setNewType(byte byNewType = typeMax, uint16_t uiON = TFT_BLACK, uint16_t uiOFF = TFT_BLACK);
    
    /*
    * =================================================================
    * Function:     setOnStatus()
    * Description:  Switch ON-State of button
    * IN bSetStatus:   Status to overwrite current pressed-status
    * IN bRedraw:       Should button redraw automatically
    * Returns:  void
    * =================================================================
    */
    void setOnStatus(bool bSetStatus = false, bool bRedraw = true);

    /*
    * =================================================================
    * Function:     setButtonText()
    * Description:  Overwrite status, if button should shown pressed
    * IN pressed:   Status to overwrite current pressed-status
    * IN first Text:    First line for button
    * IN secondText:    Second line for button
    * IN thirdText:     Third line for button
    * IN bRedraw:       Should button redraw automatically
    * Returns:      true if redraw is needed, otherwise false
    * =================================================================
    */
    bool setButtonText(String firstText = "", String secondText = "", String thirdText = "", bool bRedraw = false);

    /*
    * =================================================================
    * Function:     setPressed()
    * Description:  Overwrite status, if button should shown pressed
    * IN pressed:   Status to overwrite current pressed-status
    * Returns:      void
    * =================================================================
    */
    void setPressed(bool bPressed);

    /*
    * =================================================================
    * Function:     setRadius()
    * Description:  Set the radius for the edges of this button
    * IN uiRadius:  Radius for the edges of this button
    * Returns:      void
    * =================================================================
    */
    void setRadius(uint16_t uiRadius = 0);
    /***********************
    //  Getter-Functions    
    ***********************/

    /*
    * =================================================================
    * Function:     getState()
    * Description:  Returns if button shows on or off
    * Returns:      true if on, otherwise false
    * =================================================================
    */
    bool getState()
    {
        return m_bON;
    }

    /*
    * =================================================================
    * Function:     getIsHidden()
    * Description:  Returns if button is hidden or not
    * Returns:      true if hidden, otherwise false
    * =================================================================
    */
    bool getIsHidden()
    {
        return m_bHideButton;
    }

    /*
    * =================================================================
    * Function:     drawButton()
    * Description:  Draw button with all needed content
    * Returns:      void
    * =================================================================
    */
    void drawButton();
    
    /*
    * =================================================================
    * Function:     contains()
    * Description:  Check if coordinates are in space of this button
    * IN iX:        Pressed X-Coord
    * IN iY:        Pressed Y-Coord
    * Returns:      true if coord in range of button, otherwise false
    * =================================================================
    */
    bool contains(int16_t iX, int16_t iY);

    /*
    * =================================================================
    * Function:     hideButton()
    * Description:  Hide button on tft
    * IN iBgColor:  Background color from display
    * Returns:      void
    * =================================================================
    */
    void hideButton(uint16_t iBgColor = TFT_SILVER);

    /*
    * =================================================================
    * Function:     showButton()
    * Description:  Make button visible (again)
    * Returns:      void
    * =================================================================
    */
    void showButton();

    /*
    * =================================================================
    * Function:     redrawButton()
    * Description:  Redraw complete button
    * IN iBgColor:  Background color from display
    * Returns:      void
    * =================================================================
    */
    void redrawButton(uint16_t iBgColor);

private:

    /*
    * =================================================================
    * Function:     drawSymbol()
    * Description:  Draw symbol into button
    * Returns:      void
    * =================================================================
    */
    void drawSymbol();

    /*
    * =================================================================
    * Function:     drawText()
    * Description:  Draw text into button
    * Returns:      void
    * =================================================================
    */
    void drawText();

    TFT_eSPI *m_ptft;
    byte      m_byTextLength;
    byte      m_byType;   //Stores the type of current button (see eType in Global.h!!!)
    int16_t   m_iX;       //X-Coordinate upper left corner for button
    int16_t   m_iY;       //Y-Coordinate upper left corner for button
    uint16_t  m_iRadiusButtonEdge;
    uint16_t  m_uiWidth;   //Set button width
    uint16_t  m_uiHeight;  //Set button height
    uint8_t   m_uiTextsize; //Text size
    uint16_t  m_outlinecolor, m_fillcolor, m_textcolor, m_outlinewidth, m_SymbolON, m_SymbolOFF;
    String    m_FirstText, m_SecondText, m_ThirdText; // Button text is 9 chars maximum unless long_name used
    bool      m_bON, m_bCurrstate, m_bHideButton; // Button states
};

#endif