/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Driver                                                        */
/*   Role: UART Management Driver                                          */
/*   Filename: QAD_UARTMgr.cpp                                             */
/*   Date: 5th November 2021                                               */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Includes
#include "QAD_UARTMgr.hpp"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------


	//------------------------
	//------------------------
	//QAD_UARTMgr Constructors

//QAD_UARTMgr::QAD_UARTMgr
//QAD_UARTMgr Constructor
//
//Fills out details for the system's UART peripherals
//As this is a private method is a singleton class, this method will be called the first time the class's get() method is called.
QAD_UARTMgr::QAD_UARTMgr() {

	for (uint8_t i=0; i<QAD_UART_PeriphCount; i++) {
		m_sUARTs[i].eState = QAD_UART_Unused;
	}

	//Set UART Periph ID
	m_sUARTs[QAD_UART1].eUART = QAD_UART1;
	m_sUARTs[QAD_UART2].eUART = QAD_UART2;
	m_sUARTs[QAD_UART3].eUART = QAD_UART3;
	m_sUARTs[QAD_UART4].eUART = QAD_UART4;
	m_sUARTs[QAD_UART5].eUART = QAD_UART5;
	m_sUARTs[QAD_UART6].eUART = QAD_UART6;
	m_sUARTs[QAD_UART7].eUART = QAD_UART7;
	m_sUARTs[QAD_UART8].eUART = QAD_UART8;

	//Set Instances
	m_sUARTs[QAD_UART1].pInstance = USART1;
	m_sUARTs[QAD_UART2].pInstance = USART2;
	m_sUARTs[QAD_UART3].pInstance = USART3;
	m_sUARTs[QAD_UART4].pInstance = UART4;
	m_sUARTs[QAD_UART5].pInstance = UART5;
	m_sUARTs[QAD_UART6].pInstance = USART6;
	m_sUARTs[QAD_UART7].pInstance = UART7;
	m_sUARTs[QAD_UART8].pInstance = UART8;

	//Set IRQs
	m_sUARTs[QAD_UART1].eIRQ = USART1_IRQn;
	m_sUARTs[QAD_UART2].eIRQ = USART2_IRQn;
	m_sUARTs[QAD_UART3].eIRQ = USART3_IRQn;
	m_sUARTs[QAD_UART4].eIRQ = UART4_IRQn;
	m_sUARTs[QAD_UART5].eIRQ = UART5_IRQn;
	m_sUARTs[QAD_UART6].eIRQ = USART6_IRQn;
	m_sUARTs[QAD_UART7].eIRQ = UART7_IRQn;
	m_sUARTs[QAD_UART8].eIRQ = UART8_IRQn;

}


	//--------------------------------------
	//--------------------------------------
	//QAD_UARTMgr Private Management Methods

//QAD_UARTMgr::imp_registerUART
//QAD_UARTMgr Private Management Method
//
//To be called from static method registerUART()
//Used to register a UART peripheral as being used by a driver
//Returns QA_OK if registration is successful, or returns QA_Error_PeriphBusy if the selected UART is already in use
QA_Result QAD_UARTMgr::imp_registerUART(QAD_UART_Periph eUART) {
	if (eUART >= QAD_UARTNone)
		return QA_Fail;

  if (m_sUARTs[eUART].eState)
  	return QA_Error_PeriphBusy;

  m_sUARTs[eUART].eState = QAD_UART_InUse;
  return QA_OK;
}


//QAD_UARTMgr::imp_deregisterUART
//QAD_UARTMgr Private Management Method
//
//To be called from static method deregisterUART()
//Used to deregister a UART peripheral to mark it as no longer being used by a driver
//eUART - the UART peripheral to be deregistered
void QAD_UARTMgr::imp_deregisterUART(QAD_UART_Periph eUART) {
	if (eUART >= QAD_UARTNone)
		return;

  m_sUARTs[eUART].eState = QAD_UART_Unused;
}


	//---------------------------------
	//---------------------------------
	//QAD_UARTMgr Private Clock Methods

//QAD_UARTMgr::imp_enableClock
//QAD_UARTMgr Private Clock Method
//
//To be called from static method enableClock()
//Used to enable the clock for a specific UART peripheral
//eUART - the UART peripheral to enable the clock for
void QAD_UARTMgr::imp_enableClock(QAD_UART_Periph eUART) {
  switch (eUART) {
    case (QAD_UART1):
    	__HAL_RCC_USART1_CLK_ENABLE();
      __HAL_RCC_USART1_FORCE_RESET();
      __HAL_RCC_USART1_RELEASE_RESET();
      break;
    case (QAD_UART2):
    	__HAL_RCC_USART2_CLK_ENABLE();
      __HAL_RCC_USART2_FORCE_RESET();
      __HAL_RCC_USART2_RELEASE_RESET();
      break;
    case (QAD_UART3):
    	__HAL_RCC_USART3_CLK_ENABLE();
      __HAL_RCC_USART3_FORCE_RESET();
      __HAL_RCC_USART3_RELEASE_RESET();
      break;
    case (QAD_UART4):
    	__HAL_RCC_UART4_CLK_ENABLE();
      __HAL_RCC_UART4_FORCE_RESET();
      __HAL_RCC_UART4_RELEASE_RESET();
      break;
    case (QAD_UART5):
    	__HAL_RCC_UART5_CLK_ENABLE();
      __HAL_RCC_UART5_FORCE_RESET();
      __HAL_RCC_UART5_RELEASE_RESET();
      break;
    case (QAD_UART6):
    	__HAL_RCC_USART6_CLK_ENABLE();
      __HAL_RCC_USART6_FORCE_RESET();
      __HAL_RCC_USART6_RELEASE_RESET();
      break;
    case (QAD_UART7):
    	__HAL_RCC_UART7_CLK_ENABLE();
      __HAL_RCC_UART7_FORCE_RESET();
      __HAL_RCC_UART7_RELEASE_RESET();
      break;
    case (QAD_UART8):
    	__HAL_RCC_UART8_CLK_ENABLE();
      __HAL_RCC_UART8_FORCE_RESET();
      __HAL_RCC_UART8_RELEASE_RESET();
      break;
    case (QAD_UARTNone):
    	break;
  }
}


//QAD_UARTMgr::imp_disableClock
//QAD_UARTMgr Private Clock Method
//
//To be called from static method disableClock()
//Used to disable the clock for a specific UART peripheral
//eUART - the UART peripheral to disable the clock for
void QAD_UARTMgr::imp_disableClock(QAD_UART_Periph eUART) {
  switch (eUART) {
    case (QAD_UART1):
    	__HAL_RCC_USART1_CLK_DISABLE();
      break;
    case (QAD_UART2):
    	__HAL_RCC_USART2_CLK_DISABLE();
      break;
    case (QAD_UART3):
    	__HAL_RCC_USART3_CLK_DISABLE();
      break;
    case (QAD_UART4):
    	__HAL_RCC_UART4_CLK_DISABLE();
      break;
    case (QAD_UART5):
    	__HAL_RCC_UART5_CLK_DISABLE();
      break;
    case (QAD_UART6):
    	__HAL_RCC_USART6_CLK_DISABLE();
      break;
    case (QAD_UART7):
    	__HAL_RCC_UART7_CLK_DISABLE();
      break;
    case (QAD_UART8):
    	__HAL_RCC_UART8_CLK_DISABLE();
      break;
    case (QAD_UARTNone):
    	break;
  }
}


	//----------------------------------
	//----------------------------------
	//QAD_UARTMgr Private Status Methods

//QAD_UARTMgr::imp_getUARTsActive
//QAD_UARTMgr Private Status Method
//
//To be called from static method getUARTsActive()
//Returns the number of UART peripherals that are currently in-use (registered/active)
uint8_t QAD_UARTMgr::imp_getUARTsActive(void) {
  uint8_t uCount = 0;
  for (uint8_t i=0; i<QAD_UART_PeriphCount; i++) {
  	if (m_sUARTs[i].eState)
  		uCount++;
  }
  return uCount;
}


//QAD_UARTMgr::imp_getUARTsInactive
//QAD_UARTMgr Private Status Method
//
//To be called from static method getUARTsInactive()
//Returns the number of UART peripherals that are currently not being used (deregistered/inactive)
uint8_t QAD_UARTMgr::imp_getUARTsInactive(void) {
  uint8_t uCount = 0;
  for (uint8_t i=0; i<QAD_UART_PeriphCount; i++) {
  	if (!m_sUARTs[i].eState)
  		uCount++;
  }
  return uCount;
}














