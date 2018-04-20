/*
 * motor_pids.c
 *
 * Created: 16.03.2018 12:04:57
 *  Author: kamil
 */ 

#include "motor_pids.h"
#include "config.h"

void set_motor_PIDS(float _Kp, float _Kd, float _Ki)
{
	R_set_speed = 0;
	R_speed_error = 0;
	R_last_error = 0;
	R_correction = 0;
	R_integrated_error = 0;
	R_last_enkoder_read = 0;
	R_current_speed = 0;
	
	L_set_speed = 0;
	L_speed_error = 0;
	L_last_error = 0;
	L_correction = 0;
	L_integrated_error = 0;
	L_last_enkoder_read = 0;
	L_current_speed = 0;
	
	Kp = _Kp;
	Kd = _Kd;
	Ki = _Ki;
}

void R_PID()
{
	
	R_current_speed = 13 * R_ENKODER;
	R_speed_error = R_set_speed - (R_current_speed);
	if(abs(R_integrated_error) < 100)
	{
		if(R_integrated_error < 65000)
		R_integrated_error += R_speed_error;
	}
	else
	{
		R_integrated_error = 0;
	}
	R_correction = Kp * R_speed_error + Kd * (R_speed_error - R_last_error) + Ki *	R_integrated_error;
	motorR((R_set_speed + R_correction));
	R_last_error = R_speed_error;
	R_ENKODER = 0;
}

void L_PID()
{
	L_current_speed = - (13 * L_ENKODER);
	L_speed_error = L_set_speed - (L_current_speed);
	if(abs(L_integrated_error) < 100)
	{
		if(L_integrated_error < 65000)
			L_integrated_error += L_speed_error;
	}
	else
	{
		L_integrated_error = 0;
	}
	L_correction = Kp * L_speed_error + Kd * (L_speed_error - L_last_error) + Ki *	L_integrated_error;
	motorL((L_set_speed + L_correction));
	L_last_error = L_speed_error;
	L_ENKODER = 0;
}

void motorR_set_speed(int16_t speed)
{
	if(speed > 500)
	{
		R_set_speed = 500;
	}
	else if(speed < -500)
	{
		R_set_speed = -500;
	}
	else
	{
		R_set_speed = speed;
	}
}

void motorL_set_speed(int16_t speed)
{
	if(speed > 500)
	{
		L_set_speed = 500;
	}
	else if(speed < -500)
	{
		L_set_speed = -500;
	}
	else
	{
		L_set_speed = speed;
	}
}