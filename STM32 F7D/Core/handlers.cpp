/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Core                                                          */
/*   Role: Exception and Interrupt Handlers                                */
/*   Filename: handlers.cpp                                                */
/*   Date: 22nd September 2021                                             */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */


//Includes
#include "handlers.hpp"

#include "QAS_Serial_Dev_UART.hpp"
#include "QAD_GPIO.hpp"


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

extern QAS_Serial_Dev_UART* UART_STLink;

extern QAD_GPIO_Output* GPIO_UserLED_Red;
extern QAD_GPIO_Output* GPIO_UserLED_Green;


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

  //---------------------------
  //---------------------------
  //Exception Handler Functions

//NMI_Handler
//Exception Handler Function
void  NMI_Handler(void) {

}


//HardFault_Handler
//Exception Handler Function
void  HardFault_Handler(void) {
	GPIO_UserLED_Red->on();
	GPIO_UserLED_Green->on();
  while(1) {}
}


//MemManage_Handler
//Exception Handler Function
void  MemManage_Handler(void) {
	GPIO_UserLED_Red->on();
	GPIO_UserLED_Green->on();
  while(1) {}
}


//BusFault_Handler
//Exception Handler Function
void  BusFault_Handler(void) {
	GPIO_UserLED_Red->on();
	GPIO_UserLED_Green->on();
  while(1) {}
}


//UsageFault_Handler
//Exception Handler Function
void  UsageFault_Handler(void) {
	GPIO_UserLED_Red->on();
	GPIO_UserLED_Green->on();
  while(1) {}
}


//SVC_Handler
//Exception Handler Function
void  SVC_Handler(void) {

}


//DebugMon_Handler
//Exception Handler Function
void  DebugMon_Handler(void) {

}


//PendSV_Handler
//Exception Handler Function
void  PendSV_Handler(void) {

}


//SysTick_Handler
//Exception Handler Function
void  SysTick_Handler(void) {
  HAL_IncTick();
}


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

  //---------------------------
  //---------------------------
  //Interrupt Handler Functions


//USART1_IRQHandler
//Interrupt Handler Function
void USART1_IRQHandler(void) {
	UART_STLink->handler(NULL);
}












