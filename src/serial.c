#include "pico/stdlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NO_JOB_RUNNING 0x00;
#define JOB_RUNNING 0x01

#define MAX_INPUT_SIZE 0x100

bool has_job = NO_JOB_RUNNING;

char *readLine()
{
    char inputChar = 0xff;
    char *string = malloc(MAX_INPUT_SIZE);
    unsigned int char_i = 0;

    // Initialize string memory
    for (int i = 0; i < MAX_INPUT_SIZE; ++i)
    {
        string[i] = 0x00;
    }

    while (inputChar != 0x00)
    {
        inputChar = getchar();
        string[char_i] = inputChar;
        char_i++;
    }

    return string;
}

int main()
{
    // Initialize IO
    stdio_init_all();

    while (1)
    {
        char *line = readLine();
        printf("%s:%p\n", line, line);

        if (!strncmp(line, "JOB?", 4)) {
            printf("Job: %x", has_job);
        }

        if (!strncmp(line, "JOB:", 4)) {
            // assign_job(line);
            printf("Job received");                  
        }
        
        free(line);
    }
}