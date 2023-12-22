// Licensed under the MIT License.

// Trebuchet?! Part 1

#include <ctype.h>
#include <stdio.h>
#include <time.h>
#define BUFFER_SIZE 256

static int numeric(char digit)
{
    if (!digit)
    {
        return 0;
    }

    return digit - '0';
}

int main()
{
    char buffer[BUFFER_SIZE];
    long sum = 0;
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stdin))
    {
        char tens = 0;
        char ones = 0;

        for (char* p = buffer; *p; p++)
        {
            if (!isdigit(*p))
            {
                continue;
            }

            if (!tens)
            {
                tens = *p;
            }

            ones = *p;
        }

        sum += (numeric(tens) * 10) + numeric(ones);
    }

    printf("01a %ld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
