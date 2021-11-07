/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Systems - LCD Rendering                                       */
/*   Role: LCD Rendering System                                            */
/*   Filename: QAS_LCD.cpp                                                 */
/*   Date: 16th October 2021                                               */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Includes
#include "QAS_LCD.hpp"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------


//QAS_LCD::imp_init
//QAS_LCD Initialization Method
QA_Result QAS_LCD::imp_init(void) {
  if (m_eInitState) {
  	return QA_OK;
  }

	//Init LTDC
	if (QAD_LTDC::init())
		return QA_Fail;

	m_eInitState = QA_Initialized;

	m_pDrawBuffer = NULL;
	m_uDrawColor  = 0x0000;

	//Add Fonts to Font Manager List
	m_cFontMgr.clear();

	m_cFontMgr.add("SegoeUI20ptSB",
			           QAS_LCD_Fonts_SegoeUI20ptSB_Desc,
								 QAS_LCD_Fonts_SegoeUI20ptSB_Data,
								 QAS_LCD_Fonts_SegoeUI20ptSB_Height,
								 QAS_LCD_Fonts_SegoeUI20ptSB_SpaceWidth,
								 QAS_LCD_Fonts_SegoeUI20ptSB_CharGap);

	m_cFontMgr.add("SegoeUI12pt",
			           QAS_LCD_Fonts_SegoeUI12pt_Desc,
								 QAS_LCD_Fonts_SegoeUI12pt_Data,
								 QAS_LCD_Fonts_SegoeUI12pt_Height,
								 QAS_LCD_Fonts_SegoeUI12pt_SpaceWidth,
								 QAS_LCD_Fonts_SegoeUI12pt_CharGap);

	//Return
	return QA_OK;
}


//QAS_LCD::imp_deinit
//QAS_LCD Initialization Method
void QAS_LCD::imp_deinit(void) {

	//Clear Font Manager Font List
	m_cFontMgr.clear();

	//Deinit LTDC
	QAD_LTDC::deinit();

	m_eInitState = QA_NotInitialized;
}


  //-------------------------------
  //QAS_LCD Rendering Setup Methods

//QAS_LCD::imp_flipLayer0
//QAS_LCD Rendering Setup Method
void QAS_LCD::imp_flipLayer0(void) {
  QAD_LTDC::flipLayer0Buffers();
}


//QAS_LCD::imp_flipLayer1
//QAS_LCD Rendering Setup Method
void QAS_LCD::imp_flipLayer1(void) {
  QAD_LTDC::flipLayer1Buffers();
}


//QAS_LCD::imp_setDrawBuffer
//QAS_LCD Rendering Setup Method
void QAS_LCD::imp_setDrawBuffer(QAD_LTDC_LayerIdx eLayer) {

	switch (eLayer) {
	  case (Layer0):
	  	m_pDrawBuffer = QAD_LTDC::getLayer0BackBuffer();
	  	break;
	  case (Layer1):
	  	m_pDrawBuffer = QAD_LTDC::getLayer1BackBuffer();
	  	break;
	}
	m_cFontMgr.setDrawBuffer(m_pDrawBuffer);
}


//QAS_LCD::imp_setDrawColor
//QAS_LCD Rendering Setup Method
void QAS_LCD::imp_setDrawColor(uint16_t uColor) {
  m_uDrawColor = uColor;
  m_cFontMgr.setDrawColor(m_uDrawColor);
}


  //-------------------------
  //QAS_LCD Rendering Methods

//QAS_LCD::imp_clearBuffer
//QAS_LCD Rendering Method
void QAS_LCD::imp_clearBuffer(void) {
  for (uint32_t i=0; i<QAD_LTDC_PIXELCOUNT; i++)
  	m_pDrawBuffer->pixel[i] = m_uDrawColor;
}


//QAS_LCD::imp_drawPixel
//QAS_LCD Rendering Method
void QAS_LCD::imp_drawPixel(QAT_Vector2& cPos) {
  m_pDrawBuffer->pixel[cPos.x + (cPos.y & QAD_LTDC_WIDTH)].pxl(m_uDrawColor);
}


//QAS_LCD::imp_drawLine
//QAS_LCD Rendering Method
void QAS_LCD::imp_drawLine(QAT_Vector2& cStart, QAT_Vector2& cEnd) {
  if (cStart.x == cEnd.x)
  	imp_drawVLine(cStart, cEnd); else
  if (cStart.y == cEnd.y)
  	imp_drawHLine(cStart, cEnd); else
    imp_drawALine(cStart, cEnd);
}


//QAS_LCD::imp_drawHLine
//QAS_LCD Rendering Method
void QAS_LCD::imp_drawHLine(QAT_Vector2& cStart, QAT_Vector2& cEnd) {
  uint32_t xs;
  uint32_t xe;
  if (cStart.x < cEnd.x) {
  	xs = cStart.x;
    xe = cEnd.x;
  } else {
  	xs = cEnd.x;
  	xe = cStart.x;
  }

  uint32_t yofs = cStart.y * QAD_LTDC_WIDTH;
  for (uint32_t i=xs; i<(xe+1); i++) {
  	m_pDrawBuffer->pixel[i + yofs].pxl(m_uDrawColor);
  }
}


//QAS_LCD::imp_drawVLine
//QAS_LCD Rendering Method
void QAS_LCD::imp_drawVLine(QAT_Vector2& cStart, QAT_Vector2& cEnd) {
  uint32_t ys;
  uint32_t ye;
  if (cStart.y < cEnd.y) {
  	ys = cStart.y;
  	ye = cEnd.y;
  } else {
  	ys = cEnd.y;
  	ye = cStart.y;
  }

  uint32_t xofs = cStart.x;
  for (uint32_t i=ys; i<(ye+1); i++) {
  	m_pDrawBuffer->pixel[(i * QAD_LTDC_WIDTH) + xofs].pxl(m_uDrawColor);
  }
}


//QAS_LCD::imp_drawALine
//QAS_LCD Rendering Method
void QAS_LCD::imp_drawALine(QAT_Vector2& cStart, QAT_Vector2& cEnd) {
  int16_t uDeltaX = QAS_LCD_ABS(cEnd.x-cStart.x);
  int16_t uDeltaY = QAS_LCD_ABS(cEnd.y-cStart.y);
  int16_t uX = cStart.x;
  int16_t uY = cStart.y;

  int16_t uXInc1;
  int16_t uXInc2;
  int16_t uYInc1;
  int16_t uYInc2;

  if (cEnd.x >= cStart.x) {
    uXInc1 = 1;
    uXInc2 = 1;
  } else {
    uXInc1 = -1;
    uXInc2 = -1;
  }

  if (cEnd.y >= cStart.y) {
    uYInc1 = 1;
    uYInc2 = 1;
  } else {
    uYInc1 = -1;
    uYInc2 = -1;
  }

  int16_t uDenominator;
  int16_t uNumerator;
  int16_t uNumAdd;
  int16_t uNumPixels;

  if (uDeltaX >= uDeltaY) {
    uXInc1       = 0;
    uYInc2       = 0;
    uDenominator = uDeltaX;
    uNumerator   = uDeltaX / 2;
    uNumAdd      = uDeltaY;
    uNumPixels   = uDeltaX;
  } else {
    uXInc2       = 0;
    uYInc1       = 0;
    uDenominator = uDeltaY;
    uNumerator   = uDeltaY / 2;
    uNumAdd      = uDeltaX;
    uNumPixels   = uDeltaY;
  }

  for (uint16_t i=0; i<uNumPixels; i++) {
    m_pDrawBuffer->pixel[uX+(uY*QAD_LTDC_WIDTH)].pxl(m_uDrawColor);

    uNumerator += uNumAdd;
    if (uNumerator >= uDenominator) {
      uNumerator -= uDenominator;
      uX += uXInc1;
      uY += uYInc1;
    }
    uX += uXInc2;
    uY += uYInc2;
  }
}

//QAS_LCD::imp_drawRect
//QAS_LCD Rendering Method
void QAS_LCD::imp_drawRect(QAT_Vector2& cStart, QAT_Vector2& cEnd) {
  uint32_t xs;
  uint32_t xe;
  if (cStart.x < cEnd.y) {
    xs = cStart.x;
    xe = cEnd.x;
  } else {
    xs = cEnd.y;
    xe = cStart.y;
  }

  uint32_t ys;
  uint32_t ye;
  if (cStart.y < cEnd.y) {
    ys = cStart.y;
    ye = cEnd.y;
  } else {
    ys = cEnd.y;
    ye = cStart.y;
  }

  //Top & Bottom
  uint32_t yt = ys*QAD_LTDC_WIDTH;
  uint32_t yb = ye*QAD_LTDC_WIDTH;
  for (uint32_t x=xs; x<(xe+1); x++) {
    m_pDrawBuffer->pixel[x+yt].pxl(m_uDrawColor);
    m_pDrawBuffer->pixel[x+yb].pxl(m_uDrawColor);
  }

  //Left & Right
  for (uint32_t y=(ys+1); y<ye; y++) {
    m_pDrawBuffer->pixel[xs+y*QAD_LTDC_WIDTH].pxl(m_uDrawColor);
    m_pDrawBuffer->pixel[xe+y*QAD_LTDC_WIDTH].pxl(m_uDrawColor);
  }
}


//QAS_LCD::imp_drawRectFill
//QAS_LCD Rendering Method
void QAS_LCD::imp_drawRectFill(QAT_Vector2& cStart, QAT_Vector2& cEnd) {
  uint32_t xs;
  uint32_t xe;
  if (cStart.x < cEnd.x) {
    xs = cStart.x;
    xe = cEnd.x;
  } else {
    xs = cEnd.x;
    xe = cStart.x;
  }

  uint32_t ys;
  uint32_t ye;
  if (cStart.y < cEnd.y) {
    ys = cStart.y;
    ye = cEnd.y;
  } else {
    ys = cEnd.y;
    ye = cStart.y;
  }

  uint32_t yofs;
  for (uint32_t y=ys; y<(ye+1); y++) {
    yofs = y*QAD_LTDC_WIDTH;
    for (uint32_t x=xs; x<(xe+1); x++) {
      m_pDrawBuffer->pixel[x+yofs].pxl(m_uDrawColor);
    }
  }
}









