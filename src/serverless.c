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

uint64_t addAllNumbers(int num);
void core1_interrupt_handler();

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
    // Receive Raw Value, Convert and Print Temperature Value
    while (multicore_fifo_rvalid()){
        printf("%u", addAllNumbers((uint64_t) multicore_fifo_pop_blocking()));
        // sleep_ms(BUFFER_WRITE_PADDING_MILLISECONDS);
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
        char_i++;
    }

    return string;
}

int main() {
    // Initialize IO
    stdio_init_all();
    
    // Necessary to see initial output
    sleep_ms(3000);

    // Initialize multicore
    multicore_launch_core1(core1_entry);

    printf("START CORE 0\n");

    // Primary Core 0 loop
    while (1)
    {
        char *line = readLine();
        uint32_t len = strlen(line);

        // printf("%s:%p\n", line, line)
        if (!strncmp(line, "JOB?", 4)) {
            printf("%x\n", has_job);
        } else if (!strncmp(line, "JOB:", 4)) {
            has_job = JOB_RUNNING;
            for (int i = 0; i < len; ++i) {
                multicore_fifo_push_blocking((uint32_t)line[i]);
                sleep_ms(BUFFER_WRITE_PADDING_MILLISECONDS);
            }
        }
        
        free(line);
    }
}

// Program to run on core 1
uint64_t addAllNumbers(int num) {
    printf("Starting %i\n", num);
    double total_x = 0;
    double total_y = 0;
    for (unsigned int i = 0; i < 0xFFFFFF; ++i) {

        total_x += tan(i);
        for (unsigned int j = 0; j < 0xFFFFFFFF; ++j) {
            total_y += tan(j);
        }
    }
    printf("Finish %i\n", num);
    return total_x;
}