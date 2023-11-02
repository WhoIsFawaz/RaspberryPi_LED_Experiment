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
#define NUMBER_OF_LEDS 2

#define RED 27      // GPIO Pin 27
#define GREEN 13    // GPIO Pin 13

#define TIMESTAMP_START 10000   // Start of timestamp
#define BLINK_DURATION 60       // Duration of in seconds

// Program States
#define TURN_OFF 0
#define TURN_ON 1
#define BLINK 2
#define BLINK_ALL 3
#define EXIT 4

// LED Blink Selection/position in array
#define BLINK_GREEN 0
#define BLINK_RED 1
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
void blinkAll();
int getBlinkLed();
int getBlinkFrequency();
int getBlinkDutyCycle();
int getBlinkBrightness();
int confirmBlinkSelection();
void blinkLedsWithConfig();
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
            file = freopen(NULL, "w", file);
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
            file = freopen(NULL, "w", file);
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
            case BLINK_ALL:
                blinkAll();
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
    printf(" ____   ____         ____     ____     ____    ____   \n");
    printf("|    |      |       |        |    |   |    |       |  \n");
    printf("|____|   ___|       |  ___   |____|   |____|    ___|  \n");
    printf("|      |            |     |  |    \\   |       |      \n");
    printf("|      |____        |_____|  |     \\  |       |____   \n");

    printf("\n===== LED STUDENT DEVICE =====\n");
    printf("\n[0] Turn off all LEDs\n");
    printf("[1] Turn on all LEDs\n");
    printf("[2] Blink LED\n");
    printf("[3] Blink all LEDs\n");
    printf("[4] Exit\n");
    printf("\nYour Selection: ");
    scanf("%d", &selection);
    return selection;
}

// For troubleshooting, turning off LEDs and PWM. Use this to test the connection of your LED and Pi
void turnOffLeds() {
    system("clear");
    printf("\nTurning off all LEDs...\n");
    digitalWrite(GREEN, LOW);
    softPwmWrite(GREEN, 0);
    digitalWrite(RED, LOW);
    softPwmWrite(RED, 0);
}

// For troubleshooting, turning on LEDs and PWM. Use this to test the connection of your LED and Pi
void turnOnLeds() {
    system("clear");
    printf("\nTurning on all LEDs...\n");
    digitalWrite(GREEN, HIGH);
    softPwmWrite(GREEN, 100);
    digitalWrite(RED, HIGH);
    softPwmWrite(RED, 100);
}

// When user wants to blink LED, this function will get all the blinking configurations
void blink() {
    system("clear");
    printf("\nBlink...\n");
    int frequencies[NUMBER_OF_LEDS] = {0, 0};
    int brightness[NUMBER_OF_LEDS] = {0, 0};
    int dutyCycles[NUMBER_OF_LEDS] = {0, 0};

    int blinkLed = getBlinkLed();
    frequencies[blinkLed] = getBlinkFrequency(blinkLed);
    dutyCycles[blinkLed] = getBlinkDutyCycle(blinkLed);
    brightness[blinkLed] = getBlinkBrightness(blinkLed);

    if (confirmBlinkSelection(frequencies, dutyCycles, brightness) == CONFIRM) {
        blinkLedsWithConfig(frequencies, dutyCycles, brightness);
        system("clear");
    } else return;
}

// When user wants to blink all LEDs, this function will get all the blinking configurations
void blinkAll() {
    system("clear");
    printf("\nBlink all LEDs...\n");
    int frequencies[NUMBER_OF_LEDS] = {0, 0};
    int dutyCycles[NUMBER_OF_LEDS] = {0, 0};
    int brightness[NUMBER_OF_LEDS] = {0, 0};

    for (int i = 0; i < NUMBER_OF_LEDS; i++) {
        frequencies[i] = getBlinkFrequency(i);
        dutyCycles[i] = getBlinkDutyCycle(i);
        brightness[i] = getBlinkBrightness(i);
    }

    if (confirmBlinkSelection(frequencies, dutyCycles, brightness) == CONFIRM) {
        blinkLedsWithConfig(frequencies, dutyCycles, brightness);
        system("clear");
    } else return;
}

// Menu to get user selection on which LED to blink
int getBlinkLed() {
    int selection;
    printf("\nSelect LED to blink.\n\n");
    printf("[0] Green LED\n");
    printf("[1] Red LED\n");
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
int getBlinkFrequency(int blinkLed) {
    int selection;
    if (blinkLed == BLINK_GREEN) {
        printf("Enter frequency to blink green LED.\n\n");
    } else {
        printf("Enter frequency to blink red LED.\n\n");
    }
    printf("Enter whole numbers between 0 to 10\n\n");
    printf("Frequency (Hz): ");
    scanf("%d", &selection);
    
    if (selection < 0 || selection > 10) {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        getBlinkFrequency(blinkLed);
    } else {
        system("clear");
        return selection;
    }
}

// Menu to get user selection on LED Duty Cycle
int getBlinkDutyCycle(int blinkLed) {
    int selection;
    if (blinkLed == BLINK_GREEN) {
        printf("Enter duty cycle for green LED.\n\n");
    } else {
        printf("Enter duty cycle for red LED.\n\n");
    }
    printf("Enter whole numbers between 0 to 100\n\n");
    printf("Duty Cycle (%%): ");
    scanf("%d", &selection);
    
    if (selection < 0 || selection > 100) {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        getBlinkDutyCycle(blinkLed);
    } else {
        system("clear");
        return selection;
    }
}

// Menu to get user selection on LED Brightness/Duty Cycle
int getBlinkBrightness(int blinkLed) {
    int selection;
    if (blinkLed == BLINK_GREEN) {
        printf("Enter brightness to blink green LED.\n\n");
    } else {
        printf("Enter brightness to blink red LED.\n\n");
    }
    printf("Enter whole numbers between 0 to 100\n\n");
    printf("Brightness (%%): ");
    scanf("%d", &selection);
    
    if (selection < 0 || selection > 100) {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        getBlinkBrightness(blinkLed);
    } else {
        system("clear");
        return selection;
    }
}

// Updated function to confirm the configuration for all LEDs
int confirmBlinkSelection(int frequencies[NUMBER_OF_LEDS], int dutyCycles[NUMBER_OF_LEDS], int brightness[NUMBER_OF_LEDS]) {
    int selection;

    printf("Confirm your blink configurations for the LEDs.\n\n");
    for (int i = 0; i < NUMBER_OF_LEDS; i++) {
        if (frequencies[i] != 0 && brightness[i] != 0) {
            char blinkLedString[] = "Green";
            if (i == BLINK_RED) {
                strcpy(blinkLedString, "Red");
            }
            printf("%s LED\n", blinkLedString);
            printf("  - Frequency: %dHz\n", frequencies[i]);
            printf("  - Duty Cycle: %dHz\n", dutyCycles[i]);
            printf("  - Brightness: %d%%\n", brightness[i]);
        }
    }
    printf("[1] Confirm Configuration\n");
    printf("[0] Return to Home\n");
    printf("\nYour Selection: ");

    scanf("%d", &selection);

    if (selection < 0 || selection > 1) {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        confirmBlinkSelection(frequencies, dutyCycles, brightness);
    } else {
        return selection;
    }
}

// Blinks all the LED according to the user configuration
void blinkLedsWithConfig(int frequencies[NUMBER_OF_LEDS], int dutyCycles[NUMBER_OF_LEDS], int brightness[NUMBER_OF_LEDS]) {
    printf("\nBlinking...\n");

    int onTimes[NUMBER_OF_LEDS] = {0, 0};
    int offTimes[NUMBER_OF_LEDS] = {0, 0};

    // Calculate onTime and offTime based on the duty cycle (brightness)
    for (int i = 0; i < NUMBER_OF_LEDS; i++) {
        float dutyCycle = (float)dutyCycles[i] / 100.0f;                    // Calculate duty cycle based on the user's input (brightness)
        float cycleTime = (1.0f / frequencies[i]) * 1000.0f;                // Calculate the time for one cycle based on the user's input (frequency)
        onTimes[i] = cycleTime * dutyCycle;                                 // Calculate the on-time in milliseconds based on duty cycle
        offTimes[i] = cycleTime * (1.0f - dutyCycle);                       // Calculate the off-time in milliseconds based on duty cycle
    }

    // Initialize waveform data
    WaveformData data;
    data.greenState = 0;
    data.redState = 0;

    // Initialize timestamps for both LEDs
    unsigned long greenTimestamp = TIMESTAMP_START;  // Start at 10,000 ms - this will ensure neater representation of the recorded data in the csv
    unsigned long redTimestamp = TIMESTAMP_START;    // Start at 10,000 ms - this will ensure neater representation of the recorded data in the csv
    unsigned long timestampLimit = TIMESTAMP_START + (BLINK_DURATION * 1000);  // Set the timestamp limit to 70,000 ms - this will ensure the LED will run for 60 seconds

    // Blinking
    unsigned long previousMillis[NUMBER_OF_LEDS] = {0, 0};
    unsigned int ledStates[NUMBER_OF_LEDS] = {LOW, LOW};

    // Inside the while loop to control LED blinking
    while (1) {
        unsigned long currentMillis = millis();

        // Check if it's time to change the LED state (on or off)
        for (int i = 0; i < NUMBER_OF_LEDS; i++) {
            if (currentMillis - previousMillis[i] >= (ledStates[i] == LOW ? offTimes[i] : onTimes[i])) {
                previousMillis[i] = currentMillis;

                // Setting Blink LED
                int blinkLed = 0;
                if (i == BLINK_GREEN) {
                    blinkLed = GREEN;
                } else {
                    blinkLed = RED;
                }

                if (dutyCycles[i] == 0) {
                    ledStates[i] = LOW;
                } else if (dutyCycles[i] == 100) {
                    ledStates[i] = HIGH;
                } else {
                    ledStates[i] = (ledStates[i] == LOW) ? HIGH : LOW;                      // Toggle the LED state between LOW and HIGH
                }

                softPwmWrite(blinkLed, ledStates[i] == HIGH ? brightness[i] : 0);      // Set the LED brightness based on its state
                digitalWrite(blinkLed, ledStates[i]);                                  // Update the physical state of the LED    

                // Record waveform data
                data.timestamp = (blinkLed == GREEN) ? greenTimestamp : redTimestamp;

                if (blinkLed == GREEN) {
                    data.greenState = ledStates[i];                                            // If the green LED is blinking, update the green LED state in the waveform data
                    writeWaveformDataGreen(data, frequencies[i], dutyCycles[i]);               // Record green led data to csv file
                    greenTimestamp += (ledStates[i] == HIGH) ? onTimes[i] : offTimes[i];       // Update the green LED timestamp
                } else {
                    data.redState = ledStates[i];                                              // If the red LED is blinking, update the red LED state in the waveform data
                    writeWaveformDataRed(data, frequencies[i], dutyCycles[i]);                 // Record red led data to csv file
                    redTimestamp += (ledStates[i] == HIGH) ? onTimes[i] : offTimes[i];         // Update the red LED timestamp
                }
            }
        } 

        // Check if the timestamp limit is reached, and exit the loop if true
        if (data.timestamp >= timestampLimit) {
            break;
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
