/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Core                                                          */
/*   Role: Setup                                                           */
/*   Filename: setup.hpp                                                   */
/*   Date: 22nd September 2021                                             */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Prevent Recursive Inclusion
#ifndef __SETUP_HPP_
#define __SETUP_HPP_


//Includes
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

  //Result Enum
  //This is used as a return value for multiple functions to show if the function has succeeded or failed
  //An OK result will provide a boolean false, while any error will provide a boolean true
enum QA_Result : uint8_t {
	QA_OK = 0,                   //Function has succeeded
	QA_Fail,                     //Function has failed, with a non-specific error
	QA_Error_PeriphBusy,         //Function has not been able to initialize a particular peripheral as the peripheral is busy
	QA_Error_PeriphNotSupported  //Function has not been able to initialize a particular peripheral as the peripheral doesn't support the required functionality
};


  //Init State Enum
  //This is used to store whether a particular driver or system is initialized or not
enum QA_InitState : uint8_t {QA_NotInitialized = 0, QA_Initialized};


  //Active State Enum
  //This is used to store whether a particular driver or system is currently active or not
enum QA_ActiveState : uint8_t {QA_Inactive = 0, QA_Active};


	//----------------------------------------
	//----------------------------------------
	//----------------------------------------


//-------------------------------
//QAD_IRQHandler_CallbackFunction
//This is a generic callback function pointer definition that is used within drivers / systems.
typedef void (*QAD_IRQHandler_CallbackFunction)(void* pData);


//----------------------------
//QAD_IRQHandler_CallbackClass
//This is a generic class definition to be inherited by a parent class that requires the handler method to be called by drivers / systems.
class QAD_IRQHandler_CallbackClass {
public:

  virtual void handler(void* pData) = 0; //This is a pure virtual function that must be defined within the inheriting/parent class
};


	//----------------------------------------
	//----------------------------------------
	//----------------------------------------


  //----------------
	//GPIO Definitions

#define QAD_USERLED_RED_GPIO_PORT       GPIOJ
#define QAD_USERLED_RED_GPIO_PIN        GPIO_PIN_13  // J13

#define QAD_USERLED_GREEN_GPIO_PORT     GPIOJ
#define QAD_USERLED_GREEN_GPIO_PIN      GPIO_PIN_5   // J5

#define QAD_USERBUTTON_GPIO_PORT        GPIOA
#define QAD_USERBUTTON_GPIO_PIN         GPIO_PIN_0   // A0


	//-----------------
	//UART1 Definitions
  //
  //These are used for the UART/Serial link via STLink on the STM32F769I Discovery board

#define QAD_UART1_TX_PORT     GPIOA
#define QAD_UART1_TX_PIN      GPIO_PIN_9        //A9
#define QAD_UART1_TX_AF       GPIO_AF7_USART1
#define QAD_UART1_RX_PORT     GPIOA
#define QAD_UART1_RX_PIN      GPIO_PIN_10       //A10
#define QAD_UART1_RX_AF       GPIO_AF7_USART1
#define QAD_UART1_BAUDRATE    57600
#define QAD_UART1_TX_FIFOSIZE 256
#define QAD_UART1_RX_FIFOSIZE 256


  //----------------
  //LTDC Definitions
  //
  //These are used to define the GPIO pins used to control resetting of the otm8009a LCD controller, as well as enabling and disabling the LCD backlight

#define QAD_LTDC_BACKLIGHT_PORT           GPIOI
#define QAD_LTDC_BACKLIGHT_PIN            GPIO_PIN_14          //I14

#define QAD_LTDC_RESET_PORT               GPIOJ
#define QAD_LTDC_RESET_PIN                GPIO_PIN_15          //J15


	//-------------------
	//QuadSPI Definitions

#define QAD_QUADSPI_CS_PORT               GPIOB
#define QAD_QUADSPI_CS_PIN                GPIO_PIN_6          //B6
#define QAD_QUADSPI_CS_AF                 GPIO_AF10_QUADSPI

#define QAD_QUADSPI_CLK_PORT              GPIOB
#define QAD_QUADSPI_CLK_PIN               GPIO_PIN_2          //B2
#define QAD_QUADSPI_CLK_AF                GPIO_AF9_QUADSPI

#define QAD_QUADSPI_DATA0_PORT            GPIOC
#define QAD_QUADSPI_DATA0_PIN             GPIO_PIN_9          //C9
#define QAD_QUADSPI_DATA0_AF              GPIO_AF9_QUADSPI

#define QAD_QUADSPI_DATA1_PORT            GPIOC
#define QAD_QUADSPI_DATA1_PIN             GPIO_PIN_10         //C10
#define QAD_QUADSPI_DATA1_AF              GPIO_AF9_QUADSPI

#define QAD_QUADSPI_DATA2_PORT            GPIOE
#define QAD_QUADSPI_DATA2_PIN             GPIO_PIN_2          //E2
#define QAD_QUADSPI_DATA2_AF              GPIO_AF9_QUADSPI

#define QAD_QUADSPI_DATA3_PORT            GPIOD
#define QAD_QUADSPI_DATA3_PIN             GPIO_PIN_13         //D13
#define QAD_QUADSPI_DATA3_AF              GPIO_AF9_QUADSPI


	//-----------------
	//SDMMC Definitions

#define QAD_SDMMC_CARDDETECT_PORT         GPIOI
#define QAD_SDMMC_CARDDETECT_PIN          GPIO_PIN_15        //I15

#define QAD_SDMMC_CLK_PORT                GPIOD
#define QAD_SDMMC_CLK_PIN                 GPIO_PIN_6         //D6
#define QAD_SDMMC_CLK_AF                  GPIO_AF11_SDMMC2

#define QAD_SDMMC_CMD_PORT                GPIOD
#define QAD_SDMMC_CMD_PIN                 GPIO_PIN_7         //D7
#define QAD_SDMMC_CMD_AF                  GPIO_AF11_SDMMC2

#define QAD_SDMMC_DATA0_PORT              GPIOG
#define QAD_SDMMC_DATA0_PIN               GPIO_PIN_9         //G9
#define QAD_SDMMC_DATA0_AF                GPIO_AF11_SDMMC2

#define QAD_SDMMC_DATA1_PORT              GPIOG
#define QAD_SDMMC_DATA1_PIN               GPIO_PIN_10        //G10
#define QAD_SDMMC_DATA1_AF                GPIO_AF11_SDMMC2

#define QAD_SDMMC_DATA2_PORT              GPIOB
#define QAD_SDMMC_DATA2_PIN               GPIO_PIN_3         //B3
#define QAD_SDMMC_DATA2_AF                GPIO_AF10_SDMMC2

#define QAD_SDMMC_DATA3_PORT              GPIOB
#define QAD_SDMMC_DATA3_PIN               GPIO_PIN_4         //B4
#define QAD_SDMMC_DATA3_AF                GPIO_AF10_SDMMC2


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

	//----------------------------
	//Interrupt Request Priorities

#define QAD_IRQPRIORITY_UART1    ((uint8_t) 0x09) //Priority for the TX/RX interrupts for UART1 handler,
                                                  //which is used for serial via STLink on the STM32F769I Discovery board.

#define QAD_IRQPRIORITY_EXTI     ((uint8_t) 0x0A) //Priority to be used by external interrupt handlers. Shared by all external interrupts

#define QAD_IRQPRIORITY_FLASH    ((uint8_t) 0x0E)




//Prevent Recursive Inclusion
#endif /* __SETUP_HPP_ */
