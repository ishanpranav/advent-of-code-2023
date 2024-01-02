// Licensed under the MIT License.

// Cosmic Expansion Part 2

#include <stdio.h>
#include <time.h>
#define DIMENSION 141
#define SCALE 1000000

static long long scan(long long vector[], int max, int galaxies)
{
    int i = 1;
    long long result = 0;
    long long distance = 1;
    long long passed = vector[0];

    while (i <= max)
    {
        long long value = vector[i];

        if (value)
        {
            result += ((2 * passed) - galaxies + value) * value * distance;
            passed += value;
            distance++;
        }
        else
        {
            distance += SCALE;
        }

        i++;
    }

    return result;
}

int main(void)
{
    int n = 0;
    int galaxies = 0;
    int minI = DIMENSION;
    int minJ = DIMENSION;
    int maxI = 0;
    int maxJ = 0;
    long long x[DIMENSION] = { 0 };
    long long y[DIMENSION] = { 0 };
    char buffer[DIMENSION + 1];
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stdin))
    {
        char current;

        for (int j = 0; (current = buffer[j]); j++)
        {
            if (buffer[j] == '#')
            {
                y[n]++;
                x[j]++;
                galaxies++;
            }

            if (j > maxJ)
            {
                maxJ = j;
            }
            else if (j < minJ)
            {
                minJ = j;
            }
        }

        if (y[n])
        {
            maxI = n;

            if (n < minI)
            {
                minI = n;
            }
        }

        n++;
    }

    long long result =
        scan(y + minI, maxI - minI, galaxies) +
        scan(x + minJ, maxJ - minJ, galaxies);

    printf("11b %lld %lf\n", result, (double)(clock() - start) / CLOCKS_PER_SEC);
}
