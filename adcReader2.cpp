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
#include <Iir.h>

#include "SPISetup.h"
#include "ADCSetup.h"
#include "adcReader2.h"

#define CHAN_CONFIG_SINGLE  8
#define CHAN_CONFIG_DIFF    0

//	#define TRUE                (1==1)
//	#define FALSE               (!TRUE)


#define MAX_SAMPLES 65536 //buffer samples

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

adcReader2::adcReader2() //added to header
//	do i need void? - don't think i need void because this is set-up that excecuted at startup
{
//	fprintf(stderr,"PLEASE RUN!\n");
// 	set up ringbuffer (ADC)
	samples = new int[MAX_SAMPLES];
// 	pointer for incoming data (ADC)
	pIn = samples;
// 	pointer for outgoing data (ADC)
	pOut = samples;

//	Testing where to setup IIR buffer. (Decided on setting it up in the IIR Thread)
//	set up ringbuffer (IIR)
//  samplesIIR = new int[MAX_SAMPLES];
//  pointer for incoming data (IIR)
//  pInIIR = samplesIIR;
//	pointer for outgoing data (IIR)
//  pOutIIR = samplesIIR;


	wiringPiSetup();
	spiSetup(0);
	running = TRUE; //Boolean used to stop the code from running
	fprintf(stderr,"We are running!\n");

}

int adcReader2::readData(int channel) // Reads data from input channel of the ADC
{
	myAnalogRead(0,CHAN_CONFIG_SINGLE,channel);
}

//	void adcReader2::testing()
//	{
//		fprintf(stderr,"We can run functions\n"); //
//	}

//	REQUIRED THREADY THINGS
void adcReader2::run()
{
//	fprintf(stderr, "Run has started!\n");

	fprintf(stderr,"We are running!\n");

	while(running) {

		int value = readData(0);	//Get a value from channel 0 of the ADC
//		testing();
		*pIn = value; 				//put input value in current position pointed to by Pin
		if (pIn == (&samples[MAX_SAMPLES-1]))//if the sample index is at end of buffer
									//(sizeof(samples)/sizeof(*samples))
			pIn = samples; 			//start at beginning of buffer again
		else
			pIn++;	
	}
	//we never opened fd or sysfs_fd, so we don't need to close
}

int adcReader2::getSample()
{
	assert(pOut!=pIn); //ensure that we've not caught up to input samples
	//hassample should avoid this
	int value = *pOut; //set local value variable to get the value
			//stored in the address pointed by pOut.

	if(pOut == (&samples[MAX_SAMPLES-1]))//if we've reached end
		pOut = samples;		//start from beginning
	else
		pOut++;			//or don't

//	printf("ADC samples: %d\n", value);
	return value;			//return the output value
}
int adcReader2::hasSample() //saftey function ensures we are always behind input
{
	return (pOut!=pIn); //if we already have the current input sample
			   // then pOut index is hte same as pIn
}

void adcReader2::quit()
{
	running = false;
	exit(0);
}