// Licensed under the MIT license.

// Lens Library Part 1

#include <stdio.h>
#include <time.h>

int main(void)
{
    long sum = 0;
    int hash = 0;
    char current;
    clock_t start = clock();

    while ((current = getc(stdin)) != EOF)
    {
        switch (current)
        {
            case '\n': continue;
            case ',':
                sum += hash;
                hash = 0;
                continue;
        }

        hash = ((hash + current) * 17) % 256;
    }

    sum += hash;

    printf("15a %ld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
