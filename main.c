/*
 * linefollower
 *
 * Created: 2017.07.21
 * Author : Dominik Markowski, Jan Zyczkowski
 */ 

#define  F_CPU    32000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include "config.h"
#include "bluetooth.h"
#include "adc_config.h"
#include <stdio.h>

/*
Opis portów
port	funkcja		
A2		-	POZIOM BATERII			-		ZROBIONE
A3-A6	-	ODCZYT FOTOTRANZYSTOROW	-		NIE ZROBIONE
B4-B7	-	ODCZYT FOTOTRANZYSTOROW -		NIEZROBIONE
C0-C1	-	ENKODER MOTOR L			-		ZROBIONE
C2-C3	-	BLUETOOTH				-		ZROBIONE
D0		-	PWM	MOTOR R				-		ZROBIONE
D1		-	INPUT MOTOR 2 R			-		ZROBIONE
D2		-	INPUT MOTOR 1 R			-		ZROBIONE
D3		-	PWM	MOTOR L				-		ZROBIONE
D4		-	INPUT 2 MOTOR L			-		ZROBIONE
D5		-	INPUT 1	MOTOR L			-		ZROBIONE
F0-F1	-	ENKODER MOTOR R			-		ZROBIONE
F2-F3	-	PRZYCISKI				-		ZROBIONE
F5-F7	-	DIODY LED				-		ZROBIONE
*/


int R_set_speed = 0, R_speed_error = 0, R_last_error = 0, R_correction = 0, R_integrated_error = 0, R_last_enkoder_read = 0, R_current_speed = 0;
int16_t L_set_speed = 0, L_speed_error = 0, L_last_error = 0, L_correction = 0, L_integrated_error = 0, L_last_enkoder_read = 0, L_current_speed = 0;
float Kp = 5, Kd = 4, Ki = 2;


void R_PID(void)
{
	
	R_current_speed = 7 * R_ENKODER;
	R_speed_error = R_set_speed - (R_current_speed);
	R_integrated_error += R_speed_error;
	R_correction = Kp * R_speed_error + Kd * (R_speed_error - R_last_error) + Ki *	R_integrated_error;
	motorR((R_set_speed + R_correction));
	R_last_error = R_speed_error;
	R_ENKODER = 0;
}

void L_PID(void)
{
	L_current_speed = 7 * ( - L_ENKODER);
	L_speed_error = L_set_speed - (L_current_speed);
	L_integrated_error += L_speed_error;
	L_correction = Kp * L_speed_error + Kd * (L_speed_error - L_last_error) + Ki *	L_integrated_error;
	motorL((L_set_speed + L_correction));
	L_last_error = L_speed_error;
	L_ENKODER = 0;
}

//Przerwanie przycisk
ISR(PORTF_INT0_vect){
}
//Przerwanie przycisk
ISR(PORTF_INT1_vect){	
}
//przerwanie pomiar baterii
ISR(ACA_AC0_vect){
	PORTF_OUTCLR=PIN7_bm;//zmiena stanu diody gdy bateria jest poni¿ej poziomu dopuszczalnego
}

ISR(TCD1_OVF_vect)
{
	R_PID();
	L_PID();
}



ISR(TCC1_OVF_vect)
{
	
}

ISR(OSC_OSCF_vect) {									// przerwanie w razie awarii oscylatora
	OSC.XOSCFAIL	|=	OSC_XOSCFDIF_bm;				// kasowanie flagi przerwania
}

ISR(ADCA_CH0_vect)
{
	ADC_interrupt_procedure(&ADCA.CH0, 0);	
}

ISR(ADCA_CH1_vect)
{
	ADC_interrupt_procedure(&ADCA.CH1, 1);
}
ISR(ADCA_CH2_vect)
{
	ADC_interrupt_procedure(&ADCA.CH2, 2);
}
ISR(ADCA_CH3_vect)
{
	ADC_interrupt_procedure(&ADCA.CH3, 3);
}
ISR(ADCB_CH0_vect)
{
	ADC_interrupt_procedure(&ADCB.CH0, 4);
}

ISR(ADCB_CH1_vect)
{
	ADC_interrupt_procedure(&ADCB.CH1, 5);
}
ISR(ADCB_CH2_vect)
{
	ADC_interrupt_procedure(&ADCB.CH2, 6);
}
ISR(ADCB_CH3_vect)
{
	ADC_interrupt_procedure(&ADCB.CH3, 7);
}




FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);


int main(void) {
	setall();
	stdout = stdin = &uart_str;
	
	//motorL(500);
	//motorR(-500);
	//R_set_speed = 10;
	//L_set_speed = 10;
	uint8_t i;        
	void pmic_init(void);  
	void setADC(void);
	                                                                                                                                                                                                                                                                                                                                                                                            
	while(1)
	{
		
		start_adc_read();
		/*scanf("%i", &R_set_speed);
		L_set_speed = R_set_speed;
		//printf("%i                 %i\n", R_current_speed, L_current_speed);*/
		_delay_ms(500);
		
		
		for(i = 0; i < 8; i++)
		{
			printf("%i  ", transoptors[i]);
		}
		printf("\n");
	}
	
}
