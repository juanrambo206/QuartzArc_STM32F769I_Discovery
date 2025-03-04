/* ----------------------------------------------------------------------- */
/*                                                                         */
/*   Quartz Arc                                                            */
/*                                                                         */
/*   STM32 F769I Discovery                                                 */
/*                                                                         */
/*   System: Systems - LCD Rendering                                       */
/*   Role: LCD Rendering System - Fonts                                    */
/*   Filename: QAS_LCD_Fonts.hpp                                           */
/*   Date: 5th November 2021                                               */
/*   Created By: Benjamin Rosser                                           */
/*                                                                         */
/*   This code is covered by Creative Commons CC-BY-NC-SA license          */
/*   (C) Copyright 2021 Benjamin Rosser                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

//Prevent Recursive Inclusion
#ifndef __QAS_LCD_FONTS_HPP_
#define __QAS_LCD_FONTS_HPP_


//Includes
#include "setup.hpp"

#include "QAD_LTDC.hpp"

#include "QAT_Vector.hpp"

#include <string.h>
#include <vector>


  //------------------------------------------
	//------------------------------------------
	//------------------------------------------


//-----------------------
//QAS_LCD_FONTNAME_LENGTH
//
//Used to determine the maximum length of a name for a font stored in the Font Manager
#define QAS_LCD_FONTNAME_LENGTH  ((uint8_t)48)


//----------------
//QAS_LCD_FontDesc
//
//This structure is used to describe required elements of each character in a particular font/type face
//An array of this structure is required for each font, and is part of the data generated when creating fonts
//using "The Dot Factory" application.
typedef struct {
	uint16_t uWidth;  //The width in pixels of a particular character
	uint16_t uOffset; //The offset in bytes to the start of the bitmap data in the font data array for a particular character
} QAS_LCD_FontDesc;


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------


//------------
//QAS_LCD_Font
//
//This class is used to hold data specific to an individual font.
//The QAS_LCD_FontMgr uses an array (std::vector) of this class to more easily add, remove and select a particular font
class QAS_LCD_Font {
public:

	char m_cName[QAS_LCD_FONTNAME_LENGTH];  //Stores the name of the font.
	                                        //This is used when using the QAS_LCD_FontMgr::setFontByName() method to select a particular font

	QAS_LCD_FontDesc* m_pDesc;              //A pointer to an array of QAS_LCD_FontDesc structures storing the widths and data offsets for each character
	uint8_t*          m_pData;              //A pointer to the bitmap data for characters in the font

	uint16_t          m_uHeight;            //The height in pixels of the the font
	uint16_t          m_uSpaceWidth;        //The width in pixels of the space character for the font
	uint16_t          m_uCharGap;           //The width in pixels of the gap in between each character when rendering strings

public:

	//------------
  //Constructors

	QAS_LCD_Font() = delete;  //Delete default constructor as details for the font need to be supplied upon class creation

	//This is the constructor to be used, which allows the required details of the font to be supplied
	//strName     - a C-style string containing the name of the font (must be no longer than QAS_LCD_FONTNAME_LENGTH, including the null termination character)
	//pDesc       - a pointer to an array of QAS_LCD_FontDesc structures for the font
	//pData       - a pointer to an array of uint8_t values storing the bitmap data for the font
	//uHeight     - the height in pixels of the font
	//uSpaceWidth - the width in pixels of the space character of the font
	//uCharGap    - the width in pixels of the gap between each character when rendering strings of characters
	QAS_LCD_Font(const char* strName, const QAS_LCD_FontDesc* pDesc, const uint8_t* pData, uint16_t& uHeight, uint16_t& uSpaceWidth, uint16_t& uCharGap) :
		m_uHeight(uHeight),
		m_uSpaceWidth(uSpaceWidth),
		m_uCharGap(uCharGap) {

		m_pDesc = (QAS_LCD_FontDesc*)pDesc;
		m_pData = (uint8_t*)pData;

		//Clear the font name character array to null characters prior to copying strName into the array
		//This is partly done to make sure that the below equality operators work correctly in all cases
		for (uint8_t i=0; i<QAS_LCD_FONTNAME_LENGTH; i++) {
			m_cName[i] = 0;
		}
		strcpy(m_cName, strName);
	}


	//---------
	//Operators

	//Equality operator used to check if two fonts are the same. This is done by comparing the font names.
	//other - the QAS_LCD_Font class to compare to the current class
	//Returns true if classes match, or false if they do not
	bool operator==(QAS_LCD_Font& other) {

		//Iterate through each character in the name of the current and other font.
		//If any of the characters do not match then return false.
		//This is done so that if a comparison fails, the algorithm doesn't continue to check characters, which would be a waste of CPU cycles.
		for (uint8_t i=0; i<QAS_LCD_FONTNAME_LENGTH; i++) {
			if (m_cName[i] != other.m_cName[i])
				return false;
		}

		//If all characters have matched then return true
		return true;
	}

	//Equality operator used to check if a font name is the same as the name supplied by strName
	//strName - the C-style string to compare against the font name of the current class
	//Returns true if the names match, or false if they do not
	bool operator==(const char* strName) {

		//Determine the number of characters to iterate through.
		//If the length of strName is smaller than QAS_LCD_FONTNAME_LENGTH then we want to limit the
		//number of iterations to the length of strName in order to avoid a buffer overflows
		uint8_t uLen = strlen(strName);
		uint8_t uIdxLen = (uLen < QAS_LCD_FONTNAME_LENGTH) ? uLen : QAS_LCD_FONTNAME_LENGTH;

		//Iterate through each character
		//If any of the characters do not match then return false.
		//This is done so that if a comparison fails, the algorithm doesn't continue to check characters, which would be a waste of CPU cycles.
		for (uint8_t i=0; i<uIdxLen; i++) {
			if (m_cName[i] != strName[i])
				return false;
		}

		//If all characters have matched then return true
		return true;
	}

	//Assignment operator
	//This is used to copy data from one QAS_LCD_Font class to another
	//This may not be required at this stage with the Font Manager, but is included for future proofing
	QAS_LCD_Font& operator=(QAS_LCD_Font& other) {
		for (uint8_t i=0; i<QAS_LCD_FONTNAME_LENGTH; i++)
			m_cName[i] = other.m_cName[i];

		m_pDesc       = other.m_pDesc;
		m_pData       = other.m_pData;
		m_uHeight     = other.m_uHeight;
		m_uSpaceWidth = other.m_uSpaceWidth;
		m_uCharGap    = other.m_uCharGap;
		return *this;
	}

};


	//------------------------------------------
	//------------------------------------------
	//------------------------------------------


//---------------
//QAS_LCD_FontMgr
//
//System class used for managing the system of available fonts and their associated data, as well as providing methods
//for rendering text.
//This class is intended to be used by the QAS_LCD class
class QAS_LCD_FontMgr {
private:

	std::vector<QAS_LCD_Font*> m_vFonts;       //A vector containing the array of QAS_LCD_Font class pointers to the specific fonts

	int8_t                     m_iCurrentIdx;  //The index of the currently selected font. Will be -1 if no font is selected.
	QAS_LCD_Font*              m_pCurrent;     //A pointer to the QAS_LCD_Font class of the currently selected font

	QAD_LTDC_Buffer*           m_pBuffer;      //A pointer to the currently selected rendering buffer.
	uint16_t                   m_uColor;       //Stores the currently selected rendering color

public:

	//--------------------------
	//Constructors / Destructors

	//Default constructor, which clears all data to default on class construction
	QAS_LCD_FontMgr() :
		m_iCurrentIdx(-1),
		m_pCurrent(NULL),
		m_pBuffer(NULL),
		m_uColor(0x0000) {
		m_vFonts.clear();
	}

	//Class destructor which iterates through all fonts stored in m_vFonts vector and
	//deletes/deallocates them to prevent memory leaks when class is destroyed
	~QAS_LCD_FontMgr() {
		for (uint8_t i=0; i<m_vFonts.size(); i++) {
			delete m_vFonts[i];
		}
	}


	//NOTE: See QAS_LCD_Fonts.cpp for details of the below methods

	//------------------
	//Management Methods

	void add(const char* strName, const QAS_LCD_FontDesc* pDesc, const uint8_t* pData, uint16_t uHeight, uint16_t uSpaceWidth, uint16_t uCharGap);
	void remove(const char* strName);
	void clear(void);
	int8_t find(const char* strName);
	void setFontByName(const char* strName);
	void setFontByIndex(uint8_t uIdx);


	//------------
	//Data Methods

	uint8_t getHeight(void) const;
	uint8_t getSpaceWidth(void) const;
	uint8_t getCharGap(void) const;
	uint16_t getCharWidth(char ch);
	uint16_t getStringWidth(const char* str);

	//-----------------
	//Rendering Methods

	void setDrawBuffer(QAD_LTDC_Buffer* pBuffer);
	void setDrawColor(uint16_t uColor);

	void drawChar(QAT_Vector2_16 cPos, char& ch);
	void drawStrL(QAT_Vector2_16 cPos, const char* str);
	void drawStrC(QAT_Vector2_16 cPos, const char* str);
	void drawStrR(QAT_Vector2_16 cPos, const char* str);

private:

	//-----------------
	//Rendering Methods

	void drawCharP(QAT_Vector2_16 cPos, char ch);

};


//Prevent Recursive Inclusion
#endif /* __QAS_LCD_FONTS_HPP_ */














