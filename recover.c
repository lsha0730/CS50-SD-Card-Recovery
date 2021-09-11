#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t BYTE;

bool is_jpeg_start (BYTE *buffer);

int main(int argc, char *argv[])
{
    //Checking for correct use of commandline arguments
    if (argc != 2) {
        printf("Usage: ./recover file\n");
        return 1;
    }

    //Main component
    int filenumber = 000;
    bool jpeg_found = false;

    BYTE buffer[512*sizeof(BYTE)];

    FILE *input = fopen(argv[1], "r");
    FILE *output = NULL;

    //Iterate through each block of 512 bytes
    while (fread(&buffer, sizeof(BYTE), 512, input) == 512) {

        if (is_jpeg_start(buffer)) {
            jpeg_found = true;
        }

        if (jpeg_found) {
            //We're in the middle of the string of jpegs
            if (is_jpeg_start(buffer)) {
                //Head

                //Close the previous iteration's jpeg
                if (output != NULL) {
                    fclose(output);
                }

                //Name output files in the format "000.jpg"
                filenumber++;
                char filename[8];
                sprintf(filename, "%03d.jpg", filenumber - 1);

                //Write these 512 bytes to the opened write file
                output = fopen(filename, "w");
                fwrite(buffer, sizeof(BYTE), 512, output);

            } else {
                //Body
                fwrite(buffer, sizeof(BYTE), 512, output);
            }
        }
    }

    if (!jpeg_found) {
        printf("Unsupported File Format\n");
    }
    fclose(input);

}

//Checks if the first four bytes of the current block is a jpeg signature
bool is_jpeg_start (BYTE *buffer) {
    if (buffer[0]==0xff && buffer[1]==0xd8 && buffer[2]==0xff && buffer[3]>=0xe0 && buffer[3]<=0xef) {
        return true;
    } else {
        return false;
    }
}