/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Driver                                                        */
/*   Role: FT6206 Touch Controller Driver                                  */
/*   Filename: QAD_FT2606.cpp                                              */
/*   Date: 3rd December 2021                                               */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Includes
#include "QAD_FT6206.hpp"

#include "QAD_LTDC.hpp"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------


	//---------------------------------
	//---------------------------------
	//QAD_FT6206 Initialization Methods

//QAD_FT6206::imp_init
//QAD_FT6206 Initialization Method
QA_Result QAD_FT6206::imp_init(QAD_I2C* cI2C) {

	//Store Handle to I2C Driver
	if (!cI2C)
		return QA_Fail;

	m_cI2C = cI2C;

	//Initialize FT6206 device
	if (imp_confirmAddress())
		return QA_Fail;

	//Clear Data
	imp_clearData();

	//Set Driver States
	m_eInitState = QA_Initialized;

	//Return
	return QA_OK;
}


//QAD_FT6206::imp_deinit
//QAD_FT6206 Initialization Method
void QAD_FT6206::imp_deinit(void) {
  if (!m_eInitState)
  	return;

  //Set Driver States
  m_eInitState = QA_NotInitialized;
}


	//-----------------------------
	//-----------------------------
	//QAD_FT6206 Processing Methods

//QAD_FT6206::imp_poll
//QAD_FT6206 Processing Method
void QAD_FT6206::imp_poll(uint32_t& uTicks) {
	m_uData_LastX = m_uData_CurX;
	m_uData_LastY = m_uData_CurY;

	m_uData_CurX = 0;
	m_uData_CurY = 0;

	//---------------
	//Check for touch
	uint8_t uNumTouch = 0;
	m_cI2C->read8Bit(m_uAddr, m_uReg_Status, &uNumTouch);
	if (uNumTouch > 0) {
		uint8_t uDataXY[4];
		m_cI2C->readMultiple8Bit(m_uAddr, m_uReg_Touch1, uDataXY, sizeof(uDataXY));

		uint16_t uRawX = ((uDataXY[2] & 0x0F) << 8) | uDataXY[3];
		uint16_t uRawY = ((uDataXY[0] & 0x0F) << 8) | uDataXY[1];

		//Flip Y
		uRawY = (QAD_LTDC_HEIGHT - 1) - uRawY;

		uint32_t uX = 0;
		uint32_t uY = 0;

		uint16_t uXDiff = (uRawX > uX) ? (uRawX - uX) : (uX - uRawX);
		uint16_t uYDiff = (uRawY > uY) ? (uRawY - uY) : (uY - uRawY);

		if ((uXDiff + uYDiff) > 5) {
			uX = uRawX;
			uY = uRawY;
		}

		if ((uX >= 800) | (uY >= 480)) {
			uNumTouch = 0;
		} else {
			m_uData_CurX = uX;
			m_uData_CurY = uY;
		}
	}


	//------------------
	//Process Touch Data
	m_uData_LastDown = m_uData_CurDown;
	m_uData_CurDown  = (uNumTouch>0);

	m_uData_New = ((!m_uData_LastDown) && (m_uData_CurDown));
	m_uData_End = ((m_uData_LastDown) && (!m_uData_CurDown));
	m_uData_Event = (m_uData_LastDown || m_uData_CurDown);

	if (m_uData_LastDown && m_uData_CurDown) {

		m_iData_MoveX = m_uData_CurX - m_uData_LastX;
		m_iData_MoveY = m_uData_CurY - m_uData_LastY;
		m_uData_Long_Count += uTicks;
		m_uData_Long = (m_uData_Long_Count >= m_uLongTouchThreshold);

	} else {

		m_iData_MoveX = 0;
		m_iData_MoveY = 0;
		if (!m_uData_End) {
			m_uData_Long = false;
			m_uData_Long_Count = 0;
			m_uData_Long_Pulse = 0;
		}

	}

	if (m_uData_New) {
		m_uData_StartX = m_uData_CurX;
		m_uData_StartY = m_uData_CurY;
	}

	if (m_uData_Long && m_uData_End) {
		m_uData_End = false;
	}

	if (m_uData_Long) {
		if (m_uData_Long_Pulse > 0)
			m_uData_Long = false;
		m_uData_Long_Pulse++;
	}

}


	//-----------------------
	//-----------------------
	//QAD_FT6206 Data Methods

//QAD_FT6206::imp_getTouchWithin
//QAD_FT6206 Data Method
bool QAD_FT6206::imp_getTouchWithin(QAT_Vector2_16& cStart, QAT_Vector2_16& cEnd) {
	return ((m_uData_CurX >= cStart.x) && (m_uData_CurX <= cEnd.x) &&
					(m_uData_CurY >= cStart.y) && (m_uData_CurY <= cEnd.y));
}


	//-----------------------
	//-----------------------
	//QAD_FT6206 Tool Methods

//QAD_FT6206::imp_confirmAddress
//QAD_FT6206 Tool Method
QA_Result QAD_FT6206::imp_confirmAddress(void) {
	uint8_t uResA = 0;
	uint8_t uResB = 0;
	m_cI2C->read8Bit(m_uAddrA, m_uReg_ID, &uResA);
	m_cI2C->read8Bit(m_uAddrB, m_uReg_ID, &uResB);

	if (uResA == m_uID) {
		m_uAddr = m_uAddrA;
	} else if (uResB == m_uID) {
		m_uAddr = m_uAddrB;
	} else {
		return QA_Fail;
	}

	return QA_OK;
}


//QAD_FT6206::imp_clearData
//QAD_FT6206 Tool Method
void QAD_FT6206::imp_clearData(void) {
  m_uData_CurDown     = false;
  m_uData_LastDown    = false;
  m_uData_Event       = false;
  m_uData_New         = false;
  m_uData_End         = false;
  m_uData_Long        = false;
  m_uData_Long_Pulse  = 0;
  m_uData_Long_Count  = 0;

  m_uData_CurX        = 0;
  m_uData_CurY        = 0;
  m_uData_LastX       = 0;
  m_uData_LastY       = 0;
  m_iData_MoveX       = 0;
  m_iData_MoveY       = 0;
  m_uData_StartX      = 0;
  m_uData_StartY      = 0;
}
