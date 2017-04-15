#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>

#include "SPISetup.h"
#include "ADCSetup.h"
#include "adcReader2.h"

#define CHAN_CONFIG_SINGLE  8
#define CHAN_CONFIG_DIFF    0

//#define TRUE                (1==1)
//#define FALSE               (!TRUE)


#define MAX_SAMPLES 65536 //Number of samples in the ring buffer (to be changed)

static int myFd ; //File Descriptor
 

// Sets up the SPI on the Raspberry Pi
void adcReader2::spiSetup (int spiChannel)
{
    int clockFreq = 1000000; //SPI clock frequency
    if ((myFd = SPISetup (spiChannel, clockFreq)) < 0)
    {
        fprintf (stderr, "Can't open the SPI bus: %s\n", strerror (errno)) ;
        exit (EXIT_FAILURE) ;
    }
}

// Reads the analog values from the MCP3008
int adcReader2::myAnalogRead(int spiChannel,int channelConfig,int analogChannel) 
{
    if(analogChannel<0 || analogChannel>7)
        return -1;
    unsigned char buffer[3] = {1}; //'Buffer' is configured to set the MCP3008 in Single Ended mode, 
				   //with CH0 the channel selected
    buffer[1] = (channelConfig+analogChannel) << 4;
    SPIDataRW(spiChannel, buffer, 3); //Configuration values are sent to the MCP3008
    return ( (buffer[1] & 3 ) << 8 ) + buffer[2]; // The converted value is written on the last 10 bits of the input word
}


// Mannick: I've added 'void' before the following function
void adcReader2::adcReader2()
{
	int ret = 0;
	samples = new int[MAX_SAMPLES]; // set up ringbuffer
	pIn = samples; // pointer for incoming data
	pOut = samples; // pointer for outgoing data

	wiringPiSetup();
	spiSetup(0);
	running = TRUE; //Boolean used to stop the code from running
	fprintf(stderr,"We are running!\n");
	
}

int adcReader2::readData(int channel) // Reads data from input channel of the ADC
{
	myAnalogRead(0,CHAN_CONFIG_SINGLE,channel);
}

int adcReader2::getSample()
{
	assert(pOut!=pIn); //ensure that we've not caught up to input samples
	//hassample should avoid this (Mannick: Indeed, we should remove it probably)
	int value = *pOut; //set local value variable to get the value
			//stored in the address pointed by pOut.

	if(pOut == (&samples[MAX_SAMPLES-1]))//if we've reached end
		pOut = samples;		//start from beginning
	else
		pOut++;			//or don't

	return value;			//return the output value
}

int adcReader2::hasSample() //safety function ensures we are always behind input
{
	return (pOut!=pIn); //if we already have the current input sample
			   // then pOut index is hte same as pIn
}

void adcReader2::quit() 
{
	running = false;
	exit(0);
}


//REQUIRED THREADY THINGS
void adcReader2::run()
{

	fprintf(stderr,"System is on!\n");
	
	while(running) { 
		//Add checking if we get input value
		int value = readData(0);//Get a value from channel 0 of the ADC (note change it elsewhere too)
		printf("%i\n",value);
		*pIn = value; //Put input value in current position pointed to by Pin
		if (pIn == (&samples[MAX_SAMPLES-1]))//If the sample index is at end of buffer
			pIn = samples; //start at beginning of buffer again (Mannick: I'm confused, this is a bit similar to getSample)
		else
			pIn++;		//else, go to next index
		//no end if needed? (Mannick: Not sure about the format of functions, should check)
		}
}

