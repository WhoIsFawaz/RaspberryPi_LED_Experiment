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

// Libraries to import
#include <wiringPi.h>
#include <softPwm.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definitions
#define CONFIRM 1        // Defines the confirm value used in menus

#define GREEN_PIN 13 // GPIO Pin 13
#define RED_PIN 27   // GPIO Pin 27

// LED Blink Selection / LED position in array
// IMPORTANT!!
// This represents both the input used to select the LED user wants to use AND the position of the LED in any array containing LEDs
// MUST START FROM 0 AND BE SEQUENTIAL
// DON'T CHANGE UNLESS YOU KNOW WHAT YOU'RE DOING
#define GREEN 0
#define RED 1

#define NUMBER_OF_LEDS 2 // Define total number of LEDs (used to define the length of arrays containing LEDs)

#define TIMESTAMP_START 10000 // Start of timestamp
#define BLINK_DURATION 10     // Duration of in seconds

// Program States
#define TURN_OFF 0
#define TURN_ON 1
#define BLINK 2
#define BLINK_ALL 3
#define EXIT 4

// File to store waveform data
#define WAVEFORM_FILE_GREEN "green_waveform_data.csv"
#define WAVEFORM_FILE_RED "red_waveform_data.csv"

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
void writeWaveformHeader();
void writeWaveformData();
void endProgram();

// Main Programme
int main(void)
{
    setupProgram();
    startProgram();
    endProgram();
    return 0;
}

// Sets up the LED GPIO pins as output and PWM
void setupProgram()
{
    wiringPiSetupGpio();
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    softPwmCreate(GREEN_PIN, 0, 100);
    softPwmCreate(RED_PIN, 0, 100);
    system("clear");
}

// Takes the input of the user selection and directs it to different states of the program
void startProgram()
{
    int selection;

    do
    {
        selection = getUserSelection();

        switch (selection)
        {
        case TURN_OFF:
            turnOffLeds(); // Off both LEDs for troubleshooting purposes
            break;
        case TURN_ON:
            turnOnLeds(); // On both LEDs for troubleshooting purposes
            break;
        case BLINK:
            blink(); // Blink single LED
            break;
        case BLINK_ALL:
            blinkAll(); // Blink all LEDs
            break;
        case EXIT:
            break; // Exit the programme
        default:
            system("clear");
            printf("\nInvalid Input. Try Again...\n");
            break;
        }
    } while (selection != EXIT);
}

// The main menu that determines what the user wants to do
int getUserSelection()
{
    int selection;
    // printf(" _______  _______    _______  ______    _______  __   __  _______    _______   \n");
    // printf("|       ||       |  |       ||    _ |  |       ||  | |  ||       |  |       |  \n");
    // printf("|    _  ||____   |  |    ___||   | ||  |   _   ||  | |  ||    _  |  |____   |  \n");
    // printf("|   |_| | ____|  |  |   | __ |   |_||_ |  | |  ||  |_|  ||   |_| |   ____|  |  \n");
    // printf("|    ___|| ______|  |   ||  ||    __  ||  |_|  ||       ||    ___|  | ______|  \n");
    // printf("|   |    | |_____   |   |_| ||   |  | ||       ||       ||   |      | |_____   \n");
    // printf("|___|    |_______|  |_______||___|  |_||_______||_______||___|      |_______|  \n");
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
void turnOffLeds()
{
    system("clear");
    printf("\nTurning off all LEDs...\n");
    digitalWrite(GREEN_PIN, LOW);
    softPwmWrite(GREEN_PIN, 0);
    digitalWrite(RED_PIN, LOW);
    softPwmWrite(RED_PIN, 0);
}

// For troubleshooting, turning on LEDs and PWM. Use this to test the connection of your LED and Pi
void turnOnLeds()
{
    system("clear");
    printf("\nTurning on all LEDs...\n");
    digitalWrite(GREEN_PIN, HIGH);
    softPwmWrite(GREEN_PIN, 100);
    digitalWrite(RED_PIN, HIGH);
    softPwmWrite(RED_PIN, 100);
}

// When user wants to blink single LED, this function will get all the blinking configurations
void blink()
{
    system("clear");
    printf("\nBlink...\n");
    int frequencies[NUMBER_OF_LEDS] = {-1, -1};
    int brightness[NUMBER_OF_LEDS] = {-1, -1};

    int led = getBlinkLed();
    frequencies[led] = getBlinkFrequency(led);
    brightness[led] = getBlinkBrightness(led);

    if (confirmBlinkSelection(frequencies, brightness) == CONFIRM)
    {
        blinkLedsWithConfig(frequencies, brightness);
        system("clear");
    }
    else
    {
        system("clear");
        return;
    }
}

// When user wants to blink both LEDs, this function will get all the blinking configurations
void blinkAll()
{
    system("clear");
    printf("\nBlink all LEDs...\n");
    int frequencies[NUMBER_OF_LEDS] = {-1, -1}; // Create array of frequency for each LED
    int brightness[NUMBER_OF_LEDS] = {-1, -1}; // Create array of brightness for each LED

    // Loop through all LEDs
    for (int i = 0; i < NUMBER_OF_LEDS; i++)
    {
        frequencies[i] = getBlinkFrequency(i); // Assign freqeuncy for each LED
        brightness[i] = getBlinkBrightness(i); // Assign brightness for each LED
    }

    if (confirmBlinkSelection(frequencies, brightness) == CONFIRM)
    {
        blinkLedsWithConfig(frequencies, brightness);
        system("clear");
    }
    else
    {
        system("clear");
        return;
    }
}

// Menu to get user selection on which LED to blink
int getBlinkLed()
{
    int selection;
    printf("\nSelect LED to blink.\n\n");
    printf("[%i] Green LED\n", GREEN);
    printf("[%i] Red LED\n", RED);
    printf("\nYour Selection: ");
    scanf("%d", &selection);

    if (selection != GREEN && selection != RED)
    {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        getBlinkLed();
    }
    else
    {
        system("clear");
        return selection;
    }
}

// Menu to get user selection on LED Frequency
int getBlinkFrequency(int led)
{
    int selection;
    if (led == GREEN)
    {
        printf("Enter frequency to blink green LED.\n\n");
    }
    else
    {
        printf("Enter frequency to blink red LED.\n\n");
    }
    printf("Enter whole numbers between 0 to 10\n\n");
    printf("Frequency (Hz): ");
    scanf("%d", &selection);

    if (selection < 0 || selection > 10)
    {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        getBlinkFrequency(led);
    }
    else
    {
        system("clear");
        return selection;
    }
}

// Menu to get user selection on LED Brightness/Duty Cycle
int getBlinkBrightness(int led)
{
    int selection;
    if (led == GREEN)
    {
        printf("Enter brightness to blink green LED.\n\n");
    }
    else
    {
        printf("Enter brightness to blink red LED.\n\n");
    }
    printf("Enter whole numbers between 0 to 100\n\n");
    printf("Brightness (%%): ");
    scanf("%d", &selection);

    if (selection < 0 || selection > 100)
    {
        system("clear");
        printf("Invalid Input. Try Again...\n\n");
        getBlinkBrightness(led);
    }
    else
    {
        system("clear");
        return selection;
    }
}

// Function to confirm the selection of blink configurations for the LEDs
int confirmBlinkSelection(int frequencies[NUMBER_OF_LEDS], int brightness[NUMBER_OF_LEDS])
{
    int selection; // Variable to store the user's selection

    printf("Confirm your blink configurations for the LEDs.\n\n"); // Printing a message for the user to confirm their configurations for the LEDs
    for (int i = 0; i < NUMBER_OF_LEDS; i++)                       // Looping through the LEDs
    {
        if (frequencies[i] != -1 && brightness[i] != -1) // Checking if the frequency and brightness for the LED exists, if it doesn't then don't print LED details
        {
            char blinkLedString[] = "Green"; // Initializing a character array for the LED color
            if (i == RED)                    // Checking if the LED is red and change the LED color string to red
            {
                strcpy(blinkLedString, "Red");
            }
            printf("%s LED\n", blinkLedString);              // Printing the LED color
            printf("  - Frequency: %dHz\n", frequencies[i]); // Printing the frequency of the LED
            printf("  - Brightness: %d%%\n", brightness[i]); // Printing the brightness of the LED
        }
    }
    printf("\n[1] Confirm Configuration\n"); // Printing the option for confirming the configuration
    printf("[0] Return to Home\n");          // Printing the option for returning to the home menu
    printf("\nYour Selection: ");

    scanf("%d", &selection); // Taking the user's selection

    if (selection < 0 || selection > 1) // Checking if the selection is not within the valid range
    {
        system("clear");                                // Clearing the screen
        printf("Invalid Input. Try Again...\n\n");      // Printing a message for invalid input
        confirmBlinkSelection(frequencies, brightness); // Calling the function again to get valid input
    }
    else
        return selection; // Returning the user's selection
}

// Function to control the blinking of LEDs based on the configurations provided
void blinkLedsWithConfig(unsigned int frequencies[NUMBER_OF_LEDS], unsigned int brightness[NUMBER_OF_LEDS])
{
    printf("\nBlinking...\n"); // Printing a message indicating that the LEDs are blinking

    // Initializing arrays to store on and off times for each LED
    int onTimes[NUMBER_OF_LEDS] = {0, 0};
    int offTimes[NUMBER_OF_LEDS] = {0, 0};

    // Calculating the on and off times for each LED based on the provided frequency and brightness
    for (int i = 0; i < NUMBER_OF_LEDS; i++)
    {
        float dutyCycle = (float)brightness[i] / 100.0f;     // Calculating the duty cycle
        float cycleTime = (frequencies[i] > 0 ? (1.0f / frequencies[i]) : 1.0f) * 1000.0f; // Calculating the time for one cycle (default is one second)
        onTimes[i] = cycleTime * dutyCycle;                  // Calculating the on-time in milliseconds
        offTimes[i] = cycleTime * (1.0f - dutyCycle);        // Calculating the off-time in milliseconds
    }

    // Initializing variables for timestamps and the timestamp limit
    unsigned long timestamps[] = {TIMESTAMP_START, TIMESTAMP_START};          // Initializing the timestamps for each LED (each timestamp will increase with each blink)
    unsigned long timestampLimit = TIMESTAMP_START + (BLINK_DURATION * 1000); // Setting the timestamp limit
    int done = FALSE; // Flag to check if timestamp of LED goes over timestamp limit

    // Flag to check if headers were written for all LED's CSV files
    int headersWritten = FALSE;

    // Initializing arrays to store time when LEDs were last blinked and LED states
    long previousBlinkMillis[NUMBER_OF_LEDS] = {0, 0}; // Storing time when LED was last turned on or off for each LED
    int ledStates[NUMBER_OF_LEDS] = {LOW, LOW};        // Storing the state of each LED

    // Looping indefinitely to control the blinking of LEDs
    while (!done)
    {
        unsigned long currentMillis = millis(); // Getting the current time in milliseconds

        // Checking if it's time to change the LED state (on or off)
        for (int i = 0; i < NUMBER_OF_LEDS; i++)
        {
            // Checking if it's time to change the LED state (on or off)
            if (currentMillis - previousBlinkMillis[i] >= (ledStates[i] == LOW ? offTimes[i] : onTimes[i]))
            {
                previousBlinkMillis[i] = currentMillis; // Updating the previous blink milliseconds

                // Handling the LED states based on the brightness values
                if (brightness[i] == 0)
                {
                    ledStates[i] = LOW; // Turning the LED off if brightness is 0
                }
                else if (brightness[i] == 100)
                {
                    ledStates[i] = HIGH; // Turning the LED on at full intensity if brightness is 100
                }
                else
                {
                    ledStates[i] = (ledStates[i] == LOW) ? HIGH : LOW; // Toggling the LED state based on the current state
                }

                // Determining which LED pin is to be used
                int ledPin = 0;
                if (i == GREEN)
                {
                    ledPin = GREEN_PIN; // Use the green LED pin
                }
                else
                {
                    ledPin = RED_PIN; // Use the red LED pin
                }

                // Setting the LED brightness based on the state
                softPwmWrite(ledPin, ledStates[i] == HIGH ? brightness[i] : 0); 
                // Updating the physical state of the LED
                digitalWrite(ledPin, ledStates[i]);       


                // Checking if CSV headers were written
                if (!headersWritten)
                {
                    writeWaveformHeader(i, frequencies[i], brightness[i]); // Write waveform header in CSV if not written
                }                      
                // Writing waveform data to a file
                writeWaveformData(i, timestamps[i], ledStates[i]); 
                // Updating the LED timestamp based on the state
                timestamps[i] += (ledStates[i] == HIGH) ? onTimes[i] : offTimes[i];

                // Checking if the timestamp limit has been reached, and setting done flag to true if so
                if (timestamps[i] >= timestampLimit)
                {
                    done = TRUE;
                }    
            }
        }

        // Set flag to true after first iteration since headers should have been written then
        headersWritten = TRUE;
    }
}

/* Add a function to write waveform data of LED to the file */
void writeWaveformHeader(int led, int blinkFrequency, int blinkDutyCycle)
{
    char ledString[] = "Green";
    FILE *file;

    if (led == GREEN)
    {
        file = fopen(WAVEFORM_FILE_GREEN, "w"); // Open file in write mode
    }
    else
    {
        file = fopen(WAVEFORM_FILE_RED, "w"); // Open file in append mode
        strcpy(ledString, "Red");
    }

    if (file != NULL)
    {
        fprintf(file, "Frequency of %s LED is: %dHz & Duty Cycle of %s LED is: %d%%\n\n", ledString, blinkFrequency, ledString, blinkDutyCycle);
        fprintf(file, "The timestamp in Millisecond | The state of the %s LED\n", ledString);
        fclose(file);
    }
}

/* Add a function to write waveform data of LED to the file */
void writeWaveformData(int led, long timestamp, int state)
{
    FILE *file;

    if (led == GREEN)
    {
        file = fopen(WAVEFORM_FILE_GREEN, "a"); // Open file in append mode
    }
    else
    {
        file = fopen(WAVEFORM_FILE_RED, "a"); // Open file in append mode
    }

    if (file != NULL)
    {
        fprintf(file, "              %d          ,             %d\n", timestamp, state); // The large spacing in between is for neater looks in the CSV file
        fclose(file);
    }
}

// Resetting and cleaning up before safely exiting the program
void endProgram()
{
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
