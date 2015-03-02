/*
 * ISR.c
 *
 * Created: 9/13/2014 11:18:39 AM
 *  Author: baghi
 */ 



#include <avr/interrupt.h>




#define LEDPort (PORTB)
#define LEDPin (5)
#define M1Port (PORTD)
#define M1Pin (3)
#define M2Port (PORTD)
#define M2Pin (4)
#define Reed1 (PINB&(1<<7))
#define Reed2 (PINB&(1<<0))
#define Opto (PINB&(1<<2))
#define OptoEnPort (PORTB)
#define OptoEnPin (1)
#define DataIn (PIND&(1<<5))
#define DataOutPort (PORTD)
#define DataOutPin (6)

#define LEDOn (LEDPort|=(1<<LEDPin))
#define LEDOff (LEDPort&=~(1<<LEDPin))
#define M1On (M1Port|=(1<<M1Pin))
#define M1Off (M1Port&=~(1<<M1Pin))
#define M2On (M2Port|=(1<<M2Pin))
#define M2Off (M2Port&=~(1<<M2Pin))
#define OptoEnOn (OptoEnPort|=(1<<OptoEnPin))
#define OptoEnOff (OptoEnPort&=~(1<<OptoEnPin))
#define DataOutOn (DataOutPort|=(1<<DataOutPin))
#define DataOutOff (DataOutPort&=~(1<<DataOutPin))



extern char Flag100ms,Flag10ms;





ISR(TIMER0_OVF_vect)
{
	TCNT0=159;
	Flag100ms=1;
}


ISR(TIMER2_OVF_vect)
{
	TCNT2=99;
	Flag10ms=1;
}