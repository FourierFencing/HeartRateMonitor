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


#define MAX_SAMPLES 65536 //not really intelligently thought out

static int myFd ;
 
//char *usage = "Usage: mcp3008 all|analogChannel[1-8] [-l] [-ce1] [-d]";
// -l   = load SPI driver,  default: do not load
// -ce1  = spi analogChannel 1, default:  0
// -d   = differential analogChannel input, default: single ended
 
void adcReader2::loadSpiDriver() //in header
{
    if (system("gpio load spi") == -1)
    {
        fprintf (stderr, "Can't load the SPI driver: %s\n", strerror (errno)) ;
        exit (EXIT_FAILURE) ;
    }
}
 
void adcReader2::spiSetup (int spiChannel) //in header
{
    if ((myFd = SPISetup (spiChannel, 1000000)) < 0)
    {
        fprintf (stderr, "Can't open the SPI bus: %s\n", strerror (errno)) ;
        exit (EXIT_FAILURE) ;
    }
}
 
int adcReader2::myAnalogRead(int spiChannel,int channelConfig,int analogChannel) //in header
{
    if(analogChannel<0 || analogChannel>7)
        return -1;
    unsigned char buffer[3] = {1}; // start bit
    buffer[1] = (channelConfig+analogChannel) << 4;
    SPIDataRW(spiChannel, buffer, 3);
    return ( (buffer[1] & 3 ) << 8 ) + buffer[2]; // get last 10 bits
}


adcReader2::adcReader2() //added to header 
 //do i need void? - don't think i need void because this is set-up that excecuted at startup
{

	wiringPiSetup();
	spiSetup(0);
//	running = TRUE;
	fprintf(stderr,"We are running!\n");
	
}

int adcReader2::readData() //protected in header! therefore needs :: to excecute as a thread?
{
	return myAnalogRead(0, 8, 1-1);
}


//REQUIRED THREADY THINGS
void adcReader2::run()
{
	running = true;

	fprintf(stderr,"We are running!\n");

	while(running) { 

		//apprently we aren't gonna check to see if we see anything

		int value = readData();//get a value from a buffer
		// realistically we could easily just call myAnalogRead here
		//is there anyreason why we don't (other than to obufscate the code?)
		printf("%i",value);
	
		*pIn = value; //put input value in current position pointed to by Pin
		if (pIn == (&samples[MAX_SAMPLES-1]))//if the sample index is at end of buffer
			pIn = samples; //start at beginning of buffer again
		else
			pIn++;		//else, go to next index
		//no end if needed?
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

void adcReader2::pabort(const char *s)
{
	perror(s);
	abort();
}
