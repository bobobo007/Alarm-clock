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
#include "Display.h"
#include "defines.h"

//Function to convert a character to a 7segment code
// *************************************************
uint8_t funk_segcod(unsigned char cha)
{
	uint8_t cod;
	switch (cha)
	{
	case '0':
		cod = 0xC0;	//0b11000000
		break;
	case '1':
		cod = 0xF9;	//0b11111001
		break;
	case '2':
		cod = 0xA4;	//0b10100100
		break;
	case '3':
		cod = 0xB0;	//0b10110000
		break;
	case '4':
		cod = 0x99;	//0b10011001
		break;
	case '5':
		cod = 0x92;	//0b10010010
		break;
	case '6':
		cod = 0x82;	//0b10000010
		break;
	case '7':
		cod = 0xF8;	//0b11111000
		break;
	case '8':
		cod = 0x80;	//0b10000000
		break;
	case '9':
		cod = 0x90;	//0b10010000
		break;
	case 'A':
	case 'a':
		cod = 0x88;	//0b10001000
		break;
	case 'B':
	case 'b':
		cod = 0x83;	//0b10000011
		break;
	case 'C':
	case 'c':
		cod = 0xC6;	//0b11000110
		break;
	case 'D':
	case 'd':
		cod = 0xA1;	//0b10100001
		break;
	case 'E':
	case 'e':
		cod = 0x86;	//0b10000110
		break;
	case 'F':
	case 'f':
		cod = 0x8E;	//0b10001110
		break;
	case 'H':
	case 'h':
		cod = 0x89;	//0b10001001
		break;
	case 'J':
	case 'j':
		cod = 0xF0;	//0b11110000
		break;
	case 'L':
	case 'l':
		cod = 0xC7;	//0b11000111
		break;
	case 'M':
	case 'm':
		cod = 0xC8;	//0b11001000
		break;
	case 'N':
	case 'n':
		cod = 0xAB;	//0b10101011
		break;
	case 'O':
	case 'o':
		cod = 0xA3;	//0b10100011
		break;
	case 'P':
	case 'p':
		cod = 0x8C;	//0b10001100
		break;
	case 'R':
	case 'r':
		cod = 0xAF;	//0b10101111
		break;
	case 'S':
	case 's':
		cod = 0x92;	//0b10010010
		break;
	case 'T':
	case 't':
		cod = 0x87;	//0b10000111
		break;
	case 'W':
	case 'w':
		cod = 0xC1;	//0b11000001
		break;
	case 'U':
	case 'u':
		cod = 0xC1;	//0b11000001
		break;
	case 'g':
		cod = 0x9C;	//0b10011100
		break;
	case '-':
		cod = 0xFF;	//0b10111111
		break;
	case ' ':
	default:
		cod = 0xFF; //empty space
	    }
	return cod;
}

//Function for displaying one character on one segment
// *****************************************
void funk_write_char (unsigned char cha, uint8_t dig)
{
	SEG_A_GPIO_Port->ODR &= ~(0x000000FF);				//reset all segments outputs
	DIG1_GPIO_Port->ODR |= (0x0000000F);				//reset all digit outputs
	SEG_A_GPIO_Port->ODR |= (cha);						//set segments outputs

	uint32_t help = DIG1_GPIO_Port->ODR | (0x0000000F);	//set digit outputs
	switch (dig)
	{
	case 0:
		DIG1_GPIO_Port->ODR = help & ~(0x00000001);
		return;
	case 1:
		DIG1_GPIO_Port->ODR = help & ~(0x00000002);
		return;
	case 2:
		DIG1_GPIO_Port->ODR = help & ~(0x00000004);
		return;
	case 3:
		DIG1_GPIO_Port->ODR = help & ~(0x00000008);
		return;
	case 4:
		DIG1_GPIO_Port->ODR = help & ~(0x00000000);
		return;
	}
}

//Function to display all characters on the display
// *************************************************
void funk_write_all(unsigned char* cha, uint8_t ld1, uint8_t co, uint8_t bl)
{
	uint8_t t,s;									//auxiliary variable
	uint8_t*p;										//pointer
	t=ld1>>4;
	t &= (1<<co);
	ld1 &=(1<<co);									//which LED has to illuminate
	p = cha + co;									//the pointer points to the right place
	if(co==4) funk_write_char(0xFF,(4));			//If the counter is 0, does not show anything - it's used to control the display brightness
	else
	{
		s = bl*t;
		if(s)										//If Digit is supposed to blink, do not show at 500ms nothing
		{
			if(ld1)funk_write_char(0x7F,(3-co));	//illuminate only the LED
			funk_write_char(0xFF,(3-co));

		}
		else
		{

			if(ld1)funk_write_char((funk_segcod(*p)& 0x7F),(3-co));		//8-th bit is zero, if the LED shut illuminate
			else funk_write_char(funk_segcod(*p),(3-co));				//8-th bit is 1, if the LED shut not illuminate
		}
	}
}


//Function to convert a number to a 7-segment code
// ***********************************************
void funk_displnum (unsigned char* cha, uint8_t num)
{
	unsigned char a = '0';
	unsigned char b = '0';
	if(num<10) b=num + 0x30;		//if the number is smaller from then the first number will be zero
	else							//if the number is no smaller from then then it is displayed normally
	{
		a=num/10 + 0x30;			//the first number
		b=num%10 + 0x30;			//the second number
	}
	*cha=a;							//Save the number
	cha ++;
	*cha=b;
}

//Function to convert day of the week to 7-segment code
// ****************************************************
void funk_displwday(unsigned char* cha, uint8_t wday)
{
#if defined SLOVAK
	switch(wday)
	{
		case 1:
			*cha = 'P';
			cha++;
			*cha = 'O';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 2:
			*cha = 'U';
			cha++;
			*cha = 'T';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 3:
			*cha = 'S';
			cha++;
			*cha = 'E';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 4:
			*cha = 'S';
			cha++;
			*cha = 'T';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 5:
			*cha = 'P';
			cha++;
			*cha = 'A';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 6:
			*cha = 'S';
			cha++;
			*cha = 'O';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 7:
			*cha = 'N';
			cha++;
			*cha = 'E';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 8:
			*cha = 'P';
			cha++;
			*cha = 'O';
			cha++;
			*cha = 'N';
			cha++;
			*cha = 'E';
		break;
		case 9:
			*cha = 'P';
			cha++;
			*cha = 'O';
			cha++;
			*cha = 'P';
			cha++;
			*cha = 'A';
		break;
		case 10:
			*cha = 'S';
			cha++;
			*cha = 'O';
			cha++;
			*cha = 'N';
			cha++;
			*cha = 'E';
		break;
		default:
			*cha = 'N';
			cha++;
			*cha = 'O';
			cha++;
			*cha = 'O';
			cha++;
			*cha = 'P';
	}
#endif
#if defined ENGLISH
	switch(wday)
	{
		case 1:
			*cha = 'M';
			cha++;
			*cha = 'O';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 2:
			*cha = 'T';
			cha++;
			*cha = 'U';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 3:
			*cha = 'W';
			cha++;
			*cha = 'E';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 4:
			*cha = 'T';
			cha++;
			*cha = 'H';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 5:
			*cha = 'F';
			cha++;
			*cha = 'R';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 6:
			*cha = 'S';
			cha++;
			*cha = 'A';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 7:
			*cha = 'S';
			cha++;
			*cha = 'U';
			cha++;
			*cha = ' ';
			cha++;
			*cha = ' ';
		break;
		case 8:
			*cha = 'M';
			cha++;
			*cha = 'O';
			cha++;
			*cha = 'S';
			cha++;
			*cha = 'U';
		break;
		case 9:
			*cha = 'M';
			cha++;
			*cha = 'O';
			cha++;
			*cha = 'F';
			cha++;
			*cha = 'R';
		break;
		case 10:
			*cha = 'S';
			cha++;
			*cha = 'A';
			cha++;
			*cha = 'S';
			cha++;
			*cha = 'U';
		break;
		default:
			*cha = 'N';
			cha++;
			*cha = 'O';
			cha++;
			*cha = 'O';
			cha++;
			*cha = 'P';
	}
#endif
}
