#include <stdlib.h>
#include <stdio.h>

#include "pico/stdio.h"

int main()
{
    stdio_init_all();

    char go = getchar();

    int n = 500;

    int p[n];  // An array to store a partition
    int k = 0; // Index of last element in a partition
    p[k] = n;  // Initialize first partition as number itself
    int rem_val;
    while (1)
    {
        rem_val = 0;

        while (k >= 0 && p[k] == 1)
        {
            rem_val += p[k];
            k--;
        }

        if (k < 0)
            break;

        p[k]--;
        rem_val++;

        while (rem_val > p[k])
        {
            p[k + 1] = p[k];
            rem_val = rem_val - p[k];
            k++;
        }

        p[k + 1] = rem_val;
        k++;
    }

    printf(":%i:", k);
}