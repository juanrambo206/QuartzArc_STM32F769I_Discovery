/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Driver                                                        */
/*   Role: FT6206 Touch Controller Driver                                  */
/*   Filename: QAD_FT2606.hpp                                              */
/*   Date: 3rd December 2021                                               */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Prevent Recursive Inclusion
#ifndef __QAD_FT6206_HPP_
#define __QAD_FT6206_HPP_


//Includes
#include "setup.hpp"

#include "QAD_I2C.hpp"

#include "QAT_Vector.hpp"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------


//----------
//QAD_FT6206
class QAD_FT6206 {
private:

	//FT6206 Constants
	const uint8_t    m_uAddrA          = 0x54;
	const uint8_t    m_uAddrB          = 0x70;

	const uint8_t    m_uReg_ID         = 0xA8;
	const uint8_t    m_uReg_GMode      = 0xA4;
	const uint8_t    m_uReg_Status     = 0x02;
	const uint8_t    m_uReg_Touch1     = 0x03;
	const uint8_t    m_uReg_Touch2     = 0x09;

	const uint8_t    m_uID             = 0x11;

	const uint8_t    m_uStatusMask     = 0x0F;

	const uint8_t    m_uGMode_Interrupt_Polling = 0x00;
	const uint8_t    m_uGMode_Interrupt_Trigger = 0x01;
	const uint8_t    m_uGMode_Interrupt_Mask    = 0x03;


	//Processing Constants
	const uint16_t   m_uLongTouchThreshold = 2600;


	//
	QAD_I2C*         m_cI2C;

	QA_InitState     m_eInitState;

	uint8_t          m_uAddr;

	bool             m_uData_CurDown;
	bool             m_uData_LastDown;
	bool             m_uData_Event;
	bool             m_uData_New;
	bool             m_uData_End;
	bool             m_uData_Long;
	uint8_t          m_uData_Long_Pulse;
	uint32_t         m_uData_Long_Count;

	uint16_t         m_uData_CurX;
	uint16_t         m_uData_CurY;
	uint16_t         m_uData_LastX;
	uint16_t         m_uData_LastY;
	int16_t          m_iData_MoveX;
	int16_t          m_iData_MoveY;
	uint16_t         m_uData_StartX;
	uint16_t         m_uData_StartY;


	//------------
	//Constructors

	QAD_FT6206() :
	  m_cI2C(NULL),
		m_eInitState(QA_NotInitialized) {}

public:

	//-----------------------------------------------
	//Delete Copy Constructor and Assignment Operator
	QAD_FT6206(const QAD_FT6206& other) = delete;
	QAD_FT6206& operator=(const QAD_FT6206& other) = delete;


	//-----------------
	//Singleton Methods

	static QAD_FT6206& get(void) {
		static QAD_FT6206 instance;
		return instance;
	}


	//----------------------
	//Initialization Methods

	static QA_Result init(QAD_I2C* cI2C) {
		return get().imp_init(cI2C);
	}

	static void deinit(void) {
		get().imp_deinit();
	}


	//------------------
	//Processing Methods

	static void poll(uint32_t uTicks) {
		get().imp_poll(uTicks);
	}



	//------------
	//Data Methods

	static bool getEvent(void) {
		return get().m_uData_Event;
	}

	static bool getNew(void) {
		return get().m_uData_New;
	}

	static bool getEnd(void) {
		return get().m_uData_End;
	}

	static bool getDown(void) {
		return get().m_uData_CurDown;
	}

	static bool getLong(void) {
		return get().m_uData_Long;
	}

	static uint16_t getCurX(void) {
		return get().m_uData_CurX;
	}

	static uint16_t getCurY(void) {
		return get().m_uData_CurY;
	}

	static int16_t getMoveX(void) {
		return get().m_iData_MoveX;
	}

	static int16_t getMoveY(void) {
		return get().m_iData_MoveY;
	}

	static uint16_t getStartX(void) {
		return get().m_uData_StartX;
	}

	static uint16_t getStartY(void) {
		return get().m_uData_StartY;
	}

	static bool getTouchWithin(QAT_Vector2_16 cStart, QAT_Vector2_16 cEnd) {
		return get().imp_getTouchWithin(cStart, cEnd);
	}


private:

	//----------------------
	//Initialization Methods
	QA_Result imp_init(QAD_I2C* cI2C);
	void imp_deinit(void);


	//------------------
  //Processing Methods
	void imp_poll(uint32_t& uTicks);


	//------------
	//Data Methods
	bool imp_getTouchWithin(QAT_Vector2_16& cStart, QAT_Vector2_16& cEnd);


	//------------
	//Tool Methods
	QA_Result imp_confirmAddress(void);

	void imp_clearData(void);

};


//Prevent Recursive Inclusion
#endif /* __QAD_FT6206_HPP_ */
