#include "window.h"
#include "adcReader2.h"
#include "iirThread.h"

Window::Window() : gain(5), count(0)  //why do we still need gain? We can take it off
{

	for( int index=0; index<plotDataSize; ++index )
	{
		adc_xData[index] = index;
		adc_yData[index] = gain * 1;
		iir_xData[index] = index;
		iir_yData[index] = gain * 1;
	}

	//ADC value plot.
	adc_curve = new QwtPlotCurve;
	adc_plot = new QwtPlot;
	// make a plot curve from the data and attach it to the plot
	adc_curve->setSamples(adc_xData, adc_yData, plotDataSize);
	adc_curve->attach(adc_plot);

	adc_plot->replot();
	adc_plot->show();
//	fprintf(stderr, "am i still alive?\n");
	//IIR value plot.
	iir_curve = new QwtPlotCurve("IIR Value Curve");
	iir_plot = new QwtPlot;
	iir_curve->setSamples(iir_xData, iir_yData, plotDataSize);
	iir_curve->attach(iir_plot);
	iir_plot->replot();
	iir_plot->show();

//	plotBusy = false;  //whenfft is calcualting and we're busy, don't plot until done

	hLayout = new QHBoxLayout;
	hLayout->addWidget(adc_plot);
	hLayout->addWidget(iir_plot);

	setLayout(hLayout);
//	debug: 
//	fprintf(stderr,"I just created adcReader2\n"); 
//	end debug;
	IirThread = new iirThread();
	IirThread->start();
}

 Window::~Window() {
// 	tells the thread to no longer run its endless loop
//	debug: 
//	fprintf(stderr,"I'm deleting adcReader\n");
//	end debug;
	IirThread->quit();
//	adcReader->quit();
// 	wait until the run method has terminated
	IirThread->wait();
//	adcReader->wait();
	delete IirThread;
//	delete adcReader;
	 
}

void Window::timerEvent( QTimerEvent * )
{
// 	Check we are not modifying the plot more than once at a time (allows for frame skips if processor is busy).
//	if( plotBusy ) return; //doesn't do anything
//	plotBusy = true;
		
    int inVal =0;
	count = 0;
//	int plot_buffer[plotDataSize];
    while((IirThread->hasIIRSample())) //remains true while we're not at end of buffer
	{
        inVal = gain * (IirThread->getIIRSample()); //gets sample from the buffer

// 	add the new input to the plot
       	memmove( iir_yData, iir_yData+1, (plotDataSize-1) * sizeof(double) );//shift ydata buffer?
        iir_yData[plotDataSize-1] = inVal; //actual data value incoming
	    iir_curve->setSamples(iir_xData, iir_yData, plotDataSize);  
		
// 	fprintf(stderr,"starting plot");
		iir_plot->replot();
	
	

 	while((IirThread->hasAdcSample())) //remains true while we're not at end of buffer
	{
        inVal = gain * (IirThread->getAdcSample()); //gets sample from the buffer
// 	add the new input to the plot
       	memmove( adc_yData, adc_yData+1, (plotDataSize-1) * sizeof(double) );//shift ydata buffer?
        	
		adc_yData[plotDataSize-1] = inVal; //actual data value incoming
     	adc_curve->setSamples(adc_xData, adc_yData, plotDataSize);  

// 	fprintf(stderr,"starting plot");
		adc_plot->replot();
	
//	fprintf(stderr,"finished plot");


void Window::setGain(double gain)
{
	this->gain = gain;
}
