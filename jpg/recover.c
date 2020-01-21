/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 * Ryan Raishart on 8/10/2016
 * rraishart@gmail.com
 * Recovers JPEGs from a forensic image.
 */
 
 // TODO - Implement fread eof condition / Fix memory leaks
 
#include <stdio.h>
#include <stdint.h>
#include <cs50.h>
#include <string.h>
#define BLOCK_SIZE 512
typedef uint8_t  BYTE;

int main(int argc, char* argv[])
{
    // open card file
    FILE* card = fopen("card.raw", "r");
    if (card == NULL)
    {
        printf("Could not open %s.\n", "card.raw");
        return 1;
    }
    // create  buffer
    BYTE buffer[BLOCK_SIZE];
    // set flag if we've found a jpg
    bool jpgFound = false;
    // keep track of how many jpg's we have
    int jpgCount = 0;
    FILE* img = NULL;    
    // fill buffer untill jpg sig is found
    while (fread(buffer, sizeof(BYTE), BLOCK_SIZE, card) == BLOCK_SIZE)
    {
        // if start of new jpg
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && 
           (buffer[3] == 0xe0 || buffer[3] == 0xe1))
        {
            char* title = malloc(strlen("000.jpg") + 1);
            // if this is first jpg
            if (jpgFound == false)
            {
                // set flag
                jpgFound = true;
               
                // create file name
                sprintf(title, "%03d.jpg", jpgCount);
                // open file
                img = fopen(title, "a");
                // write to file
                fwrite(&buffer, sizeof(BYTE), BLOCK_SIZE, img);
                // TODO create new jpg
                jpgCount++;
               }
               else
            {
                // close current jpg and create new
                fclose(img);
                // create file name
                sprintf(title, "%03d.jpg", jpgCount);
                // open file
                img = fopen(title, "a");
                // write to file
                fwrite(&buffer, sizeof(BYTE), BLOCK_SIZE, img);
                jpgCount++;
               }
            free(title);
           }
        else
        {
            if (jpgFound == true)
            {
                // write to current jpg
                fwrite(&buffer, sizeof(BYTE), BLOCK_SIZE, img);
            }
        }
    }
    fclose(img);
    fclose(card);
    
    return 0;
}
