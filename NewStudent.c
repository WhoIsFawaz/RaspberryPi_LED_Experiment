/* 
=== HOW TO RUN ===
Step 1: cd into C file location
Step 2: gcc -o NewStudent NewStudent.c -lwiringPi
Step 3: ./NewStudent

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
*/

#include <wiringPi.h>
#include <softPwm.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definitions
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

// Function Prototypes
void setupProgram();
void startProgram();
int getUserSelection();
void turnOffLeds();
void turnOnLeds();
void blink();
int getBlinkLed();
int getBlinkFrequency();
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

/* Add a function to write waveform data of GREEN LED to the file */
void writeWaveformDataGreen(WaveformData data, int blinkFrequency, int blinkBrightness) {
    FILE *file = fopen(WAV_FILEGreen, "a"); // Open file in append mode

    if (file != NULL) {
        if (!headersWrittenGreen) {
            fprintf(file, "Frequency of Green LED is: %dHz & Duty Cycle of Green LED is: %d%%\n\n", blinkFrequency, blinkBrightness); 
            fprintf(file, "The timestamp in Millisecond | The state of the Green LED\n");
            headersWrittenGreen = 1;
        }
        fprintf(file, "              %d          ,             %d\n", data.timestamp, data.greenState);
        fclose(file);
    }
}

/* Add a function to write waveform data of RED LED to the file */
void writeWaveformDataRed(WaveformData data, int blinkFrequency, int blinkBrightness) {
    FILE *file = fopen(WAV_FILERed, "a"); // Open file in append mode

    if (file != NULL) {
        if (!headersWrittenRed) {
            fprintf(file, "Frequency of Red LED is: %dHz & Duty Cycle of Red LED is: %d%%\n\n", blinkFrequency, blinkBrightness); 
            fprintf(file, "The timestamp in Millisecond | The state of the Red LED\n");
            headersWrittenRed = 1;
        }
        fprintf(file, "              %d          ,             %d\n", data.timestamp, data.redState);
        fclose(file);
    }
}

// Main Programme
int main(void) {
    setupProgram();
    startProgram();
    endProgram();
    return 0;
}

// Sets up the LED GPIO pins as output and PWM
void setupProgram() {
    wiringPiSetupGpio();
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    softPwmCreate(GREEN, 0, 100);
    softPwmCreate(RED, 0, 100);
    system("clear");
}

// Takes the input of the user selection and directs it to different states of the program
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
}

// The main menu that determines what the user wants to do 
int getUserSelection() {
    int selection;
    printf(" ____    ___         _____    _____    ____    ___   \n");
    printf("|    |      |       |        |     |  |    |      |  \n");
    printf("|____|   ___|       |  ___   |_____|  |____|   ___|  \n");
    printf("|       |           |     |  |    \\   |       |      \n");
    printf("|       |___        |_____|  |     \\  |       |___   \n");

    printf("\n===== LAD STUDENT DEVICE =====\n");
    printf("\n[0] Turn off both LEDs\n");
    printf("[1] Turn on both LEDs\n");
    printf("[2] Blink LED\n");
    printf("[3] Exit\n");
    printf("\nYour Selection: ");
    scanf("%d", &selection);
    return selection;
}

// For troubleshooting, turning off LEDs and PWM. Use this to test the connection of your LED and Pi
void turnOffLeds() {
    system("clear");
    printf("\nTurning off both LEDs...\n");
    digitalWrite(GREEN, LOW);
    softPwmWrite(GREEN, 0);
    digitalWrite(RED, LOW);
    softPwmWrite(RED, 0);
}

// For troubleshooting, turning on LEDs and PWM. Use this to test the connection of your LED and Pi
void turnOnLeds() {
    system("clear");
    printf("\nTurning on both LEDs...\n");
    digitalWrite(GREEN, HIGH);
    softPwmWrite(GREEN, 100);
    digitalWrite(RED, HIGH);
    softPwmWrite(RED, 100);
}

// When user wants to blink LED, this function will get all the blinking configurations
void blink() {
    system("clear");
    printf("\nBlink...\n");
    int blinkLed = getBlinkLed();
    int frequency = getBlinkFrequency();
    int brightness = getBlinkBrightness();

    if (confirmBlinkSelection(blinkLed, frequency, brightness) == CONFIRM) {
        blinkLedWithConfig(blinkLed, frequency, brightness);
        system("clear");
    } else return;
}

// Menu to get user selection on which LED to blink
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

// Menu to get user selection on LED Frequency
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

// Menu to get user selection on LED Brightness/Duty Cycle
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

// Menu for the user to acknowledge the blink configurations
int confirmBlinkSelection(int blinkLed, int blinkFrequency, int blinkBrightness) {
    int selection;
    char blinkLedString[] = "Green";

    if (blinkLed == BLINK_RED) {
        strcpy(blinkLedString, "Red");
    }

    printf("Confirm your blink configurations.\n\n");
    printf("LED to blink: %s\n", blinkLedString);
    printf("Blink Frequency: %dHz\n", blinkFrequency);
    printf("Blink Brightness: %d%%\n\n", blinkBrightness);
    printf("[1] Confirm Configuration\n");
    printf("[0] Return to Home\n");
    printf("\nYour Selection: ");

    scanf("%d", &selection);

    if (selection < 0 || selection > 1) {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        confirmBlinkSelection(blinkLed, blinkFrequency, blinkBrightness);
    } else {
        return selection;
    }
}

// Blinks the LED according to the user configuration
void blinkLedWithConfig(int blinkLed, int blinkFrequency, int blinkBrightness) {
    printf("\nBlinking...\n");

    // Calculate onTime and offTime based on the duty cycle (brightness)
    float dutyCycle = (float)blinkBrightness / 100.0f;      // Calculate duty cycle based on the user's input (brightness)
    float cycleTime = 1.0f / blinkFrequency;                // Calculate the time for one cycle based on the user's input (frequency)
    float onTime = cycleTime * dutyCycle * 1000;            // Calculate the on-time in milliseconds based on duty cycle
    float offTime = cycleTime * (1.0f - dutyCycle) * 1000;  // Calculate the off-time in milliseconds based on duty cycle

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
    unsigned long greenTimestamp = 10000;  // Start at 10,000 ms - this will ensure neater representation of the recorded data in the csv
    unsigned long redTimestamp = 10000;    // Start at 10,000 ms - this will ensure neater representation of the recorded data in the csv
    unsigned long timestampLimit = 70000;  // Set the timestamp limit to 70,000 ms - this will ensure the LED will run for 60 seconds

    // Blinking
    unsigned long previousMillis = 0;
    int ledState = LOW;

    // Inside the while loop to control LED blinking
    while (1) {
        unsigned long currentMillis = millis();

        // Check if it's time to change the LED state (on or off)
        if (currentMillis - previousMillis >= (ledState == LOW ? offTime : onTime)) {
            previousMillis = currentMillis;
            ledState = (ledState == LOW) ? HIGH : LOW;                      // Toggle the LED state between LOW and HIGH
            softPwmWrite(blinkLed, ledState == HIGH ? blinkBrightness : 0); // Set the LED brightness based on its state
            digitalWrite(blinkLed, ledState);                               // Update the physical state of the LED

            // Record waveform data
            data.timestamp = (blinkLed == GREEN) ? greenTimestamp : redTimestamp;

            if (blinkLed == GREEN) {
                data.greenState = ledState;                                    // If the green LED is blinking, update the green LED state in the waveform data
                writeWaveformDataGreen(data, blinkFrequency, blinkBrightness); // Record green led data to csv file
                greenTimestamp += (ledState == HIGH) ? onTime : offTime;       // Update the green LED timestamp
            } else {
                data.redState = ledState;                                      // If the red LED is blinking, update the red LED state in the waveform data
                writeWaveformDataRed(data, blinkFrequency, blinkBrightness);   // Record red led data to csv file
                redTimestamp += (ledState == HIGH) ? onTime : offTime;         // Update the red LED timestamp
            }

            // Check if the timestamp limit is reached, and exit the loop if true
            if (data.timestamp >= timestampLimit) {
                break;
            }
        }
    }
}

// Resetting and cleaning up before safely exiting the program
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
