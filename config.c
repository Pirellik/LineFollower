#define  F_CPU    32000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include "config.h"
#include "bluetooth.h"
#include "motor_pids.h"
#include "transoptors.h"

void OscXtal(void) {
	// konfiguracja generatora kwarcowego
	OSC.XOSCCTRL	=	OSC_FRQRANGE_12TO16_gc |		// wybór kwarcu od 12 do 16 MHZ
	OSC_XOSCSEL_XTAL_16KCLK_gc;		// czas na uruchomienie generatora
	OSC.CTRL		=	OSC_XOSCEN_bm;					// uruchomienie generatora kwarcowego
	// czekanie na ustabilizowanie siê generatora
	for(uint8_t i=0; i<255; i++) {
		if(OSC.STATUS & OSC_XOSCRDY_bm) {
			_delay_ms(20);
			CPU_CCP			=	CCP_IOREG_gc;			// odblokowanie zmiany Ÿród³a sygna³u zegarowego
			CLK.CTRL		=	CLK_SCLKSEL_XOSC_gc;	// wybór Ÿród³a sygna³u zegarowego na XTAL 16MHz
			// uk³ad nadzoruj¹cy kwarc
			CPU_CCP			=	CCP_IOREG_gc;			// odblokowanie modyfikacji wa¿nych rejestrów
			OSC.XOSCFAIL	=	OSC_XOSCFDEN_bm;		// w³¹czenie uk³adu detekcji b³êdu sygna³u zegarowego
			
			return;										// wyjœcie z funkcji jeœli generator siê uruchomi³
		}
		_delay_us(10);
	}
}
void Osc32MHz(void) {
	OSC.CTRL     =    OSC_RC32MEN_bm;       // w³¹czenie oscylatora 32MHz
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // czekanie na ustabilizowanie siê generatora
	CPU_CCP      =    CCP_IOREG_gc;         // odblokowanie zmiany Ÿród³a sygna³u
	CLK.CTRL     =    CLK_SCLKSEL_RC32M_gc; // zmiana Ÿród³a sygna³u na RC 32MHz
	_delay_ms(1000);
}

void setMotors(void){
	//------------ustawienia silnika 1
	PORTD.DIRSET	=	PIN5_bm|//inpu2
	PIN4_bm|//input1
	PIN3_bm;//pwm
	PORTD.OUTCLR	=	PIN5_bm;//input 1->0
	PORTD.OUTCLR	=	PIN4_bm;//input 2->0
	//------------ustawienia silnika 2
	PORTD.DIRSET	=	PIN2_bm|//inpu2
	PIN1_bm|//input1
	PIN0_bm;//pwm
	PORTD.OUTCLR	=	PIN1_bm;//input 1->0
	PORTD.OUTCLR	=	PIN2_bm;//input 2->0
	
	TCD0.CTRLB		=	TC_WGMODE_SINGLESLOPE_gc|		// pwm singleslope
	TC0_CCDEN_bm|
	TC0_CCAEN_bm;
	TCD0.PER		=	500;
	TCD0.CCD		=	0;
	TCD0.CCA		=	0;
	TCD0.CTRLA		=	TC_CLKSEL_DIV1_gc;
}

void motorR(float pulse_width)//kierowanie silnikiem prawym (-500,500)
{ 
	int16_t width = pulse_width;
	if(width == 0)
	{
		PORTD.OUTCLR	=	PIN5_bm;//input 1->0
		PORTD.OUTCLR	=	PIN4_bm;//input 2->0
	}
	else if(width > 0)
	{
		PORTD.OUTCLR	=	PIN5_bm;//input 1->0
		PORTD.OUTSET	=	PIN4_bm;//input 2->1
	}
	else
	{
		PORTD.OUTSET	=	PIN5_bm;//input 1->1
		PORTD.OUTCLR	=	PIN4_bm;//input 2->0
	}
	/*
	if(width > 80)
	width = 80;
	else if(width < -80)
	width = -80;
	*/
	TCD0.CCD		=	abs(width);
}
void motorL(float pulse_width)//kierowanie silnikiem lewym (-500, 500)
{ 
	int16_t width = pulse_width;
	if(width == 0)
	{
		PORTD.OUTCLR	=	PIN1_bm;//input 1->0
		PORTD.OUTCLR	=	PIN2_bm;//input 2->0
	}
	else if(width < 0)
	{
		PORTD.OUTCLR	=	PIN1_bm;//input 1->0
		PORTD.OUTSET	=	PIN2_bm;//input 2->1
	}
	else
	{
		PORTD.OUTSET	=	PIN1_bm;//input 1->1
		PORTD.OUTCLR	=	PIN2_bm;//input 2->0
	}
	/*
	if(width > 80)
		width = 80;
	else if(width < -80)
		width = -80;
		*/
	TCD0.CCA		=	abs(width);
}

void set_switches()
{
	//-----------------------------		przyciski	-----------------------------------------------------------------------
	PORTF.DIRCLR		=	PIN2_bm|PIN3_bm;					// pin F2 F3 jako wejœcie
	PORTF.INT0MASK		=   PIN2_bm;               // pin F2 ma generowaæ przerwania INT0
	PORTF.INT1MASK		=   PIN3_bm;
	PORTF.PIN2CTRL		=   PORT_OPC_PULLUP_gc|    // pull-up na F2
	PORT_ISC_FALLING_gc;   // przerwanie wywo³uje zbocze opadaj¹ce
	PORTF.PIN3CTRL		=   PORT_OPC_PULLUP_gc|    // pull-up na F3
	PORT_ISC_FALLING_gc;   // przerwanie wywo³uje zbocze opadaj¹ce
	PORTF.INTCTRL		=   PORT_INT0LVL_HI_gc| PORT_INT1LVL_HI_gc;   // poziom LO dla przerwania INT0 portu F2 F3
}

void set_leds()
{
	//-----------------------------------diody LED--------------------------------------
	PORTF.DIRSET		=	PIN5_bm|
	PIN6_bm|
	PIN7_bm;
	PORTF.OUTCLR		=	PIN5_bm|
	PIN6_bm|
	PIN7_bm;
}

void set_main_interrupts()
{
	//==============================	timery		=========================================================
	TCD1.INTCTRLA     =    TC_OVFINTLVL_LO_gc;         // przepe³nienie ma generowaæ przerwanie LO
	TCD1.CTRLB        =    TC_WGMODE_NORMAL_gc;        // tryb normalny
	TCD1.PER = 192;
	
	TCC1.INTCTRLA     =    TC_OVFINTLVL_MED_gc;         // przepe³nienie ma generowaæ przerwanie LO
	TCC1.CTRLB        =    TC_WGMODE_NORMAL_gc;        // tryb normalny
	TCC1.PER = 192;
	
	TCD1.CTRLA        =    TC_CLKSEL_DIV1024_gc;
	TCC1.CTRLA        =    TC_CLKSEL_DIV1024_gc;
}


void set_encoders(void)
{
	//-------------------------------	enkodera R	------------------------------------------------------
	PORTC.PIN0CTRL		=	PORT_ISC_LEVEL_gc | PORT_OPC_PULLUP_gc;
	PORTC.PIN1CTRL		=	PORT_ISC_LEVEL_gc | PORT_OPC_PULLUP_gc;
	EVSYS.CH0MUX		=   EVSYS_CHMUX_PORTC_PIN0_gc;        // pin C0 wywo³uje zdarzenie
	EVSYS.CH0CTRL		=   EVSYS_QDEN_bm|                    // w³¹czenie dekodera w systemie zdarzeñ
	EVSYS_DIGFILT_8SAMPLES_gc;        // filtr cyfrowy
	TCC0.CTRLA			=   TC_CLKSEL_EVCH0_gc;               // taktowanie systemem zdarzeñ
	TCC0.CTRLD			=   TC_EVACT_QDEC_gc |                // w³¹czenie dekodera kwadraturowego
	TC_EVSEL_CH0_gc;                  // dekoder zlicza impulsy z kana³u 0*/
	//-------------------------------	enkodera L	------------------------------------------------------
	PORTF.PIN0CTRL		=	PORT_ISC_LEVEL_gc | PORT_OPC_PULLUP_gc;
	PORTF.PIN1CTRL		=	PORT_ISC_LEVEL_gc | PORT_OPC_PULLUP_gc;
	EVSYS.CH2MUX		=   EVSYS_CHMUX_PORTF_PIN0_gc;        // pin E0 wywo³uje zdarzenie
	EVSYS.CH2CTRL		=   EVSYS_QDEN_bm|                    // w³¹czenie dekodera w systemie zdarzeñ
	EVSYS_DIGFILT_8SAMPLES_gc;        // filtr cyfrowy
	TCF0.CTRLA			=   TC_CLKSEL_EVCH2_gc;               // taktowanie systemem zdarzeñ
	TCF0.CTRLD			=   TC_EVACT_QDEC_gc |                // w³¹czenie dekodera kwadraturowego
	TC_EVSEL_CH2_gc;                  // dekoder zlicza impulsy z kana³u 0*/

	L_ENKODER=0;
	R_ENKODER=0;
}	
void ledYellow(void)
{
	PORTF_OUTTGL=PIN5_bm;
}
void ledGreen(void)
{
	PORTF_OUTTGL=PIN6_bm;
}
	
void setbat(void)
{	//funkcja ustawiajaca przerwanie na za niski poziom baterii
	// konfiguracja komparatora 0 w porcie A
	PORTF_OUTSET=PIN7_bm;
	ACA.AC0MUXCTRL		=	AC_MUXPOS_PIN2_gc |
						AC_MUXNEG_SCALER_gc;    // wejœcie + PIN A6
	ACA.AC0CTRL			=	AC_ENABLE_bm|AC_HYSMODE_SMALL_gc|AC_INTLVL_LO_gc|AC_INTMODE_FALLING_gc;
	ACA.CTRLB			=	45;                    // pocz¹tkowe ustawienie dzielnika napiêcia
	ACA.CTRLA			=	AC_AC0OUT_bm;
}