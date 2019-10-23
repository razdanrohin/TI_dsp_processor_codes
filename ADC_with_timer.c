
#include "DSP2833x_Device.h"

// external function prototypes initializations
extern void InitAdc(void);
extern void InitSysCtrl(void);
extern void InitPieCtrl(void);
extern void InitPieVectTable(void);
void delay_loop(long );


// Prototype statements for functions found within this file.
void Gpio_select(void);
interrupt void cpu_timer0_isr(void);
interrupt void adc_isr(void);		 // ADC  End of Sequence ISR

// Global Variables
unsigned int a,n=0;
double d[250];

//###########################################################################
//						main code									
//###########################################################################

void main(void)
{	
	
//------------------- DSP Initialization -----------------------------------------//
	InitSysCtrl();				// Basic Core Init from DSP2833x_SysCtrl.c
	DINT;						// Disable all interrupts
	InitPieCtrl();				// basic setup of PIE table; from DSP2833x_PieCtrl.c
	InitPieVectTable();			// default ISR's in PIE
//------------------- ADC Initialization -----------------------------------------//
	InitAdc();									// Basic ADC setup, incl. calibration

	AdcRegs.ADCTRL1.all = 0;	   
	AdcRegs.ADCTRL1.bit.ACQ_PS = 0; 			// 7 = 8 x ADCCLK	
	AdcRegs.ADCTRL1.bit.SEQ_CASC =1; 			// 1=cascaded sequencer
	AdcRegs.ADCTRL1.bit.CPS = 0;				// divide by 1
	AdcRegs.ADCTRL1.bit.CONT_RUN = 0;			// single run mode
	AdcRegs.ADCTRL2.all = 0;			
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;		// 1=enable SEQ1 interrupt
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 =1;		// 1=SEQ1 start from ePWM_SOCA trigger
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 0;		// 0= interrupt after every end of sequence
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 1;			// ADC clock: FCLK = HSPCLK / 2 * ADCCLKPS; HSPCLK = 75MHz (see DSP2833x_SysCtrl.c);FCLK = 12.5 MHz

	AdcRegs.ADCMAXCONV.all = 0x0000;   	  		// 1 conversions from Sequencer 1
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 4; 		// Setup ADCINA0 as 1st SEQ1 conv.
	EPwm2Regs.TBCTL.all = 0xC030;			    // Configure timer control register
	EPwm2Regs.TBPRD = 1;						// TPPRD +1  =  TPWM / (HSPCLKDIV * CLKDIV * TSYSCLK)
	          									//			 =  20 µs / 6.667 ns
	
	EPwm2Regs.ETPS.all = 0x0100;				// Configure ADC start by ePWM2
	EPwm2Regs.ETSEL.all = 0x0A00;				// Enable SOCA to ADC
	
	EALLOW;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.ADCINT = &adc_isr;
	EDIS;

	InitCpuTimers();							// basic setup CPU Timer0, 1 and 2

	ConfigCpuTimer(&CpuTimer0,15,10);
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;			// CPU Timer 0
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;			// ADC
	IER |=1;
	EINT;
	ERTM;
	CpuTimer0Regs.TCR.bit.TSS = 0;				// start timer0

	while(1)
	{}
} 

void Gpio_select(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX1.all = 0;				// GPIO15 ... GPIO0 = General Puropse I/O
	GpioCtrlRegs.GPAMUX2.all = 0;				// GPIO31 ... GPIO16 = General Purpose I/O
	GpioCtrlRegs.GPBMUX1.all = 0;				// GPIO47 ... GPIO32 = General Purpose I/O
	GpioCtrlRegs.GPBMUX2.all = 0;				// GPIO63 ... GPIO48 = General Purpose I/O
	GpioCtrlRegs.GPCMUX1.all = 0;				// GPIO79 ... GPIO64 = General Purpose I/O
	GpioCtrlRegs.GPCMUX2.all = 0;				// GPIO87 ... GPIO80 = General Purpose I/O
	
	GpioCtrlRegs.GPBDIR.all = 1;				
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;			
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;  		
	GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1; 
	GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1; 
	GpioCtrlRegs.GPCDIR.all = 1;		
	
	EDIS;
}   

interrupt void cpu_timer0_isr(void)
{	
	
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 =1;
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 =0;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	
}

interrupt void  adc_isr(void)
{	
 	a = AdcMirror.ADCRESULT0;					// store results global
  	
  	if(n<249)
  	{
  		d[n]=a*0.0007326-1.5;					// { 3.0/4095 = 0.0007326 }
  	  	n=n+1;
  	}
  	else
  	n=0;
  
	// Reinitialize for next ADC sequence
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;     		// Reset SEQ1
  	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;			// Clear INT SEQ1 bit
  	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; 	// Acknowledge interrupt to PIE

}
  	
//===========================================================================
// End of SourceCode.
//===========================================================================




