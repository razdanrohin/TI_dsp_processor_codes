#include "DSP2833x_Device.h"
#include "math.h"

// external function prototypes
extern void InitSysCtrl(void);
extern void InitPieCtrl(void);
extern void InitPieVectTable(void);
	

// Prototype statements for functions found within this file.
void Gpio_select(void);
void InitSystem(void);
void delay_loop(long);
float  fs=1000,dt=0.000001;
void DAC_isr_ch1(unsigned int);

// Declare global variables here
unsigned int x,y[250],rev,count,n=0,t,f;
float Vm,pi,Omega,Plot[250];


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

 
	Vm = 1;
        pi = 3.14;
        f = 50;
     	Omega = 2*pi*f;
     	t=0;

	while(1)
	{  	
        if (t<249)
        {
          Plot[t] = Vm*sin(Omega*t);   // Generate sine wave with amplitude 1
          Plot[t]=(Plot[t]+1)*2047;    // DC shift for unipolar DAC
          y[t]=floor(Plot[t]);	       // Type convert to integer
          DAC_isr_ch1(y[t]);           // Call DAC_isr
  	      t++;
        }
        else
        t=0;
    	  		
	}
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
	GpioCtrlRegs.GPBDIR.all = 1;
	GpioCtrlRegs.GPCDIR.all = 1;
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
