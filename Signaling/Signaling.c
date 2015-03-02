#include "Signaling.h"



void SignalingSystem_Task(SignalingSystem *sys)
{
  char i=0;
  for(i=0;i<SignalQueueElements;i++)
   if((sys->SignalQueue[i].Expired==0)&&(sys->SignalQueue[i].Time==sys->SystemEPOCH)&&(sys->SignalQueue[i].Fired==0))
    sys->SignalQueue[i].Fired=1;
}



void SignalingSystem_SystemEPOCH(SignalingSystem *sys)
{
  if(sys->SystemEPOCH!=0xFFFFFFFF)
    sys->SystemEPOCH=sys->SystemEPOCH+1;
  else
    sys->SystemEPOCH=0;
}



void SignalingSystem_AddSignal(SignalingSystem *sys,unsigned long sigtime,char sigcode)
{
  unsigned long signaltime=0;
  char i;
  signaltime=sys->SystemEPOCH+sigtime;

  for(i=0;i<SignalQueueElements;i++)
   if(sys->SignalQueue[i].Expired==1)
   {
    sys->SignalQueue[i].SignalCode=sigcode;
    sys->SignalQueue[i].Time=signaltime;
    sys->SignalQueue[i].Expired=0;
    sys->SignalQueue[i].Fired=0;
    break;
   }
}





void SignalingSystem_ClearSignal(SignalingSystem *sys,char sigcode)
{
char i;
  for(i=0;i<SignalQueueElements;i++)
    if((sys->SignalQueue[i].Expired==0)&&(sys->SignalQueue[i].SignalCode!=sigcode))
      sys->SignalQueue[i].Expired=1;
}





void SignalingSystem_Init(SignalingSystem *sys)
{
   char i;
   for(i=0;i<SignalQueueElements;i++)
   {
    sys->SignalQueue[i].SignalCode=0;
    sys->SignalQueue[i].Time=0;
    sys->SignalQueue[i].Expired=0;
    sys->SignalQueue[i].Fired=0;
    break;
   }

   sys->SystemEPOCH=0;
}