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

public slots:
	void setGain(double gain);


// internal variables for the window class
private:
	QwtPlot      *adc_plot;
	QwtPlot      *iir_plot;
	QwtPlotCurve *adc_curve;
	QwtPlotCurve *iir_curve;

	// layout elements from Qt itself http://qt-project.org/doc/qt-4.8/classes.html
	QVBoxLayout  *vLayout;  // vertical layout
	QHBoxLayout  *hLayout;  // horizontal layout

	static const int plotDataSize = 300;

	// // data arrays for the plot
	double adc_xData[plotDataSize];
	double adc_yData[plotDataSize];
	double iir_xData[plotDataSize];
	double iir_yData[plotDataSize];

	double gain;
	int count;

	//adcReader2 *adcReader;//adcReader is a pointer of type adcReader2 
	iirThread *IirThread;//IirThread is a pointer of type iirThread
	//this is the same as going
	//int *sample
};

#endif // WINDOW_H
