
#include "DSP2833x_Device.h"
#include "math.h"

// external function prototypes

extern void InitSysCtrl(void);
extern void InitPieCtrl(void);
extern void InitPieVectTable(void);

// Prototype statements for functions found within this file.

// Global Variables
float y[100],m[100],x[100],f=1000;
int c=1;
float dt=50e-6;   

//###########################################################################
//						main code									
//###########################################################################
void main(void)
{	
	
//------------------- DSP Initialization -----------------------------------------//
	InitSysCtrl();				// Basic Core Init from DSP2833x_SysCtrl.c
	EALLOW;
   	SysCtrlRegs.WDCR= 0x00AF;	// Re-enable the watchdog 
   	EDIS;						// 0x00AF  to NOT disable the Watchdog, Prescaler = 64
	DINT;						// Disable all interrupts
	InitPieCtrl();				// basic setup of PIE table; from DSP2833x_PieCtrl.c
	InitPieVectTable();			// default ISR's in PIE
//---------------- DSP Initialization complete -----------------------------------------//
	
	x[0]=0; y[0]=1;   // Set initial state of harmonic oscillator
	while(1)
        {
            c=1;   // Initialize counter     	
        	while(c<100)
        	{
  					// --- Harmonic Oscillator-------//	  
        			x[c]=x[c-1]+6.28*f*dt*y[c-1];
        		    y[c]=y[c-1]-6.28*f*dt*x[c];
    				
    				c++; 		// Update counter
        	}    
        	// --- Reinitialize buffers -------//
   			y[0]=y[99];
        	x[0]=x[99];
        	
        }
} 

//===========================================================================
// End of SourceCode.
//===========================================================================




        	