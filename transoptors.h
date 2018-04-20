/*
 * adc_config.h
 *
 * Created: 13.03.2018 20:13:29
 *  Author: kamil
 */ 
#ifndef ADC_CONFIG_H
#define ADC_CONFIG_H

#define F_CPU 32000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include <stdbool.h>

#define WHITE 0
#define BLACK 1

int8_t voltage_treshold; //próg bieli
uint8_t adcread[8];
uint8_t transoptors[8]; //odczyt transoptorów
int8_t weights[8]; //wagi czujników

void start_read(void);
void ADC_interrupt_procedure(ADC_CH_t *adcch, uint8_t i);
void setADC(void);
void pmic_init(void);
//void ADC_CH_Init(ADC_CH_t *adcch,register8_t muxpos);



#endif