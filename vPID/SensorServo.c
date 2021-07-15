/*
 * SensorServo.c
 *
 * Created: 22.08.2011 17:15:59
 */ 
// Compiler includes
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
// Modul includes
#include "global.h"
#include "lcd.h"
#include "HMI.h"
#include "PID.h"
// external Arrays
extern float  ref_value [7][7];
//----------------------------------------------------------
void  vSensor ( void * pvParameters)
{ unsigned char fototrans, ft_c, led = 0, zeros = 0;
  float pos_part = 0.0, smooth_pos = 0.0;
	float old = 0.0, old1 = 0.0, old2 = 0.0, old3 = 0.0;
	float old4 = 0.0, old5 = 0.0, old6 = 0.0, old7 = 0.0;
	float  old8 = 0.0, old9 = 0.0;
	// LED sequencer on
	cli();  PORTC = (1 << led) | (PORTC & 0b10000000);  sei();
	// Super loop of task vSensor
  for (;;)  
  { vTaskDelay (1);  // 1x 3ms
		// Get part of position info from light breakers values
		fototrans = ((PIND & 0xf0) >> 4) | ((PINA & 0x0f) << 4);
		// Calculate part of position value
		for (ft_c = 0; ft_c < 7; ft_c++)
		{ if ( ! (fototrans & (1 << ft_c)))
			{ if (fabs(ref_value [ft_c][led]) > 0.0005)  zeros++;
				pos_part = pos_part + ref_value [ft_c][led];
			}
		}
		// Rotate to next LED (0..6)
		led++;
		if (led > 6)
		{ 
      if (zeros > 0)  position = pos_part / (float) zeros;
			else  position = 0.0;
			smooth_pos = (position + old + old1 + old2 + old3 \
				+ old4 + old5 + old6 + old7 + old8 + old9) / 11.0;
			old9 = old8;  old8 = old7;  old7 = old6;
			old6 = old5;  old5 = old4;  old4 = old3;
			old3 = old2;  old2 = old1;  old1 = old;  old = position;
			PORTG = PORTG ^ 1;
 			xQueueSend (QueueSensor, &smooth_pos, portMAX_DELAY); 	
 			led = 0;  zeros = 0;  pos_part = 0.0;
		}  // end if
		// Switch the real LED values and keep attention to servo ISR
		cli();  PORTC = (1 << led) | (PORTC & 0b10000000);  sei();
  } // end
} // end task

//----------------------------------------------------------
void  vServo ( void * pvParameters)
{ unsigned int  MIDPOS = 356;
  float  DELTA = 125.0, beta;
  for (;;)  // Super-Loop
  { // Read sensor data from queue
    xQueueReceive (QueueServo, &beta, portMAX_DELAY); 
		// Check the limit of the value
		if (beta > +1.0)  beta = +1.0;
		if (beta < -1.0)  beta = -1.0;
		// Give the value to global var
		angle = beta;
		// Generate timer compare value of servo signal
 		//OCR3A = MIDPOS; // check horizontal position of BoiP
 		OCR3A = (unsigned int) (MIDPOS - (int)(beta * DELTA));
  } // end for
} // end of task servo
//----------------------------------------------------------
