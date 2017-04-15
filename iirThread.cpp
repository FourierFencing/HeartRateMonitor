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

#include "adcReader2.h"
#include "iirThread.h"
//almost certainly don't need all these

//this is the IIR class - iirThread.cpp
//This implements a seperate thread that takes the ADC data and converts it to IIR
// this might also implement FFT stuff, idk
#define MAX_SAMPLES 65536

iirThread :: iirThread()
{
  int ret=0; //noone knows why, but it makes me feel good.
  
  //setting up filter stuff
//  const int order = 3;  //order of 3 for the filter
//  Iir::Butterworth::BandPass<order> fL; 
 // Iir::Butterworth::BandPass<order> fW;  //using a Bandpass filter to detect frequencies from fencer's lame
	//these are set in header now
 // const float samplingrate = 100000; // Sample rate in Hz
 // const float centre_frequency_L = 10500; //The centre frequency of the fencer's lame
 // const float centre_frequency_W = 13500; //The centre frequency of the fencer's weapon guard
 // const float frequency_width = 1000;  //Width of both frequencies
  fL.setup(order, samplingrate, centre_frequency_L, frequency_width);  //Setup
  fW.setup(order, samplingrate, centre_frequency_W, frequency_width);
  fL.reset();  //reset
  fW.reset();
  
   //set up ringbuffer (IIR)
  samplesIIR = new int[MAX_SAMPLES];
  // pointer for incoming data (IIR)
  pInIIR = samplesIIR;
  //pointer for outgoing data (IIR)
  pOutIIR = samplesIIR;
  running = TRUE;
		adcReader = new adcReader2();
		adcReader->start();

	fprintf(stderr,"We are running iirThread!\n");
}

void iirThread::run()
{
	fprintf(stderr,"We are running iirThread!\n");
	while(running)
	{
//		fprintf(stderr,"We're running to glory\n");
		if(adcReader->hasSample()) //if we've not caught up with adcReader (pOut != pIn) then we take a sample
	  	{
//			fprintf(stderr,"stil here\n");
			int adcValue = adcReader->getSample();
			float valueIIR = fL.filter(adcValue); //pointer to class is set up as adcReader in our .h, NOT adcReader2
			*pInIIR = valueIIR;                            //put input value in current position pointed to by Pin
//			fprintf(stderr,"On this day we fight for glory\n");
			if (pInIIR == (&samplesIIR[MAX_SAMPLES-1])) //if the sample index is at end of buffer
			  	pInIIR = samplesIIR;                      //start at beginning of buffer again
			else
				pInIIR++;                                 //else, go to next index
//			fprintf(stderr,"still running\n");	  	
		}
//		fprintf(stderr,"Why we no run?\n");
	}
}

void iirThread::quit()
{
	running =FALSE;
	exit(0);
}
void iirThread::pabort(const char *s)
{
	perror(s);
	abort();
}




int iirThread::hasIIRSample() //saftey function ensures we are always behind input
{
	return (pOutIIR!=pInIIR); //if we already have the current input sample
			   // then pOut index is hte same as pIn
}


int iirThread::getIIRSample()
{
	assert(pOutIIR!=pInIIR); //ensure that we've not caught up to input samples
       //has sample should avoid this
	int valueIIR = *pOutIIR; //set local value variable to get the value
                      //stored in the address pointed by pOut.

	if(pOutIIR == (&samplesIIR[MAX_SAMPLES-1]))//if we've reached end
		pOutIIR = samplesIIR;         //start from beginning
	else
		pOutIIR++;                 //or don't

	//printf("IIR samples: %d\n", valueIIR);
	return valueIIR;
}
