// Licensed under the MIT License.

// Scratchcards Part 2

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 128
#define CARDS_CAPACITY 256
#define DECIMAL_SET_CAPACITY 99

struct DecimalSet
{
    bool set[DECIMAL_SET_CAPACITY];
};

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

int main(void)
{
    int card = 0;
    char buffer[BUFFER_SIZE];
    long cards[CARDS_CAPACITY] = { 0 };
    clock_t start = clock();

    for (int i = 0; i < CARDS_CAPACITY; i++)
    {
        cards[i] = 1;
    }

    while (fgets(buffer, sizeof buffer, stdin))
    {
        char* begin = strchr(buffer, ':');

        if (!begin)
        {
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        char first;
        char second;
        int matches = 0;
        char* next = strchr(buffer, '|');
        struct DecimalSet winningNumbers = { 0 };

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
            if (decimal_set_contains(&winningNumbers, first, second))
            {
                matches++;
            }
        }

        for (int i = 1; i <= matches; i++)
        {
            cards[card + i] += cards[card];
        }

        card++;
    }

    long sum = 0;

    for (int i = 0; i < card; i++)
    {
        sum += cards[i];
    }

    printf("04b %ld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
