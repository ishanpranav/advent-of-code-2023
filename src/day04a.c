// Licensed under the MIT License.

// Scratchcards Part 1

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 128
#define DECIMAL_SET_CAPACITY 99

struct DecimalSet
{
    bool set[DECIMAL_SET_CAPACITY];
};

typedef char* String;
typedef struct DecimalSet* DecimalSet;

static int decimal_set_index(char tens, char ones)
{
    int result = ones - '0' - 1;

    if (tens == ' ')
    {
        return result;
    }

    return ((tens - '0') * 10) + result;
}

void decimal_set_add(DecimalSet instance, char tens, char ones)
{
    instance->set[decimal_set_index(tens, ones)] = true;
}

bool decimal_set_contains(DecimalSet instance, char tens, char ones)
{
    return instance->set[decimal_set_index(tens, ones)];
}

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day04a <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    long sum = 0;
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stream))
    {
        char* begin = strchr(buffer, ':');

        if (!begin)
        {
            fclose(stream);
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        char first;
        char second;
        long score = 0;
        char* next = strchr(buffer, '|');
        struct DecimalSet winningNumbers = { 0 };

        if (!next)
        {
            fclose(stream);
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        for (char* p = begin + 2;
            p < next && (first = p[0]) && (second = p[1]);
            p += 3)
        {
            decimal_set_add(&winningNumbers, first, second);
        }

        for (char* p = next + 2;
            (first = p[0]) && (second = p[1]);
            p += 3)
        {
            if (!decimal_set_contains(&winningNumbers, first, second))
            {
                continue;
            }

            if (score)
            {
                score *= 2;
            }
            else
            {
                score = 1;
            }
        }

        sum += score;
    }

    printf("%ld : %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
