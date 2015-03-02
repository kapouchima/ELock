
/* -----------------------------------------------------------------------------
 - Simple Signaling system for PIC18 Dveloped by : Sina Baghi
 -
 - SystemEPOCH should be called in cyclic form and it provides Time base for signaling system
 -
 - SignalingSystem_Task should be called in while(1). it's the main task of the signaling system
 -
 - SignalingSystem_AddSignal adds a single task in the signaling system
 -  The sigtime param is the signal Time from now
 -
 - SignalingSystem_ClearSignal clears all the tasks in the signaling system except the signal
 -  with the signal code passed in to the function
 -
 -------------------------------------------------------------------------------
 */



#ifndef SIGNALINGMODULE
#define SIGNALINGMODULE

#define SignalQueueElements 10

typedef struct
{
  char SignalCode;
  unsigned long Time;
  char Expired;
  char Fired;
}Signal;

typedef struct
{
  Signal SignalQueue[SignalQueueElements];
  unsigned long SystemEPOCH;
}SignalingSystem;

void SignalingSystem_SystemEPOCH(SignalingSystem *);
void SignalingSystem_AddSignal(SignalingSystem *,unsigned long ,char );
void SignalingSystem_ClearSignal(SignalingSystem *,char );
void SignalingSystem_Task(SignalingSystem *);
void SignalingSystem_Init(SignalingSystem *);

#endif