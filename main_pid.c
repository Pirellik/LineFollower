/*
 * main_PID.c
 *
 * Created: 16.03.2018 12:57:40
 *  Author: kamil
 */ 

#include "transoptors.h"
#include "main_pid.h"
#include "motor_pids.h"

void set_main_PID(uint16_t _base_speed, float Kp, float Kd, float Ki)
{
	base_speed = _base_speed;
	main_Kp = Kp;
	main_Kd = Kd;
	main_Ki = Ki;
	line_error = 0;
	last_line_error = 0;
	integrated_line_error = 0;
}

void calculate_error()
{
	if(flag == 1)
	{
	last_line_error = line_error;
	line_error = 0;
	uint8_t i = 0;
	float number = 0;
	for(i; i < 8; i++)
	{
		number += transoptors[i];
		line_error += weights[i]*transoptors[i];
	}
	if(number != 0)
	{
		line_error = line_error / number;
		base_speed = 90;
	}
	if(abs(last_line_error) >= weights[6] && number == 0)
	{
		line_error = last_line_error;
		base_speed = 50;
	}
	}
}

void int_error()
{
	integrated_line_error += line_error;
}

void update_motor_speeds()
{
	float correction;
	correction = main_Kp * line_error + main_Kd * (line_error - last_line_error);
	int16_t int_correction = correction;
	motorR_set_speed(base_speed + int_correction);
	motorL_set_speed(base_speed - int_correction);
}