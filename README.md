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

## PID working

The position of the ball on the inclined plane is obtained using the data received from the photoelectric
sensors on the inclined plane. This position is compared to the reference position and an appropriate
output is generated by the PID controller according to which a Pulse width modulated signal
is sent to the motor.

![image](https://user-images.githubusercontent.com/48198017/116827480-67844600-ab99-11eb-8669-5427693aa486.png)

**Proportional gain** scales the error value e(t) by proportionally maintaining the size and conserving
the nature of the error (+/-). The proportional gain alone without the error multiplied to it does not
provide proper control as the error´s size and nature are not considered.

With the increase in proportional gain value the rise time and steady state error decreases but the
overshoot increases. Also, with the increase in proportional gain the stability decreases which can be
compensated by a small increase in derivative gain.

*Proportional term = proportional gain x error*

**Integral gain** deals with the past errors where, it scales the sum of the previous errors and the integration
of which account for the Integral term. as the error keeps reducing the control effect of the
proportional term reduces because it is proportional to the current error value, however, the Integral
term compensates to this as its value keeps increasing over time. When the error is eliminated the
Integral term ceases to grow.

Steady state error can be eliminated by increasing the value of Integral gain, but the stability degrades.

*Integral term = Integral gain x accumulated error*

**Derivative gain** scales the rate of change of error and forms the derivative term. the derivative term
is the best way to predict the future error value following the trend of the change in error over time.
The rapid the change, greater the controlling effect.

Kd has no influence over the steady state error. However, overshoot and settling time can be reduced
by increasing the derivative gain. With a slight increase in Kd the stability of the system can be improved

*Derivative term = derivative gain x rate of change of error*

the summation of all the above individual term results in the **final PID output**.

*Y = Proportional term + Integral term + Derivative term*


