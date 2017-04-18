# Real Time Embedded Programming - Team 8

This is the code to for Fourier Fencing Systems. This project makes use of a Raspberry Pi and a MCP3008 ADC to detect certain frequecies sent from a transmitter. An LED lights up when the frewuency is detected.
The raw data gotten from the ADC is displayed on QT as well as the IIR frequency component and when a required frequency is detected, a signal is sent via Bluetooth to an Android Device. The Android code is in a different repo -https://github.com/FourierFencing/fourierfencingclient.


## Requirements

1. A Raspberry Pi
2. The git commandline tool on the RPi
3. MCP3008 ADC
4. An LED
5. Android Studio and a debugging device

## Installation

From the RPi, clone this archive:

```git clone https://github.com/FourierFencing/Team8-FFS.git```

Open the directory

```cd Team8-FFS```

Then compile the code

```qmake QTTest2.pro```


```make```


## Usage

### Displaying on QT

1. ```startx ./Team8-FFS```

### Displaying on Phone via Bluetooth

1. ```startx ./Team8-FFS```
2. Start the Fourier Fencing app on Android device and connect to "raspberry pi"


## Sources

**Code adapted from arudino community**

-https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino


**Previous projects proved useful**

-https://github.com/tianyiran02/RTEM_weightscale

-https://github.com/katterek/MultiADC


**Bernd Porr's GitHub also useful**

-https://github.com/berndporr/qwt-example

-https://github.com/berndporr/rpi_AD7705_test_software


**Bernd Porr's IIR library GitHub**

-https://github.com/berndporr/iir1


**Nice source for SPI**

-http://www.raspberry-projects.com/pi/programming-in-c/spi/using-the-spi-interface

