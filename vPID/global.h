/*
 * global.h
 *
 * Created: 10.08.2011 17:16:14
 *  Author: operator
 */ 


#ifndef GLOBAL_H_
#define GLOBAL_H_

// Queue handles
xQueueHandle  QueueTaster, QueueSensor, QueueServo;
// Tasks handles
xTaskHandle  vIO_SRAM_to_LCD_Handle, vTaster_Handle, vHMI_Handle;
xTaskHandle  vServo_Handle, vSensor_Handle, vPID_Handle;
// LCD contents and cursor position
char  SRAM_LCD_Text [80];
unsigned char  Cursor_X, Cursor_Y;
// PID parameter
float  reference, proportional, integral, derivative;
// BoiP parameter
float  position, angle;  // PID controller in and out
// Servo parameter
unsigned char  servo_power;  // On=1 and Off=0

#endif /* GLOBAL_H_ */