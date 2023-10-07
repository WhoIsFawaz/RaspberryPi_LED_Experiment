# RaspberryPi LED Blinker

![Raspberry Pi](https://www.raspberrypi.org/app/uploads/2011/10/Raspi-PGB001.png)

Control and experiment with LED blinking using a Raspberry Pi. This project is written in C and utilizes the WiringPi library for GPIO control. It offers a range of features for controlling LEDs, including turning LEDs on and off, customizing blinking patterns with adjustable frequency and brightness, and recording waveform data for analysis.

## Features

- Turn on and off both green and red LEDs independently.
- Customize LED blinking by selecting the LED to blink, setting the frequency, and adjusting brightness.
- Record and store waveform data, including timestamps and LED states, in CSV files.
- Experiment with LED blinking patterns to understand the impact of frequency and brightness.
- User-friendly command-line interface for easy interaction.

## Getting Started

### Prerequisites

Before you begin, make sure you have:

- A Raspberry Pi running Raspbian or a compatible operating system.
- WiringPi library installed on your Raspberry Pi. You can follow the installation instructions [here](https://learn.sparkfun.com/tutorials/raspberry-gpio/c-wiringpi-setup).
- GNUPlot library installed on your Windows/Linux Machine. You can follow the installation instructions [here](http://www.gnuplot.info/).

### Installation

1. Clone this repository to your Raspberry Pi:

   ```shell
   git clone https://github.com/WhoIsFawaz/RaspberryPi_LED_Experinment.git

### How to use
1. On your Rasberry Pi, enter the following commands to compile and start the NewStudent.c file.
   >gcc -o NewStudent NewStudent.c -lwiringPi
   
   >./NewStudent
   
2. Follow the on screen instructions to get the output you want.
3. Once the process has finished, 2 CSV files will be created for the Green Led and Red Led respectively.
4. You can SCP the file over from your Rasberry Pi to your local host.
   >scp pi@raspberrypi.local:/path/to/example.txt ~/Downloads/
5. On Visual Studio Code Editior, you can then save the 2 CSV files together with the DisplayPlot.c file.
6. To visualise the graph in the pictorial version, enter the following codes in the VSC Terminal.
   >gcc -o DisplayPlot DisplayPlot.c
   >.\DisplayPlot  
7. With that, an Waveform.png file will be created which will show the dataset in a Data Analyst POV!

**_Have fun and happy learning!!!_**
   
