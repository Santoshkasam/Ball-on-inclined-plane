# Ball-on-inclined-plane
**Project description**: A ball is balanced on an inclined plane through PID control with a servo motor as an actuator. The control algorithm is developed in an Integrated Development Environment (Atmel Studio 7) and executed on the Atmega128 micro-controller, running on FreeRTOS, to control the position of the ball.

- Equipment: photo electric sensors, servo-motor, Atmega128 micro-controller, servo motor arm, plane with side walls and hollow plastic ball.

- Controller: PID

- Control variable: PWM signal (input power to the servo motor)

## Pulse width modulation working

Pulse width modulation technique is used to control analogue devices by controlling the power delivered
to them. In the current experiment the motor arm position is controlled by modulating the
pulse width of the signal i.e., by varying the duty cycle of the signal.



![image](https://user-images.githubusercontent.com/48198017/116827022-47538780-ab97-11eb-937b-a11244539f2a.png)

**Fig. 1:** Duty cycles corresponding to different arm positions


![image](https://user-images.githubusercontent.com/48198017/116827088-a87b5b00-ab97-11eb-9664-8ab407d63ba5.png "servo motor arm positions with respect to pulse width")

**Fig. 2:** Servo motor arm positions with respect to pulse width

