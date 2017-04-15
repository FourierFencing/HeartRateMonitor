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
	int ret=0;
//	fprintf(stderr,"PLEASE RUN!\n");
	// set up ringbuffer (ADC)
	samples = new int[MAX_SAMPLES];
	// pointer for incoming data (ADC)
	pIn = samples;
	// pointer for outgoing data (ADC)
	pOut = samples;

        //set up ringbuffer (IIR)
//        samplesIIR = new int[MAX_SAMPLES];
        // pointer for incoming data (IIR)
  //      pInIIR = samplesIIR;
        //pointer for outgoing data (IIR)
    //    pOutIIR = samplesIIR;


	wiringPiSetup();
	spiSetup(0);
	running = TRUE;
	fprintf(stderr,"We are running!\n");

}

int adcReader2::readData() //protected in header! therefore needs :: to excecute as a thread?
{
	return myAnalogRead(0, 8, 1-1);
}

//void adcReader2::testing()
//{
//	fprintf(stderr,"We can run functions\n"); //
//}

//REQUIRED THREADY THINGS
void adcReader2::run()
{
/*	fprintf(stderr, "Run has started!\n");

	const int order = 3;  //order of 3 for the filter
        Iir::Butterworth::BandPass<order> fL;  //using a Bandpass filter to detect frequencies from fencer's lame
        Iir::Butterworth::BandPass<order> fW;  //using a Bandpass filter to detect frequencies from fencer's lame
        const float samplingrate = 100000; // Sample rate in Hz
        const float centre_frequency_L = 10500; //The centre frequency of the fencer's lame
        const float centre_frequency_W = 13500; //The centre frequency of the fencer's weapon guard
        const float frequency_width = 3000;  //Width of both frequencies
        fL.setup (order, samplingrate, centre_frequency_L, frequency_width);  //Setup
        fW.setup (order, samplingrate, centre_frequency_W, frequency_width);
        fL.reset();  //reset
        fW.reset();
*/
	fprintf(stderr,"We are running!\n");

	while(running) {

		//apprently we aren't gonna check to see if we see anything


		int value = readData();//get a value from a buffer
	//	float b = fL.filter(readData());
		// realistically we could easily just call myAnalogRead here
		//is there anyreason why we don't (other than to obufscate the code?)
	//	printf("%i\n",value);
//		testing();
		*pIn = value; //put input value in current position pointed to by Pin
		if (pIn == (&samples[MAX_SAMPLES-1]))//if the sample index is at end of buffer
			//(sizeof(samples)/sizeof(*samples))
			pIn = samples; //start at beginning of buffer again
		else
			pIn++;		//else, go to next index
		//no end if needed?
	//	fprintf(stderr,"We got a sample! %i\n",value); //check to see what sample we get (and if we get it)
        	//int valueIIR = b;
        	//printf("Input=%f\n", b);
		//*pIn = valueIIR; //put input value in current position pointed to by Pin
        	//if (pInIIR == (&samplesIIR[MAX_SAMPLES-1]))//if the sample index is at end of buffer
	        	//(sizeof(samples)/sizeof(*samples))
        	//     	pInIIR = samplesIIR; //start at beginning of buffer again
        	//else
        	//     	pInIIR++;          //else, go to next index
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


//int adcReader2::getIIRSample()
//{
//	assert(pOutIIR!=pInIIR); //ensure that we've not caught up to input samples
 //       //hassample should avoid this
 //       int valueIIR = *pOutIIR; //set local value variable to get the value
  //                      //stored in the address pointed by pOut.

//        if(pOutIIR == (&samplesIIR[MAX_SAMPLES-1]))//if we've reached end
//                pOutIIR = samplesIIR;         //start from beginning
//      else
//                pOutIIR++;                 //or don't
//
//	printf("IIR samples: %d\n", valueIIR);
//        return valueIIR;
//}
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
