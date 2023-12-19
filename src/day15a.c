// Licensed under the MIT license.

// Lens Library Part 1

#include <stdio.h>
#include <time.h>

typedef char* String;

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day15a <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    long sum = 0;
    int hash = 0;
    char current;
    clock_t start = clock();

    while ((current = getc(stream)) != EOF)
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

    printf("%ld : %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
