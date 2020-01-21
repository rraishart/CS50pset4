/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Edited by Ryan Raishart on 8/08/2016
 * rraishart@gmail.com
 * 
 * Scales a BMP by a factor(1-100)
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy factor infile outfile\n");
        return 1;
    }

    
    int factor = atoi(argv[1]);
    if (factor > 100 || factor < 1)
    {
        printf("Factor must be 1-100\n");
        return 2;
    }
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // determine padding for input scanlines
    int inpPadding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // Store input file attributes and update outs
    int inpBiWidth = bi.biWidth;
    bi.biWidth *= factor;
    
    int inpBiHeight = bi.biHeight;
    bi.biHeight *= factor;
    
    // determine padding for output scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
   // Updates header using new height/width
    bi.biSizeImage = (bi.biWidth * sizeof(RGBTRIPLE) + padding) * 
    abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(bf) + sizeof(bi);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(inpBiHeight); i < biHeight; i++)
    {
        // scale verticaly
        for (int z = 0; z < factor; z++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < inpBiWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;
    
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
                // scale horizontaly 
                for (int y = 0; y < factor; y++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // skip over padding, if any
            fseek(inptr, inpPadding, SEEK_CUR);

            // then add it back (to demonstrate how)
            for (int k = 0; k < padding; k++)
            {
                fputc(0x00, outptr);
            }
            // go back to start of scanline
            if (z < factor - 1)
            {
                int widthSize = (inpBiWidth * sizeof(RGBTRIPLE) + 
                inpPadding) * -1;
                fseek(inptr, widthSize, SEEK_CUR);
            }
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
