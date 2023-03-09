/**
 * |----------------------------------------------------------------------
 * | Copyright (c) 2018 Bohumir Coufal
 * |
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software,
 * | and to permit persons to whom the Software is furnished to do so,
 * | subject to the following conditions:
 * |
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * |
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */

#include "main.h"
#include "Menu.h"
#include "Display.h"
#include "defines.h"

uint8_t buttmask = 0;				//mask of buttons
uint8_t prev = 0;					//the previous mask of buttons
uint8_t flagbt = 0;					//button flag
uint8_t flagca = 0;					//calibration flag
uint8_t flagbr = 0;					//brightness flag
uint8_t flag = ON;					//flag
uint8_t mod = 0;					//views mode
uint8_t set = 0;					//setting
uint8_t setm = 0;					//setting counter
uint8_t flagbt01 = OFF;				//Flag BT1
uint8_t flagbt02 = OFF;				//Flag BT2
uint8_t flagbt03 = OFF;				//Flag BT3
uint8_t flagbt04 = OFF;				//Flag BT4

struct stime {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t wday;
};
struct stime al1, tm;
uint8_t dat;
uint8_t mon;
uint8_t yea;
uint8_t cal;
uint8_t calp;
uint8_t bri;

uint8_t setmax [10] =  {1,0,1,0,0,0,0,0,1,0};//How many values will be set in a given mode, for example, in Mode 1 Daysc, it will only set seconds = 0

//Functions

//The evaluation feature which button has been pressed is called every 100 milliseconds
// *************************************************
uint8_t funk_press_button(uint8_t co)
{
	uint8_t butt = 0;
	buttmask = MASK_TL & 0x000000F0;										//Some button has been pressed?
	if ((buttmask == prev))													//The same button was already pressed once?
	{
		if (!(HAL_GPIO_ReadPin(BT1_GPIO_Port, BT1_Pin))) butt = 1;
		if (!(HAL_GPIO_ReadPin(BT2_GPIO_Port, BT2_Pin))) butt = 2;
		if (!(HAL_GPIO_ReadPin(BT3_GPIO_Port, BT3_Pin))) butt = 3;
		if (!(HAL_GPIO_ReadPin(BT4_GPIO_Port, BT4_Pin))) butt = 4;
		if(butt != 0)
		{
			if (flagbt == OFF)												//If the button is pressed first time, set the flag
			{
				flagbt = butt;
			}
			else
			{
				if ((co == 10) && (flagbt == butt))				//If the button is compressed longer than 1 seconds increase the button value by 4
				{
					butt = butt + 4;
					flagbt = flagbt + 4;
				}
				else
				{
					if ((co == 20) && (flagbt == (butt + 4)))	//If the button is compressed longer than 2 seconds increase the button value by 8
					{
						butt = butt + 8;
						flagbt = flagbt + 4;
					}
					else
					{
						if (flagbt == (butt + 4) || (butt + 8)) butt = flagbt;	//If the button has been compressed more than 1s leave the value from flagbt
					}
				}
			}
		}
		else flagbt = OFF;		//The button was not compressed - reset the flag
	}
	prev = buttmask;
	return butt;				//Function Returns the button value 1-4 if the button 1 to 4 was compressed only once,
								//5-8 if it was pressed longer than one second and 9-12 if compressed for more than two seconds
}

//Function to operate the pressed button
// *******************************************
uint8_t funk_evaluate_button (RTC_HandleTypeDef *hrtca, uint8_t button, int *buz, ClockTypeDef *Clock1, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate, RTC_AlarmTypeDef *sAlarmA)
{
	uint8_t wr = 0;
	switch (button)
	{
		case TL_FUNK:												//Function button has been pressed
			if (flagbt01 == OFF) flagbt01 = TL_FUNK;				//if the Functions button has not been previously pressed, set the flag
		break;
		case TL_FUNK2S:												//Function button has been pressed longer than 2 seconds
			{
				flagbt01 = TL_FUNK2S;
				set = 1;
				funk_write_reg(&sTime[0], &sDate[0], &sAlarmA[0]);	//Enrollment of time, date and alarm to auxiliary registers
			}
		break;
		case TL_UP:													//Up button has been pressed
			if (flagbt02 == OFF)
				{
					flagbt02 = ON;
					funk_updown (UP);								//If the UP button has been pressed less than 1s "funk_updown" function is called only once
				}
		break;
		case TL_UP1S:												//Up button has been pressed longer than 1 seconds
			funk_updown (UP);										//If the UP button has been pressed longer than 1s "funk_updown" function is called always
		break;
		case TL_UP2S:												//Up button has been pressed longer than 2 seconds
			funk_updown (UP);										//If the UP button has been pressed longer than 2s "funk_updown" function is called always
		break;
		case TL_DOWN:												//Down button has been pressed
			if (flagbt03 == OFF)
				{
					flagbt03 = ON;
					funk_updown (DOWN);								//If the DOWN button has been pressed less than 1s "funk_updown" function is called only once
				}
		break;
		case TL_DOWN1S:												//Down button has been pressed longer than 1 seconds
			funk_updown (DOWN);										//If the DOWN button has been pressed longer than 1s "funk_updown" function is called always
		break;
		case TL_DOWN2S:												//Down button has been pressed longer than 2 seconds
			funk_updown (DOWN);										//If the DOWN button has been pressed longer than 2s "funk_updown" function is called always
		break;
		case TL_WAKE:												//Alarm button has been pressed
			*buz = OFF;												//Switch off the Buzzer
		break;
		case TL_WAKE2S:												//Alarm button has been pressed longer than 2 seconds
			if (flagbt04 == OFF)
			{
				flagbt04 = ON;
				if(*(&Clock1[0].alarmx) == 0)
				{
					*(&Clock1[0].alarmx) = 0b00000100;				//alarm on = 0b00000100, alarm off = 0b00000000
					funk_write_reg(&sTime[0], &sDate[0], &sAlarmA[0]);
					funk_conf_alarm(&hrtca[0], &sAlarmA[0]);
				}
				else
				{
					*(&Clock1[0].alarmx) = 0b00000000;
					if (HAL_RTC_DeactivateAlarm(&hrtca[0], RTC_ALARM_A) != HAL_OK)
					{
						Error_Handler();
					}
				}
				HAL_RTCEx_BKUPWrite(&hrtca[0], RTC_BKP_DR1, *(&Clock1[0].alarmx));
			}
		break;
		case OFF:													//No button has been pressed
			if(flagbt01 == TL_FUNK)
			{
				if(set == 0)
				{
					mod ++;											//Increasing function by one
					if (mod == MAXFUNK) mod = 0;					//If the function has reached the maximum put it to zero
				}
				else
				{
					if(setm == setmax[mod])
					{
						if (!((mod == ALARM11)||(mod == DDATE)))	//If set the alarm do not turn off the setup
						{
							set = 0;
							funk_read_reg(&sTime[0], &sDate[0], &sAlarmA[0]);	//Reading of time, date and alarm from auxiliary registers
							wr = mod+1;
						}
						else mod ++;								//Increasing function by one
						setm = 0;
					}
					else setm++;
					if (mod == MAXFUNK) mod = 0;					//If the function has reached the maximum put it to zero
				}
			}
			flagbt01 = OFF;
			flagbt02 = OFF;
			flagbt03 = OFF;
			flagbt04 = OFF;
		break;
	}
	return wr;
}

//Function to display and operate the entire alarm clock
// *****************************************************
void funk_menu (RTC_HandleTypeDef *hrtca, ClockTypeDef *Clock1, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate, RTC_AlarmTypeDef *sAlarmA)
{
	if (set == 0)																	//setting off
	{
		switch (mod)
		{
			case TIME:
				funk_displnum (&Clock1[0].displayx[0],*(&sTime[0].Hours));			//viewing time
				funk_displnum (&Clock1[0].displayx[2],*(&sTime[0].Minutes));
				*(&Clock1[0].ledx) = 0b0000011 | *(&Clock1[0].alarmx);
				funk_set_time(&hrtca[0], &sTime[0]);
			break;
			case DAYSEC:
				funk_displwday (&Clock1[0].displayx[0],*(&sDate[0].WeekDay));		//viewing day of the week
				funk_displnum (&Clock1[0].displayx[2],*(&sTime[0].Seconds));		//viewing seconds
				*(&Clock1[0].ledx) = 0b00000000 | *(&Clock1[0].alarmx);
				funk_set_date(&hrtca[0], &sDate[0]);
			break;
			case DDATE:
				funk_displnum (&Clock1[0].displayx[0],*(&sDate[0].Date));			//viewing date
				funk_displnum (&Clock1[0].displayx[2],*(&sDate[0].Month));
				*(&Clock1[0].ledx) = 0b00001010 | *(&Clock1[0].alarmx);
			break;
			case DYEAR:
				funk_displnum (&Clock1[0].displayx[0],20);							//viewing year
				funk_displnum (&Clock1[0].displayx[2],*(&sDate[0].Year));
				*(&Clock1[0].ledx) = 0b00000000 | *(&Clock1[0].alarmx);
				funk_set_date(&hrtca[0], &sDate[0]);
			break;
			case TEMP:
				funk_displnum (&Clock1[0].displayx[0],*(&Clock1[0].temperature));	//viewing temperature
				*(&Clock1[0].displayx[2]) = 'g';
				*(&Clock1[0].displayx[3]) = 'C';
				*(&Clock1[0].ledx) = 0b00000000 | *(&Clock1[0].alarmx);
			break;
			case BRIGHT:
				if(flagbr == OFF)
				{
					bri = *(&Clock1[0].brightx);
					flagbr = ON;
				}
#if defined SLOVAK
				*(&Clock1[0].displayx[0]) = 'J';									//viewing brightness
				*(&Clock1[0].displayx[1]) = 'A';
#endif
#if defined ENGLISH
				*(&Clock1[0].displayx[0]) = 'B';									//viewing brightness
				*(&Clock1[0].displayx[1]) = 'R';
#endif
				funk_displnum (&Clock1[0].displayx[2],bri);
				*(&Clock1[0].ledx) = 0b00000000 | *(&Clock1[0].alarmx);
				funk_set_bright(&hrtca[0]);
			break;
			case CALIB:
				flagbr = OFF;
				if(flagca == OFF)
				{
					cal = *(&Clock1[0].calibrationx);
					flagca = ON;
				}
				*(&Clock1[0].displayx[0]) = 'C';									//viewing calibration
				*(&Clock1[0].displayx[1]) = 'A';
				funk_displnum(&Clock1[0].displayx[2],cal);
				*(&Clock1[0].ledx) = 0b00000000 | *(&Clock1[0].alarmx);
				funk_set_cal(&hrtca[0]);
			break;
			case ALARM10:
				flagca = OFF;
				*(&Clock1[0].displayx[0]) = 'A';									//viewing alarm
				*(&Clock1[0].displayx[1]) = 'L';
				funk_displnum (&Clock1[0].displayx[2],1);
				*(&Clock1[0].ledx) = 0b00000000 | *(&Clock1[0].alarmx);
			break;
			case ALARM11:
				funk_displnum(&Clock1[0].displayx[0],*(&sAlarmA[0].AlarmTime.Hours));
				funk_displnum(&Clock1[0].displayx[2],*(&sAlarmA[0].AlarmTime.Minutes));
				*(&Clock1[0].ledx) = 0b00000011 | *(&Clock1[0].alarmx);
			break;
			case ALARM12:
				funk_displwday(&Clock1[0].displayx[0],*(&sAlarmA[0].AlarmDateWeekDay));
				*(&Clock1[0].ledx) = 0b00000000 | *(&Clock1[0].alarmx);
				funk_set_alarm(&hrtca[0], &sAlarmA[0]);
				if(!*(&Clock1[0].alarmx))
				{
					if (HAL_RTC_DeactivateAlarm(&hrtca[0], RTC_ALARM_A) != HAL_OK)
					{
						Error_Handler();
					}
				}
			break;
		}
	}
	else																				//seting on
	{
		switch (mod)
		{
			case TIME:
				funk_displnum (&Clock1[0].displayx[0],tm.hour);							//viewing time
				funk_displnum (&Clock1[0].displayx[2],tm.minute);
				if(setm == 0)*(&Clock1[0].ledx) = 0b00110011;
				else *(&Clock1[0].ledx) = 0b11000011 | *(&Clock1[0].alarmx);
				flag = OFF;
			break;
			case DAYSEC:
				funk_displwday (&Clock1[0].displayx[0],tm.wday);						//viewing day of the week
				funk_displnum (&Clock1[0].displayx[2],tm.second);						//viewing seconds
				if(setm == 0)*(&Clock1[0].ledx) = 0b00110000 | *(&Clock1[0].alarmx);
				flag = OFF;
			break;
			case DDATE:
				funk_displnum (&Clock1[0].displayx[0],dat);								//viewing date
				funk_displnum (&Clock1[0].displayx[2],mon);
				if(setm == 0)*(&Clock1[0].ledx) = 0b00111010 | *(&Clock1[0].alarmx);
				else *(&Clock1[0].ledx) = 0b11001010 | *(&Clock1[0].alarmx);
				flag = OFF;
			break;
			case DYEAR:
				funk_displnum (&Clock1[0].displayx[0],20);								//viewing year
				funk_displnum (&Clock1[0].displayx[2],yea);
				*(&Clock1[0].ledx) = 0b11110000 | *(&Clock1[0].alarmx);
				flag = OFF;
			break;
			case BRIGHT:
#if defined SLOVAK
				*(&Clock1[0].displayx[0]) = 'J';									//viewing brightness
				*(&Clock1[0].displayx[1]) = 'A';
#endif
#if defined ENGLISH
				*(&Clock1[0].displayx[0]) = 'B';									//viewing brightness
				*(&Clock1[0].displayx[1]) = 'R';
#endif
				funk_displnum (&Clock1[0].displayx[2],bri);
				if(setm == 0)*(&Clock1[0].ledx) = 0b11000000 | *(&Clock1[0].alarmx);
				*(&Clock1[0].brightx) = bri;
				flag = OFF;
			break;
			case CALIB:
				*(&Clock1[0].displayx[0]) = 'C';										//viewing calibration
				*(&Clock1[0].displayx[1]) = 'A';
				funk_displnum(&Clock1[0].displayx[2],cal);
				if(setm == 0)*(&Clock1[0].ledx) = 0b11000000 | *(&Clock1[0].alarmx);
				*(&Clock1[0].calibrationx) = cal;
				flag = OFF;
			break;
			case ALARM10:
				*(&Clock1[0].displayx[0]) = 'A';										//viewing alarm
				*(&Clock1[0].displayx[1]) = 'L';
				funk_displnum (&Clock1[0].displayx[2],1);
				if(setm == 0)*(&Clock1[0].ledx) = 0b00000000 | *(&Clock1[0].alarmx);
				else *(&Clock1[0].ledx) = 0b11000000 | *(&Clock1[0].alarmx);
			break;
			case ALARM11:
				funk_displnum(&Clock1[0].displayx[0],al1.hour);
				funk_displnum(&Clock1[0].displayx[2],al1.minute);
				if(setm == 0)*(&Clock1[0].ledx) = 0b00110011 | *(&Clock1[0].alarmx);
				else *(&Clock1[0].ledx) = 0b11000000 | *(&Clock1[0].alarmx);
				flag = OFF;
				break;
			case ALARM12:
				funk_displwday(&Clock1[0].displayx[0],al1.wday);
				if(setm == 0)*(&Clock1[0].ledx) = 0b11110000 | *(&Clock1[0].alarmx);
				flag = OFF;
			break;
		}
	}
}

//Function to read from RTC registers and write to auxiliary registers
//********************************************************************
void funk_write_reg(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate, RTC_AlarmTypeDef *sAlarmA)
{
	tm.hour = *(&sTime[0].Hours);
	tm.minute = *(&sTime[0].Minutes);
	tm.second = *(&sTime[0].Seconds);
	tm.wday = *(&sDate[0].WeekDay);
	dat = *(&sDate[0].Date);
	mon = *(&sDate[0].Month);
	yea = *(&sDate[0].Year);
	al1.hour = *(&sAlarmA[0].AlarmTime.Hours);
	al1.minute = *(&sAlarmA[0].AlarmTime.Minutes);
	al1.wday = *(&sAlarmA[0].AlarmDateWeekDay);
}

//Function to read from auxiliary registers and write to RTC registers
//********************************************************************
void funk_read_reg(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate, RTC_AlarmTypeDef *sAlarmA)
{
	*(&sTime[0].Hours) = tm.hour;
	*(&sTime[0].Minutes) = tm.minute;
	*(&sTime[0].Seconds) = tm.second;
	*(&sDate[0].WeekDay) = tm.wday;
	*(&sDate[0].Date) = dat;
	*(&sDate[0].Month) = mon;
	*(&sDate[0].Year) = yea;
	*(&sAlarmA[0].AlarmTime.Hours) = al1.hour;
	*(&sAlarmA[0].AlarmTime.Minutes) = al1.minute;
	*(&sAlarmA[0].AlarmDateWeekDay) = al1.wday;
}

//Function to increase or reduce value by one
// ******************************************
void funk_updown(uint8_t dir)
{
	uint8_t idata;
	uint8_t jdata = 1;
	if(set)
	{
		if (((mod == TIME) || (mod == ALARM11)) && (setm == NASTAVENIE1))					//will set the hour
		{
			idata = 23;
			jdata = 0;
			if(mod == TIME)
			{
				if (dir == DOWN)
				{
					if(tm.hour == jdata) tm.hour = idata;
					else tm.hour--;
				}
				else
				{
					if(tm.hour == idata) tm.hour = jdata;
					else tm.hour++;
				}
			}
			if(mod == ALARM11)
			{
				if (dir == DOWN)
				{
					if(al1.hour == jdata) al1.hour = idata;
					else al1.hour--;
				}
				else
				{
					if(al1.hour == idata) al1.hour = jdata;
					else al1.hour++;
				}
			}
		}
		if (((mod == TIME) || (mod == ALARM11) || (mod == DAYSEC)) && (setm == NASTAVENIE2))	//will set the minute
		{
			idata = 59;
			jdata = 0;
			if(mod == TIME)
			{
				if (dir == DOWN)
				{
					if(tm.minute == jdata) tm.minute = idata;
					else tm.minute--;
				}
				else
				{
					if(tm.minute == idata) tm.minute = jdata;
					else tm.minute++;
				}
			}
			if(mod == ALARM11)
			{
				if (dir == DOWN)
				{
					if(al1.minute == jdata) al1.minute = idata;
					else al1.minute--;
				}
				else
				{
					if(al1.minute == idata) al1.minute = jdata;
					else al1.minute++;
				}
			}
			if(mod == DAYSEC)
			{
				if (dir == DOWN)
				{
					if(tm.second == jdata) tm.second = idata;
					else tm.second--;
				}
				else
				{
					if(tm.second == idata) tm.second = jdata;
					else tm.second++;
				}
			}
		}
		if ((mod == DAYSEC) && (setm == NASTAVENIE1))							//will set the day of the week
		{
			idata = 7;
			if (dir == DOWN)
			{
				if(tm.wday == jdata) tm.wday = idata;
				else tm.wday--;
			}
			else
			{
				if(tm.wday == idata) tm.wday = jdata;
				else tm.wday++;
			}
		}
		if ((mod == ALARM12) && (setm == NASTAVENIE1))							//will set the day of the week for alarm
		{
			idata = 8;
			if (dir == DOWN)
			{
				if(al1.wday == jdata) al1.wday = idata;
				else al1.wday--;
			}
			else
			{
				if(al1.wday == idata) al1.wday = jdata;
				else al1.wday++;
			}
		}
		if ((mod == DDATE) && (setm == NASTAVENIE1))							//will set the datum
		{
			idata = 31;
			if (dir == DOWN)
			{
				if(dat == jdata) dat = idata;
				else dat--;
			}
			else
			{
				if(dat == idata) dat = jdata;
				else dat++;
			}
		}
		if ((mod == DDATE) && (setm == NASTAVENIE2))							//will set the month
		{
			idata = 12;
			if (dir == DOWN)
			{
				if(mon == jdata) mon = idata;
				else mon--;
			}
			else
			{
				if(mon == idata) mon = jdata;
				else mon++;
			}
		}
		if ((mod == DYEAR) && (setm == NASTAVENIE1))							//will set the year
		{
			idata = 99;
			jdata = 0;
			if (dir == DOWN)
			{
				if(yea == jdata) yea = idata;
				else yea--;
			}
			else
			{
				if(yea == idata) yea = jdata;
				else yea++;
			}
		}
		if ((mod == CALIB) && (setm == NASTAVENIE1))							//will set the calibration
		{
			idata = 99;
			jdata = 0;
			if (dir == DOWN)
			{
				if(cal == jdata) cal = idata;
				else cal--;
			}
			else
			{
				if(cal == idata) cal = jdata;
				else cal++;
			}
		}
		if ((mod == BRIGHT) && (setm == NASTAVENIE1))							//will set the brightness
		{
			idata = 20;
			jdata = 1;
			if (dir == DOWN)
			{
				if(bri == jdata) bri = idata;
				else bri--;
			}
			else
			{
				if(bri == idata) bri = jdata;
				else bri++;
			}
		}
	}
}

//Function to write the time to RTC registers
// ******************************************
void funk_set_time (RTC_HandleTypeDef *hrtca, RTC_TimeTypeDef *sTime)
{
	if(flag == OFF)
	{
		flag = ON;
		*(&sTime[0].Hours) = tm.hour;
		*(&sTime[0].Minutes) = tm.minute;
		*(&sTime[0].Seconds) = 0;
		if (HAL_RTC_SetTime(&hrtca[0], &sTime[0], RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

//Function to write the date to RTC registers
// ******************************************
void funk_set_date (RTC_HandleTypeDef *hrtca, RTC_DateTypeDef *sDate)
{
	if(flag == OFF)
	{
		flag = ON;
		*(&sDate[0].WeekDay) = tm.wday;
		*(&sDate[0].Date) = dat;
		*(&sDate[0].Month) = mon;
		*(&sDate[0].Year) = yea;
		if (HAL_RTC_SetDate(&hrtca[0], &sDate[0], RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

//Function to write the alarm to RTC registers
// *******************************************
void funk_set_alarm (RTC_HandleTypeDef *hrtca, RTC_AlarmTypeDef *sAlarmA)
{
	if(flag == OFF)
	{
		flag = ON;
		funk_conf_alarm(&hrtca[0], &sAlarmA[0]);
	}
}

//Function to configure and set the alarm
// ******************************************
void funk_conf_alarm (RTC_HandleTypeDef *hrtca, RTC_AlarmTypeDef *sAlarmA)
{
	*(&sAlarmA[0].AlarmTime.Hours) = al1.hour;
	*(&sAlarmA[0].AlarmTime.Minutes) = al1.minute;
	*(&sAlarmA[0].AlarmTime.Seconds) = 0x0;
	*(&sAlarmA[0].AlarmTime.SubSeconds) = 0x0;
	*(&sAlarmA[0].Alarm) = RTC_ALARM_A;
	*(&sAlarmA[0].AlarmDateWeekDay) = al1.wday;
	if (al1.wday <= 7)
	{
		*(&sAlarmA[0].AlarmMask) = RTC_ALARMMASK_NONE;
		*(&sAlarmA[0].AlarmDateWeekDaySel) = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
		*(&sAlarmA[0].AlarmDateWeekDay) = al1.wday;
	}
	else
	{
		*(&sAlarmA[0].AlarmMask) = RTC_ALARMMASK_DATEWEEKDAY;
		*(&sAlarmA[0].AlarmDateWeekDaySel) = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
		*(&sAlarmA[0].AlarmDateWeekDay) = al1.wday;
	}
	if (HAL_RTC_SetAlarm_IT(&hrtca[0], &sAlarmA[0], RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
}

//Function to write brightness in RTC RAM
// ******************************************
void funk_set_bright (RTC_HandleTypeDef *hrtca)
{
	if(flag == OFF)
	{
		flag = ON;
		HAL_RTCEx_BKUPWrite(&hrtca[0], RTC_BKP_DR3, bri);
	}
}

//Function to write calibration in RTC RAM
// ******************************************
void funk_set_cal (RTC_HandleTypeDef *hrtca)
{
	if(flag == OFF)
	{
	  flag = ON;
	  if (cal <= 50)
	  {
		  calp = 50 - cal;
		  HAL_RTCEx_SetSmoothCalib(&hrtca[0],RTC_SMOOTHCALIB_PERIOD_32SEC, RTC_SMOOTHCALIB_PLUSPULSES_RESET,calp);
	  }
	  else
	  {
		  calp = cal - 50;
		  HAL_RTCEx_SetSmoothCalib(&hrtca[0],RTC_SMOOTHCALIB_PERIOD_32SEC, RTC_SMOOTHCALIB_PLUSPULSES_SET,calp);
	  }
	  HAL_RTCEx_BKUPWrite(&hrtca[0], RTC_BKP_DR2, cal);
	}
}
