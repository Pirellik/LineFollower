/*
 * main_PID.h
 *
 * Created: 16.03.2018 12:57:51
 *  Author: kamil
 */ 
#ifndef MAIN_PID_H
#define MAIN_PID_H

#define F_CPU 32000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stddef.h>

int16_t base_speed; //Predkosc bazowa
		uint8_t flag;
		
		
float main_Kp, main_Kd, main_Ki, line_error, last_line_error, integrated_line_error; 
uint8_t transoptors[8];


void set_main_PID(uint16_t _base_speed, float Kp, float Kd, float Ki);
void calculate_error(void);
void int_error(void);
void update_motor_speeds(void);

#endif