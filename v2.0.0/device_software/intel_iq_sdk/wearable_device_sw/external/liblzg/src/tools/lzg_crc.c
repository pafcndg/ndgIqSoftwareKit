#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lzg.h>
#include "../lib/internal.h"


void ShowUsage(char *prgName)
{
    fprintf(stderr, "Usage: %s [options] infile\n", prgName);
}

int main(int argc, char **argv)
{
    char *inName;
    FILE *inFile;
    size_t fileSize;
    unsigned char *buffer;
    // Default arguments
    inName = NULL;

    inName = argv[1];

    // Read input file
    inFile = fopen(inName, "rb");
    if (inFile)
    {
        fseek(inFile, 0, SEEK_END);
        fileSize = (size_t) ftell(inFile);
        fseek(inFile, 0, SEEK_SET);
        if (fileSize > 0)
        {
            buffer = (unsigned char*) malloc(fileSize);
            if (!buffer)
                return 0;
            printf("0x%x\n", _LZG_CalcChecksum(buffer, fileSize));

            // Free memory
            free(buffer);
        }

        fclose(inFile);
    }

    return 0;
}

