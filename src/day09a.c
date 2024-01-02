// Licensed under the MIT License.

// Mirage Maintenance Part 1

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 128
#define DELIMITERS " "
#define TERMS_CAPACITY 32

struct Series
{
    long terms[TERMS_CAPACITY];
    int count;
};

typedef char* String;
typedef struct Series* Series;

void series(Series instance)
{
    instance->count = 0;
}

void series_add(Series instance, long term)
{
    int n = instance->count;

    instance->terms[n] = term;
    instance->count = n + 1;
}

long series_lagrange_extrapolate(Series instance, int k)
{
    double sum = 0;
    long n = instance->count;

    for (int j = 0; j < n; j++)
    {
        double product = instance->terms[j];

        for (int i = 0; i < j; i++)
        {
            product *= (double)(k - i) / (j - i);
        }

        for (int i = j + 1; i < n; i++)
        {
            product *= (double)(k - i) / (j - i);
        }

        sum += product;
    }

    return lround(sum);
}

long series_lagrange_next(Series instance)
{
    return series_lagrange_extrapolate(instance, instance->count);
}

int main(void)
{
    long sum = 0;
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stdin))
    {
        String token = strtok(buffer, DELIMITERS);

        if (!token)
        {
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        struct Series terms;

        series(&terms);

        do
        {
            series_add(&terms, atol(token));
        }
        while ((token = strtok(NULL, DELIMITERS)));

        sum += series_lagrange_next(&terms);
    }

    printf("09a %ld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
