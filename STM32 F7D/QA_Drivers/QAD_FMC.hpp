/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Driver                                                        */
/*   Role: FMC / SDRAM Driver                                              */
/*   Filename: QAD_FMC.hpp                                                 */
/*   Date: 30th September 2021                                             */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Prevent Recursive Inclusion
#ifndef __QAD_FMC_HPP_
#define __QAD_FMC_HPP_


//Includes
#include "setup.hpp"



	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

//Access Defines
#define QAD_FMC_32BITWORD_COUNT ((uint32_t)4194304)

typedef struct {
	__IO uint32_t data[QAD_FMC_32BITWORD_COUNT];
} QAD_FMC_32;

#define QAD_FMC_PTR       ((QAD_FMC_32*)0xC0000000)



	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

//-------
//QAD_FMC
class QAD_FMC {
private:

	const uint32_t m_uModeReg_BurstLength_1 = 0x00000000;
	const uint32_t m_uModeReg_BurstLength_2 = 0x00000001;
	const uint32_t m_uModeReg_BurstLength_4 = 0x00000002;
	const uint32_t m_uModeReg_BurstLength_8 = 0x00000004;

	const uint32_t m_uModeReg_BurstType_Seq = 0x00000000;
	const uint32_t m_uModeReg_BurstType_Int = 0x00000008;

	const uint32_t m_uModeReg_CASLatency_2  = 0x00000020;
	const uint32_t m_uModeReg_CASLatency_3  = 0x00000030;

	const uint32_t m_uModeReg_OpMode_Std    = 0x00000000;

	const uint32_t m_uModeReg_WriteBurst_Prog   = 0x00000000;
	const uint32_t m_uModeReg_WriteBurst_Single = 0x00000200;

	const uint32_t m_uTimeout  = 0xFFFF;
	const uint32_t m_uRefresh  = 0x0603;

	static const uint32_t m_uBaseAddr   = 0xC0000000;
	static const uint32_t m_uSize       = 0x01000000;

private:

	SDRAM_HandleTypeDef m_sHandle;
	QA_InitState        m_eState;

	QAD_FMC() :
	  m_eState(QA_NotInitialized) {}

public:

	QAD_FMC(const QAD_FMC& other) = delete;
	QAD_FMC& operator=(const QAD_FMC& other) = delete;

  static QAD_FMC& get(void) {
  	static QAD_FMC instance;
  	return instance;
  }

  static QA_InitState getState(void) {
  	return get().m_eState;
  }

  static QA_Result init(void) {
  	return get().imp_init();
  }

  static QA_Result test(void) {
  	return get().imp_test();
  }

  static uint32_t getBaseAddr(void) {
  	return m_uBaseAddr;
  }

  static uint32_t getSize(void) {
  	return m_uSize;
  }

private:

  QA_Result imp_init();
  void cmd(FMC_SDRAM_CommandTypeDef& pCmd);

  QA_Result imp_test(void);

};


//Prevent Recursive Inclusion
#endif /* __QAD_FMC_HPP_ */
