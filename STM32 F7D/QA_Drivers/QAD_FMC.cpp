/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Driver                                                        */
/*   Role: FMC / SDRAM Driver                                              */
/*   Filename: QAD_FMC.cpp                                                 */
/*   Date: 30th September 2021                                             */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Includes
#include "QAD_FMC.hpp"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

  //------------------------------
  //QAD_FMC Initialization Methods

//QAD_FMC::imp_init
//QAD_FMC Initialization Method
QA_Result QAD_FMC::imp_init() {
	if (m_eState)
		return QA_OK;

	GPIO_InitTypeDef GPIO_Init = {0};

	FMC_SDRAM_TimingTypeDef   SDRAM_Timing = {0};
	FMC_SDRAM_CommandTypeDef  SDRAM_Cmd = {0};

	//Initialize GPIOs
	GPIO_Init.Mode      = GPIO_MODE_AF_PP;
	GPIO_Init.Pull      = GPIO_PULLUP;
	GPIO_Init.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init.Alternate = GPIO_AF12_FMC;

	GPIO_Init.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOD, &GPIO_Init);

		//GPIO E
	GPIO_Init.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE, &GPIO_Init);

		//GPIO F
	GPIO_Init.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOF, &GPIO_Init);

		//GPIO G
	GPIO_Init.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOG, &GPIO_Init);

		//GPIO H
	GPIO_Init.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOH, &GPIO_Init);

		//GPIO I
	GPIO_Init.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
	HAL_GPIO_Init(GPIOI, &GPIO_Init);


		//Initialize FMC Clock
	__HAL_RCC_FMC_CLK_ENABLE();

		//Initialize Peripheral
	m_sHandle.Instance                = FMC_SDRAM_DEVICE;
	m_sHandle.Init.SDBank             = FMC_SDRAM_BANK1;
	m_sHandle.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_8;
	m_sHandle.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_12;
	m_sHandle.Init.MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_32;
	m_sHandle.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
	m_sHandle.Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_3;
	m_sHandle.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
	m_sHandle.Init.SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_2;
	m_sHandle.Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;
	m_sHandle.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0;

	SDRAM_Timing.LoadToActiveDelay    = 2;
	SDRAM_Timing.ExitSelfRefreshDelay = 7;
	SDRAM_Timing.SelfRefreshTime      = 4;
	SDRAM_Timing.RowCycleDelay        = 7;
	SDRAM_Timing.WriteRecoveryTime    = 2;
	SDRAM_Timing.RPDelay              = 2;
	SDRAM_Timing.RCDDelay             = 2;

	if (HAL_SDRAM_Init(&m_sHandle, &SDRAM_Timing) != HAL_OK) {
		return QA_Fail;
	}


	//Initialize SDRAM IC

		//Enable Clock
	SDRAM_Cmd.CommandMode            = FMC_SDRAM_CMD_CLK_ENABLE;
	SDRAM_Cmd.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
	SDRAM_Cmd.AutoRefreshNumber      = 1;
	SDRAM_Cmd.ModeRegisterDefinition = 0;
	cmd(SDRAM_Cmd);

		//Delay 1ms
	HAL_Delay(1);

		//Precharge All Banks
	SDRAM_Cmd.CommandMode            = FMC_SDRAM_CMD_PALL;
	SDRAM_Cmd.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
	SDRAM_Cmd.AutoRefreshNumber      = 1;
	SDRAM_Cmd.ModeRegisterDefinition = 0;
	cmd(SDRAM_Cmd);

		//Configure Auto Refresh
	SDRAM_Cmd.CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	SDRAM_Cmd.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
	SDRAM_Cmd.AutoRefreshNumber      = 8;
	SDRAM_Cmd.ModeRegisterDefinition = 0;
	cmd(SDRAM_Cmd);

		//Program Mode Register
	SDRAM_Cmd.CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
	SDRAM_Cmd.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
	SDRAM_Cmd.AutoRefreshNumber      = 1;
	SDRAM_Cmd.ModeRegisterDefinition = m_uModeReg_BurstLength_1 | m_uModeReg_BurstType_Seq | m_uModeReg_CASLatency_3 |
																		 m_uModeReg_OpMode_Std | m_uModeReg_WriteBurst_Single;
	cmd(SDRAM_Cmd);


	//Program Refresh Rate
	HAL_SDRAM_ProgramRefreshRate(&m_sHandle, m_uRefresh);

	//
	m_eState = QA_Initialized;
	return QA_OK;
}


//QAD_FMC::cmd
//QAD_FMC Initialization Method
void QAD_FMC::cmd(FMC_SDRAM_CommandTypeDef& pCmd) {
	HAL_SDRAM_SendCommand(&m_sHandle, &pCmd, 0);
}


  //--------------------
  //QAD_FMC Test Methods

//QAD_FMC::imp_test
//QAD_FMC Test Method
QA_Result QAD_FMC::imp_test(void) {

	//Write to SDRAM
	for (uint32_t i=0; i < QAD_FMC_32BITWORD_COUNT; i++) {
		QAD_FMC_PTR->data[i] = i;
	}

	//Read from SDRAM
	for (uint32_t i=0; i < QAD_FMC_32BITWORD_COUNT; i++) {
		if (QAD_FMC_PTR->data[i] != i)
			return QA_Fail;
	}

	return QA_OK;
}




