# Real Time Embedded Programming - Team 8

This is the code to an heart rate monitor project. This project makes use of a Raspberry Pi, a pulse sensor and a MCP3008 ADC to measure the heart rate of a user. An LED pulses with the users heart beat.
The raw data gotten from the ADC is displayed on QT and the final calculated Beats Per Minute is sent via Bluetooth to an Android Device. The Android code is in the Android Studio folder.


## Requirements

1. A Raspberry Pi
2. The git commandline tool on the RPi
3. MCP3008 ADC
4. An LED
5. Android Studio and a debugging device

## Installation

From the RPi, clone this archive:
```git clone https://github.com/FourierFencing/HeartRateMonitor.git```

Open the directory
```cd HeartRateMonitor```

Then compile the code
```qmake```
```make```


## Usage

### Displaying on QT

1. ```startx ./HeartRateMonitor```

### Displaying on Phone via Bluetooth

1. ```startx ./HeartRateMonitor```
2. Start the Heart Rate Monitor app on Android device and connect to "raspberry pi"
