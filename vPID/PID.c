/*
 * PID.c
 *
 * Created: 10.08.1921 12:40:11
 *  Author: Santosh Kumar Kasam
 */ 
// Compiler includes
#include <math.h>
#include <avr/io.h>
// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
// Modul includes
#include "global.h"
#include "PID.h"
//----------------------------------------------------------
void  vPID ( void * pvParameters)
{ float x = 0.0 ;//yp = 0.0;
	// Place here your local parameters
	//float ek1=0.0,ek, ek2=0.0,yc,TN,TV,Kpid,delt=0.018;
	float ek=0,p=0,i=0,d=0,Tot_error=0,prev_error=0,t=0.02,yc=0;
	/*
	ek = error, difference between the ball position and the reference point
	p = proportional term
	i = Integral term
	d = derivative term
	Tot_error = accumulates error over the time
	t = sampling time
	yc = appropriate PID output signal  
	*/

  // Super loop of task
  for (;;)
  { //Get position value from sensor queue
     
	  xQueueReceive (QueueSensor, &x, portMAX_DELAY);
	  
		/*	Compute here your digital PID formula with the 
				help of following global variables:
				reference, proportional, integral, derivative
				and please remember the sampling time
		*/
		
		ek=(reference-x);
		p=proportional*ek;
		i=integral*t*(ek+Tot_error);
		d=derivative*(ek-prev_error) / t;
		
		Tot_error=ek+Tot_error;
		prev_error=ek;
		yc=p+i+d; // the final PID output is calculated

		
		// Write angle value to servo queue
		xQueueSend (QueueServo, &yc, portMAX_DELAY);
		
  } // end for loop
} // end of task PID
//----------------------------------------------------------
