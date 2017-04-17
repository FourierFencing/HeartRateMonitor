#ifndef WINDOW_H
#define WINDOW_H

#include <qwt/qwt_thermo.h>
#include <qwt/qwt_knob.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>

#include <QBoxLayout>
//#include <QCheckBox>

#include "adcReader2.h"
#include "iirThread.h"

// class definition 'Window'
class Window : public QWidget
{
	// must include the Q_OBJECT macro for for the Qt signals/slots framework to work with this class
	Q_OBJECT

public:
	Window(); // default constructor - called when a Window is declared without arguments

	~Window();

	void timerEvent( QTimerEvent * );
//	void resizePlots();
//	void resamplePlots();

public slots:
	void setGain(double gain);
//	void setLength(double length);
//	void dftMode(int state);
//	void IIRMode(int state);

// internal variables for the window class
private:
//	QwtKnob      *knob_gain;
//	QwtKnob      *knob_length;
//	QCheckBox    *button_dft;
//	QCheckBox    *button_toggle;
	QwtPlot      *adc_plot;
	QwtPlot      *iir_plot;
	QwtPlotCurve *adc_curve;
	QwtPlotCurve *iir_curve;

	// layout elements from Qt itself http://qt-project.org/doc/qt-4.8/classes.html
	QVBoxLayout  *vLayout;  // vertical layout
//	QVBoxLayout  *vLayout2;  // vertical layout2
	QHBoxLayout  *hLayout;  // horizontal layout

	static const int plotDataSize = 300;

	// // data arrays for the plot
	double adc_xData[plotDataSize];
	double adc_yData[plotDataSize];
	double iir_xData[plotDataSize];
	double iir_yData[plotDataSize];
//	uint8_t setting_gain;
//	double setting_length; // The length of the plot in seconds.
//	bool setting_toggle;
//	bool plotResize;       // Orders the update thread to resize plot.
//	bool plotResample;       // Orders the update thread to resize plot.
//	double plotFreq;
//	volatile bool plotBusy; // The plot is in the process of bing manipulated. 
//	uint32_t plotDataSize; // The number of samples.

	// data arrays for the plot
//	double* xData;
//	double* yData;
//	double* yData2;
//
//	double* dft_f;
//	double* dft_adc;
//	double* dft_iir;
//	bool dft_on;

	double gain;
	int count;

	//adcReader2 *adcReader;//adcReader is a pointer of type adcReader2 
	iirThread *IirThread;//IirThread is a pointer of type iirThread
	//this is the same as going
	//int *sample
};

#endif // WINDOW_H
