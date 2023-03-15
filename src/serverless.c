#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include "hardware/adc.h"

#define NO_JOB_RUNNING 0x00;
#define JOB_RUNNING 0x01

#define MAX_INPUT_SIZE 0x100
#define FACTOR_ARR_SIZE 0x200

#define BUFFER_WRITE_PADDING_MILLISECONDS 10

bool has_job = NO_JOB_RUNNING;

/*
    Two kinds of communication
    Job query:
        Orch. sends "JOB?"
        Comp. sends 0 for no job is running, 1 for job is running
    Job push:
        Orch. sends "JOB:" + data for job
        Comp. sends "JOBSTART"
        Comp. sends result of job
*/

/*
TODO: expand list of programs to run, potentially in a different header file
TODO: Potentially have the option to send AMD assembly (unlinked ofc) to run
*/

void core1_interrupt_handler();
int* findPrimeFactors(int num);
int isFactor(int number, int factor);
int isPrime(int num);
char *readLine();


// Core 1 execution starts here, sets core1_interrupt_handler to be called when FIFO buffer has data
void core1_entry() {

    printf("START CORE 1\n");

    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);

    irq_set_enabled(SIO_IRQ_PROC1, true);

    // Infinte while Loop to wait for interrupt
    while (1) {
        tight_loop_contents();
    }
}

// Called whenever FIFO buffer has data
void core1_interrupt_handler() {
    printf("Interrupt receieved\n");
    
    while (multicore_fifo_rvalid()){
        int buffer = multicore_fifo_pop_blocking();
        printf("Data in intercore buffer: %i\n", buffer);
        int* factors = findPrimeFactors(buffer);
        multicore_fifo_push_blocking(sizeof(factors));
        free(factors);
    }

    multicore_fifo_clear_irq(); // Clear interrupt
}

// Read USB input until a certain
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

    while (inputChar != '|')
    {
        inputChar = getchar();
        string[char_i] = inputChar;
        // Echo for `screen` program
        printf("%c", string[char_i]);
        char_i++;
    }

    // Remove char delimiter
    string[strlen(string) - 1] = 0x00;

    return string;
}

int main() {
    // Initialize IO
    stdio_init_all();
    
    // Necessary to see initial output
    sleep_ms(8000);

    // Initialize multicore
    multicore_launch_core1(core1_entry);

    printf("START CORE 0\n");

    // Primary Core 0 loop
    while (1)
    {
        char *line = readLine();
        printf("\n");
        uint32_t len = strlen(line);
        int *data = (int *)(&(line[4]));

        if (!strncmp(line, "JOB?", 4)) {
            printf("%x\n", has_job);
        } else if (!strncmp(line, "JOB:", 4)) {
            has_job = JOB_RUNNING;
            for (int i = 0; i < sizeof(data) / sizeof(int); ++i) {
                multicore_fifo_push_blocking(data[i]);
                int result = multicore_fifo_pop_blocking();
                printf("Number of prime factors: %i\n", result - 1);
            }
        }
        
        free(line);
    }
}


// By Amy Techavimol
int* findPrimeFactors(int num)
{
    printf("Finding prime factors of %d \n", num);
    int* factors = malloc(FACTOR_ARR_SIZE);
    for (int i = 0; i < FACTOR_ARR_SIZE / 4; ++i) {
        factors[i] = 0x00;
    }

    int index = 0;

    for (int i = 1; i <= num; ++i)
    {
        if (isFactor(num, i) == 1)
        {
            if (isPrime(i) == 1)
            {
                printf("%d is a prime factor of %d \n", i, num);
                factors[index] = i;
                ++index;
            }
        }
    }

    return factors;
}

// By Amy Techavimol
int isFactor(int number, int factor)
{
    if (number % factor == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// By Amy Techavimol
int isPrime(int num)
{
    int count = 0;
    for (int i = 1; i < num; ++i)
    {
        if (num % i == 0)
        {
            count++;
        }
    }

    if (count == 1)
    {
        return 1;
    }

    return 0;
}