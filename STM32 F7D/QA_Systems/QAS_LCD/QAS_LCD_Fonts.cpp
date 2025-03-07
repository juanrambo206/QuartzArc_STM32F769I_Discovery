/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Systems - LCD Rendering                                       */
/*   Role: LCD Rendering System - Fonts                                    */
/*   Filename: QAS_LCD_Fonts.cpp                                           */
/*   Date: 5th November 2021                                               */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Includes
#include "QAS_LCD_Fonts.hpp"


  //------------------------------------------
	//------------------------------------------
	//------------------------------------------


	//----------------------------------
	//----------------------------------
	//QAS_LCD_FontMgr Management Methods

//QAS_LCD_FontMgr::add
//QAS_LCD_FontMgr Management Method
//
//Used to add a new font to the font manager
//strName     - a C-style string containing the name of the font (must be no longer than QAS_LCD_FONTNAME_LENGTH, including the null termination character)
//pDesc       - a pointer to an array of QAS_LCD_FontDesc structures for the font
//pData       - a pointer to an array of uint8_t values storing the bitmap data for the font
//uHeight     - the height in pixels of the font
//uSpaceWidth - the width in pixels of the space character of the font
//uCharGap    - the width in pixels of the gap between each character when rendering strings of characters
void QAS_LCD_FontMgr::add(const char* strName, const QAS_LCD_FontDesc* pDesc, const uint8_t* pData, uint16_t uHeight, uint16_t uSpaceWidth, uint16_t uCharGap) {

	//Create the QAS_LCD_Font class for the new font, providing the required details
	QAS_LCD_Font* cFont = new QAS_LCD_Font(strName, pDesc, pData, uHeight, uSpaceWidth, uCharGap);

	//Add the pointer to the font class to the m_vFonts vector
  m_vFonts.push_back(cFont);
}


//QAS_LCD_FontMgr::remove
//QAS_LCD_FontMgr Management Method
//
//Used to remove a font from the font manager
//strName - The name of the font to be removed.
void QAS_LCD_FontMgr::remove(const char* strName) {

	//Find index of font matching the provided font name
  int8_t iIdx = find(strName);

  //If a matching font has been found then remove it from the m_vFonts vector
  if (iIdx >= 0)
  	m_vFonts.erase(m_vFonts.begin()+iIdx);
}


//QAS_LCD_FontMgr::clear
//QAS_LCD_FontMgr Management Method
//
//Used to clear all fonts from the font manager
void QAS_LCD_FontMgr::clear(void) {
  m_vFonts.clear();
}


//QAS_LCD_FontMgr::find
//QAS_LCD_FontMgr Management Method
//
//Used to find if a font exists in the Font Manager
//strName - The name of the font to search for
//Returns -1 if a matching font is not found, or the index of the font if it is gound
int8_t QAS_LCD_FontMgr::find(const char* strName) {
  int8_t iIdx = -1;
  for (uint8_t i=0; i<m_vFonts.size(); i++)
  	if (*m_vFonts[i] == strName) {
  		iIdx = i;
  	}
  return iIdx;
}


//QAS_LCD_FontMgr::setFontByName
//QAS_LCD_FontMgr Management Method
//
//Used to set the current selected font
//strName - The name of the font to select
void QAS_LCD_FontMgr::setFontByName(const char* strName) {

	//Search for matching font
	int8_t iIdx = find(strName);

	//If no matching font is found then set current font to none
  if (iIdx < 0) {
  	m_iCurrentIdx = -1;
  	m_pCurrent    = NULL;
  	return;
  }

  //If a matching font is found then set details as required
  m_iCurrentIdx = iIdx;
  m_pCurrent    = m_vFonts[iIdx];
}


//QAS_LCD_FontMgr::setFontByIndex
//QAS_LCD_FontMgr Management Method
//
//Used to set the currently selected font
//uIdx - The index of the font to select
void QAS_LCD_FontMgr::setFontByIndex(uint8_t uIdx) {

	//If uIdx is outside of the range of fonts currently stored in m_vFonts index then set current font to none
  if (uIdx >= m_vFonts.size()) {
  	m_iCurrentIdx = -1;
  	m_pCurrent    = NULL;
  	return;
  }

  //Set current font details as required
  m_iCurrentIdx = uIdx;
  m_pCurrent    = m_vFonts[uIdx];
}


  //----------------------------
	//----------------------------
	//QAS_LCD_FontMgr Data Methods

//QAS_LCD_FontMgr::getHeight
//QAS_LCD_FontMgr Data Methods
//
//Returns the height in pixels of the currently selected font, or 0 if no font is currently selected
uint8_t QAS_LCD_FontMgr::getHeight(void) const {
	if (m_iCurrentIdx < 0)
		return 0;
	return m_pCurrent->m_uHeight;
}


//QAS_LCD_FontMgr::getSpaceWidth
//QAS_LCD_FontMgr Data Methods
//
//Returns the width in pixels of the space character for the currently selected font, or 0 if no font is currently selected
uint8_t QAS_LCD_FontMgr::getSpaceWidth(void) const {
	if (m_iCurrentIdx < 0)
		return 0;
	return m_pCurrent->m_uSpaceWidth;
}


//QAS_LCD_FontMgr::getCharGap
//QAS_LCD_FontMgr Data Methods
//
//Returns the width is pixels of the gap between each character for the currently selected font, or 0 if no font is currently selected
uint8_t QAS_LCD_FontMgr::getCharGap(void) const {
	if (m_iCurrentIdx < 0)
		return 0;
	return m_pCurrent->m_uCharGap;
}


//QAS_LCD_FontMgr::getCharWidth
//QAS_LCD_FontMgr Data Methods
//
//Returns the width in pixels of a specific character for the currently selected font.
//If no font is currently selected, or an invalid character is selected then 0 will be returned
uint16_t QAS_LCD_FontMgr::getCharWidth(char ch) {
  if (m_iCurrentIdx < 0)
    return 0;
  uint16_t uWidth = 0;
  if (ch == 32) {
    uWidth = m_pCurrent->m_uSpaceWidth;
  } else if ((ch >= 33) && (ch <= 126)) {
    uWidth = m_pCurrent->m_pDesc[ch-33].uWidth;
  }
  return uWidth;
}


//QAS_LCD_FontMgr::getStringWidth
//QAS_LCD_FontMgr Data Methods
//
//Returns is width in pixels for a specific C-style string based on the currently selected font.
//If no font is currently selected then 0 will be returned
uint16_t QAS_LCD_FontMgr::getStringWidth(const char* str) {
  uint16_t uWidth = 0;
  uint16_t uLength = strlen(str);
  uint8_t  uChar;

  if ((uLength == 0) || (m_iCurrentIdx < 0))
    return uWidth;

  for (uint8_t i=0; i<uLength; i++) {
    uChar = (uint8_t)str[i];
    if (uChar == 32) {
      uWidth += (m_pCurrent->m_uSpaceWidth + m_pCurrent->m_uCharGap);
    } else if ((uChar >= 33) && (uChar <= 126)) {
      uWidth += (m_pCurrent->m_pDesc[uChar-33].uWidth + m_pCurrent->m_uCharGap);
    }
  }
  uWidth -= m_pCurrent->m_uCharGap;
  return uWidth;
}


  //---------------------------------
	//---------------------------------
	//QAS_LCD_FontMgr Rendering Methods

//QAS_LCD_FontMgr::setDrawBuffer
//QAS_LCD_FontMgr Data Methods
//
//Sets the current frame buffer for text to be rendered to
//This method is to be called by QAS_LCD::imp_SetDrawBuffer() method
void QAS_LCD_FontMgr::setDrawBuffer(QAD_LTDC_Buffer* pBuffer) {
  m_pBuffer = pBuffer;
}


//QAS_LCD_FontMgr::setDrawColor
//QAS_LCD_FontMgr Data Methods
//
//Sets the current color for text rendering
//This method is to be called by QAS_LCD::imp_setDrawColor() method
void QAS_LCD_FontMgr::setDrawColor(uint16_t uColor) {
  m_uColor = uColor;
}


//QAS_LCD_FontMgr::drawChar
//QAS_LCD_FontMgr Data Methods
//
//Used to draw an individual character using the currently selected font/typeface
//Character will be drawn to currently selected draw buffer with currently selected draw color
//cPos - A QAD_Vector2_16 class that defines the X and Y coordinates that define the position of upper-left location of the character to be drawn
//ch   - A referenece to the specific character to be rendered
void QAS_LCD_FontMgr::drawChar(QAT_Vector2_16 cPos, char& ch) {
  if ((m_pBuffer == NULL) || (m_iCurrentIdx < 0))
    return;

  if ((ch >= 33) && (ch <= 126))
    drawCharP(cPos, ch);
}


//QAS_LCD_FontMgr::drawStrL
//QAS_LCD_FontMgr Data Methods
//
//Used to draw a left-aligned string of characters using the currently selected font/typeface
//String will be drawn to currently selected draw buffer with currently selected draw color
//cPos - A QAD_Vector2_16 class that defines the X and Y coordinates that define the position of the upper-left location of the string to be drawn
//str  - The C-style string to be drawn
void QAS_LCD_FontMgr::drawStrL(QAT_Vector2_16 cPos, const char* str) {
  if ((m_pBuffer == NULL) || (m_iCurrentIdx < 0))
    return;

  uint8_t uLen = strlen(str);
  if (uLen < 0) return;

  QAT_Vector2_16 cDrawPos = cPos;
  for (uint8_t i=0; i<uLen; i++) {
  	if ((str[i] >= 33) && (str[i] <= 126)) {
      drawCharP(cDrawPos, str[i]);
      cDrawPos.x += getCharWidth(str[i]) + getCharGap();
  	} else {
  		cDrawPos.x += getSpaceWidth() + getCharGap();
  	}
  }
}


//QAS_LCD_FontMgr::drawStrC
//QAS_LCD_FontMgr Data Methods
//
//Used to draw a center-aligned string of characters using the currently selected font/typeface
//String will be drawn to currently selected draw buffer with currently selected draw color
//cPos - A QAD_Vector2_16 class that defines the X and Y coordinates that define the position of the upper-center location of the string to be drawn
//str  - The C-style string to be drawn
void QAS_LCD_FontMgr::drawStrC(QAT_Vector2_16 cPos, const char* str) {
  if ((m_pBuffer == NULL) || (m_iCurrentIdx < 0))
    return;

  uint8_t uLen = strlen(str);
  if (uLen < 0) return;

  QAT_Vector2_16 cDrawPos = cPos;
  cDrawPos.x -= (getStringWidth(str) / 2);
  for (uint8_t i=0; i<uLen; i++) {
  	if ((str[i] >= 33) && (str[i] <= 126)) {
      drawCharP(cDrawPos, str[i]);
      cDrawPos.x += getCharWidth(str[i]) + getCharGap();
  	} else {
  		cDrawPos.x += getSpaceWidth() + getCharGap();
  	}
  }
}


//QAS_LCD_FontMgr::drawStrR
//QAS_LCD_FontMgr Data Methods
//
//Used to draw a right-aligned string of characters using the currently selected font/typeface
//String will be drawn to currently selected draw buffer with currently selected draw color
//cPos - A QAD_Vector2_16 class that defines the X and Y coordinates that define the position of the upper-right location of the string to be drawn
//str  - The C-style string to be drawn
void QAS_LCD_FontMgr::drawStrR(QAT_Vector2_16 cPos, const char* str) {
  if ((m_pBuffer == NULL) || (m_iCurrentIdx < 0))
    return;

  uint8_t uLen = strlen(str);
  if (uLen < 0) return;

  QAT_Vector2_16 cDrawPos = cPos;
  cDrawPos.x -= getStringWidth(str);
  for (uint8_t i=0; i<uLen; i++) {
  	if ((str[i] >= 33) && (str[i] <= 126)) {
      drawCharP(cDrawPos, str[i]);
      cDrawPos.x += getCharWidth(str[i]) + getCharGap();
  	} else {
  		cDrawPos.x += getSpaceWidth() + getCharGap();
  	}
  }
}


  //-----------------------------------------
  //-----------------------------------------
  //QAS_LCD_FontMgr Private Rendering Methods

//QAS_LCD_FontMgr::drawCharP
//QAS_LCD_FontMgr Private Rendering Method
//
//Used to draw an individual character based on selected font's bitmap data
//This method is to be used by drawChar(), drawStrL(), drawStrC() and drawStrR() methods
void QAS_LCD_FontMgr::drawCharP(QAT_Vector2_16 cPos, char ch) {
  uint16_t uLetter = (uint8_t)ch - 33;
  uint16_t uWidth  = m_pCurrent->m_pDesc[uLetter].uWidth;
  uint32_t uOffset = m_pCurrent->m_pDesc[uLetter].uOffset;

  uint8_t uLine;
  uint8_t uLineInc;
  uint32_t uCur;

  for (uint8_t uHeight=0; uHeight<m_pCurrent->m_uHeight; uHeight++) {
    uLineInc = 0;
    uCur = cPos.x+((cPos.y+uHeight)*QAD_LTDC_WIDTH);
    for (uint16_t i=0; i<uWidth; i++) {
      if (uLineInc == 0) {
        uLine = m_pCurrent->m_pData[uOffset];
        uOffset++;
      }
      if (uLine & 0x01)
        m_pBuffer->pixel[uCur+i] = m_uColor;
      uLine = uLine >> 1;
      uLineInc++;
      if (uLineInc > 7)
        uLineInc = 0;
    }
  }
}















