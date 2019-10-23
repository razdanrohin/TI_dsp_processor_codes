#include "DSP2833x_Device.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
// external function prototypes
extern void InitSysCtrl(void);
extern void InitPieCtrl(void);
extern void InitPieVectTable(void);
// Global Variables

int i,j,k,L,M,N;
float y[20],x[10],h[10];
void convolution(float *x, int N, float *h, float *y);
//###########################################################################
//						main code									
//###########################################################################


void main(void)
{	
	//###########################################################################
	//						System Initialization									
	//###########################################################################
	
	InitSysCtrl();	// Basic Core Init from DSP2833x_SysCtrl.c
	EALLOW;
   	SysCtrlRegs.WDCR= 0x00AF;	// Re-enable the watchdog 
  	EDIS;			// 0x00AF  to NOT disable the Watchdog, Prescaler = 64
	DINT;				// Disable all interrupts
	InitPieCtrl();		// basic setup of PIE table; from DSP2833x_PieCtrl.c
	InitPieVectTable();	// default ISR's in PIE
	
	//###########################################################################
	//						Linear Convolution									
	//###########################################################################

    for(i=0;i<10;i++)
    {
	 x[i]=h[i]=y[i]=0;
    }
	L=4;
	M=4;
	
	x[0]=1; h[0]=5;
	x[1]=2; h[1]=6;
	x[2]=3; h[2]=7;
	x[3]=4; h[3]=0;
	

    N = L + M - 1;
    convolution(x,N,h,y);

} 

void convolution(float *x, int N, float *h, float *y)
{
    int i,j,k;
    float temp=0;

    for(k=0;k<N;k++)
    {
	i=k+1;
	temp=0;
	for(j=0;j<N;j++)
	{
		i--;
		temp += x[j] * h[i];

		if(i==0) i += N;
	}
	y[k] = temp;
    }

}
