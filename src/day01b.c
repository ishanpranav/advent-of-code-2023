// Licensed under the MIT License.

// Trebuchet!? Part 2

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 256

typedef char* String;

static int parse(String value)
{
    char first = value[0];

    if (isdigit(first))
    {
        return first - '0';
    }

    switch (first)
    {
        case 'f':
            if (strstr(value + 1, "our") == value + 1)
            {
                return 4;
            }

            if (strstr(value + 1, "ive") == value + 1)
            {
                return 5;
            }

            return 0;

        case 's':
            if (strstr(value + 1, "ix") == value + 1)
            {
                return 6;
            }

            if (strstr(value + 1, "even") == value + 1)
            {
                return 7;
            }

            return 0;

        case 't':
            if (strstr(value + 1, "wo") == value + 1)
            {
                return 2;
            }

            if (strstr(value + 1, "hree") == value + 1)
            {
                return 3;
            }

            return 0;
    }

    if (strstr(value, "one") == value)
    {
        return 1;
    }

    if (strstr(value, "eight") == value)
    {
        return 8;
    }

    if (strstr(value, "nine") == value)
    {
        return 9;
    }

    return 0;
}

int main(void)
{
    char buffer[BUFFER_SIZE];
    long sum = 0;
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stdin))
    {
        int tens = 0;
        int ones = 0;

        for (char* p = buffer; *p; p++)
        {
            int current = parse(p);

            if (!current)
            {
                continue;
            }

            if (!tens)
            {
                tens = current;
            }

            ones = current;
        }

        sum += (tens * 10) + ones;
    }

    printf("01b %ld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
