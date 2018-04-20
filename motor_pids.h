/*
 * motor_pids.h
 *
 * Created: 16.03.2018 12:05:16
 *  Author: kamil
 */ 
#ifndef MOTOR_PIDS_H
#define MOTOR_PIDS_H

#define F_CPU 32000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stddef.h>

int16_t 
		R_speed_error, //Odchylenie predkosci rzeczywistej od zadanej
		R_last_error, //Poprzednie odchylenie predkosci rzeczywistej od zadanej
		R_set_speed, //Predkosc zadana (-500, 500)
		R_integrated_error, //Suma algebraiczna bledow do czlonu calkujacego
		R_last_enkoder_read, //Poprzedni odczyt enkodera
		R_current_speed; //Aktualna predkosc na podstawie enkodera
float	R_correction; //Poprawka obliczona przez regulator
		

int16_t 
		L_speed_error, //Odchylenie predkosci rzeczywistej od zadanej
		L_last_error, //Poprzednie odchylenie predkosci rzeczywistej od zadanej
		
		L_integrated_error, //Suma algebraiczna bledow do czlonu calkujacego
		L_last_enkoder_read, //Poprzedni odczyt enkodera
		L_current_speed, //Aktualna predkosc na podstawie enkodera
		L_set_speed; //Predkosc zadana (-500, 500)
float	L_correction; //Poprawka obliczona przez regulator
		
float	Kp, //Wzmocnienie czlonu proporjonalnego
		Kd, //Wzmocnienie czlonu rozniczkujacego
		Ki; //Wzmocnienie czlonu calkujacego

void set_motor_PIDS(float _Kp, float _Kd, float _Ki); //Inicjalizuje zmienne zwiazane z regulatorem PID oraz zadaje jego nastawy
void R_PID(void); //Liczy i stosuje poprawke sterowania PWM dla silnika R
void L_PID(void); //Liczy i stosuje poprawke sterowania PWM dla silnika L
void motorR_set_speed(int16_t speed); //Ustawia predkosc silnika prawego (-500, 500)
void motorL_set_speed(int16_t speed); //Ustawia predkosc silnika lewego (-500, 500)


#endif