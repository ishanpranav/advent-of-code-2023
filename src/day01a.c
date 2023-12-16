// Licensed under the MIT License.

// Trebuchet?! Part 1

#include <ctype.h>
#include <stdio.h>
#include <time.h>
#define BUFFER_SIZE 256

typedef char* String;

static int numeric(char digit)
{
    if (!digit)
    {
        return 0;
    }

    return digit - '0';
}

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day01a <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    char buffer[BUFFER_SIZE];
    long sum = 0;
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stream))
    {
        char tens = 0;
        char ones = 0;
        char current;

        for (char* p = buffer; (current = *p); p++)
        {
            if (!isdigit(current))
            {
                continue;
            }

            if (!tens)
            {
                tens = current;
            }

            ones = current;
        }

        sum += (numeric(tens) * 10) + numeric(ones);
    }

    printf("%ld : %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
