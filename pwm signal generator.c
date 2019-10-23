#include "DSP2833x_Device.h"     // Headerfile Include File

float x[500],y[500],m[500],s[400];
int i,a,n,count=1,c=1,t;
long int fs=1000,ft=50000;
float dt=0.000001;


void main(void)
{
	i=0;
	m[0]=0;
	a=1;
	n=1;
	x[0]=0;
	y[0]=1;

while(1)
{
	

	while(c<=500)
	{
			m[c]=m[c-1]+6.28*fs*dt*y[c-1];
		    y[c]=y[c-1]-6.28*fs*dt*m[c];
		    c++;
	}
	while(i<=500)
	{
		while(count<=5)
		{
			x[i+1]=x[i]+0.2;
			i++;
			count++;
		}
		while(count>5&&count<=15)
		{
			x[i+1]=x[i]-0.2;
			i++;
			count++;
		}
		while(count>15&&count<=20)
		{
			x[i+1]=x[i]+0.2;
			i++;
			count++;
		}
		
		count=1;
	}

	i=0;
	
	while(i<401)
	{
		if(m[i]>x[i])
		{
			s[i]=1;
		}
		else
		{
			s[i]=-1;
		}
		i++;
		
	}
	i=0;
	
}
	
}



