/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Driver                                                        */
/*   Role: RTC Driver                                                      */
/*   Filename: QAD_RTC.hpp                                                 */
/*   Date: 18th November 2021                                              */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Prevent Recursive Inclusion
#ifndef __QAD_RTC_HPP_
#define __QAD_RTC_HPP_


//Includes
#include "setup.hpp"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------


//-------
//QAD_RTC
//
//Singleton class
//Driver class for Real Time Clock (RTC) peripheral, which is used to keep accurate
//date and time.
//This is a singleton class due to only one RTC peripheral existing on the STM32 device,
//preventing potential conflicts if multiple instances of the class were to exist.
class QAD_RTC {
private:

	QA_InitState       m_eInitState; //Stores whether the driver is currently initialized.
	                                 //A member of QA_InitState enum defined in setup.hpp

	RTC_HandleTypeDef  m_sHandle;    //Handle used by the HAL functions to access the RNG peripheral

	RTC_TimeTypeDef    m_sTime;      //Used to temporarily store time data when setting time,
	                                 //or when capturing a timestamp using the update() method

	RTC_DateTypeDef    m_sDate;      //Used to temporarily store date data when setting the date,
	                                 //or when capturing a timestamp using the update() method


	//------------
	//Constructors

	//As this is a private method within a singleton class, this method will be called the
	//first time the static get() method is called
	QAD_RTC() :
		m_eInitState(QA_NotInitialized),
		m_sHandle({0}) {}

public:

	//------------------------------------------------------------------------------
	//Delete copy constructor and assignment operator due to being a singleton class
	QAD_RTC(const QAD_RTC& other) = delete;
	QAD_RTC& operator=(const QAD_RTC& other) = delete;


	//-----------------
	//Singleton Methods
	//
	//Used to retrieve a reference to the singleton class
	static QAD_RTC& get(void) {
		static QAD_RTC instance;
		return instance;
	}


	//----------------------
	//Initialization Methods

	//Used to initialize the RTC peripheral
	//Returns QA_OK if the initialization is successful, or QA_Fail if initialization fails
	static QA_Result init(void) {
		return get().imp_init();
	}

	//Used to deinitialize the RTC peripheral
	static void deinit(void) {
		get().imp_deinit();
	}


	//---------------
	//Control Methods

	//Used to set the current time
	//uHour   - The current hour. A value between 0-23
	//uMinute - The current minute. A value between 0-59
	//uSecond - The current second. A value between 0-59
	//Returns QA_OK if successful, or QA_Fail if setting of time fails
	static QA_Result setTime(uint8_t uHour, uint8_t uMinute, uint8_t uSecond) {
		return get().imp_setTime(uHour, uMinute, uSecond);
	}


	//Used to set the current date
	//uWeekDay - The current day of the week. A value between 1-7
	//uDay     - The current day of the month. A value between 1-31
	//uMonth   - The current month. A value between 1-12
	//uYear    - The last two digits of the current year. A value between 00-99
	//Returns QA_OK if successful, or QA_Fail if setting of the date fails
	static QA_Result setDate(uint8_t uWeekDay, uint8_t uDay, uint8_t uMonth, uint8_t uYear) {
		return get().imp_setDate(uWeekDay, uDay, uMonth, uYear);
	}


	//Captures the current time and date and stores in m_sTime and m_sDate, so that values can
	//be retrieved by the below data methods
	//Returns QA_OK if successful, or QA_Fail if unable to get the current time/date
	static QA_Result update(void) {
		return get().imp_update();
	}


	//------------
	//Data Methods

	//NOTE: The update() method should be called prior to using the below get*() methods
	//in order to capture the most recent date/time values

	//Used to retrieve the hour value from m_sTime
	//Returns a value between 0-23
	static uint8_t getHour(void) {
		return get().imp_getHour();
	}


	//Used to retrieve the minute value from m_sTime
	//Returns a value between 0-59
	static uint8_t getMinute(void) {
		return get().imp_getMinute();
	}


	//Used to retrieve the second value from m_sTime
	//Returns a value between 0-59
	static uint8_t getSecond(void) {
		return get().imp_getSecond();
	}


	//Used to retrieve the current day-of-the-week value from m_sDate
	//Returns a value between 1-7
	static uint8_t getWeekday(void) {
		return get().imp_getWeekday();
	}


	//Used to retrieve the current day-of-the-week value from m_sDate, as a string containing
	//the name of the day.
	//strWeekday - a c-style string to copy the name of the day into. Must be able to fit at
	//             least 10 characters
	static void getWeekDayStr(char* strWeekday) {
		get().imp_getWeekdayStr(strWeekday);
	}


	//Used to retrieve the current day-of-the-month value from m_sDate
	//Returns a value between 1-31
	static uint8_t getDay(void) {
		return get().imp_getDay();
	}


  //Used to retrieve the current month value from m_sDate, as a string containing
	//the name of the month.
  //Returns a value between 1-12
	static uint8_t getMonth(void) {
		return get().imp_getMonth();
	}


  //Used to retrieve the current month value from m_sDate, as a string containing
	//the name of the month.
	//strMonth - a c-style string to copy the name of the current month into. Must be able to
	//           fit at least 10 characters
	static void getMonthStr(char* strMonth) {
		get().imp_getMonthStr(strMonth);
	}


	//Used to retrieve the current year value from m_sYear
	//Returns a value between 2000-2099
	static uint16_t getYear(void) {
		return get().imp_getYear();
	}


private:

  //NOTE: See QAD_RTC.cpp for details of the following methods


	//----------------------
	//Initialization Methods

	QA_Result imp_init(void);
	void imp_deinit(void);


	//---------------
	//Control Methods

	QA_Result imp_setTime(uint8_t uHour, uint8_t uMinute, uint8_t uSecond);
	QA_Result imp_setDate(uint8_t uWeekDay, uint8_t uDay, uint8_t uMonth, uint8_t uYear);
	QA_Result imp_update(void);


	//------------
	//Data Methods
	uint8_t imp_getHour(void);
	uint8_t imp_getMinute(void);
	uint8_t imp_getSecond(void);

	uint8_t imp_getWeekday(void);
	void imp_getWeekdayStr(char* strWeekday);
	uint8_t imp_getDay(void);
	uint8_t imp_getMonth(void);
	void imp_getMonthStr(char* strMonth);
	uint16_t imp_getYear(void);


};


//Prevent Recursive Inclusion
#endif /* __QAD_RTC_HPP_ */













