
#include "DSP2833x_Device.h"
#include "math.h"

// external function prototypes

extern void InitSysCtrl(void);
extern void InitPieCtrl(void);
extern void InitPieVectTable(void);

// Prototype statements for functions found within this file.
void Gpio_select(void);
void DAC_isr_ch1(unsigned int );
void delay_loop(long );


// Global Variables
float y[100],m[100],x[100],f=1;
int c=1;
float dt=0.0005;//5micro
unsigned int rev, count, Y;
//###########################################################################
//						main code									
//###########################################################################
void main(void)
{	
	InitSysCtrl();	// Basic Core Init from DSP2833x_SysCtrl.c

	EALLOW;
   	SysCtrlRegs.WDCR= 0x00AF;	// Re-enable the watchdog 
   	EDIS;			// 0x00AF  to NOT disable the Watchdog, Prescaler = 64

	DINT;				// Disable all interrupts
	
	Gpio_select();		// GPIO9, GPIO11, GPIO34 and GPIO49 as output
					    // to 4 LEDs at Peripheral Explorer)

	InitPieCtrl();		// basic setup of PIE table; from DSP2833x_PieCtrl.c
	
	InitPieVectTable();	// default ISR's in PIE

	

	EPwm2Regs.TBCTL.all = 0xC030;	// Configure timer control register
	/*
	 bit 15-14     11:     FREE/SOFT, 11 = ignore emulation suspend
	 bit 13        0:      PHSDIR, 0 = count down after sync event
	 bit 12-10     000:    CLKDIV, 000 => TBCLK = HSPCLK/1
	 bit 9-7       000:    HSPCLKDIV, 000 => HSPCLK = SYSCLKOUT/1
	 bit 6         0:      SWFSYNC, 0 = no software sync produced
	 bit 5-4       11:     SYNCOSEL, 11 = sync-out disabled
	 bit 3         0:      PRDLD, 0 = reload PRD on counter=0
	 bit 2         0:      PHSEN, 0 = phase control disabled
	 bit 1-0       00:     CTRMODE, 00 = count up mode
	*/

	EPwm2Regs.TBPRD = 1;	// TPPRD +1  =  TPWM / (HSPCLKDIV * CLKDIV * TSYSCLK)
	          				//			 =  20 µs / 6.667 ns
	
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;		// CPU Timer 0
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;		// ADC

	IER |=1;

	EINT;
	ERTM;

	CpuTimer0Regs.TCR.bit.TSS = 0;			// start timer0
	m[0]=0; y[0]=1;
	while(1)
        {
            c=1;        	
        	while(c<100)
        	{
        			m[c]=m[c-1]+6.28*f*dt*y[c-1];
        		    y[c]=y[c-1]-6.28*f*dt*m[c];
        		    //x[c]=10+m[c];
        		    Y=floor((m[c]+1)*2046);
        		    DAC_isr_ch1(Y);
        			c++;
        	}    
   			y[0]=y[99];
        	m[0]=m[99];
        	x[0]=x[99];
        	
        }
} 

void Gpio_select(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX1.all = 0;		// GPIO15 ... GPIO0 = General Puropse I/O
	GpioCtrlRegs.GPAMUX2.all = 0;		// GPIO31 ... GPIO16 = General Purpose I/O
	GpioCtrlRegs.GPBMUX1.all = 0;		// GPIO47 ... GPIO32 = General Purpose I/O
	GpioCtrlRegs.GPBMUX2.all = 0;		// GPIO63 ... GPIO48 = General Purpose I/O
	GpioCtrlRegs.GPCMUX1.all = 0;		// GPIO79 ... GPIO64 = General Purpose I/O
	GpioCtrlRegs.GPCMUX2.all = 0;		// GPIO87 ... GPIO80 = General Purpose I/O
	 
	GpioCtrlRegs.GPADIR.all = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;	// peripheral explorer: LED LD1 at GPIO9
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;	// peripheral explorer: LED LD2 at GPIO11

	GpioCtrlRegs.GPBDIR.all = 1;		// GPIO63-32 as inputs
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;	// peripheral explorer: LED LD3 at GPIO34
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1; // peripheral explorer: LED LD4 at GPIO49
	GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1; 
	GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1; 
	GpioCtrlRegs.GPCDIR.all = 1;		// GPIO87-64 as inputs
	
	
	GpioCtrlRegs.GPCDIR.bit.GPIO68 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO69 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO70 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO71 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO72 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO73 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO74 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO75 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO76 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO77 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO78 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO79 = 1;
	
	GpioCtrlRegs.GPBDIR.bit.GPIO43 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO44 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO45 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO46 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO47 = 1;
		
	EDIS;
}   
  	
void delay_loop(long end)
{
	long i;
	for (i = 0; i < end; i++)
	{
		asm(" NOP");
		EALLOW;
		SysCtrlRegs.WDKEY = 0x55;
		SysCtrlRegs.WDKEY = 0xAA;
		EDIS;
	}
}	
  	
  	
void DAC_isr_ch1(unsigned int x)
{	
	//CS active
	GpioDataRegs.GPBCLEAR.bit.GPIO45=1;
	
	// A0,A1=0 FOR CHANNEL 1
	GpioDataRegs.GPBCLEAR.bit.GPIO46=1;
	GpioDataRegs.GPBCLEAR.bit.GPIO47=1;
	
	// R/Wbar low for writing
	GpioDataRegs.GPBCLEAR.bit.GPIO43=1;
	
		// LDAC pin active low 
	GpioDataRegs.GPBCLEAR.bit.GPIO44=1;
	
	// Load actual data
	rev=0;
	count=16;
	while(count>0)
	{
		rev=rev*2+x%2;
		x=x/2;
		count--;
	}
	GpioDataRegs.GPCDAT.all=rev; 


	delay_loop(1);



}

//===========================================================================
// End of SourceCode.
//===========================================================================




        	