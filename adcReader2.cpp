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
#include <Butterworth.h>

#include "SPISetup.h"
#include "ADCSetup.h"
#include "adcReader2.h"

#define CHAN_CONFIG_SINGLE  8
#define CHAN_CONFIG_DIFF    0

//#define TRUE                (1==1)
//#define FALSE               (!TRUE)

static int myFd ;
 
//char *usage = "Usage: mcp3008 all|analogChannel[1-8] [-l] [-ce1] [-d]";
// -l   = load SPI driver,  default: do not load
// -ce1  = spi analogChannel 1, default:  0
// -d   = differential analogChannel input, default: single ended
 
void loadSpiDriver()
{
    if (system("gpio load spi") == -1)
    {
        fprintf (stderr, "Can't load the SPI driver: %s\n", strerror (errno)) ;
        exit (EXIT_FAILURE) ;
    }
}
 
void spiSetup (int spiChannel)
{
    if ((myFd = SPISetup (spiChannel, 1000000)) < 0)
    {
        fprintf (stderr, "Can't open the SPI bus: %s\n", strerror (errno)) ;
        exit (EXIT_FAILURE) ;
    }
}
 
int myAnalogRead(int spiChannel,int channelConfig,int analogChannel)
{
    if(analogChannel<0 || analogChannel>7)
        return -1;
    unsigned char buffer[3] = {1}; // start bit
    buffer[1] = (channelConfig+analogChannel) << 4;
    SPIDataRW(spiChannel, buffer, 3);
    return ( (buffer[1] & 3 ) << 8 ) + buffer[2]; // get last 10 bits
}


void setup()
{

	wiringPiSetup();
	spiSetup(0);
//	running = TRUE;
	fprintf(stderr,"We are running!\n");
}

int readData()
{
	return myAnalogRead(0, 8, 1-1); //
}

int ADCreader::filter(int sample_in, char** f)
{
	float b = f.filter(sample_in);   //let's hope that the state of each filter can be remembered seperately. 
					//what retains it's values after exciting this function? 
					//For simplicity, i could just put this line in main. 
					//still need to work out buffer stuff
}


//the char should point to the filter class object that has already been set-up. 
//in this way, we can have multiple filter threads running using different filter classes. 
//EG, In setup  we have:

int main(){ //to be setup
 
	Iir::Butterworth::LowPass<order> fW1, fW2, fL1, fL2;
	const float samplingrate = 100000; // Hz
//	const float cutoff_frequency_W1 = 10000; // Hz
//	const float cutoff_frequency_W2 = 10000; // Hz
//	const float cutoff_frequency_L1 = 10000; // Hz
	const float cutoff_frequency_L2 = 10000; // Hz
//	fW1.setup (order, samplingrate, cutoff_frequency_W1);
//	fW2.setup (order, samplingrate, cutoff_frequency_W2);
//	fL1.setup (order, samplingrate, cutoff_frequency_L1);
	fL2.setup (order, samplingrate, cutoff_frequency_L2);
//	fW1.reset();
//	fW2.reset();
//	fL1.reset();
	fL2.reset();
	//this should be in run()
	while(1){
		i = ReadData();
//		outW1 = ADCreader::filter(i, &fW1); //gives a single frequency output of fW1 if it exists
//		outW2 = ADCreader::filter(i, &fW2); //gives a single frequency output of fW2 if it exists
//		outL1 = ADCreader::filter(i, &fL1); //gives a single frequency output of fL1 if it exists
		outL2 = ADCreader::filter(i, &fL2); //gives a single frequency output of fL2 if it exists

		//Then thresholding & truthtable
		}
}
