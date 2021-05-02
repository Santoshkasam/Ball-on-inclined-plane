/*
 * EmCoEx_Wippe_v10.c
 *
 * Created: 28.07.2011 09:34:25
 *  Author: W.Lang
 */ 
// Compiler includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
// Modul includes
#include "global.h"
#include "lcd.h"
#include "HMI.h"
#include "ServoSensor.h"
#include "PID.h"
// Modul global vars
const char strLCD [21] PROGMEM =   "LCD library - Ok    "; 
const char strQue [21] PROGMEM =   "Queues created      "; 
const char strTask [21] PROGMEM =  "Tasks created       "; 
const char strSch [21] PROGMEM =   "Scheduler started   "; 
const char strStack [21] PROGMEM = "Stack overflow task:"; 
//----------------------------------------------------------
 void vApplicationStackOverflowHook (xTaskHandle *pxTask, const portCHAR *pcTaskName)
{	lcd_clrscr();  	lcd_puts_p(strStack);
	lcd_puts(pcTaskName);  	
	cli();  PORTC = PORTC & 0b10000000;  while (1);
} 
//----------------------------------------------------------
// Global vars for following function
unsigned char  Servo_counter = 0;
// Function is called by FreeRTOS ticker every 3ms
void  vApplicationTickHook (void)
{ TCNT3 = 0; // Delete timer counter reg 3
  Servo_counter ++; // Every 7 times generate a servo signal
  if (Servo_counter > 6) // Historical reason from flight model remote control
  { Servo_counter = 0;
    if (servo_power == 1) PORTC = PORTC | 0b10000000; // Generate servo signal
  }
}
//----------------------------------------------------------
ISR (TIMER3_COMPA_vect)
{ PORTC = PORTC & 0b01111111; // Finish servo signal
}
//----------------------------------------------------------
int  main()
{ char  RTOS_Allright = 1, ret_val = 0;
// Init port direction
  DDRD = DDRD & 0x0f;  // PD4..7 inputs
  PORTD = PORTD | 0xf0;  // Pullups at PD4..7
  DDRA = 0; // all PAs inputs
  PORTA = 0xff; // all PAs with pullups
  DDRC = 255; // all pins outputs
  DDRG = 3; // internal LEDs
  PORTG = 0b00111; // Pullup PG2 and LEDs off
// Init timer 3
  TCCR3A = 0;
  TCCR3B = 0b00000011; // 
  TCCR3C = 0;
  TCNT3 = 0;
  OCR3A = 3000;
  ETIMSK = ETIMSK | 0b00010000; // OCIE3A
  ASSR = 0;
// Init LCD
  lcd_init (LCD_DISP_ON_CURSOR_BLINK);
  lcd_clrscr ();
  lcd_puts_p (strLCD);
// Create queues
  QueueTaster = xQueueCreate (10, sizeof(char));
  if (QueueTaster == 0)  RTOS_Allright = 0;
  QueueSensor = xQueueCreate (1, sizeof(float));
  if (QueueSensor == 0)  RTOS_Allright = 0;
  QueueServo = xQueueCreate (1, sizeof(float));
  if (QueueServo == 0)  RTOS_Allright = 0;
  lcd_puts_p (strQue);
// Start tasks
  ret_val = xTaskCreate (vSensor, (const signed portCHAR *)\
   "Sensor", configMINIMAL_STACK_SIZE  + (30 * sizeof (float)),\
    NULL, tskIDLE_PRIORITY+2, &vSensor_Handle);
  if (ret_val != pdPASS)  RTOS_Allright = 0;

  ret_val = xTaskCreate (vIO_SRAM_to_LCD, (const signed portCHAR *) \
   "LCD", configMINIMAL_STACK_SIZE  + (20 * sizeof (char)),\
    NULL, tskIDLE_PRIORITY+1, &vIO_SRAM_to_LCD_Handle);
  if (ret_val != pdPASS)  RTOS_Allright = 0;

  ret_val = xTaskCreate (vTaster, (const signed portCHAR *) \
   "Taster", configMINIMAL_STACK_SIZE  + (20 * sizeof (char)),\
    NULL, tskIDLE_PRIORITY+1, &vTaster_Handle);
  if (ret_val != pdPASS)  RTOS_Allright = 0;

  ret_val = xTaskCreate (vHMI, (const signed portCHAR *) \
   "HMI", configMINIMAL_STACK_SIZE  + (100 * sizeof (char)),\
    NULL, tskIDLE_PRIORITY+1, &vHMI_Handle);
  if (ret_val != pdPASS)  RTOS_Allright = 0;

  ret_val = xTaskCreate (vServo, (const signed portCHAR *) \
   "Servo", configMINIMAL_STACK_SIZE  + (5 * sizeof (float)),\
    NULL, tskIDLE_PRIORITY+2, &vServo_Handle);
  if (ret_val != pdPASS)  RTOS_Allright = 0;

  ret_val = xTaskCreate (vPID, (const signed portCHAR *) \
   "PID", configMINIMAL_STACK_SIZE  + (20 * sizeof (float)),\
    NULL, tskIDLE_PRIORITY+2, &vPID_Handle);
  if (ret_val != pdPASS)  RTOS_Allright = 0;

  lcd_puts_p (strTask);
// Start Scheduler
  if (RTOS_Allright == 1) 
  {   lcd_puts_p (strSch);
      vTaskStartScheduler ();
  } // end if
} // end main
//----------------------------------------------------------
