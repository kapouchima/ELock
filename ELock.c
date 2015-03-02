/*
 * ELock.c
 *
 * Created: 2/23/2015 3:09:31 PM
 *  Author: baghi
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Signaling/Signaling.h"

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


void MotorManager();
void LEDManager();
void Send_Task();
void Receive_Task();
void InterfaceManager();

const enum
{
	Open,
	Close
};


char State=0,LockOpenFlag=0,LockCloseFlag=0,LockState=0,MotorManagerState=0,LockOpenTempFlag=0;
SignalingSystem SigSys,SigSysData;
char Flag100ms=0,Flag10ms=0,ReceiveEnable=1,SendState=10,ReceiveState=0,SendFlag=0,SendData=0,ReceiveFlag=0,ReceiveData=0,LEDFlag=0,LEDState=0;











int main(void)
{
	
	unsigned char i=0,ActionFlag=0;

	DDRD=0b01011000;
	DDRB=0b00100010;
	TCCR0|=0b101;
	TIMSK|=(1<<0);
	TCCR2=0b00000100;
	TIMSK|=(1<<6);
	
	sei();
	
	OptoEnOn;
	LEDOff;
	
	if(Opto)
		LockState=Close;
	else
		LockState=Open;
	
	SignalingSystem_Init(&SigSys);
	SignalingSystem_Init(&SigSysData);
	
	LEDFlag=3;
	
	
    while(1)
    {
		
		/*
		if(Reed2)
			LEDOn;
		else
			LEDOff;
		*/
		
		SignalingSystem_Task(&SigSys);
		SignalingSystem_Task(&SigSysData);
		
		MotorManager();
		Send_Task();
		Receive_Task();
		InterfaceManager();
		LEDManager();
		
		if(Flag100ms==1)
		{
			//LEDPort^=(1<<LEDPin);
			SignalingSystem_SystemEPOCH(&SigSys);
			Flag100ms=0;
		}
		
		if(Flag10ms==1)
		{
			SignalingSystem_SystemEPOCH(&SigSysData);
			Flag10ms=0;
		}
		
		
		if((LockOpenTempFlag==1)&&(!Reed2))
		{
			LockOpenFlag=1;LockOpenTempFlag=0;
		}
		
		
		
    }
}









void MotorManager()
{
	switch(MotorManagerState)
	{
		case 0:
			SignalingSystem_ClearSignal(&SigSys,1);SignalingSystem_ClearSignal(&SigSys,2);SignalingSystem_ClearSignal(&SigSys,10);SignalingSystem_ClearSignal(&SigSys,11);
			if(LockOpenFlag){MotorManagerState=1;LockOpenFlag=0;}
			if(LockCloseFlag){MotorManagerState=10;LockCloseFlag=0;}
			break;
			
		case 1:
			M1Off;M2On;LEDOn;MotorManagerState=2;SignalingSystem_AddSignal(&SigSys,100,10);
			break;
			
		case 2:
			if(SignalingSystem_CheckSignal(&SigSys,10)){M1Off;M2Off;LEDOff;MotorManagerState=0;}
			if(!Opto){MotorManagerState=3;SignalingSystem_AddSignal(&SigSys,17,1);}
			break;
		
		case 3:
			if(SignalingSystem_CheckSignal(&SigSys,1)){M1Off;M2Off;LEDOff;LockState=Close;MotorManagerState=0;}
			break;
			
		case 10:
			M2Off;M1On;LEDOn;MotorManagerState=11;SignalingSystem_AddSignal(&SigSys,100,11);
			break;
		
		case 11:
			if(SignalingSystem_CheckSignal(&SigSys,11)){M1Off;M2Off;LEDOff;MotorManagerState=0;}
			if(Opto){MotorManagerState=12;SignalingSystem_AddSignal(&SigSys,15,2);}
			break;
	
		case 12:
			if(SignalingSystem_CheckSignal(&SigSys,2)){M1Off;M2Off;LEDOff;LockState=Open;MotorManagerState=0;}
			break;
		    
	}
}













void Send_Task()
{
	if(SendFlag)
		{SendState=0;SendFlag=0;ReceiveEnable=0;}
			
	switch(SendState)
	{
		case 0: 
		DataOutOn;SignalingSystem_AddSignal(&SigSysData,10,1);SendState=1;
			break;
			
		case 1: if(SignalingSystem_CheckSignal(&SigSysData,1)){DataOutOff;SignalingSystem_AddSignal(&SigSysData,10,2);SendState=2;}
			break;
			
		case 2: if(SignalingSystem_CheckSignal(&SigSysData,2)){if(SendData&0b10){DataOutOff;}else{DataOutOn;}SignalingSystem_AddSignal(&SigSysData,10,3);SendState=3;}
			break;
			
		case 3: if(SignalingSystem_CheckSignal(&SigSysData,3)){if(SendData&0b01){DataOutOff;}else{DataOutOn;}SignalingSystem_AddSignal(&SigSysData,10,4);SendState=4;}
			break;
			
		case 4: if(SignalingSystem_CheckSignal(&SigSysData,4)){DataOutOff;SendState=10;ReceiveEnable=1;}
	}
}









void Receive_Task()
{
	static char dat;
	if(ReceiveEnable)
	{
		switch(ReceiveState)
		{
			case 0: if(!DataIn){SignalingSystem_AddSignal(&SigSysData,4,200);dat=0;ReceiveState=1;}
				break;
				
			case 1: if(SignalingSystem_CheckSignal(&SigSysData,200))if(!DataIn){SignalingSystem_AddSignal(&SigSysData,10,201);ReceiveState=2;}else{ReceiveState=0;}
				break;
				
			case 2: if(SignalingSystem_CheckSignal(&SigSysData,201))if(DataIn){SignalingSystem_AddSignal(&SigSysData,10,202);ReceiveState=3;}else{ReceiveState=0;}
				break;
			
			case 3: if(SignalingSystem_CheckSignal(&SigSysData,202)){if(DataIn)dat|=0b10;SignalingSystem_AddSignal(&SigSysData,10,203);ReceiveState=4;}
				break;
				
			case 4: if(SignalingSystem_CheckSignal(&SigSysData,203)){if(DataIn)dat|=0b01;SignalingSystem_AddSignal(&SigSysData,20,204);ReceiveState=5;}
					break;
					
			case 5: if(SignalingSystem_CheckSignal(&SigSysData,204)){ReceiveState=0;ReceiveData=dat;ReceiveFlag=1;}
		}
	}
}

















void InterfaceManager()
{
	if(ReceiveFlag)
	{
		
		if(ReceiveData==0b00)
		{
			//Get State
			SendData=0;
			if(LockState==Open)
			{
				SendData=0b10;
				SendFlag=1;
			}
			
			if(LockState==Close)
			{
				if(Reed2)
					SendData=0b01; //Door is Open
				else
					SendData=0b11; //Door is Closed
				SendFlag=1;
			}
			
			
		}
		
		
		
		
		if(ReceiveData==0b01)
		{
			//Close Lock
			LockCloseFlag=1;
			SendData=0b11;
			SendFlag=1;
			LEDFlag=1;
		}
		
		
		
		
		
		if(ReceiveData==0b10)
		{
			//Open Lock
			LockOpenTempFlag=1;
			SendData=0b11;
			SendFlag=1;
			LEDFlag=2;
		}
		
		
		
		
		
		if(ReceiveData==0b11)
		{
			//Ping
			SendData=0b11;
			SendFlag=1;	
		}
		
		ReceiveFlag=0;
	}
}







void LEDManager()
{
	switch(LEDState)
	{
		case 0: if(LEDFlag==1){LEDOn;SignalingSystem_AddSignal(&SigSys,3,70);LEDState=1;}
				if(LEDFlag==2){LEDOn;SignalingSystem_AddSignal(&SigSys,3,70);LEDState=10;}
				if(LEDFlag==3){LEDOn;SignalingSystem_AddSignal(&SigSys,3,70);LEDState=20;}
				break;
				
		case 1: if(SignalingSystem_CheckSignal(&SigSys,70)){LEDOff;LEDFlag=0;LEDState=0;}
			break;
			
		case 10: if(SignalingSystem_CheckSignal(&SigSys,70)){LEDOff;LEDState=11;SignalingSystem_AddSignal(&SigSys,3,71);}
			break;
			
		case 11: if(SignalingSystem_CheckSignal(&SigSys,71)){LEDFlag=1;LEDState=0;}
			break;
			
		case 20: if(SignalingSystem_CheckSignal(&SigSys,70)){LEDOff;LEDState=21;SignalingSystem_AddSignal(&SigSys,3,71);}
			break;
	
		case 21: if(SignalingSystem_CheckSignal(&SigSys,71)){LEDFlag=2;LEDState=0;}
			break;
			
	}
	
}