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
13 GREEN_PIN LED
27 RED_PIN LED
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
#define CONFIRM 1

#define NUMBER_OF_LEDS 2 // Define total number of leds

#define GREEN_PIN 13    // GPIO Pin 13
#define RED_PIN 27      // GPIO Pin 27

// LED Blink Selection / position in array
#define GREEN 0
#define RED 1

#define TIMESTAMP_START 10000   // Start of timestamp
#define BLINK_DURATION 60       // Duration of in seconds

// Program States
#define TURN_OFF 0
#define TURN_ON 1
#define BLINK 2
#define BLINK_ALL 3
#define EXIT 4

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
int getBlinkBrightness();
int confirmBlinkSelection();
void blinkLedsWithConfig();
void writeWaveformData();
void endProgram();

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
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    softPwmCreate(GREEN_PIN, 0, 100);
    softPwmCreate(RED_PIN, 0, 100);
    system("clear");
}

// Takes the input of the user selection and directs it to different states of the program
void startProgram() {
    int selection;

    do {
        selection = getUserSelection();

        switch(selection) {
            case TURN_OFF:
                turnOffLeds(); // Off both led for troubleshooting purposes
                break;
            case TURN_ON:
                turnOnLeds(); // On both led for troubleshooting purposes
                break;
            case BLINK:
                blink();      // On single led
                break;
            case BLINK_ALL:
                blinkAll();   // On all led
                break;
            case EXIT:
                break;        // Exit the programme
            default:
                printf("\nInvalid Input. Try Again...\n");
                break;
        }
    } while (selection != EXIT);
}

// The main menu that determines what the user wants to do 
int getUserSelection() {
    int selection;
    printf(" _______  _______    _______  ______    _______  __   __  _______    _______   \n");
    printf("|       ||       |  |       ||    _ |  |       ||  | |  ||       |  |       |  \n");
    printf("|    _  ||____   |  |    ___||   | ||  |   _   ||  | |  ||    _  |  |____   |  \n");
    printf("|   |_| | ____|  |  |   | __ |   |_||_ |  | |  ||  |_|  ||   |_| |   ____|  |  \n");
    printf("|    ___|| ______|  |   ||  ||    __  ||  |_|  ||       ||    ___|  | ______|  \n");
    printf("|   |    | |_____   |   |_| ||   |  | ||       ||       ||   |      | |_____   \n");
    printf("|___|    |_______|  |_______||___|  |_||_______||_______||___|      |_______|  \n");

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
    digitalWrite(GREEN_PIN, LOW);
    softPwmWrite(GREEN_PIN, 0);
    digitalWrite(RED_PIN, LOW);
    softPwmWrite(RED_PIN, 0);
}

// For troubleshooting, turning on LEDs and PWM. Use this to test the connection of your LED and Pi
void turnOnLeds() {
    system("clear");
    printf("\nTurning on all LEDs...\n");
    digitalWrite(GREEN_PIN, HIGH);
    softPwmWrite(GREEN_PIN, 100);
    digitalWrite(RED_PIN, HIGH);
    softPwmWrite(RED_PIN, 100);
}

// When user wants to blink single LED, this function will get all the blinking configurations
void blink() {
    system("clear");
    printf("\nBlink...\n");
    int frequencies[NUMBER_OF_LEDS] = {0, 0};
    int brightness[NUMBER_OF_LEDS] = {0, 0};

    int led = getBlinkLed();
    frequencies[led] = getBlinkFrequency(led);
    brightness[led] = getBlinkBrightness(led);

    if (confirmBlinkSelection(frequencies, brightness) == CONFIRM) {
        blinkLedsWithConfig(frequencies, brightness);
        system("clear");
    } else {
        system("clear");
        return;
    }
}

// When user wants to blink both LEDs, this function will get all the blinking configurations
void blinkAll() {
    system("clear");
    printf("\nBlink all LEDs...\n");
    unsigned int frequencies[NUMBER_OF_LEDS] = {0, 0};
    unsigned int brightness[NUMBER_OF_LEDS] = {0, 0};

    for (int i = 0; i < NUMBER_OF_LEDS; i++) {
        frequencies[i] = getBlinkFrequency(i);
        brightness[i] = getBlinkBrightness(i);
    }

    if (confirmBlinkSelection(frequencies, brightness) == CONFIRM) {
        blinkLedsWithConfig(frequencies, brightness);
        system("clear");
    } else {
        system("clear");
        return;
    }
}

// Menu to get user selection on which LED to blink
int getBlinkLed() {
    int selection;
    printf("\nSelect LED to blink.\n\n");
    printf("[%i] Green LED\n", GREEN);
    printf("[%i] Red LED\n", RED);
    printf("\nYour Selection: ");
    scanf("%d", &selection);
    
    if (selection != GREEN && selection != RED) {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        getBlinkLed();
    } else {
        system("clear");
        return selection;
    }
}

// Menu to get user selection on LED Frequency
int getBlinkFrequency(int led) {
    int selection;
    if (led == GREEN) {
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
        getBlinkFrequency(led);
    } else {
        system("clear");
        return selection;
    }
}

// Menu to get user selection on LED Brightness/Duty Cycle
int getBlinkBrightness(int led) {
    int selection;
    if (led == GREEN) {
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
        getBlinkBrightness(led);
    } else {
        system("clear");
        return selection;
    }
}

// Updated function to confirm the configuration for all LEDs
int confirmBlinkSelection(unsigned int frequencies[NUMBER_OF_LEDS], unsigned int brightness[NUMBER_OF_LEDS]) {
    int selection;

    printf("Confirm your blink configurations for the LEDs.\n\n");
    for (int i = 0; i < NUMBER_OF_LEDS; i++) {
        if (frequencies[i] != 0 && brightness[i] != 0) {
            char blinkLedString[] = "Green";
            if (i == RED) {
                strcpy(blinkLedString, "Red");
            }
            printf("%s LED\n", blinkLedString);
            printf("  - Frequency: %dHz\n", frequencies[i]);
            printf("  - Brightness: %d%%\n", brightness[i]);
        }
    }
    printf("\n[1] Confirm Configuration\n");
    printf("[0] Return to Home\n");
    printf("\nYour Selection: ");

    scanf("%d", &selection);

    if (selection < 0 || selection > 1) {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        confirmBlinkSelection(frequencies, brightness);
    } else return selection;
}

// Blinks all the LED according to the user configuration
void blinkLedsWithConfig(unsigned int frequencies[NUMBER_OF_LEDS], unsigned int brightness[NUMBER_OF_LEDS]) {
    printf("\nBlinking...\n");

    unsigned int onTimes[NUMBER_OF_LEDS] = {0, 0};
    unsigned int offTimes[NUMBER_OF_LEDS] = {0, 0};

    // Calculate onTime and offTime based on the duty cycle (brightness)
    for (int i = 0; i < NUMBER_OF_LEDS; i++) {
        float dutyCycle = (float)brightness[i] / 100.0f;                    // Calculate duty cycle based on the user's input (brightness)
        float cycleTime = (1.0f / frequencies[i]) * 1000.0f;                // Calculate the time for one cycle based on the user's input (frequency)
        onTimes[i] = cycleTime * dutyCycle;                                 // Calculate the on-time in milliseconds based on duty cycle
        offTimes[i] = cycleTime * (1.0f - dutyCycle);                       // Calculate the off-time in milliseconds based on duty cycle
    }

    // Initialize timestamps for both LEDs
    unsigned long timestamps[] = {TIMESTAMP_START, TIMESTAMP_START};  // Start at 10,000 ms - this will ensure neater representation of the recorded data in the csv
    unsigned long timestampLimit = TIMESTAMP_START + (BLINK_DURATION * 1000);  // Set the timestamp limit to 70,000 ms - this will ensure the LED will run for 60 seconds

    // Blinking
    unsigned long previousMillis[NUMBER_OF_LEDS] = {0, 0};
    unsigned int ledStates[NUMBER_OF_LEDS] = {LOW, LOW};

    // Inside the while loop to control LED blinking
    while (1) {
        unsigned long currentMillis = millis();  // Get the current time in milliseconds

        // Check if it's time to change the LED state (on or off)
        for (int i = 0; i < NUMBER_OF_LEDS; i++) {
            if (currentMillis - previousMillis[i] >= (ledStates[i] == LOW ? offTimes[i] : onTimes[i])) {
                previousMillis[i] = currentMillis;  // Update the previousMillis to the current time

                if (brightness[i] == 0) {
                    ledStates[i] = LOW;             // If brightness is set to 0, turn the LED off
                } else if (brightness[i] == 100) {
                    ledStates[i] = HIGH;            // If brightness is set to 100, turn the LED on at full intensity
                } else {
                    ledStates[i] = (ledStates[i] == LOW) ? HIGH : LOW;  // Toggle the LED state between LOW and HIGH based on the current state
                }

                // Determine which LED is currently being blinked
                int ledPin = 0;
                if (i == GREEN) {
                    ledPin = GREEN_PIN;   // Set the LED to green if i represents the green LED
                } else {
                    ledPin = RED_PIN;     // Set the LED to red if i represents the red LED
                }

                softPwmWrite(ledPin, ledStates[i] == HIGH ? brightness[i] : 0);           // Set the LED brightness based on its state
                digitalWrite(ledPin, ledStates[i]);                                       // Update the physical state of the LED

                writeWaveformData(i, timestamps[i], ledStates[i], frequencies[i], brightness[i]);               // Record  led data to csv file
                timestamps[i] += (ledStates[i] == HIGH) ? onTimes[i] : offTimes[i];                                  // Update the  LED timestamp
            }
        }

        // Check if the timestamp limit is reached, and exit the loop if true
        if (timestamps[0] >= timestampLimit) {
            break;
        }
    }
}

/* Add a function to write waveform data of GREEN_PIN LED to the file */
void writeWaveformData(unsigned int led, unsigned long timestamp, unsigned int state, unsigned int blinkFrequency, unsigned int blinkDutyCycle) {
    static int headersWritten[] = {0, 0};
    char ledString[] = "Green";
    FILE *file;

    if (led == GREEN) {
        file = fopen(WAV_FILEGreen, "a"); // Open file in append mode
    } else {
        file = fopen(WAV_FILERed, "a"); // Open file in append mode
        strcpy(ledString, "Red");
    }

    if (file != NULL) {
        if (!headersWritten[led]) {
            file = freopen(NULL, "w", file); // New updates to the file will be written over the existing contents in the file
            fprintf(file, "Frequency of %s LED is: %dHz & Duty Cycle of %s LED is: %d%%\n\n", ledString, blinkFrequency, ledString, blinkDutyCycle); 
            fprintf(file, "The timestamp in Millisecond | The state of the %s LED\n", ledString);
            headersWritten[led] = 1;
        }
        fprintf(file, "              %d          ,             %d\n", timestamp, state); // The large spacing in between is for neater looks in the CSV file
        fclose(file);
    }
}

// Resetting and cleaning up before safely exiting the program
void endProgram() {
    system("clear");
    printf("\nCleaning Up...\n");

    // Turn Off LEDs
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, LOW);

    // Turn Off LED Software PWMSS
    softPwmWrite(GREEN_PIN, 0);
    softPwmWrite(RED_PIN, 0);

    // Reset Pins to Original INPUT State
    pinMode(GREEN_PIN, INPUT);
    pinMode(RED_PIN, INPUT);

    printf("Bye!\n\n");
}
