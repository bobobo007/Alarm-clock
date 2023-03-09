/**
 * @author  Bohumir Coufal
 * @email   bohumir.coufal@gmail.com
 * @version v1.0
 * @ide     TrueSTUDIO
 * @license MIT
 * @brief   Library for 7segment display
 *
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2019 Bohumir Coufal

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stm32f0xx_hal.h"

//Definitions
#define ON		1
#define	OFF		0

//Functions
/**
 * @brief	Function to convert a character to a 7segment code
 * @param	cha: character that can be shown on the display
 * @retval	code for 7-segment display
 */
uint8_t funk_segcod(unsigned char cha);

/**
 * @brief	Function for displaying one character on one segment
 * @param	cha: character that can be shown on the display
 * @param	dig: display position 0 to 3
 * @retval	nothing
 */
void funk_write_char (unsigned char cha, uint8_t dig);

/**
 * @brief	Function to display all characters on the display
 * @param	*cha: pointer to the characters to be shown on the display
 * @param	ld1: information on which LEDs should be lit.
 * @param	co: the segment counter, 0 to 3 shows the individual segments, 4 does not light anything
 * 			this regulates the brightness of the display
 * @param	bl: 0 do not flash 1 flash
 * @retval	nothing
 */
void funk_write_all(unsigned char* cha, uint8_t ld1, uint8_t co, uint8_t bl);

/**
 * @brief	Function to convert a number to a 7-segment code
 * @param	*cha: pointer to the characters where the codes are to be stored
 * @param	num: the two-digit number to be converted
 * @retval	nothing
 */
void funk_displnum (unsigned char* cha, uint8_t num);

/**
 * @brief	Function to convert day of the week to 7-segment code
 * @param	*cha: pointer to the characters where the codes are to be stored
 * @param	wday: day of the week to be converted
 * @retval	nothing
 */
void funk_displwday(unsigned char* cha, uint8_t wday);

#endif /* DISPLAY_H_ */
