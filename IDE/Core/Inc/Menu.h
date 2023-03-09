/**
 * @author  Bohumir Coufal
 * @email   bohumir.coufal@gmail.com
 * @version v1.0
 * @ide     TrueSTUDIO
 * @license MIT
 * @brief   Library for menu in alarm-clock with 7segment display
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
#ifndef MENU_H_
#define MENU_H_
#include "stm32f0xx_hal.h"

//Definitions
#define MASK_TL 	GPIOB->IDR
#define ON			1
#define	OFF			0
#define TL_FUNK		1
#define TL_UP		2
#define TL_DOWN		3
#define TL_WAKE		4
#define TL_FUNK1S	5
#define TL_UP1S		6
#define TL_DOWN1S	7
#define TL_WAKE1S	8
#define TL_FUNK2S	9
#define TL_UP2S		10
#define TL_DOWN2S	11
#define TL_WAKE2S	12
#define MAXFUNK		10
#define UP			1
#define DOWN		2
#define TIME		0
#define DAYSEC		1
#define DDATE		2
#define DYEAR		3
#define TEMP		4
#define BRIGHT		5
#define CALIB		6
#define ALARM10		7
#define ALARM11		8
#define ALARM12		9
#define NASTAVENIE1	0
#define NASTAVENIE2	1

typedef struct {
	uint8_t ledx;			//which LED is to be lit and which segment is supposed to flash
	uint8_t alarmx;			//alarm on or off
	uint8_t brightx;		//display brightness value
	uint8_t calibrationx;	//calibration value
	uint8_t temperature;	//temperature value
	uint8_t displayx [4];	//four characters that will be explained on the display
} ClockTypeDef;

//Functions
/**
 * @brief	The evaluation feature which button has been pressed is called every 100 milliseconds
 * @param	co: Millisecond counter. From 1 to 20. 10 means 10*100ms = 1s, 20 means 20*100ms = 2s
 * @retval	Function Returns the button value 1-4 if the button 1 to 4 was compressed only once,
 * 			5-8 if it was pressed longer than one second and 9-12 if compressed for more than two seconds
 */
uint8_t funk_press_button(uint8_t co);
/**
 * @brief	Function to operate the pressed button
 * @param	hrtca RTC handle
 * @param	button: number of the pressed button
 * @param	buz: pointer buzzer on or off
 * @param	Budik: pointer of structure with menu informations
 * @param	sTime: pointer of structure with time informations
 * @param	sDate: pointer of structure with date informations
 * @param	sAlarmA: pointer of structure with alarm informations
 * @retval	0 read RTC
 * 			1 do not read RTC
 */
uint8_t funk_evaluate_button (RTC_HandleTypeDef *hrtca, uint8_t button, int *buz, ClockTypeDef *Budik, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate, RTC_AlarmTypeDef *sAlarmA);
/**
 * @brief	Function to display and operate the entire alarm clock
 * @param	hrtc RTC handle
 * @param	Budik: pointer of structure with menu informations
 * @param	sTime: pointer of structure with time informations
 * @param	sDate: pointer of structure with date informations
 * @param	sAlarmA: pointer of structure with alarm informations
 * @retval	nothing
 */
void funk_menu (RTC_HandleTypeDef *hrtca, ClockTypeDef *Budik, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate, RTC_AlarmTypeDef *sAlarmA);
/**
 * @brief	Function to read from RTC registers and write to auxiliary registers
 * @param	sTime: pointer of structure with time informations
 * @param	sDate: pointer of structure with date informations
 * @param	sAlarmA: pointer of structure with alarm informations
 * @retval	nothing
 */
void funk_write_reg(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate, RTC_AlarmTypeDef *sAlarmA);
/**
 * @brief	Function to read from auxiliary registers and write to RTC registers
 * @param	sTime: pointer of structure with time informations
 * @param	sDate: pointer of structure with date informations
 * @param	sAlarmA: pointer of structure with alarm informations
 * @retval	nothing
 */
void funk_read_reg(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate, RTC_AlarmTypeDef *sAlarmA);
/**
 * @brief	Function to increase or reduce value by one
 * @param	1 means increasing 2 means decreasing
 * @retval	nothing
 */
void funk_updown(uint8_t dir);
/**
 * @brief	Function to write the time to RTC registers
 * @param	hrtca RTC handle
 * @param	sTime: pointer of structure with time informations
 * @retval	nothing
 */
void funk_set_time (RTC_HandleTypeDef *hrtca, RTC_TimeTypeDef *sTime);
/**
 * @brief	Function to write the date to RTC registers
 * @param	hrtc RTC handle
 * @param	sDate: pointer of structure with date informations
 * @retval	nothing
 */
void funk_set_date (RTC_HandleTypeDef *hrtca, RTC_DateTypeDef *sDate);
/**
 * @brief	Function to write the alarm to RTC registers
 * @param	hrtc RTC handle
 * @param	sAlarmA: pointer of structure with alarm informations
 * @retval	nothing
 */
void funk_set_alarm (RTC_HandleTypeDef *hrtca, RTC_AlarmTypeDef *sAlarmA);
/**
 * @brief	Function to configure and set the alarm
 * @param	hrtc RTC handle
 * @param	sAlarmA: pointer of structure with alarm informations
 * @retval	nothing
 */
void funk_conf_alarm (RTC_HandleTypeDef *hrtca, RTC_AlarmTypeDef *sAlarmA);
/**
 * @brief	Function to write brightness in RTC RAM
 * @param	hrtc RTC handle
 * @retval	nothing
 */
void funk_set_bright (RTC_HandleTypeDef *hrtca);
/**
 * @brief	Function to write calibration in RTC RAM
 * @param	hrtc RTC handle
 * @retval	nothing
 */
void funk_set_cal (RTC_HandleTypeDef *hrtca);

#endif /* MENU_H_ */
