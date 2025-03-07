/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Driver                                                        */
/*   Role: Quad SPI / MX25L512 Flash Driver                                */
/*   Filename: QAD_QuadSPI.cpp                                             */
/*   Date: 22nd January 2022                                               */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Includes
#include "QAD_QuadSPI.hpp"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

  //-------------------
  //Command Definitions

	  //Reset Commands
#define MX25L512_CMD_RESET_ENABLE                 ((uint8_t)0x66)
#define MX25L512_CMD_RESET_MEMORY                 ((uint8_t)0x99)

    //Register Operation Commands
#define MX25L512_CMD_READ_STATUS_REG              ((uint8_t)0x05)
#define MX25L512_CMD_READ_CFG_REG                 ((uint8_t)0x15)
#define MX25L512_CMD_WRITE_STATUS_CFG_REG         ((uint8_t)0x01)

    //4-byte Address Mode Commands
#define MX25L512_CMD_ENTER_4_BYTE_ADDR_MODE       ((uint8_t)0xB7)

    //Qaud Mode Commands
#define MX25L512_CMD_ENTER_QUAD_MODE              ((uint8_t)0x35)
#define MX25L512_CMD_EXIT_QUAD_MODE               ((uint8_t)0xF5)

    //Read Operation Commands
#define MX25L512_CMD_READ_4_BYTE_ADDR             ((uint8_t)0xEC) //0x13)

    //Write Operation Commands
#define MX25L512_CMD_WRITE_ENABLE                 ((uint8_t)0x06)

    //Program Operation Commands
#define MX25L512_CMD_PAGE_PROG_4_BYTE_ADDR        ((uint8_t)0x12)

    //Erase Operation Commands
#define MX25L512_CMD_SUBSECTOR_ERASE_4_BYTE_ADDR  ((uint8_t)0x21)
#define MX25L512_CMD_SECTOR_ERASE_4_BYTE_ADDR     ((uint8_t)0xDC)
#define MX25L512_CMD_BULK_ERASE                   ((uint8_t)0xC7)


  //--------------------
  //Register Definitions

	  //Status Register
#define MX25L512_SR_WIP                      ((uint8_t)0x01)  //Write In Progress Bit
#define MX25L512_SR_WREN                     ((uint8_t)0x02)  //Write Enabled Bit
#define MX25L512_SR_QUADEN                   ((uint8_t)0x40)  //Quad IO Mode Enabled Bit

    //Configuration Register
#define MX25L512_CR_ODS                      ((uint8_t)0x07)  //Output driver strength
#define MX25L512_CR_ODS_30                   ((uint8_t)0x07)  //Output driver strength 30 ohms (default)
#define MX25L512_CR_ODS_15                   ((uint8_t)0x06)  //Output driver strength 15 ohms
#define MX25L512_CR_ODS_20                   ((uint8_t)0x05)  //Output driver strength 20 ohms
#define MX25L512_CR_ODS_45                   ((uint8_t)0x03)  //Output driver strength 45 ohms
#define MX25L512_CR_ODS_60                   ((uint8_t)0x02)  //Output driver strength 60 ohms
#define MX25L512_CR_ODS_90                   ((uint8_t)0x01)  //Output driver strength 90 ohms
#define MX25L512_CR_TB                       ((uint8_t)0x08)  //Top/Bottom bit used to configure the block protect area
#define MX25L512_CR_PBE                      ((uint8_t)0x10)  //Preamble Bit Enable
#define MX25L512_CR_4BYTE                    ((uint8_t)0x20)  //3-bytes or 4-bytes addressing
#define MX25L512_CR_NB_DUMMY                 ((uint8_t)0xC0)  //Number of dummy clock cycles


  //-------------------------
  //Configuration Definitions

#define MX25L512_DUMMY_CYCLES_READ_QUAD      3
#define MX25L512_DUMMY_CYCLES_READ_QUAD_IO   10

#define MX25L512_ERASE_CHIP_MAXTIME          600000
#define MX25L512_ERASE_SECTOR_MAXTIME        2000
#define MX25L512_ERASE_SUBSECTOR_MAXTIME     800


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

	//----------------------------------
	//----------------------------------
	//QAD_QuadSPI Initialization Methods

//QAD_QuadSPI::imp_init
//QAD_QuadSPI Initialization Method
QA_Result QAD_QuadSPI::imp_init(void) {
  if (m_eInitState)
  	return QA_OK;

  //Initialized Peripheral
  if (imp_periphInit())
  	return QA_Fail;

  //Set Driver States
  m_eInitState         = QA_Initialized;
  m_eMemoryMappedState = QAD_QuadSPI_MemoryMapped_Disabled;

  //Return
  return QA_OK;
}


//QAD_QuadSPI::imp_deinit
//QAD_QuadSPI Initialization Method
void QAD_QuadSPI::imp_deinit(void) {
  if (!m_eInitState)
  	return;

  //Deinitialization Peripheral
  imp_periphDeinit(DeinitFull);

  //Set Driver States
  m_eInitState         = QA_NotInitialized;
  m_eMemoryMappedState = QAD_QuadSPI_MemoryMapped_Disabled;
}


//---------------------------------------------
//---------------------------------------------
//QAD_QuadSPI Peripheral Initialization Methods

//QAD_QuadSPI::imp_periphInit
//QAD_QuadSPI Peripheral Initialization Method
QA_Result QAD_QuadSPI::imp_periphInit(void) {
	GPIO_InitTypeDef GPIO_Init = {0};

	//----------
	//Init GPIOs

	//CS Pin
	GPIO_Init.Pin       = QAD_QUADSPI_CS_PIN;
	GPIO_Init.Mode      = GPIO_MODE_AF_PP;
	GPIO_Init.Pull      = GPIO_PULLUP;
	GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
	GPIO_Init.Alternate = QAD_QUADSPI_CS_AF;
	HAL_GPIO_Init(QAD_QUADSPI_CS_PORT, &GPIO_Init);

	//Clk Pin
	GPIO_Init.Pin       = QAD_QUADSPI_CLK_PIN;
	GPIO_Init.Pull      = GPIO_NOPULL;
	GPIO_Init.Alternate = QAD_QUADSPI_CLK_AF;
	HAL_GPIO_Init(QAD_QUADSPI_CLK_PORT, &GPIO_Init);

	//Data 0 Pin
	GPIO_Init.Pin       = QAD_QUADSPI_DATA0_PIN;
	GPIO_Init.Alternate = QAD_QUADSPI_DATA0_AF;
	HAL_GPIO_Init(QAD_QUADSPI_DATA0_PORT, &GPIO_Init);

	//Data 1 Pin
	GPIO_Init.Pin       = QAD_QUADSPI_DATA1_PIN;
	GPIO_Init.Alternate = QAD_QUADSPI_DATA1_AF;
	HAL_GPIO_Init(QAD_QUADSPI_DATA1_PORT, &GPIO_Init);

	//Data 2 Pin
	GPIO_Init.Pin       = QAD_QUADSPI_DATA2_PIN;
	GPIO_Init.Alternate = QAD_QUADSPI_DATA2_AF;
	HAL_GPIO_Init(QAD_QUADSPI_DATA2_PORT, &GPIO_Init);

	//Data 3 Pin
	GPIO_Init.Pin       = QAD_QUADSPI_DATA3_PIN;
	GPIO_Init.Alternate = QAD_QUADSPI_DATA3_AF;
	HAL_GPIO_Init(QAD_QUADSPI_DATA3_PORT, &GPIO_Init);

	//---------------
	//Init Peripheral

	//Enable QSPI Clock
	__HAL_RCC_QSPI_CLK_ENABLE();

	//Init QSPI
	m_sHandle.Instance                = QUADSPI;
	m_sHandle.Init.ClockPrescaler     = 1;
	m_sHandle.Init.FifoThreshold      = 16;
	m_sHandle.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
	m_sHandle.Init.FlashSize          = POSITION_VAL(0x4000000) - 1;
	m_sHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_4_CYCLE;
	m_sHandle.Init.ClockMode          = QSPI_CLOCK_MODE_0;
	m_sHandle.Init.FlashID            = QSPI_FLASH_ID_1;
	m_sHandle.Init.DualFlash          = QSPI_DUALFLASH_DISABLE;
	if (HAL_QSPI_Init(&m_sHandle) != HAL_OK) {
		imp_periphDeinit(DeinitPartial);
		return QA_Fail;
	}

	//Enable IRQ
	HAL_NVIC_SetPriority(QUADSPI_IRQn, QAD_IRQPRIORITY_FLASH, 0x00);
  HAL_NVIC_EnableIRQ(QUADSPI_IRQn);


	//-------------
	//Init Flash IC

	//Reset Memory
	if (imp_resetMemory()) {
		imp_periphDeinit(DeinitFull);
		return QA_Fail;
	}

	//Enter QPI Mode
	if (imp_enterMemoryQPI()) {
		imp_periphDeinit(DeinitFull);
		return QA_Fail;
	}

	//Enter Four Byte Address Mode
	if (imp_enterFourByteAddress()) {
		imp_periphDeinit(DeinitFull);
		return QA_Fail;
	}

	//Configure Memory Dummy Cycles
	if (imp_dummyCyclesCfg()) {
		imp_periphDeinit(DeinitFull);
		return QA_Fail;
	}

	//Configure Output Drive Strength
	if (imp_outputDriveStrengthCfg()) {
		imp_periphDeinit(DeinitFull);
		return QA_Fail;
	}

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_periphDeinit
//QAD_QuadSPI Peripheral Initialization Method
void QAD_QuadSPI::imp_periphDeinit(QAD_QuadSPI::DeinitMode eMode) {

	if (eMode == DeinitFull) {

		//Deinitialize QuadSPI Peripheral
		HAL_QSPI_DeInit(&m_sHandle);

	}

	//Disable QSPI Clock
	__HAL_RCC_QSPI_CLK_DISABLE();

	//Deinit GPIOs
	HAL_GPIO_DeInit(QAD_QUADSPI_CS_PORT, QAD_QUADSPI_CS_PIN);
	HAL_GPIO_DeInit(QAD_QUADSPI_CLK_PORT, QAD_QUADSPI_CLK_PIN);
	HAL_GPIO_DeInit(QAD_QUADSPI_DATA0_PORT, QAD_QUADSPI_DATA0_PIN);
	HAL_GPIO_DeInit(QAD_QUADSPI_DATA1_PORT, QAD_QUADSPI_DATA1_PIN);
	HAL_GPIO_DeInit(QAD_QUADSPI_DATA2_PORT, QAD_QUADSPI_DATA2_PIN);
	HAL_GPIO_DeInit(QAD_QUADSPI_DATA3_PORT, QAD_QUADSPI_DATA3_PIN);

}


//------------------------------------------
//------------------------------------------
//------------------------------------------

//--------------------------------------
//--------------------------------------
//QAD_QuadSPI Memory Mapped Mode Methods

//QAD_QuadSPI::imp_enterMemoryMapped
//QAD_QuadSPI Memory Mapped Mode Method
QA_Result QAD_QuadSPI::imp_enterMemoryMapped(void) {
	QSPI_CommandTypeDef 	   sCmd;
	QSPI_MemoryMappedTypeDef sMMCfg;

	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_READ_4_BYTE_ADDR;
	sCmd.AddressMode       = QSPI_ADDRESS_4_LINES;
	sCmd.AddressSize       = QSPI_ADDRESS_32_BITS;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_4_LINES;
	sCmd.DummyCycles       = MX25L512_DUMMY_CYCLES_READ_QUAD_IO;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	sMMCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
	sMMCfg.TimeOutPeriod     = 0;

	if (HAL_QSPI_MemoryMapped(&m_sHandle, &sCmd, &sMMCfg) != HAL_OK)
		return QA_Fail;

	m_eMemoryMappedState = QAD_QuadSPI_MemoryMapped_Enabled;

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_exitMemoryMapped
//QAD_QuadSPI Memory Mapped Mode Method
QA_Result QAD_QuadSPI::imp_exitMemoryMapped(void) {
	if (m_eInitState) {
		imp_deinit();
		return imp_init();
	}
	return QA_OK;
}


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

	//------------------------
	//------------------------
	//QAD_QuadSPI Data Methods

	//----
	//Read

//QAD_QuadSPI::imp_read
//QAD_QuadSPI Data Method
QA_Result QAD_QuadSPI::imp_read(uint32_t uAddr, uint8_t* pData, uint32_t uSize) {
	QSPI_CommandTypeDef sCmd;

	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_READ_4_BYTE_ADDR;
	sCmd.AddressMode       = QSPI_ADDRESS_4_LINES;
	sCmd.AddressSize		   = QSPI_ADDRESS_32_BITS;
	sCmd.Address           = uAddr;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_4_LINES;
	sCmd.DummyCycles       = MX25L512_DUMMY_CYCLES_READ_QUAD_IO;
	sCmd.NbData            = uSize;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;
	MODIFY_REG(m_sHandle.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_1_CYCLE);
	if (HAL_QSPI_Receive(&m_sHandle, pData, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		MODIFY_REG(m_sHandle.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_4_CYCLE);
		return QA_Fail;
	}
	MODIFY_REG(m_sHandle.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_4_CYCLE);

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_readSubsector
//QAD_QuadSPI Data Method
QA_Result QAD_QuadSPI::imp_readSubsector(uint32_t uIdx, uint8_t* pData) {
	return imp_read(uIdx * m_uSubsectorSize, pData, m_uSubsectorSize);
}


//QAD_QuadSPI::imp_readSector
//QAD_QuadSPI Data Method
QA_Result QAD_QuadSPI::imp_readSector(uint32_t uIdx, uint8_t* pData) {
	return imp_read(uIdx * m_uSectorSize, pData, m_uSectorSize);
}


//-----
//Write

//QAD_QuadSPI::imp_write
//QAD_QuadSPI Data Method
QA_Result QAD_QuadSPI::imp_write(uint32_t uAddr, uint8_t* pData, uint32_t uSize) {
	QSPI_CommandTypeDef sCmd;
	uint32_t uEndAddr;
	uint32_t uCurSize;
	uint32_t uCurAddr;

	uCurSize = m_uPageSize - (uAddr % m_uPageSize);
	if (uCurSize > uSize)
		uCurSize = uSize;

	uCurAddr = uAddr;
	uEndAddr = uAddr + uSize;

	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_PAGE_PROG_4_BYTE_ADDR;
	sCmd.AddressMode       = QSPI_ADDRESS_4_LINES;
	sCmd.AddressSize       = QSPI_ADDRESS_32_BITS;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_4_LINES;
	sCmd.DummyCycles       = 0;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	do {
		sCmd.Address = uCurAddr;
		sCmd.NbData  = uCurSize;

		if (imp_writeEnable())
			return QA_Fail;
		if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
			return QA_Fail;
		if (HAL_QSPI_Transmit(&m_sHandle, pData, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
			return QA_Fail;
		if (imp_autoPollingMemReady(HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != 0)
			return QA_Fail;

		uCurAddr += uCurSize;
		pData += uCurSize;
		uCurSize = ((uCurAddr + m_uPageSize) > uEndAddr) ? (uEndAddr - uCurAddr) : m_uPageSize;
	} while (uCurAddr < uEndAddr);

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_eraseAndWriteSubsector
//QAD_QuadSPI Data Method
QA_Result QAD_QuadSPI::imp_eraseAndWriteSubsector(uint32_t uIdx, uint8_t* pData) {

	uint32_t uAddr = uIdx * m_uSubsectorSize;

	if (imp_eraseSubsectorAddr(uAddr))
		return QA_Fail;

	if (imp_write(uAddr, pData, m_uSubsectorSize))
		return QA_Fail;

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_eraseAndWriteSector
//QAD_QuadSPI Data Method
QA_Result QAD_QuadSPI::imp_eraseAndWriteSector(uint32_t uIdx, uint8_t* pData) {

	uint32_t uAddr = uIdx * m_uSectorSize;

	if (imp_eraseSectorAddr(uAddr))
		return QA_Fail;

	if (imp_write(uAddr, pData, m_uSectorSize))
		return QA_Fail;

	//Return
	return QA_OK;
}


//-----
//Erase

//QAD_QuadSPI::imp_eraseSubsectorAddr
//QAD_QuadSPI Data Method
QA_Result QAD_QuadSPI::imp_eraseSubsectorAddr(uint32_t uAddr) {
	QSPI_CommandTypeDef sCmd;

	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_SUBSECTOR_ERASE_4_BYTE_ADDR;
	sCmd.AddressMode       = QSPI_ADDRESS_4_LINES;
	sCmd.AddressSize       = QSPI_ADDRESS_32_BITS;
	sCmd.Address           = uAddr;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_NONE;
	sCmd.DummyCycles       = 0;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (imp_writeEnable() != 0)
		return QA_Fail;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;
	if (imp_autoPollingMemReady(MX25L512_ERASE_SUBSECTOR_MAXTIME))
		return QA_Fail;

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_eraseSectorAddr
//QAD_QuadSPI Data Method
QA_Result QAD_QuadSPI::imp_eraseSectorAddr(uint32_t uAddr) {
	QSPI_CommandTypeDef sCmd;

	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_SECTOR_ERASE_4_BYTE_ADDR;
	sCmd.AddressMode       = QSPI_ADDRESS_4_LINES;
	sCmd.AddressSize       = QSPI_ADDRESS_32_BITS;
	sCmd.Address           = uAddr;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_NONE;
	sCmd.DummyCycles       = 0;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (imp_writeEnable())
		return QA_Fail;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;
	if (imp_autoPollingMemReady(MX25L512_ERASE_SUBSECTOR_MAXTIME))
		return QA_Fail;

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_eraseSubsector
//QAD_QuadSPI Data Method
QA_Result QAD_QuadSPI::imp_eraseSubsector(uint32_t uIdx) {
	return imp_eraseSubsectorAddr(uIdx * m_uSubsectorSize);
}


//QAD_QuadSPI::imp_eraseSector
//QAD_QuadSPI Data Method
QA_Result QAD_QuadSPI::imp_eraseSector(uint32_t uIdx) {
	return imp_eraseSectorAddr(uIdx * m_uSectorSize);
}


//QAD_QuadSPI::imp_eraseChip
//QAD_QuadSPI Data Method
QA_Result QAD_QuadSPI::imp_eraseChip(void) {
	QSPI_CommandTypeDef sCmd;

	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_BULK_ERASE;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_NONE;
	sCmd.DummyCycles       = 0;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (imp_writeEnable())
		return QA_Fail;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;
	if (imp_autoPollingMemReady(MX25L512_ERASE_CHIP_MAXTIME))
		return QA_Fail;

	//Return
	return QA_OK;
}


	//------
	//Status

//QAD_QuadSPI::imp_getStatus
//QAD_QuadSPI Data Method
QAD_QuadSPI_Status QAD_QuadSPI::imp_getStatus(void) {
	QSPI_CommandTypeDef sCmd;
	uint8_t uReg;

	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_READ_STATUS_REG;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_4_LINES;
	sCmd.DummyCycles       = 0;
	sCmd.NbData            = 1;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QAD_QuadSPI_Status_Error;
	if (HAL_QSPI_Receive(&m_sHandle, &uReg, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QAD_QuadSPI_Status_Error;
	if ((uReg & MX25L512_SR_WIP) != 0)
		return QAD_QuadSPI_Status_Busy;

	//Return
	return QAD_QuadSPI_Status_Ready;
}


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

	//------------------------
	//------------------------
	//QAD_QuadSPI Tool Methods

//QAD_QuadSPI::imp_resetMemory
//QAD_QuadSPI Tool Method
QA_Result QAD_QuadSPI::imp_resetMemory(void) {
	QSPI_CommandTypeDef     sCmd;
	QSPI_AutoPollingTypeDef sCfg;
	uint8_t                 uReg;

	//------------------------------
	//Send reset command in QPI mode
	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_RESET_ENABLE;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_NONE;
	sCmd.DummyCycles       = 0;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//-------------------------
	//Send reset memory command
	sCmd.Instruction = MX25L512_CMD_RESET_MEMORY;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//------------------------------
	//Send reset command in SPI mode
	sCmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	sCmd.Instruction     = MX25L512_CMD_RESET_ENABLE;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//--------------------------------
	//Delay in case of full chip erase
	HAL_Delay(1000);

	//------------------------------------------------------
	//Configure Automatic Polling Mode to wait for WIP bit=0
	sCfg.Match           = 0;
	sCfg.Mask            = MX25L512_SR_WIP;
	sCfg.MatchMode       = QSPI_MATCH_MODE_AND;
	sCfg.StatusBytesSize = 1;
	sCfg.Interval        = 0x10;
	sCfg.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	sCmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	sCmd.Instruction       = MX25L512_CMD_READ_STATUS_REG;
	sCmd.DataMode          = QSPI_DATA_1_LINE;
	if (HAL_QSPI_AutoPolling(&m_sHandle, &sCmd, &sCfg, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//-------------------------------------
	//Initialize reading of status register
	sCmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	sCmd.Instruction       = MX25L512_CMD_READ_STATUS_REG;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_1_LINE;
	sCmd.DummyCycles       = 0;
	sCmd.NbData            = 1;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
			return QA_Fail;
	if (HAL_QSPI_Receive(&m_sHandle, &uReg, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//-----------------------
	//Enable write operations
	sCmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	sCmd.Instruction       = MX25L512_CMD_WRITE_ENABLE;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_NONE;
	sCmd.DummyCycles       = 0;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//-----------------------------------------------------------
	//Configure Automatic Polling mode to wait for write enabling
	sCfg.Match           = MX25L512_SR_WREN;
	sCfg.Mask            = MX25L512_SR_WREN;
	sCfg.MatchMode       = QSPI_MATCH_MODE_AND;
	sCfg.StatusBytesSize = 1;
	sCfg.Interval        = 0x10;
	sCfg.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	sCmd.Instruction    = MX25L512_CMD_READ_STATUS_REG;
	sCmd.DataMode       = QSPI_DATA_1_LINE;
	if (HAL_QSPI_AutoPolling(&m_sHandle, &sCmd, &sCfg, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//--------------------
	//Enable Quad SPI Mode
	sCmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	sCmd.Instruction       = MX25L512_CMD_WRITE_STATUS_CFG_REG;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_1_LINE;
	sCmd.DummyCycles       = 0;
	sCmd.NbData            = 1;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	uReg |= MX25L512_SR_QUADEN;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;
	if (HAL_QSPI_Transmit(&m_sHandle, &uReg, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//---------------------------------------------------
	//40ms write status/configuration register cycle time
	HAL_Delay(40);

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_enterFourByteAddress
//QAD_QuadSPI Tool Method
QA_Result QAD_QuadSPI::imp_enterFourByteAddress(void) {
	QSPI_CommandTypeDef sCmd;

	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_ENTER_4_BYTE_ADDR_MODE;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_NONE;
	sCmd.DummyCycles       = 0;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (imp_writeEnable())
		return QA_Fail;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;
	if (imp_autoPollingMemReady(HAL_QSPI_TIMEOUT_DEFAULT_VALUE))
		return QA_Fail;

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_dummyCyclesCfg
//QAD_QuadSPI Tool Method
QA_Result QAD_QuadSPI::imp_dummyCyclesCfg(void) {
	QSPI_CommandTypeDef sCmd;
	uint8_t uReg[2];

	//--------------------
	//Read status register
	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_READ_STATUS_REG;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_4_LINES;
	sCmd.DummyCycles       = 0;
	sCmd.NbData            = 1;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;
	if (HAL_QSPI_Receive(&m_sHandle, &(uReg[0]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//---------------------------
	//Read configuration register
	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_READ_CFG_REG;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_4_LINES;
	sCmd.DummyCycles       = 0;
	sCmd.NbData            = 1;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;
	if (HAL_QSPI_Receive(&m_sHandle, &(uReg[1]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//-----------------------
	//Enable Write Operations
	if (imp_writeEnable())
		return QA_Fail;

	//---------------------------------------------------
	//Update Configuration Register with new dummy cycles
	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_WRITE_STATUS_CFG_REG;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_4_LINES;
	sCmd.DummyCycles       = 0;
	sCmd.NbData            = 2;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//MX25L512_DUMMY_CYCLES_READ_QUAD = 3 (10 cycles in QPI mode)
	MODIFY_REG(uReg[1], MX25L512_CR_NB_DUMMY, (MX25L512_DUMMY_CYCLES_READ_QUAD << POSITION_VAL(MX25L512_CR_NB_DUMMY)));

	//Configure the write volatile configuration register command
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;
	if (HAL_QSPI_Transmit(&m_sHandle, &(uReg[0]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//40ms - Write Status/Configuration Register Cycle Time
	HAL_Delay(40);

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_enterMemoryQPI
//QAD_QuadSPI Tool Method
QA_Result QAD_QuadSPI::imp_enterMemoryQPI(void) {
	QSPI_CommandTypeDef     sCmd;
	QSPI_AutoPollingTypeDef sCfg;

	//Send QPI Enable Command
	sCmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	sCmd.Instruction       = MX25L512_CMD_ENTER_QUAD_MODE;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_NONE;
	sCmd.DummyCycles       = 0;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//Configure automatic polling mode to wait for QUADEN bit=1 and WIP bit=0
	sCfg.Match           = MX25L512_SR_QUADEN;
	sCfg.Mask            = MX25L512_SR_QUADEN | MX25L512_SR_WIP;
	sCfg.MatchMode       = QSPI_MATCH_MODE_AND;
	sCfg.StatusBytesSize = 1;
	sCfg.Interval        = 0x10;
	sCfg.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	sCmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction     = MX25L512_CMD_READ_STATUS_REG;
	sCmd.DataMode        = QSPI_DATA_4_LINES;
	if (HAL_QSPI_AutoPolling(&m_sHandle, &sCmd, &sCfg, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_exitMemoryQPI
//QAD_QuadSPI Tool Method
QA_Result QAD_QuadSPI::imp_exitMemoryQPI(void) {
	QSPI_CommandTypeDef      sCmd;

	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_EXIT_QUAD_MODE;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_NONE;
	sCmd.DummyCycles       = 0;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_outputDriveStrengthCfg
//QAD_QuadSPI Tool Method
QA_Result QAD_QuadSPI::imp_outputDriveStrengthCfg(void) {
	QSPI_CommandTypeDef sCmd;
	uint8_t uReg[2];

	//--------------------
	//Read Status Register
	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_READ_STATUS_REG;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_4_LINES;
	sCmd.DummyCycles       = 0;
	sCmd.NbData            = 1;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;
	if (HAL_QSPI_Receive(&m_sHandle, &(uReg[0]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//--------------------
	//Read Config Register
	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_READ_CFG_REG;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_4_LINES;
	sCmd.DummyCycles       = 0;
	sCmd.NbData            = 1;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;
	if (HAL_QSPI_Receive(&m_sHandle, &(uReg[1]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//-----------------------
	//Enable write operations
	if (imp_writeEnable() != 0)
		return QA_Fail;

	//----------------------------------------------------------------
	//Update the configuration register with new output drive strength
	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_WRITE_STATUS_CFG_REG;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_4_LINES;
	sCmd.DummyCycles       = 0;
	sCmd.NbData            = 2;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	MODIFY_REG( uReg[1], MX25L512_CR_ODS, (MX25L512_CR_ODS_15 << POSITION_VAL(MX25L512_CR_ODS)));
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;
	if (HAL_QSPI_Transmit(&m_sHandle, &(uReg[0]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_writeEnable
//QAD_QuadSPI Tool Method
QA_Result QAD_QuadSPI::imp_writeEnable(void) {
	QSPI_CommandTypeDef     sCmd;
	QSPI_AutoPollingTypeDef sCfg;

	//-----------------------
	//Enable write operations
	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_WRITE_ENABLE;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_NONE;
	sCmd.DummyCycles       = 0;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&m_sHandle, &sCmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//-----------------------------------------------------------
	//Configure automatic polling mode to wait for write enabling
	sCfg.Match           = MX25L512_SR_WREN;
	sCfg.Mask            = MX25L512_SR_WREN;
	sCfg.MatchMode       = QSPI_MATCH_MODE_AND;
	sCfg.StatusBytesSize = 1;
	sCfg.Interval        = 0x10;
	sCfg.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	sCmd.Instruction     = MX25L512_CMD_READ_STATUS_REG;
	sCmd.DataMode        = QSPI_DATA_4_LINES;
	if (HAL_QSPI_AutoPolling(&m_sHandle, &sCmd, &sCfg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return QA_Fail;

	//Return
	return QA_OK;
}


//QAD_QuadSPI::imp_autoPollingMemReady
//QAD_QuadSPI Tool Method
QA_Result QAD_QuadSPI::imp_autoPollingMemReady(uint32_t uTimeout) {
	QSPI_CommandTypeDef     sCmd;
	QSPI_AutoPollingTypeDef sCfg;

	//Configure automatic polling mode to wait for memory ready
	sCmd.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCmd.Instruction       = MX25L512_CMD_READ_STATUS_REG;
	sCmd.AddressMode       = QSPI_ADDRESS_NONE;
	sCmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCmd.DataMode          = QSPI_DATA_4_LINES;
	sCmd.DummyCycles       = 0;
	sCmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	sCfg.Match            = 0;
	sCfg.Mask             = MX25L512_SR_WIP;
	sCfg.MatchMode        = QSPI_MATCH_MODE_AND;
	sCfg.StatusBytesSize  = 1;
	sCfg.Interval         = 0x10;
	sCfg.AutomaticStop    = QSPI_AUTOMATIC_STOP_ENABLE;

	if (HAL_QSPI_AutoPolling(&m_sHandle, &sCmd, &sCfg, uTimeout) != HAL_OK)
		return QA_Fail;

	//Return No Error
	return QA_OK;
}




