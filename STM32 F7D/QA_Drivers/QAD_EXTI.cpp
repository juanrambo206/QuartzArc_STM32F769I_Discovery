/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Driver                                                        */
/*   Role: EXTI Driver                                                     */
/*   Filename: QAD_EXTI.cpp                                                */
/*   Date: 5th November 2021                                               */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Includes
#include "QAD_EXTI.hpp"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------


  //-----------------------------------
  //-----------------------------------
  //QAD_EXTI Constructors / Destrucrors

//QAD_EXTI::QAD_EXTI
//QAD_EXTI Constructor
//
//This method will initialize the required GPIO pin with Pull Up and Pull Down resistors disabled, and EXTI mode disabled
//pGPIO - The GPIO port for the required pin. A member of GPIO_TypeDef as defined in stm32f769xx.h
//uPin  - The pin number for the required pin. A member of GPIO_pins_define as defined in stm32f7xx_hal_gpio.h
QAD_EXTI::QAD_EXTI(GPIO_TypeDef* pGPIO, uint16_t uPin) :
  QAD_GPIO_Input(pGPIO, uPin),            //Initialize the inherited QAD_GPIO_Input driver class
	m_eEXTIState(QA_Inactive),              //Initialize the EXTI mode in disabled state
	m_eEdgeType(QAD_EXTI_EdgeType_Rising),  //Initialize the edge type in rising mode
	m_pHandlerFunction(NULL),               //Initialize handler function pointer as NULL
	m_pHandlerClass(NULL) {                 //Initialize handler class pointer as NULL

}


//QAD_EXTI::QAD_EXTI
//QAD_EXTI Constructor
//
//This method will initialize the required GPIO pin with Pull Up/Pull Down resistors and EXTI edge type in specified states
//pGPIO     - The GPIO port for the required pin. A member of GPIO_TypeDef as defined in stm32f769xx.h
//uPin      - The pin number for the required pin. A member of GPIO_pins_define as defined in stm32f7xx_hal_gpio.h
//ePull     - Specifies if pull-up or pull-down resistors are to be used. A member of QAD_GPIO_PullMode as defined in QAD_GPIO.hpp
//eEdgeType - Specifies which edges(s) the interrupt is to be triggered on. A member of QAD_EXTI_EdgeType as defined in QAD_EXTI.hpp
QAD_EXTI::QAD_EXTI(GPIO_TypeDef* pGPIO, uint16_t uPin, QAD_GPIO_PullMode ePull, QAD_EXTI_EdgeType eEdgeType) :
		QAD_GPIO_Input(pGPIO, uPin, ePull), //Initialize the inherited QAD_GPIO_Input driver class
		m_eEXTIState(QA_Inactive),          //Initialize the EXTI mode in disabled state
		m_eEdgeType(eEdgeType),             //Initialize the edge type as specified by eEdgeType
		m_pHandlerFunction(NULL),           //Initialize handler function pointer as NULL
		m_pHandlerClass(NULL) {             //Initialize handler class pointer as NULL

}


//QAD_EXTI::~QAD_EXTI
//QAD_EXTI Destructor
//
//This method is used to disable the external interrupt and deinitialize the associated GPIO upon destruction of the driver class
QAD_EXTI::~QAD_EXTI() {

	//Disables the external interrupt
	disable();

	//Deinitializes the associated GPIO Pin
	periphDeinit();
}


  //------------------------
  //------------------------
  //QAD_EXTI Handler Methods

//QAD_EXTI::handler
//QAD_EXTI Handler Method
//
//This method is to be called by the interrupt handler method from handlers.cpp
void QAD_EXTI::handler(void) {

	//Check if required pin interrupt has been triggered
  if (__HAL_GPIO_EXTI_GET_IT(m_uPin) != RESET) {

  	//Call interrupt handler callback function if one has been assigned
  	if (m_pHandlerFunction)
  		m_pHandlerFunction(NULL);

  	//Call handler method of interrupt handler callback class if one has been assigned
  	if (m_pHandlerClass)
  		m_pHandlerClass->handler(NULL);

  	//Clear pin interrupt
  	__HAL_GPIO_EXTI_CLEAR_IT(m_uPin);
  }
}


  //------------------------
  //------------------------
  //QAD_EXTI Control Methods

//QAD_EXTI::setHandlerFunction
//QAD_EXTI Control Method
//
//Used to set the pointer to the interrupt handler callback function.
//pHandler - A pointer to the handler callback function. The type is defined in setup.hpp
void QAD_EXTI::setHandlerFunction(QAD_IRQHandler_CallbackFunction pHandler) {
  m_pHandlerFunction = pHandler;
}


//QAD_EXTI::setHandlerClass
//QAD_EXTI Control Method
//
//Used to set the pointer to the interrupt handler callback class.
//pHandler - A pointer to the handler callback class. The type is defined in setup.hpp
void QAD_EXTI::setHandlerClass(QAD_IRQHandler_CallbackClass* pHandler) {
  m_pHandlerClass = pHandler;
}


//QAD_EXTI::enable
//QAD_EXTI Control Method
//
//Used to enable external interrupt for the required GPIO pin
void QAD_EXTI::enable(void) {

  //Setup GPIO
  GPIO_InitTypeDef GPIO_Init = {0};
  GPIO_Init.Pin     = m_uPin;
  GPIO_Init.Pull    = m_ePullMode;
  GPIO_Init.Speed   = GPIO_SPEED_FREQ_LOW;
  switch (m_eEdgeType) {
    case (QAD_EXTI_EdgeType_Rising):
    	GPIO_Init.Mode = GPIO_MODE_IT_RISING;
      break;
    case (QAD_EXTI_EdgeType_Falling):
    	GPIO_Init.Mode = GPIO_MODE_IT_FALLING;
      break;
    case (QAD_EXTI_EdgeType_Both):
    	GPIO_Init.Mode = GPIO_MODE_IT_RISING_FALLING;
      break;
  }
  HAL_GPIO_Init(m_pGPIO, &GPIO_Init);

  //Setup Interrupt
  switch (m_uPin) {
    case (GPIO_PIN_0):
    	m_eIRQ = EXTI0_IRQn;
      break;
    case (GPIO_PIN_1):
    	m_eIRQ = EXTI1_IRQn;
      break;
    case (GPIO_PIN_2):
    	m_eIRQ = EXTI2_IRQn;
      break;
    case (GPIO_PIN_3):
    	m_eIRQ = EXTI3_IRQn;
      break;
    case (GPIO_PIN_4):
    	m_eIRQ = EXTI4_IRQn;
      break;
    default:
    	if ((m_uPin >= GPIO_PIN_5) && (m_uPin <= GPIO_PIN_9)) {
    		m_eIRQ = EXTI9_5_IRQn;
    	} else if ((m_uPin >= GPIO_PIN_10) && (m_uPin <= GPIO_PIN_15)) {
    		m_eIRQ = EXTI15_10_IRQn;
    	}
  }

  //Set external interrupt priority. QAD_IRQPRIORITY_EXTI is defined in setup.hpp
  HAL_NVIC_SetPriority(m_eIRQ, QAD_IRQPRIORITY_EXTI, 0);
  HAL_NVIC_EnableIRQ(m_eIRQ);

  //Set State
  m_eEXTIState = QA_Active;
}


//QAD_EXTI::disable
//QAD_EXTI Control Method
//
//Disable external interrupt mode for the required pin, which places the pin back into standard GPIO input mode
void QAD_EXTI::disable(void) {
  if (!m_eEXTIState)
  	return;

  //Disable IRQ
  HAL_NVIC_DisableIRQ(m_eIRQ);

  //Set GPIO back to normal input
  GPIO_InitTypeDef GPIO_Init = {0};
  GPIO_Init.Pin     = m_uPin;
  GPIO_Init.Mode    = GPIO_MODE_INPUT;
  GPIO_Init.Pull    = m_ePullMode;
  GPIO_Init.Speed   = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(m_pGPIO, &GPIO_Init);

  //Set State
  m_eEXTIState = QA_Inactive;
}


//QAD_EXTI::setPullMode
//QAD_EXTI Control Method
//
//Sets new pull mode (Pull-up, Pull-down or No-pull)
//eMode - New pull mode. Member of QAD_GPIO_PullMode enum as defined in QAD_GPIO.hpp
void QAD_EXTI::setPullMode(QAD_GPIO_PullMode eMode) {

	//Disable EXTI if currently enabled
	bool bCurMode = false;
	if (m_eEXTIState) {
		bCurMode = true;
		disable();
	}

	//Store new pull mode
	m_ePullMode = eMode;

	//Reinitialize GPIO peripheral
	periphInit();

	//Renable EXTI if previously enabled
	if (bCurMode)
		enable();
}


