#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Open the CSV files for reading to retrieve the dataset names
    FILE *data1File = fopen("green_waveform_data.csv", "r");
    FILE *data2File = fopen("red_waveform_data.csv", "r");

    char data1Name[256];  // Assuming a maximum name length of 255 characters
    char data2Name[256];

    if (data1File && data2File) {
        // Read the first line of each CSV file to get the dataset names
        if (fgets(data1Name, sizeof(data1Name), data1File) != NULL) {
            data1Name[strcspn(data1Name, "\n")] = '\0';  // Remove the newline character
        }
        if (fgets(data2Name, sizeof(data2Name), data2File) != NULL) {
            data2Name[strcspn(data2Name, "\n")] = '\0';  // Remove the newline character
        }

        // Close the CSV files
        fclose(data1File);
        fclose(data2File);

        // Open a pipe to the Gnuplot process
        FILE *gnuplotPipe = popen("gnuplot -persistent", "w");

        if (gnuplotPipe) {
            // Send Gnuplot commands to the pipe
            fprintf(gnuplotPipe, "set terminal png size 1200,1200\n"); // Adjust the image size as needed
            fprintf(gnuplotPipe, "set output 'PlottedWaveform.png'\n");
            fprintf(gnuplotPipe, "set multiplot\n");
            fprintf(gnuplotPipe, "set xlabel 'Time (ms)'\n");
            fprintf(gnuplotPipe, "set yrange [-1:2]\n");  // Adjust the y-axis range as needed

            // Set custom tick values on the Y-axis
            fprintf(gnuplotPipe, "set ytics -1,1,2\n");
            // Limit the x-axis range to values up to 15000
            fprintf(gnuplotPipe, "set xrange [10000:15000]\n");

            // Specify that the data files are comma-separated
            fprintf(gnuplotPipe, "set datafile separator ','\n");

            // First subplot
            fprintf(gnuplotPipe, "set origin 0.0,0.5\n");
            fprintf(gnuplotPipe, "set size 1,0.5\n");
            fprintf(gnuplotPipe, "set ylabel 'High and Low State'\n");
            fprintf(gnuplotPipe, "plot 'green_waveform_data.csv' using 1:2 with steps title '(%s)' linecolor 'dark-green'\n", data1Name);

            // Second subplot
            fprintf(gnuplotPipe, "set origin 0.0,0.0\n");
            fprintf(gnuplotPipe, "set size 1,0.5\n");
            fprintf(gnuplotPipe, "set ylabel 'High and Low State'\n");
            fprintf(gnuplotPipe, "plot 'red_waveform_data.csv' using 1:3 with steps title '(%s)' linecolor 'red'\n", data2Name);

            fprintf(gnuplotPipe, "unset multiplot\n");
            fprintf(gnuplotPipe, "exit\n");

            // Close the pipe
            pclose(gnuplotPipe);
        } else {
            printf("Error: Could not open Gnuplot pipe.\n");
        }
    } else {
        printf("Error: Could not open CSV files.\n");
    }

    return 0;
}

