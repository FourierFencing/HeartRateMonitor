#ifndef ADCREADER2 //other code doesn't need _H
#define ADCREADER2
//////////////Old Header Contents/////////////
//#ifdef __cplusplus
//extern "C" {
//#endif
//	void setup();
//		int readData();
//#ifdef __cplusplus
//}
//#endif

//#endif
///////////////OLD Header Contents//////////////
#include <QThread> //Needed for multithreading
#include <stdint.h> //because we're using uint8_t

//Previously we had includes in .cpp
//going to see what happens if we don't have them here

//The class reads from the AD7705
//and stores data in the ringbuffer (which
// is actually commented with detail) 
// It is read with getSample() and checked
//if data is available with hasSample(). 

//hasSample should be performed before each 
//getSample() ideally

//The class is started in main program as a thread
// adcreader -> start();
//which is then running until the function
// adcreader -> quit(); is called

//then the apparent standard method is: 
// adcreader->wait();
// so that the thread can terminate without breaking

class adcReader2 : public QThread
{

public:
	adcReader2(); //this class is now public to use
	//visualizeIIR();
	//ring buffer functions
	int hasSample();
	int getSample();
	//IIR ring buffer function
//	int getIIRSample();
 //these should really be the only things that get called from the outside
//I don't know why other people have this again
//  public:  //comment out to see what happens
	void quit();
	void run();

protected: 
//	int readData(); //shouldn't be able to do this from the outside


private: 
	bool running;
	void pabort(const char *s);
	//functions that are used inside the .cpp
	void loadSpiDriver();
	void spiSetup (int spiChannel);
	int myAnalogRead(int spiChannel,int channelConfig,int analogChannel); //not in header
	void testing();
	int readData(); //shouldn't be able to do this from the outside

	//file descriptor on the SPI interface 
	int fd;
	//this was not used and replaced with a local variable of myFd
	
	//file escriptor on the interrupt pin
	int sysfs_fd; //not used previously 

	// data collected
	int *samples; //actually is used to store each data value
	//this is our ring buffer

	// pointer to ringbuffer
	int *pIn; 
	//pIn is the address of the current input index of the ring buffer
	//*pIn is the value at that address

	int *pOut;
	//pOut is the address of the current output index of the ring buffer
	//*pOut is the value at that address (output value) 

// data collected
  //      int *samplesIIR; //actually is used to store each data value
        //this is our ring buffer

        // pointer to ringbuffer
//        int *pInIIR; 
        //pIn is the address of the current input index of the ring buffer
        //*pIn is the value at that address

  //      int *pOutIIR;
        //pOut is the address of the current output index of the ring buffer
        //*pOut is the value at that address (output value) 


	//if pIn = pOut, then our current output index has caught up to our input index
	// this is why we use hasSample() as a condition

	// SPI stuff that isn't actually used and can be deleted. 

	uint8_t mode;
	uint8_t bits;
	uint32_t speed;
	uint16_t delay;
	int drdy_GPIO;

};

#endif
