/*
 * HMI.c
 *
 * Created: 10.08.2011 12:28:10
 *  Author: operator
 */
// Compiler includes
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <math.h>
// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
// Modul includes
#include "global.h"
#include "lcd.h"
// Module initialized vars
float ref_value [7][7] =   // Index 1 for FT and 2 for LED
{{ +1.0, +0.9, +0.8, +0.0, +0.0, +0.0, +0.0},
 { +0.0, +0.0, +0.0, +0.7, +0.6, +0.5, +0.0},
 { +0.3, +0.2, +0.0, +0.0, +0.0, +0.0, +0.4},
 { +0.0, +0.0, +0.1, +0.001, -0.1, -0.0, -0.0},
 { -0.4, -0.0, -0.0, -0.0, -0.0, -0.2, -0.3},
 { -0.0, -0.5, -0.6, -0.7, -0.0, -0.0, -0.0},
 { -0.0, -0.0, -0.0, -0.0, -0.8, -0.9, -1.0}};
/*-----------------------------------------------------------*/
// Copy RAM to LCD, position cursor and change gadget sign
//-------------------------------------------------------------
void  vIO_SRAM_to_LCD ( void * pvParameters)
{ unsigned char  zeichen, z = 0;
  for (;;)
  {	vTaskDelay (33); // 33x 3ms = 99ms
    lcd_home ();
	for (zeichen = 0; zeichen < 80; zeichen++)
	  lcd_putc (SRAM_LCD_Text[zeichen]);
	lcd_gotoxy (Cursor_X, Cursor_Y);
    z++;
    switch (z%4)
	{ case 0: SRAM_LCD_Text[79] = 254; break;
	  case 1: SRAM_LCD_Text[79] = 47; break;
	  case 2: SRAM_LCD_Text[79] = 45; break;
	  case 3: SRAM_LCD_Text[79] = 251; break;
	} // end switch
  } // end for
}
/*-----------------------------------------------------------*/
// enum for push button events
enum {NADA, UP, DOWN, LEFT, RIGHT, ENTER};
//-------------------------------------------------------------
void  vTaster ( void * pvParameters)
{ unsigned char  up=3, down=3, left=3, right=3, enter=3, para;
  for (;;)
  { vTaskDelay (3); // 3x 3ms = 9ms
    // last value = actual values
	up = (up & 0b11111101) | ((up & 0b00000001) << 1);
    down = (down & 0b11111101) | ((down & 0b00000001) << 1);
    left = (left & 0b11111101) | ((left & 0b00000001) << 1);
    right = (right & 0b11111101) | ((right & 0b00000001) << 1);
    enter = (enter & 0b11111101) | ((enter & 0b00000001) << 1);
	// read actual values from pins
	up = (up & 0b11111110) | ((PING & 0b00000100) >> 2);
    down = (down & 0b11111110) | ((PINA & 0b00100000) >> 5);
    left = (left & 0b11111110) | ((PINA & 0b10000000) >> 7);
    right = (right & 0b11111110) | ((PINA & 0b00010000) >> 4);
    enter = (enter & 0b11111110) | ((PINA & 0b01000000) >> 6);
	// process switch values - UP
	if ((up & 0b00000111) == 0)
	{ up = up | 0b00000100; SRAM_LCD_Text [19] = 0xde; }
	else
	  if (((~up) & 7) == 0)
	  { up = up & 0b11111011; SRAM_LCD_Text [19] = ' ';
	    para = UP;
	    if (QueueTaster != 0)  xQueueSend (QueueTaster, &para, 0);
      }
	// process switch values - DOWN
	if ((down & 0b00000111) == 0)
	{ down = down | 0b00000100; SRAM_LCD_Text [19] = 0xe0; }
	else
	  if (((~down) & 7) == 0)
	  { down = down & 0b11111011; SRAM_LCD_Text [19] = ' ';
	    para = DOWN;
	    if (QueueTaster != 0)  xQueueSend (QueueTaster, &para, 0);
      }
	// process switch values - LEFT
	if ((left & 0b00000111) == 0)
	{ left = left | 0b00000100; SRAM_LCD_Text [19] = 0xe1; }
	else
	  if (((~left) & 7) == 0)
	  { left = left & 0b11111011; SRAM_LCD_Text [19] = ' ';
	    para =LEFT;
	    if (QueueTaster != 0)  xQueueSend (QueueTaster, &para, 0);
      }
	// process switch values - RIGHT
	if ((right & 0b00000111) == 0)
	{ right = right | 0b00000100; SRAM_LCD_Text [19] = 0xdf; }
	else
	  if (((~right) & 7) == 0)
	  { right = right & 0b11111011; SRAM_LCD_Text [19] = ' ';
	    para = RIGHT;
	    if (QueueTaster != 0)  xQueueSend (QueueTaster, &para, 0);
      }
	// process switch values - ENTER
	if ((enter & 0b00000111) == 0)
	{ enter = enter | 0b00000100; SRAM_LCD_Text [19] = 0x1c; }
	else
	  if (((~enter) & 7) == 0)
	  { enter = enter & 0b11111011; SRAM_LCD_Text [19] = ' ';
	    para = ENTER;
	    if (QueueTaster != 0)  xQueueSend (QueueTaster, &para, 0);
      }
  } // end for
}
/*-----------------------------------------------------------*/
void  floatvalues2LCD (float  first_para, float  second_para)
{ char  sign;
	unsigned char  e0, e_1, e_2, e_3;
  if (first_para > 0.0)  sign = '+';  
  else  {sign = '-';  first_para = -1.0 * first_para;}
  SRAM_LCD_Text[34] = sign;
  e0 = (unsigned char) first_para;
  SRAM_LCD_Text[35] = e0 + '0';
  first_para = (first_para*10) - (float)(e0 * 10);
  e_1 = (unsigned char) first_para;
  SRAM_LCD_Text[37] = e_1 + '0';
  first_para = (first_para*10) - (float)(e_1 * 10);
  e_2 = (unsigned char) first_para;
  SRAM_LCD_Text[38] = e_2 + '0';
  first_para = (first_para*10) - (float)(e_2 * 10);
  e_3 = (unsigned char) first_para;
  SRAM_LCD_Text[39] = e_3 + '0';
  //
  if (second_para > 0.0)  sign = '+';  
  else  {sign = '-';  second_para = -1.0 * second_para;}
  SRAM_LCD_Text[54] = sign;
  e0 = (unsigned char) second_para;
  SRAM_LCD_Text[55] = e0 + '0';
  second_para = (second_para*10) - (float)(e0 * 10);
  e_1 = (unsigned char) second_para;
  SRAM_LCD_Text[57] = e_1 + '0';
  second_para = (second_para*10) - (float)(e_1 * 10);
  e_2 = (unsigned char) second_para;
  SRAM_LCD_Text[58] = e_2 + '0';
  second_para = (second_para*10) - (float)(e_2 * 10);
  e_3 = (unsigned char) second_para;
  SRAM_LCD_Text[59] = e_3 + '0';
}
/*-----------------------------------------------------------*/
float  change_para (float  parameter)
{ char  para, sign, e0, e_1, e_2, e_3;
  Cursor_X = 14;
  // Convert parameter to chars
  if (parameter > 0.0)  sign = '+';  
  else  {sign = '-';  parameter = -1.0*parameter;}
  e0 = (unsigned char) parameter;
  parameter = (parameter*10) - (float)(e0 * 10);
  e_1 = (unsigned char) parameter;
  parameter = (parameter*10) - (float)(e_1 * 10);
  e_2 = (unsigned char) parameter;
  parameter = (parameter*10) - (float)(e_2 * 10);
  e_3 = (unsigned char) parameter;
  // Change char values
  while (para != ENTER)
  {	  xQueueReceive (QueueTaster, &para, portMAX_DELAY);
	  switch (para)
	  { case RIGHT:
		  switch (Cursor_X)
		  { case 14:  Cursor_X = 15;  break;
			case 15:  Cursor_X = 17;  break;
			case 17:  Cursor_X = 18;  break;
			case 18:  Cursor_X = 19;  break;
		  }  break;  // case RIGHT
		case LEFT:
		  switch (Cursor_X)
		  { case 19:  Cursor_X = 18;  break;
			case 18:  Cursor_X = 17;  break;
			case 17:  Cursor_X = 15;  break;
			case 15:  Cursor_X = 14;  break;
		  }  break;  // case LEFT
		case UP:
		  switch (Cursor_X)
		  { case 14:  if (sign == '+')  sign = '-';  else  sign = '+';
			  SRAM_LCD_Text[Cursor_Y*20 + Cursor_X] = sign;  break;
			case 15:  e0 = (e0 + 1) % 10;   SRAM_LCD_Text[Cursor_Y*20 + Cursor_X] = '0' + e0;  break;
			case 17:  e_1 = (e_1 + 1) % 10;  SRAM_LCD_Text[Cursor_Y*20 + Cursor_X] = '0' + e_1;  break;
			case 18:  e_2 = (e_2 + 1) % 10;  SRAM_LCD_Text[Cursor_Y*20 + Cursor_X] = '0' + e_2;  break;
			case 19:  e_3 = (e_3 + 1) % 10;  SRAM_LCD_Text[Cursor_Y*20 + Cursor_X] = '0' + e_3;  break;
		  }  break;  // case UP
		case DOWN:
		  switch (Cursor_X)
		  { case 14:  if (sign == '+')  sign = '-';  else  sign = '+';
			  SRAM_LCD_Text[Cursor_Y*20 + Cursor_X] = sign;  break;
			case 15:  e0--;  if (e0 < 0) e0 = 9;  SRAM_LCD_Text[Cursor_Y*20 + Cursor_X] = '0' + e0;  break;
			case 17:  e_1--;  if (e_1 < 0) e_1 = 9;  SRAM_LCD_Text[Cursor_Y*20 + Cursor_X] = '0' + e_1;  break;
			case 18:  e_2--;  if (e_2 < 0) e_2 = 9;  SRAM_LCD_Text[Cursor_Y*20 + Cursor_X] = '0' + e_2;  break;
			case 19:  e_3--;  if (e_3 < 0) e_3 = 9;  SRAM_LCD_Text[Cursor_Y*20 + Cursor_X] = '0' + e_3;  break;
		  }  break;  // case UP
	  }  // end switch (para)	  
  }  // end while (para != ENTER)
  Cursor_X = 0;
  // Convert chars to parameter
  parameter = (float)e0 + 0.1*(float)e_1 + 0.01*(float)e_2 + 0.001*(float)e_3;
  if (sign == '-')  parameter = -parameter;
  return (parameter);
}  // end change_para
/*-----------------------------------------------------------*/
unsigned char  led_nr = 0;  // Define and init LED group
char zeros = 0;
float pos_part = 0.0, pos;
//
void  show_sensors (void)
{ char  sign, led_bit, ft_bit, e0, e_1, e_2, e_3;
  unsigned char  i, ft;
  // Show position
	pos = position;
	if (pos > 0.0)  sign = '+';  
	else  {sign = '-';  pos = -1.0 * pos;}
	SRAM_LCD_Text[34] = sign;
	e0 = (unsigned char) pos;
	SRAM_LCD_Text[35] = e0 + '0';
	pos = (pos*10) - (float)(e0 * 10);
	e_1 = (unsigned char) pos;
	SRAM_LCD_Text[37] = e_1 + '0';
	pos = (pos*10) - (float)(e_1 * 10);
	e_2 = (unsigned char) pos;
	SRAM_LCD_Text[38] = e_2 + '0';
	pos = (pos*10) - (float)(e_2 * 10);
	e_3 = (unsigned char) pos;
	SRAM_LCD_Text[39] = e_3 + '0';
	// Get FT bits
  ft = (PIND >> 4) | (PINA << 4);
	// Show LED bit values and FT bit values at LCD
  for (i = 0; i < 7; i++)
  { if (PORTC & (1 << i))  led_bit = '1';  else  led_bit = '0';
		SRAM_LCD_Text[48-i] = led_bit; 
		if (ft & (1 << i))  ft_bit = '1';  else  ft_bit = '0';
		SRAM_LCD_Text[59-i] = ft_bit;
		// Get part of position for actual FT values
		if ( ! (ft & (1 << i)))
		{ if (fabs(ref_value [i][led_nr]) > 0.001)  zeros++;
			pos_part = pos_part + ref_value [i][led_nr];
		}
  }
  // Wait a second for reading LCD
  PORTC = 0;  // LED off - reason Ptot of BSS123
  vTaskDelay(333);  // 333x 3ms = ca. 1s
	// Rotate to next LED (0..6)
	led_nr++;
	if (led_nr > 6)
	{ if (zeros > 0)  position = pos_part / (float) zeros;
		else  position = 0.0;
		led_nr = 0;  zeros = 0;  pos_part = 0.0;
	}  // end if
  PORTC = (1 << led_nr);
}
/*-----------------------------------------------------------*/
// Init of HMI menus
const char  Main_Menu [80] PROGMEM =   
 "Group05 EX3 2020 >Change Parameter   >Show Ball Position >Start PID Control "; 
//0123456789012345678901234567890123456789012345678901234567890123456789012345678
const char  PI_Parameter_Menu [80] PROGMEM = 
 "Parameter Values PI >Proportional=+0.123>    Integral=+0.123>Next              "; 
const char  DRT_Parameter_Menu [80] PROGMEM = 
 "Parameter Values DR >  Derivative=+0.123>   Reference=+0.123>Back              "; 
const char  PID_Menu [80] PROGMEM = 
 "PID is running ...  -   PID-Input=-0.123-  PID-Output=-0.123>Back and Stop PID "; 
const char  SBP_Menu [80] PROGMEM = 
 "Show Ball Position  -SlowPosition=-0.123-L0000000  FT1111111>Back              "; 
const char err_status [21] PROGMEM =           "status out of range "; 
const char err_Cursor_Y [21] PROGMEM =         "Cursor_Y out of rang"; 
const char err_Cursor_X [21] PROGMEM =         "Cursor_X out of rang"; 
// Enum for HMI state machine
enum {Main, Para1, Para2, PIDr, SBP};
//------------------------------------------------------------------
void  vHMI ( void * pvParameters)
{ unsigned char  lauf, para = NADA , status = Main;
  // Stop sensor task
  vTaskSuspend (vSensor_Handle);
  servo_power = 0;
  // Init cursor position
  Cursor_X = 0;  Cursor_Y = 1;
  // Init PID parameters
  reference = 0.0;  proportional = 0.2;
  integral = 0.0;  derivative = 0.0;
  position = 0.0;  angle = 0.0;
  // Super loop of task vHMI
  for (;;)
  {	vTaskDelay(20);  // 20x 3ms = 60ms
		PORTG = PORTG ^ 2;
		switch (status)
		{	case Main:
				for (lauf = 0; lauf < 78; lauf++)
					SRAM_LCD_Text[lauf] = pgm_read_byte (&Main_Menu [lauf]);
				// All LEDs off
				cli();  PORTC = PORTC & 0b10000000;  sei();
				switch (para)
				{ case UP:  if (Cursor_Y > 1)  Cursor_Y--;  break;
					case DOWN:  if (Cursor_Y < 3)  Cursor_Y++;  break;	
					case ENTER:
						switch (Cursor_Y)
					{ case 1:  status = Para1;  break;
						case 2:  status = SBP;  break;
						case 3:  status = PIDr;  break;
					}  break;  // case ENTER	
				}  break; // case Main
			case Para1:
				for (lauf = 0; lauf < 78; lauf++)
					SRAM_LCD_Text[lauf] = pgm_read_byte (&PI_Parameter_Menu [lauf]);
				floatvalues2LCD (proportional, integral);
				switch (para)
				{ case UP:  if (Cursor_Y > 1)  Cursor_Y--;  break;
					case DOWN:  if (Cursor_Y < 3)  Cursor_Y++;  break;	
					case ENTER:  
					if (Cursor_Y == 1)  proportional = change_para (proportional);
					else if (Cursor_Y == 2)  integral = change_para (integral);
					else if (Cursor_Y == 3)  status = Para2;
						break;	
				}  break;  // case Para1
			case Para2:
				for (lauf = 0; lauf < 78; lauf++)
					SRAM_LCD_Text[lauf] = pgm_read_byte (&DRT_Parameter_Menu [lauf]);
				floatvalues2LCD (derivative, reference);
				switch (para)
				{ case UP:  if (Cursor_Y > 1)  Cursor_Y--;  break;
					case DOWN:  if (Cursor_Y < 3)  Cursor_Y++;  break;	
					case ENTER:
					if (Cursor_Y == 1)  derivative = change_para (derivative);
					else if (Cursor_Y == 2)  reference = change_para (reference);
					else if (Cursor_Y == 3)  status = Main;
						break;	
				}  break;  // case Para2
			case PIDr:
				for (lauf = 0; lauf < 78; lauf++)
					SRAM_LCD_Text[lauf] = pgm_read_byte (&PID_Menu [lauf]);
				floatvalues2LCD (position, angle);
				Cursor_Y = 3;  servo_power = 1;
				vTaskResume (vSensor_Handle);
				if (para == ENTER)
				{ vTaskSuspend (vSensor_Handle);
					cli();  PORTC = PORTC & 0b10000000;  sei();
					status = Main;
					servo_power = 0;
				}  break;  // case PIDr
			case SBP:
				for (lauf = 0; lauf < 78; lauf++)
					SRAM_LCD_Text[lauf] = pgm_read_byte (&SBP_Menu [lauf]);
				show_sensors ();
				Cursor_Y = 3;
				if (para == ENTER)  status = Main;
				break;  // case SBP
			default: cli();  lcd_clrscr();  lcd_puts_p(err_status);  while(1);
		}  // end switch (status)
    if (xQueueReceive (QueueTaster, &para, 0) != pdTRUE)  para = NADA;
  } // end for
}  // end task vHMI
/*-----------------------------------------------------------*/
