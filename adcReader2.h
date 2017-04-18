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

//The class reads from the MCP3008
//and stores data in the ringbuffer (which
// is actually commented with detail) 
// It is read with getSample() and checked
//if data is available with hasSample(). 

//hasSample should be performed before each 
//getSample() ideally

//The class is started in main program as a thread
// adcreader2 -> start();
//which is then running until the function
// adcreader2 -> quit(); is called

//then the apparent standard method is: 
// adcreader2->wait();
// so that the thread can terminate without breaking

class adcReader2 : public QThread
{

public:
	adcReader2(); //this class is now public to use
	//ring buffer functions
	int hasSample();
	int getSample();

	void quit();
	void run();

protected: 
//	int readData(0); //shouldn't be able to do this from the outside

private: 
	bool running;

	//functions that are used inside the .cpp
	void spiSetup (int spiChannel);
	int myAnalogRead(int spiChannel,int channelConfig,int analogChannel); //not in header
	void testing();
	int readData(0); //shouldn't be able to do this from the outside

	//file descriptor on the SPI interface 
	int fd;
	
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

};

#endif
