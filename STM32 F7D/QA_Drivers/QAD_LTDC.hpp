/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Driver                                                        */
/*   Role: LTDC Driver                                                     */
/*   Filename: QAD_LTDC.hpp                                                */
/*   Date: 3rd October 2021                                                */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Prevent Recursive Inclusion
#ifndef __QAD_LTDC_HPP_
#define __QAD_LTDC_HPP_


//Includes
#include "setup.hpp"

#include "otm8009a.h"

#include "QAT_Pixel.hpp"


  //NOTE:
  //LTDC Driver uses a double-buffered rendering approach where rendering of elements is done to the "back-buffer", while the
  //"front-buffer" is the data currently being displayed on the LCD. This prevents the rendering of elements being visible on the
  //LCD display while the rendering is occurring, which provides a smoother and more professional look and feel to the LCD display.
  //
  //The LTDC Driver also makes use of two layers, allowing of elements rendered to layer 1 to be composited over the top of elements
  //rendered to layer 0, with transparency of elements being variable per pixel via the pixel's Alpha value.
  //
  //Each of the two layers has it's own independent set of double-buffers which can be used independently, allowing more flexibility
  //For example, a background image or user interface elements that dont change often can be rendered to layer 0 at a slower update/frame rate,
  //while foreground elements that need to be updated more often can be rendered to layer1 at a faster update/frame rate,
  //providing potential performance benefits.


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

//--------------------------------------
//QAD_LTDC Defines for LCD panel details
#define QAD_LTDC_WIDTH       800                                                //Width in pixels of LCD panel
#define QAD_LTDC_HEIGHT      480                                                //Height in pixels of LCD panel
#define QAD_LTDC_PIXELCOUNT  (QAD_LTDC_WIDTH * QAD_LTDC_HEIGHT)                 //Total number of pixels in LCD panel

#define QAD_LTDC_BUFFERSIZE  (QAD_LTDC_PIXELCOUNT * sizeof(QAT_Pixel_ARGB4444)) //Size of a single display buffer at 16bits per pixel


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

  //-----------------------
  //Frame Buffer Structures

//-----------------
//QAD_LTDC_LayerIdx
//
//Used to select which LTDC layer is being accessed
enum QAD_LTDC_LayerIdx : uint8_t {
	QAD_LTDC_Layer0 = 0, //LTDC Layer 0
	QAD_LTDC_Layer1      //LTDC Layer 1
};


//---------------
//QAD_LTDC_BufferIdx
//
//As both Layer 0 and Layer 1 are double-buffered, this enum is used to select which buffer is being accessed
enum QAD_LTDC_BufferIdx : uint8_t {
	QAD_LTDC_Buffer0 = 0,  //Buffer 0
	QAD_LTDC_Buffer1       //Buffer 1
};


//---------------
//QAD_LTDC_Buffer
//
//Structure used to represent a single 16bit per pixel buffer for the 800x480 LCD panel
typedef struct {
	QAT_Pixel_ARGB4444 pixel[QAD_LTDC_PIXELCOUNT]; //Array of QAT_Pixel_ARGB4444 classes to represent 16bit per pixel data within buffer
} QAD_LTDC_Buffer;


//---------------------
//QAD_LTDC_DoubleBuffer
//
//Structure used to represent double-buffer for a single LTDC layer
typedef struct {
	QAD_LTDC_Buffer buffers[2];  //Array of two QAD_LTDC_Buffer structures
} QAD_LTDC_DoubleBuffer;



	//------------------------------------------
	//------------------------------------------
	//------------------------------------------

//--------
//QAD_LTDC
//
//Singleton class
//Driver class for LCD-TFT Display Controller (LTDC) and DSI host (DSI) peripherals, which is used to provide an interface
//to the MB1166 WVGA LCD panel and OTM8009A display controller.
//This is setup as a singleton class due to only one LTDC peripheral being available on the STM32F769I Discovery
//board, meaning that potential conflicts are prevented by not allowing more than one instance of the class to exist.
class QAD_LTDC {
private:

	QA_InitState m_eInitState;                 //Stores whether the driver is currently initialized. Member of QA_InitState enum defined in setup.hpp

	const uint32_t m_uLCDClockRate    = 27429; //LCD Interface clock rate
	const uint32_t m_uLCDLaneByteClk  = 62500; //LCD Lane byte clock rate in kHz
	const uint32_t m_uLCDClkDivisor   = 15625; //LCD clock divisor

	DSI_HandleTypeDef  m_sDSIHandle;           //Handle used by HAL functions to access the DSI peripheral
	DSI_VidCfgTypeDef  m_sDSICfgHandle;        //Handle used by HAL functions to configure settings for DSI peripheral
	LTDC_HandleTypeDef m_sLTDCHandle;          //Handle used by HAL functions to access the LTDC peripheral

	void*              m_pLayer0Ptr;           //Pointer to double buffer for LTDC layer 0
	void*              m_pLayer1Ptr;           //Pointer to double buffer for LTDC layer 1

  QAD_LTDC_BufferIdx m_eLayer0Back;          //Index of current Layer 0 back buffer (buffer to be rendered to)
  QAD_LTDC_BufferIdx m_eLayer0Front;         //Index to current Layer 0 front buffer (buffer currently being displayed)
  QAD_LTDC_BufferIdx m_eLayer1Back;          //Index of current Layer 1 back buffer (buffer to be rendered to)
  QAD_LTDC_BufferIdx m_eLayer1Front;         //Index of current Layer 1 front buffer (buffer currently being displayed)


  //------------
  //Constructors

  //As this is a private method in a singleton class, this method will be called the first time the class's get() method is called
	QAD_LTDC() :
	  m_eInitState(QA_NotInitialized) {}

public:

	//----------------------------------------------------------------------------------
	//Delete the copy constructor and assignment operator due to being a singleton class
	QAD_LTDC(const QAD_LTDC& other) = delete;
	QAD_LTDC& operator=(const QAD_LTDC& other) = delete;


	//-----------------
	//Singleton Methods
	//
	//Used to retrieve a reference to the singleton class
	static QAD_LTDC& get(void) {
		static QAD_LTDC instance;
		return instance;
	}


	//----------------------
	//Initialization Methods

	//Used to initialize the LTDC and DSI peripheral, the OTM8009A display controller and the required reset and backlight control GPIO pins
	//Returns QA_OK if initialization is successful, or QA_Fail if initialization fails
	static QA_Result init(void) {
		return get().imp_init();
	}

	//Used to deinitialize the LTDC and DSI peripheral, and the OTM8009A display controller
	static void deinit(void) {
		get().imp_deinit();
	}


	//---------------------
	//Communication Methods

	//Used to allow OTM8009A driver (in otm8009a.h file) to communicate with OTM8009A display controller via DSI
	//uNumParams - The number of parameters being supplied
	//pParams    - Pointer to the list of parameters to be supplied
	static void dsi_IO_WriteCmd(uint32_t uNumParams, uint8_t* pParams) {
		get().imp_dsi_IO_WriteCmd(uNumParams, pParams);
	}


	//---------------
	//Control Methods

	//Used to turn LCD Panel on
	static void lcdOn(void) {
		get().imp_lcdOn();
	}

	//Used to turn LCD Panel off
	static void lcdOff(void) {
		get().imp_lcdOff();
	}

	//Used to set the Brightness of the LCD Panel
	//uBrightness - the brightness as a percentage (a value between 0 and 100)
	static void setBrightness(uint8_t uBrightness) {
		get().imp_setBrightness(uBrightness);
	}


	//---------------------
	//Layer Control Methods

	//Used to flip the front and back buffer for layer 0
	//To be used at the end of rendering visual elements to layer 0 in order for them to become visible on the LCD display
	static void flipLayer0Buffers(void) {
		get().imp_flipLayer0Buffers();
	}

	//Used to flip the front and back buffer for layer 1
	//To be used at the end of rendering visual elements to layer 1 in order for them to become visible on the LCD display
	static void flipLayer1Buffers(void) {
		get().imp_flipLayer1Buffers();
	}


	//------------------
	//Layer Data Methods

	//Returns a void pointer to the back/offscreen buffer to be rendered to for layer 0
	static void* getLayer0BackBufferPtr(void) {
		return get().imp_getLayer0BackBufferPtr();
	}

	//Returns a pointer to a QAD_LTDC_Buffer structure for the back/offscreen buffer to be rendered to for layer 0
	static QAD_LTDC_Buffer* getLayer0BackBuffer(void) {
		return get().imp_getLayer0BackBuffer();
	}

	//Returns a void pointer to the back/offscreen buffer to be rendered to for layer 1
	static void* getLayer1BackBufferPtr(void) {
		return get().imp_getLayer1BackBufferPtr();
	}

	//Returns a pointer to a QAD_LTDC_Buffer structure for the back/offscreen buffer to be rendered to for layer 1
	static QAD_LTDC_Buffer* getLayer1BackBuffer(void) {
		return get().imp_getLayer1BackBuffer();
	}


private:

	//NOTE: Please see QAD_LTDC.cpp for details of the following methods

	//----------------------
	//Initialization Methods

	QA_Result imp_init(void);
	void imp_deinit(void);


	//-------------------
	//DSI Command Methods

	void imp_dsi_IO_WriteCmd(uint32_t uNumParams, uint8_t* pParams);


	//---------------
	//Control Methods

	void imp_lcdOn(void);
	void imp_lcdOff(void);
	void imp_setBrightness(uint8_t uBrightness);


	//---------------------
	//Layer Control Methods

	void imp_flipLayer0Buffers(void);
	void imp_flipLayer1Buffers(void);


	//------------------
	//Layer Data Methods

	void* imp_getLayer0BackBufferPtr(void);
	QAD_LTDC_Buffer* imp_getLayer0BackBuffer(void);
	void* imp_getLayer1BackBufferPtr(void);
	QAD_LTDC_Buffer* imp_getLayer1BackBuffer(void);

};



//DSI Command Method
void DSI_IO_WriteCmd(uint32_t NbrParam, uint8_t* pParams);


//Prevent Recursive Inclusion
#endif /* __QAD_LTDC_HPP */






