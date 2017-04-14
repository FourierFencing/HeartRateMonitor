#include "window.h"
#include "adcReader2.h"

#include <cmath>  // for sine stuff


Window::Window() : gain(5), count(0)
{
	//knob = new QwtKnob;
	// set up the gain knob
	//knob->setValue(gain);

	// use the Qt signals/slots framework to update the gain -
	// every time the knob is moved, the setGain function will be called
	//connect( knob, SIGNAL(valueChanged(double)), SLOT(setGain(double)) );

	// set up the thermometer
//	thermo = new QwtThermo; 
//	thermo->setFillBrush( QBrush(Qt::red) );
//	thermo->setRange(0, 20);
//	thermo->show();

	setup();

	// set up the initial plot data
	for( int index=0; index<plotDataSize; ++index )
	{
		xData[index] = index;
		yData[index] = gain * 1;
	}

	curve = new QwtPlotCurve;
	plot = new QwtPlot;
	// make a plot curve from the data and attach it to the plot
	curve->setSamples(xData, yData, plotDataSize);
	curve->attach(plot);

	plot->replot();
	plot->show();


	// set up the layout - knob above thermometer
	vLayout = new QVBoxLayout;
//	vLayout->addWidget(knob);
//	vLayout->addWidget(thermo);

	// plot to the left of knob and thermometer
	hLayout = new QHBoxLayout;
	hLayout->addLayout(vLayout);
	hLayout->addWidget(plot);

	setLayout(hLayout);

	// This is a demo for a thread which can be
	// used to read from the ADC asynchronously.
	// At the moment it doesn't do anything else than
	// running in an endless loop and which prints out "tick"
	// every second.
	//adcReader = new HeartRate();
	//adcReader->start();
}

 Window::~Window() {
	// tells the thread to no longer run its endless loop
	//adcReader->quit();
	// wait until the run method has terminated
	//adcReader->wait();
	//delete adcReader;
}

void Window::timerEvent( QTimerEvent * )
{
// For test

       // double inVal = 0;
       // while(adcReader->hasSample())
//	{
       // 	inVal =  (adcReader->getSample());

		double inVal = gain * readData();
		++count;
		//inVal = (1.25 + (1.25 * (inVal/32767)) - 0.75)/0.00125 - 502.5;

	
        	// add the new input to the plot
        	memmove( yData, yData+1, (plotDataSize-1) * sizeof(double) );
        	yData[plotDataSize-1] = inVal;
        	curve->setSamples(xData, yData, plotDataSize);
        	plot->replot();


        	// set the thermometer value
//        	thermo->setValue( inVal + 10 );
	//}
}



// this function can be used to change the gain of the A/D internal amplifier
void Window::setGain(double gain)
{
	// for example purposes just change the amplitude of the generated input
	this->gain = gain;
}
