/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Driver                                                        */
/*   Role: I2C Management Driver                                           */
/*   Filename: QAD_I2CMgr.hpp                                              */
/*   Date: 27th November 2021                                              */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Prevent Recursive Inclusion
#ifndef __QAD_I2CMGR_HPP_
#define __QAD_I2CMGR_HPP_


//Includes
#include "setup.hpp"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------


//--------------
//QAD_I2C_Periph
//
//Used to select which I2C peripheral is to be used, and index into peripheral array in I2C manager
enum QAD_I2C_Periph : uint8_t {
	QAD_I2C1 = 0,
	QAD_I2C2,
	QAD_I2C3,
	QAD_I2C4,
	QAD_I2CNone
};


//-------------------
//QAD_I2C_PeriphCount
//
//I2C Peripheral Count
const uint8_t QAD_I2C_PeriphCount = QAD_I2CNone;


//-------------
//QAD_I2C_State
//
//Used to store whether a particular I2C peripheral is in use, or not currently being used
enum QAD_I2C_State : uint8_t {
	QAD_I2C_Unused = 0,
	QAD_I2C_InUse_Master,
	QAD_I2C_InUse_Slave,
	QAD_I2C_InUse_Multi,
	QAD_I2C_InvalidDevice
};


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

//------------
//QAD_I2C_Data
//
//Structure used in array within QAD_I2CMgr class to hold information for I2C peripherals
typedef struct {

	QAD_I2C_Periph    eI2C;        //Used to store which I2C peripheral is represented by the structure, used for manager menthods that find unused I2Cs

	QAD_I2C_State     eState;      //Stores whether the I2C peripheral is currently being used or not

	I2C_TypeDef*      pInstance;   //Stores the I2C_TypeDef for the I2C peripheral (defined in stm32f769xx.h)

	IRQn_Type         eIRQ_Event;  //Stores the Event IRQ Handler enum for the I2C peripheral (defined in stm32f769xx.h)
	IRQn_Type         eIRQ_Error;  //Stores the Error IRQ Handler enum for the I2C peripheral (defined in stm32f769xx.h)

} QAD_I2C_Data;


//------------------------------------------
//------------------------------------------
//------------------------------------------


//----------
//QAD_I2CMgr
//
//Singleton class
//Used to allow management of I2C peripherals in order to make sure that a driver is prevented from accessing any
//I2C peripherals that are already being used by another driver
class QAD_I2CMgr {
private:

	//I2C Peripheral Data
	QAD_I2C_Data m_sI2Cs[QAD_I2C_PeriphCount];


	//------------
	//Constructors
	QAD_I2CMgr();

public:

	//------------------------------------------------------------------------------
	//Delete copy constructor and assignment operator due to being a singleton class
	QAD_I2CMgr(const QAD_I2CMgr& other) = delete;
	QAD_I2CMgr& operator=(const QAD_I2CMgr& other) = delete;


	//-----------------
	//Singleton Methods
	//
	//Used to retrieve a reference to the singleton class
	static QAD_I2CMgr& get(void) {
		static QAD_I2CMgr instance;
		return instance;
	}


	//------------
	//Data Methods

	//Used to retrieve the current state (QAD_I2C_PeriphState enum) of a I2C peripheral
	//eI2C - The I2C peripheral to retrieve the state for. Member of QAD_I2C_Periph
	//Returns a member of QAD_I2C_State enum (QAD_I2C_Unused, etc)
	static QAD_I2C_State getState(QAD_I2C_Periph eI2C) {
		if (eI2C >= QAD_I2CNone)
			return QAD_I2C_InvalidDevice;

		return get().m_sI2Cs[eI2C].eState;
	}


	//Used to retrieve an instance for an I2C peripheral
	//eI2C - The I2C peripheral to retrieve the instance for. Member of QAD_I2C_Periph
	//Returns I2C_TypeDef, as defined in stm32f769xx.h
	static I2C_TypeDef* getInstance(QAD_I2C_Periph eI2C) {
		if (eI2C >= QAD_I2CNone)
			return NULL;

		return get().m_sI2Cs[eI2C].pInstance;
	}


	//Used to retrieve an Event IRQ Enum for an I2C peripheral
	//eI2C - The I2C peripheral to retrieve the IRQ enum for. Member of QAD_I2C_Periph
	//Returns member of IRQn_Type enum, as defined in stm32f769xx.h
	static IRQn_Type getIRQEvent(QAD_I2C_Periph eI2C) {
		if (eI2C >= QAD_I2CNone)
			return UsageFault_IRQn;

		return get().m_sI2Cs[eI2C].eIRQ_Event;
	}


	//Used to retrieve an Error IRQ Enum for an I2C peripheral
	//eI2C - The I2C peripheral to retrieve the IRQ enum for. Member of QAD_I2C_Periph
	//Returns member of IRQn_Type enum, as defined in stm32f769xx.h
	static IRQn_Type getIRQError(QAD_I2C_Periph eI2C) {
		if (eI2C >= QAD_I2CNone)
			return UsageFault_IRQn;

		return get().m_sI2Cs[eI2C].eIRQ_Error;
	}


	//------------------
	//Management Methods

	//Used to register an I2C peripheral as being used by a driver
	//eI2C - the I2C peripheral to be registered
	//Returns QA_OK if successful, or returns QA_Error_PeriphBusy if the selected I2C is already in use
	static QA_Result registerI2C(QAD_I2C_Periph eI2C, QAD_I2C_State eMode) {
		return get().imp_registerI2C(eI2C, eMode);
	}


	//Used to deregister an I2C to mark it as no longer being used by a driver
	//eI2C - the I2C Peripheral to be deregistered
	static void deregisterI2C(QAD_I2C_Periph eI2C) {
		get().imp_deregisterI2C(eI2C);
	}


	//-------------
	//Clock Methods

	//Used to enable the clock for a specific I2C peripheral
	//eI2C - the I2C peripheral to enable the clock for
	static void enableClock(QAD_I2C_Periph eI2C) {
		get().imp_enableClock(eI2C);
	}

	//Used to disable the clock for a specific I2C peripheral
	//eI2C - the I2C peripheral to disable the clock for
	static void disableClock(QAD_I2C_Periph eI2C) {
		get().imp_disableClock(eI2C);
	}


	//--------------
	//Status Methods

	//Returns the number of I2C peripherals that are currently in-use (registered/active)
	static uint8_t getI2CsActive(void) {
		return get().imp_getI2CsActive();
	}

	//Returns the number of I2C peripherals that are currently not being used (deregistered/inactive)
	static uint8_t getI2CsInactive(void) {
		return get().imp_getI2CsInactive();
	}

private:

  //NOTE: See QAD_I2CMgr.cpp for details of the following methods

	//------------------
	//Management Methods
	QA_Result imp_registerI2C(QAD_I2C_Periph eI2C, QAD_I2C_State eMode);
	void imp_deregisterI2C(QAD_I2C_Periph eI2C);


	//-------------
	//Clock Methods
	void imp_enableClock(QAD_I2C_Periph eI2C);
	void imp_disableClock(QAD_I2C_Periph eI2C);


	//--------------
	//Status Methods
	uint8_t imp_getI2CsActive(void);
	uint8_t imp_getI2CsInactive(void);

};


//Prevent Recursive Inclusion
#endif /* __QAD_I2CMGR_HPP_ */
