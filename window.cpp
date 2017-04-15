#include "window.h"
#include "adcReader2.h"

#include <algorithm> // DST 
#include <cmath>
#include <complex.h> // Add complex number support
#include <fftw3.h>  // for fourier transforms


Window::Window() : count(0)  //why do we still need gain? We can take it off
{
	//adcReader2->setup(); //set-up adcReader2 -:- do this from adcReader2
	
	// Set up the gain knob.
	knob_gain = new QwtKnob;
	setting_gain = 0;
	knob_gain->setScale(0, 7);
	knob_gain->setValue(0);
	connect( knob_gain, SIGNAL(valueChanged(double)), SLOT(setGain(double)) );

	// Set up the length knob.
	knob_length = new QwtKnob;
	setting_length = 5.0;
	knob_length->setScale(1, 10);
	knob_length->setValue(5);
	connect( knob_length, SIGNAL(valueChanged(double)), SLOT(setLength(double)) );
	// set up the initial plot data

	// DFT button.
	button_dft = new QCheckBox("Frequency Spectrum");
	dft_f = new double[1];
	dft_adc = new double[1];
	dft_iir = new double[1];
	dft_on = false;
	connect( button_dft, SIGNAL(stateChanged(int)), SLOT(dftMode(int)) );
	for( int index=0; index<plotDataSize; ++index )
	{
		xData[index] = index;
		yData[index] = gain * 1;
	}

	// Toggle between IIR values and ADC values
	button_toggle = new QCheckBox("IIR values");
	setting_toggle = false;
	connect( button_toggle, SIGNAL(stateChanged(int)), SLOT(IIRMode(int)) );
	
	// Plot data initalizatqframeion.
	plotResize = false;
	plotResample = false;
	plotFreq = 8.333333333;
	plotDataSize = 41;
	
	xData = new double[plotDataSize];
	yData = new double[plotDataSize];
	yData2 = new double[plotDataSize];
	for( uint32_t index = 0; index < plotDataSize; index+= 1 ){
		xData[index] = (plotDataSize - index - 1) * ( setting_length / (plotDataSize - 1.0) );
		yData[index] = 0;
		yData2[index] = 0;
	}

	//ADC value plot.
	curve = new QwtPlotCurve("ADC Value Curve");
	plot = new QwtPlot;
	// make a plot curve from the data and attach it to the plot
	curve->setSamples(xData, yData, plotDataSize);
	curve->attach(plot);
	plot->replot();
	plot->show();

	//IIR value plot.
	curve2 = new QwtPlotCurve("IIR Value Curve");
	plot2 = new QwtPlot;
	curve2->setSamples(xData, yData, plotDataSize);
	curve2->attach(plot);
	plot2->replot();
	plot2->show();

	plotBusy = false;

	// set up the layout - knob above thermometer
	vLayout = new QVBoxLayout;
	vLayout->addWidget(knob);
	vLayout->addWidget(knob_gain);
	vLayout->addWidget(knob_length);
	vLayout->addWidget(button_dft);

	// Plot 1 above Plot 2
	vLayout2 = new QVBoxLayout;
	vLayout2->addWidget(plot);
	vLayout2->addWidget(plot2);

	// Plots go to the right if knobs.
	hLayout = new QHBoxLayout;
	hLayout->addLayout(vLayout);
	hLayout->addLayout(vLayout2);

	setLayout(hLayout);

//	fprintf(stderr,"I am about to create adcReader2\n");//stderror prints to a different output than default printf 
	adcReader = new adcReader2();
	adcReader->start();
	//debug: 
//	fprintf(stderr,"I just created adcReader2\n"); 
	//end debug;
}

 Window::~Window() {
	// tells the thread to no longer run its endless loop
	//debug: 
//	fprintf(stderr,"I'm deleting adcReader\n");
	//end debug;
	adcReader->quit();
	// wait until the run method has terminated
	adcReader->wait();
	delete adcReader;
}

void Window::timerEvent( QTimerEvent * )
{
// For test

	// Check we are not modifying the plot more than once at a time (allows for frame skips if processor is busy).
		if( plotBusy ) return;
		plotBusy = true;
		
		if( plotResize ){
		resizePlots();
		plotResize = false;
		}

		if( plotResample ){
		resamplePlots();
		plotResample = false;
		}

        double inVal = 0;
        while(adcReader->hasSample())
//	{
        inVal = gain * (adcReader->getSample()); //gets sample from the buffer
		//currently it's not very complicated and readData is just a function without a class. 
		//double inVal = gain * adcReader2->readData(); //readData() is a function of adcReader2
		++count;
		//inVal = (1.25 + (1.25 * (inVal/32767)) - 0.75)/0.00125 - 502.5;
	
        	// add the new input to the plot
        	memmove( yData, yData+1, (plotDataSize-1) * sizeof(double) );//shift ydata buffer?
        	yData[plotDataSize-1] = inVal; //actual data value incoming
        	curve->setSamples(xData, yData, plotDataSize);  
        	plot->replot();

		inVal2 = gain * (adcReader->visualizeIIR()); //gets sample from the buffer
		++count;
        	// add the new input to the plot
        	memmove( yData2, yData2+1, (plotDataSize-1) * sizeof(double) );//shift ydata buffer?
        	yData2[plotDataSize-1] = inVal2; //actual data value incoming
        	curve->setSamples(xData, yData2, plotDataSize);  
        	plot->replot();


		if( dft_on ){
		// Some variables.
		fftw_plan dft;
		uint32_t freqs = plotDataSize / 2 + 1;
		fftw_complex* datafc1 = fftw_alloc_complex(freqs);
		fftw_complex* datafc2 = fftw_alloc_complex(freqs);

		// Transform current data into frequency space.
		dft = fftw_plan_dft_r2c_1d((int)plotDataSize, yData, datafc1, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT); // Raspberry can only estimate.
		fftw_execute(dft);
		fftw_destroy_plan(dft);

		if( !setting_toggle ){
			dft = fftw_plan_dft_r2c_1d((int)plotDataSize, yData2, datafc2, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT); // Raspberry can only estimate.
			fftw_execute(dft);
			fftw_destroy_plan(dft);
		}

		delete dft_f;
		delete dft_adc;
		delete dft_iir;

		dft_f = new double[freqs];
		dft_adc = new double[freqs];
		dft_iir = new double[freqs];

		double scale = (plotFreq / 2) / (double)(freqs - 1);
		for( uint32_t i = 0 ; i < freqs ; i+= 1 ){
			dft_f[i] = (double)i * scale;
			dft_adc[i] = cabs(datafc1[i]);
			dft_iir[i] = cabs(datafc2[i]);
		}

		fftw_free(datafc1);
		fftw_free(datafc2);

		curve->setSamples(dft_f, dft_adc, freqs);
		if( !setting_toggle ) curve2->setSamples(dft_f, dft_c2, freqs);
		}else{
		curve->setSamples(xData, yData, plotDataSize);
		if( !setting_toggle ) curve2->setSamples(xData, yData2, plotDataSize);
		}

		plot->replot();
		if( !setting_toggle ) plot2->replot();

		plotBusy = false;
}



// this function can be used to change the gain of the A/D internal amplifier
// void Window::setGain(double gain)
// {
// 	// for example purposes just change the amplitude of the generated input
// 	this->gain = gain;
// }

void Window::resamplePlots(  ){
	// Get channel frequency.
	uint32_t actual_freq = 1000000;
	//f1 = adcreader->getSampleRate(0);
	//f2 = adcreader->getSampleRate(1);

	// Calculate actual frequency (due to multi channel sampling) and samples needed.
	double f;
	//if( setting_toggle ){
	f = (double)actual_freq;
	//}else{
	//	f = 1.0 / ((1.0 / (double)f1 + 1.0 / (double)f2) * 3.0);
	//}

	plotFreq = f;
	uint32_t newsize = (uint32_t)(setting_length * f);

	// Resize if not already the required size.
	if( newsize != plotDataSize ){
		// Some variables.
		fftw_plan dft;
		uint32_t freqs = (newsize > plotDataSize ? newsize : plotDataSize) / 2 + 1;
		fftw_complex* dataf = fftw_alloc_complex(freqs);

		// Transform current data into frequency space.
		dft = fftw_plan_dft_r2c_1d((int)plotDataSize, yData, dataf, FFTW_ESTIMATE); // Raspberry can only estimate.
		fftw_execute(dft);
		fftw_destroy_plan(dft);
		delete[] yData;

		// Transform data into new sample space.
		for( uint32_t i = plotDataSize / 2 + 1 ; i < freqs ; i+= 1 ){
			dataf[i] = 0.0 + 0.0 * I;
		}
		yData = new double[newsize];
		dft = fftw_plan_dft_c2r_1d((int)newsize, dataf, yData, FFTW_ESTIMATE);
		fftw_execute(dft);
		fftw_destroy_plan(dft);

		// Normalize new samples.
		for( uint32_t i = 0 ; i < newsize ; i+= 1 ){
			yData[i]/= (double)newsize;
		}

		if( !setting_toggle ){
		//Repeat for channel 2.
			dft = fftw_plan_dft_r2c_1d((int)plotDataSize, yData2, dataf, FFTW_ESTIMATE); // Raspberry can only estimate.
			fftw_execute(dft);
			fftw_destroy_plan(dft);
		}
		delete[] yData2;

		for( uint32_t i = plotDataSize / 2 + 1 ; i < freqs ; i+= 1 ){
			dataf[i] = 0.0 + 0.0 * I;
		}
		yData2 = new double[newsize];
		dft = fftw_plan_dft_c2r_1d((int)newsize, dataf, yData2, FFTW_ESTIMATE);
		fftw_execute(dft);
		fftw_destroy_plan(dft);

		for( uint32_t i = 0 ; i < newsize ; i+= 1 ){
			yData2[i]/= (double)newsize;
		}

		// Finish off resize.
		fftw_free(dataf);
		plotDataSize = newsize;

		// Generate new scale.
		delete[] xData;
		xData = new double[plotDataSize];
		double scale = 1.0 / plotFreq; // Cached scale.
		for( uint32_t index = 0; index < plotDataSize; index+= 1 ){
			xData[index] = (plotDataSize - index - 1) * scale;
		}
	}
}

void Window::setGain(double gain)
{
	// Convert selection and update.
	uint8_t g = (uint8_t)(gain*7.0/10.0);
	if( g != setting_gain ){
		// Update filter selection.
		setting_gain = g;
		adcreader->setGain(g, 0);
		adcreader->setGain(g, 1);
		fprintf(stderr, "gain selection: %d\n", g);
	}
}

void Window::setLength(double length){
	// Convert selection and update.
	uint8_t select = (uint8_t)(length * 9.0 / 10.0 + 1.0);
	double newlength = (double)select;
	if( newlength != setting_length ){
		// Update length.
		setting_length = newlength;
		plotResize = true;
		fprintf(stderr, "length selection: %f seconds\n", newlength);
	}
}

void Window::dftMode(int state){
	fprintf(stderr, "dft mode: toggle\n");
	dft_on = !dft_on;
}

void Window::IIRMode(int state){
	fprintf(stderr, "IIR Values: toggle\n");
	setting_toggle = !setting_toggle;
	// if( setting_toggle ){
	// 	adcreader->visualizeIIR(true, 1);
	// }else{
	// 	adcreader->visualizeIIR(false, 1);
	// }
	// plotResample = true;
}

