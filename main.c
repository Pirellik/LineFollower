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
#include <stdio.h>
#include "config.h"
#include "bluetooth.h"
#include "transoptors.h"
#include "motor_pids.h"
#include "main_pid.h"



int16_t set_base_speed = 90; //Predkosc bazowa
	
float	set_main_Kp = 4.5, //Wzmocnienie czlonu proporjonalnego regulatora glownego
		set_main_Kd = 10, //Wzmocnienie czlonu rozniczkujacego regulatora glownego
		set_main_Ki = 0; //Wzmocnienie czlonu calkujacego regulatora glownego
	
float	set_Kp = 1.6, //Wzmocnienie czlonu proporjonalnego regulatora silnikow
		set_Kd = 1.9, //Wzmocnienie czlonu rozniczkujacego regulatora silnikow
		set_Ki = 0; //Wzmocnienie czlonu calkujacego regulatora silnikow


/*
Opis portów
port	funkcja		
A2		-	POZIOM BATERII			-		ZROBIONE
A3-A6	-	ODCZYT FOTOTRANZYSTOROW	-		ZROBIONE
B4-B7	-	ODCZYT FOTOTRANZYSTOROW -		ZROBIONE
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

//Przerwanie przycisk
ISR(PORTF_INT0_vect)
{
	flag = 0;
	ledYellow();
	motorR(0);
	motorL(0);
	uint8_t menu = 0;
	do 
	{
		printf("Menu:\n1 - Ustawienie predkosci bazowej\n2 - Nastawy glownego regulatora\n3 - Nastawy regulatora silnikow\n4 - Kalibracja czujnikow linii\n5 - Wyjscie\n");
		scanf("%hhu", &menu);
		switch(menu)
		{
			case 1:
			{
				printf("Podaj predkosc bazowa: ");
				scanf("%u", &set_base_speed);
				break;
			}
			case 2:
			{
				printf("Podaj wartosc wzmocnienia proporcjonalnego: ");
				scanf("%f", &set_main_Kp);
				printf("Podaj wartosc wzmocnienia rozniczkujacego: ");
				scanf("%f", &set_main_Kd);
				printf("Podaj wartosc wzmocnienia calkujacego: ");
				scanf("%f", &set_main_Ki);
				break;
			}
			case 3:
			{
				printf("Podaj wartosc wzmocnienia proporcjonalnego: ");
				scanf("%f", &set_Kp);
				printf("Podaj wartosc wzmocnienia rozniczkujacego: ");
				scanf("%f", &set_Kd);
				printf("Podaj wartosc wzmocnienia calkujacego: ");
				scanf("%f", &set_Ki);
				break;
			}
			case 4:
			{
				/*
				int8_t maxread;
				uint8_t i;
				uint8_t option = 0;
				start_read();
				printf("Poloz na bia³ym i wpisz 1:");
				scanf("%h", &option);
				ADC_interrupt_procedure(&ADCA.CH0, 0);
				ADC_interrupt_procedure(&ADCA.CH1, 1);
				ADC_interrupt_procedure(&ADCA.CH2, 2);
				ADC_interrupt_procedure(&ADCA.CH3, 3);
				ADC_interrupt_procedure(&ADCB.CH0, 4);
				ADC_interrupt_procedure(&ADCB.CH1, 5);
				ADC_interrupt_procedure(&ADCB.CH2, 6);
				ADC_interrupt_procedure(&ADCB.CH3, 7);
				if(option)
				{
					maxread = adcread[0];
					for(i = 1; i < 8; i++)
					{
						if(adcread[i] > maxread)
							maxread = adcread[i];
					}
					voltage_treshold = maxread;
					printf("Maks odczyt: %hhu", maxread);
				}
				
				*/
				
				break;
			}
			default:
			{
				menu = 5;
				break;
			}
		}
	} while (menu != 5);
	motorR_set_speed(0);
	motorL_set_speed(0);
	set_main_PID(0, 0, 0, 0);
	ledYellow();
}
//Przerwanie przycisk
ISR(PORTF_INT1_vect)
{	
	flag = 1;
	ledGreen();
	set_motor_PIDS(set_Kp, set_Kd, set_Ki);
	set_main_PID(set_base_speed, set_main_Kp, set_main_Kd, set_main_Ki);
	
	_delay_ms(3000);
	ledGreen();
}

//przerwanie pomiar baterii
ISR(ACA_AC0_vect){
	PORTF_OUTCLR=PIN7_bm;//zmienia stan diody gdy bateria jest poni¿ej poziomu dopuszczalnego
}

ISR(TCD1_OVF_vect)
{	
	
	R_PID();
	L_PID();
	
}

ISR(TCC1_OVF_vect)
{
	
	calculate_error();
	update_motor_speeds();
	/*
	ADC_interrupt_procedure(&ADCA.CH0, 0);
	ADC_interrupt_procedure(&ADCA.CH1, 1);
	ADC_interrupt_procedure(&ADCA.CH2, 2);
	ADC_interrupt_procedure(&ADCA.CH3, 3);
	ADC_interrupt_procedure(&ADCB.CH0, 4);
	ADC_interrupt_procedure(&ADCB.CH1, 5);
	ADC_interrupt_procedure(&ADCB.CH2, 6);
	ADC_interrupt_procedure(&ADCB.CH3, 7);
	*/
}

ISR(OSC_OSCF_vect) 
{									// przerwanie w razie awarii oscylatora
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

int main(void) 
{
	Osc32MHz();
	set_switches();
	set_encoders();
	setMotors();
	set_leds();
	setbat();
	bluetooth_init();
	setADC();
	pmic_init();
	set_main_interrupts();
	start_read();
	sei();
	flag = 0;
	stdout = stdin = &uart_str;
	//Kp=1;
	//Kd=0;
	//Ki=0;
	
	//motorL(200);
	//Kp = 0.4;
	//Kd = 0.4;
	//Ki = 0.005;
	//_delay_ms(25000);
	int16_t i = 0;
	//motorR_set_speed(350);
	//motorL_set_speed(350);
	//motorR_set_speed(0);
	//motorL_set_speed(0);
	                                                                                                                                                                                                                                                                                                                                                                         
	while(1)
	{
		//i = L_ENKODER;
		//if(i<2500)
		//motorR_set_speed(i/5);
		//printf("%d.", L_current_speed);
		//i++;
		/**/
		 
			for(i; i < 8; i++)
			{
				if(transoptors[i])
				printf("X");
				else
				printf(" ");
				//printf("%i   ",transoptors[i]);
			}
			/*i = 0;
			for(i; i < 8; i++)
			{
				printf("%i   ", adcread[i]);
			}*/
			printf("\n");
			_delay_ms(3);
			i = 0;
			/*
			for(i; i < 8; i++)
			{
				printf("%i   ", adcread[i]);
			}
			printf("\n");
		//printf("%i \n", R_current_speed);
			
			i = 0;*/
		
	}
}


