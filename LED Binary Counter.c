
//###########################################################################
//			Pin description
//---------------------------------------------------------------------------
//	All 4 LEDs are active low
//---------------------------------------------------------------------------
//  LED3: GPIO 80
//  LED4: GPIO 81
//  LED5: GPIO 82
//  LED1: GPIO 83
//									
//###########################################################################

#include "DSP2833x_Device.h"

// external function prototypes

	extern void InitSysCtrl(void);
	extern void InitPieCtrl(void);
	extern void InitPieVectTable(void);

// Prototype statements for functions found within this file.
	void Gpio_select(void);
	void delay_loop(long);

// Global Variables
	int counter=0;	// binary counter for digital output
	
//###########################################################################
//						main code									
//###########################################################################

void main(void)
{

//------------------- DSP Initialization -----------------------------------------//
	InitSysCtrl();				// Basic Core Init from DSP2833x_SysCtrl.c
	InitPieCtrl();				// basic setup of PIE table; from DSP2833x_PieCtrl.c
	InitPieVectTable();			// default ISR's in PIE
//---------------- DSP Initialization complete -----------------------------------//


	Gpio_select();							// Initialize GPIOs	
	
	while(1)
	{    
	  		counter++;
			if(counter&1) GpioDataRegs.GPCCLEAR.bit.GPIO80 = 1;
				else GpioDataRegs.GPCSET.bit.GPIO80 = 1;
			if(counter&2) GpioDataRegs.GPCCLEAR.bit.GPIO81 = 1;
				else GpioDataRegs.GPCSET.bit.GPIO81 = 1;
			if(counter&4) GpioDataRegs.GPCCLEAR.bit.GPIO82 = 1;
				else GpioDataRegs.GPCSET.bit.GPIO82 = 1;
			if(counter&8) GpioDataRegs.GPCCLEAR.bit.GPIO83 = 1;
				else GpioDataRegs.GPCSET.bit.GPIO83 = 1;
			if(counter==15) counter=0;
	  		delay_loop(2500);
	}
} 

void delay_loop(long end)
{
	long i,j;
	for (i = 0; i < end; i++)
	{
		for (j = 0; j < end; j++)
		{
			asm(" NOP");						// NOP: No operation
		}
	}
}	

void Gpio_select(void)
{
	EALLOW;									// To access EALLOW protected registers
		GpioCtrlRegs.GPAMUX1.all = 0;		// GPIO15 ... GPIO0 = General Puropse I/O
		GpioCtrlRegs.GPAMUX2.all = 0;		// GPIO31 ... GPIO16 = General Purpose I/O
		GpioCtrlRegs.GPBMUX1.all = 0;		// GPIO47 ... GPIO32 = General Purpose I/O
		GpioCtrlRegs.GPBMUX2.all = 0;		// GPIO63 ... GPIO48 = General Purpose I/O
		GpioCtrlRegs.GPCMUX1.all = 0;		// GPIO79 ... GPIO64 = General Purpose I/O
		GpioCtrlRegs.GPCMUX2.all = 0;		// GPIO87 ... GPIO80 = General Purpose I/O
		 
	//------------------ Set GPIO as output------------------------------------------//
		GpioCtrlRegs.GPCDIR.bit.GPIO80 = 1;	// peripheral explorer: LED3 at GPIO80
		GpioCtrlRegs.GPCDIR.bit.GPIO81 = 1;	// peripheral explorer: LED4 at GPIO81
		GpioCtrlRegs.GPCDIR.bit.GPIO82 = 1;	// peripheral explorer: LED5 at GPIO82
		GpioCtrlRegs.GPCDIR.bit.GPIO83 = 1;	// peripheral explorer: LED1 at GPIO83
	EDIS;
}   


