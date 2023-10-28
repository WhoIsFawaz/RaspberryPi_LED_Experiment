/* 
=== HOW TO RUN ===
Step 1: cd into C file location
Step 2: gcc -o student student.c -lwiringPi
Step 3: ./student

=== PRE-REQUISITES ===
Install wiringPi: https://learn.sparkfun.com/tutorials/raspberry-gpio/c-wiringpi-setup
softPwm is installed with wiringPi

=== USEFUL COMMANDS ===
Check wiringPi version: gpio -v
Check GPIO status: gpio readall

=== GPIO PIN CONNECTION ===
27 GREEN LED
13 RED LED
GROUND

GPIO14 to Monitor GPIO15
GPIO15 to Monitor GPIO14
GROUND

=== RASPBERRY PI VERSION ===
Check Version Command: cat /etc/os-release

PRETTY_NAME="Raspbian GNU/Linux 10 (buster)"
NAME="Raspbian GNU/Linux"
VERSION_ID="10"
VERSION="10 (buster)"
VERSION_CODENAME=buster
*/

#include <wiringPi.h>
#include <softPwm.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringSerial.h>
#include <unistd.h>

/* DEFINITIONS */
#define RED 27      // GPIO Pin 27
#define GREEN 13    // GPIO Pin 13

// Program States
#define TURN_OFF 0
#define TURN_ON 1
#define BLINK 2
#define EXIT 3

// LED Blink Selection
#define BLINK_GREEN 1
#define BLINK_RED 2
#define CONFIRM 1

// File to store waveform data
#define WAV_FILEGreen "green_waveform_data.csv"
#define WAV_FILERed "red_waveform_data.csv"

/* FUNCTION PROTOTYPES */
void setupProgram();
void startProgram();
int getUserSelection();
void turnOffLeds();
void turnOnLeds();
void blink();
int getBlinkLed();
int getBlinkFrequency();
int getBlinkDutyCycle();
int getBlinkBrightness();
int confirmBlinkSelection();
void blinkLedWithConfig();
void endProgram();

// Define a structure to hold waveform data
int headersWrittenGreen = 0;
int headersWrittenRed = 0;
typedef struct {
    int timestamp;  // Time in milliseconds
    int greenState; // State of the green LED
    int redState;   // State of the red LED
} WaveformData;

/* ... (rest of the code remains unchanged) ... */

// Add a function to write waveform data of GREEN LED to the file
void writeWaveformDataGreen(WaveformData data, int blinkFrequency, int blinkDutyCycle) {
    FILE *file = fopen(WAV_FILEGreen, "a"); // Open file in append mode

    if (file != NULL) {
        if (!headersWrittenGreen) {
            fprintf(file, "Frequency of Green LED is: %dHz & Duty Cycle of Green LED is: %d%\n\n", blinkFrequency, blinkDutyCycle); 
            fprintf(file, "The timestamp in Millisecond | The state of the Green LED | The state of the Red LED\n");
            headersWrittenGreen = 1;
    }
            fprintf(file,  "              %d          ,             %d              ,            %d\n", data.timestamp, data.greenState, data.redState);
    fclose(file);
    }
}

// Add a function to write waveform data of RED LED to the file
void writeWaveformDataRed(WaveformData data, int blinkFrequency, int blinkDutyCycle) {
    FILE *file = fopen(WAV_FILERed, "a"); // Open file in append mode

    if (file != NULL) {
        if (!headersWrittenRed) {
            fprintf(file, "Frequency of Red LED is: %dHz & Duty Cycle of Red LED is: %d%\n\n", blinkFrequency, blinkDutyCycle); 
            fprintf(file, "The timestamp in Millisecond | The state of the Green LED | The state of the Red LED\n");
            headersWrittenRed = 1;
    }
            fprintf(file,  "              %d          ,             %d              ,            %d\n", data.timestamp, data.greenState, data.redState);
    fclose(file);
    }
}

/* MAIN PROGRAM */
int main(void) {

    setupProgram();
    startProgram();
    endProgram();
    return 0;
}

/* 
Sets up the LED GPIO pins as output and PWM
*/
void setupProgram() {
    wiringPiSetupGpio();
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    softPwmCreate(GREEN, 0, 100);
    softPwmCreate(RED, 0, 100);
    system("clear");
}

/* 
Takes the input of the user selection and direct it to different states of the program
*/
void startProgram() {

    int selection;

    do {
        selection = getUserSelection();

        switch(selection) {

            case TURN_OFF:
                turnOffLeds();
                break;
            case TURN_ON:
                turnOnLeds();
                break;
            case BLINK:
                blink();
                break;
            case EXIT:
                break;
            default:
                printf("\nInvalid Input. Try Again...\n");
                break;
        }

    } while (selection != EXIT);
    
    return;
}

/* 
The main menu that determines what the user wants to do
*/
int getUserSelection() {

    int selection;

    printf("\n===== LED STUDENT DEVICE =====\n");
    printf("\n[0] Turn off both LEDs\n");
    printf("[1] Turn on both LEDs\n");
    printf("[2] Blink LED\n");
    printf("[3] Exit\n");
    printf("\nYour Selection: ");

    scanf("%d", &selection);

    return selection;
}

/* 
For troubleshooting, turning off LEDs and PWM
*/
void turnOffLeds() {
    system("clear");
    printf("\nTurning off both LEDs...\n");
    digitalWrite(GREEN, LOW);
    softPwmWrite(GREEN, 0);
    digitalWrite(RED, LOW);
    softPwmWrite(RED, 0);
}

/* 
For troubleshooting, turning on LEDs and PWM at 100
*/
void turnOnLeds() {
    system("clear");
    printf("\nTurning on both LEDs...\n");
    digitalWrite(GREEN, HIGH);
    softPwmWrite(GREEN, 100);
    digitalWrite(RED, HIGH);
    softPwmWrite(RED, 100);
}

/* 
When user wants to blink LED, this function will get all the blinking configuration
It gets from the user the LED to blink, frequency and brightness.
*/
void blink() {
    system("clear");
    printf("\nBlink...\n");
    
    int blinkLed = getBlinkLed();
    int frequency = getBlinkFrequency();
    int dutyCycle = getBlinkDutyCycle();
    int brightness = getBlinkBrightness();

    if (confirmBlinkSelection(blinkLed, frequency, dutyCycle, brightness) == CONFIRM) {

        blinkLedWithConfig(blinkLed, frequency, dutyCycle, brightness);
        system("clear");

    } else return;

}

/* 
Menu to get user selction on LED to blink
*/
int getBlinkLed() {

    int selection;

    printf("\nSelect LED to blink.\n\n");
    printf("[1] Green LED\n");
    printf("[2] Red LED\n");
    printf("\nYour Selection: ");

    scanf("%d", &selection);

    if (selection != BLINK_GREEN && selection != BLINK_RED) {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        getBlinkLed();
    } else {
        system("clear");
        return selection;
    }
}

/* 
Menu to get user selction on Frequency to blink
*/
int getBlinkFrequency() {

    int selection;

    printf("Enter frequency to blink.\n\n");
    printf("Enter whole numbers between 0 to 10\n");
    printf("\nFrequency (Hz): ");

    scanf("%d", &selection);

    if (selection < 0 || selection > 10) {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        getBlinkFrequency();
    } else {
        system("clear");
        return selection;
    }
}

/* 
Menu to get user selction for duty cycle for each blink
*/
int getBlinkDutyCycle() {

    int selection;

    printf("Enter the duty cycle for each blink.\n\n");
    printf("Enter percentage between 0 to 100\n");
    printf("\nDuty Cycle (%%): ");

    scanf("%d", &selection);

    if (selection < 0 || selection > 100) {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        getBlinkDutyCycle();
    } else {
        system("clear");
        return selection;
    }
}

/* 
Menu to get user selction on LED brightness
*/
int getBlinkBrightness() {

    int selection;

    printf("Select LED brightness during blink.\n\n");
    printf("Enter whole numbers between 0 to 100\n");
    printf("Brightness (%%): ");

    scanf("%d", &selection);

    if (selection < 0 || selection > 100) {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        getBlinkBrightness();
    } else {
        system("clear");
        return selection;
    }

}

/* 
Menu for user to acknowldge the blink configurations input
*/
int confirmBlinkSelection(int blinkLed, int blinkFrequency, int blinkDutyCycle, int blinkBrightness) {
    
    int selection;
    char blinkLedString[] = "Green";

    if (blinkLed == BLINK_RED) {
        strcpy(blinkLedString, "Red");
    }

    printf("Confirm your blink configrations.\n\n");
    printf("LED to blink: %s\n", blinkLedString);
    printf("Blink Frequency: %dHz\n", blinkFrequency);
    printf("Blink Duty Cycle: %d%%\n", blinkDutyCycle);
    printf("Blink Brightness: %d%%\n\n", blinkBrightness);
    printf("[1] Confirm Configuration\n");
    printf("[0] Return to Home\n");
    printf("\nYour Selection: ");

    scanf("%d", &selection);

    if (selection < 0 || selection > 1) {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        confirmBlinkSelection(blinkLed, blinkFrequency, blinkDutyCycle, blinkBrightness);
    } else {
        return selection;
    }
}

/* 
Blinks the LED according to the user configuration
*/
// Modify blinkLedWithConfig to record waveform data
void blinkLedWithConfig(int blinkLed, int blinkFrequency, int blinkDutyCycle, int blinkBrightness) {
    
    printf("\nBlinking...\n");

    // Setting Frequency
    float frequencyDuration = 1.0f / blinkFrequency * 1000;
    float onDuration = (frequencyDuration / 100) * blinkDutyCycle;

    // Setting Blink LED
    if (blinkLed == BLINK_GREEN) {
        blinkLed = GREEN;
    } else {
        blinkLed = RED;
    }

    // Initialize waveform data
    WaveformData data;
    data.greenState = 0;
    data.redState = 0;
    
    // Initialize timestamps for both LEDs
    unsigned long greenTimestamp = 10000;  // Start at 10000 ms
    unsigned long redTimestamp = 10000;    // Start at 10000 ms

    // Blinking
    unsigned long previousMillis = 0;
    int ledState = LOW;
    
    int loopDuration = 7 * blinkFrequency;
        
    for (int blink = 0; blink < loopDuration;) {
        unsigned long currentMillis = millis();

        if (ledState == LOW) {
            printf("led state low\n");
            printf("current time (%i) - previous time (%i) >= on duration\n", currentMillis, previousMillis, onDuration);
            if (currentMillis - previousMillis >= frequencyDuration) {
                printf("new freequency cycle\n");
                previousMillis = currentMillis;

                ledState = HIGH;
                softPwmWrite(blinkLed, blinkBrightness);
                printf("led on\n");
                
                blink++;
                digitalWrite(blinkLed, ledState);

                // Record waveform data for every 20ms
                for (int step = 0; step < frequencyDuration; step += 20) {
                    data.timestamp = (blinkLed == GREEN) ? greenTimestamp : redTimestamp;
                    if (blinkLed == GREEN) {
                        data.greenState = ledState;
                        writeWaveformDataGreen(data, blinkFrequency, blinkDutyCycle);
                        greenTimestamp += 20;
                    } else {
                        data.redState = ledState;
                        writeWaveformDataRed(data, blinkFrequency, blinkDutyCycle);
                        redTimestamp += 20;
                    }
                }
            }
        } else {
            printf("led state high\n");
            printf("current time (%i) - previous time (%i) >= on duration\n", currentMillis, previousMillis, onDuration);
            if (currentMillis - previousMillis >= onDuration) {
                printf("new freequency cycle\n");
                ledState = LOW;
                softPwmWrite(blinkLed, blinkBrightness);
                printf("led off\n");
            }
        }
    }

    // for (int blink = 0; blink < loopDuration;) {
    //     unsigned long currentMillis = millis();

    //     if (currentMillis - previousMillis >= onOffTime) {
    //         previousMillis = currentMillis;
    //         if (ledState == LOW) {
    //             ledState = HIGH;
    //             softPwmWrite(blinkLed, blinkBrightness);
    //         } else {
    //             ledState = LOW;
    //             softPwmWrite(blinkLed, 0);
    //         }
    //         blink++;
    //         digitalWrite(blinkLed, ledState);

    //         // Record waveform data for every 20ms
    //         for (int step = 0; step < onOffTime; step += 20) {
    //             data.timestamp = (blinkLed == GREEN) ? greenTimestamp : redTimestamp;
    //             if (blinkLed == GREEN) {
    //                 data.greenState = ledState;
    //                 writeWaveformDataGreen(data, blinkFrequency, blinkDutyCycle);
    //                 greenTimestamp += 20;
    //             } else {
    //                 data.redState = ledState;
    //                 writeWaveformDataRed(data, blinkFrequency, blinkDutyCycle);
    //                 redTimestamp += 20;
    //             }
    //         }
    //     }
    // }
}

/* 
Resetting and cleaning up before safely exiting the program.
*/
void endProgram() {
    system("clear");
    printf("\nCleaning Up...\n");
    // Turn Off LEDs
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, LOW);

    // Turn Off LED Software PWM
    softPwmWrite(GREEN, 0);
    softPwmWrite(RED, 0);

    // Reset Pins to Original INPUT State
    pinMode(GREEN, INPUT);
    pinMode(RED, INPUT);

    printf("Bye!\n\n");
}
