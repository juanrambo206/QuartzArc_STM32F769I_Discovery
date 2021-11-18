/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Driver                                                        */
/*   Role: RNG Driver                                                      */
/*   Filename: QAD_RNG.cpp                                                 */
/*   Date: 18th November 2021                                              */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Includes
#include "QAD_RNG.hpp"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------


  //------------------------------
  //------------------------------
  //QAD_RNG Initialization Methods

//QAD_RNG::imp_init
//QAD_RNG Initialization Method
QA_Result QAD_RNG::imp_init(void) {
  if (m_eInitState)
  	return QA_OK;

  //Enable RNG Clock
  __HAL_RCC_RNG_CLK_ENABLE();
  __HAL_RCC_RNG_FORCE_RESET();
  __HAL_RCC_RNG_RELEASE_RESET();

  //Initialize RNG
  m_sHandle.Instance = RNG;
  if (HAL_RNG_Init(&m_sHandle) != HAL_OK) {
  	__HAL_RCC_RNG_CLK_DISABLE();
  	return QA_Fail;
  }

  //Set Driver State
  m_eInitState = QA_Initialized;

  //Return
  return QA_OK;
}


//QAD_RNG::imp_deinit
//QAD_RNG Initialization Method
void QAD_RNG::imp_deinit(void) {
  if (!m_eInitState)
  	return;

  //Deinitialize RNG
  HAL_RNG_DeInit(&m_sHandle);

  //Set Driver State
  m_eInitState = QA_NotInitialized;
}


  //------------
  //Data Methods

//QAD_RNG::imp_getValue
//QAD_RNG Data Method
uint32_t QAD_RNG::imp_getValue(void) {
  if (!m_eInitState)
  	return 0;

	uint32_t uVal = 0;
  HAL_RNG_GenerateRandomNumber(&m_sHandle, &uVal);
  return uVal;
}
