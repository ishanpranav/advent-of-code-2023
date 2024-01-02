// Licensed under the MIT License.

// Cube Conundrum Part 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 256
#define DELIMITERS ";, "
#define EXCEPTION_FORMAT "Error: Format.\n"

typedef char* String;

int main(void)
{
    char buffer[BUFFER_SIZE];
    long sum = 0;
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stdin))
    {
        char* start = strchr(buffer, ':');

        if (!start || !strtok(start, DELIMITERS))
        {
            fprintf(stderr, EXCEPTION_FORMAT);

            return 1;
        }

        String token;
        int r = 0;
        int g = 0;
        int b = 0;
        int lastNumber = 0;

        while ((token = strtok(NULL, DELIMITERS)))
        {
            if (!lastNumber)
            {
                lastNumber = atoi(token);

                continue;
            }

            switch (token[0])
            {
                case 'r':
                    if (lastNumber > r)
                    {
                        r = lastNumber;
                    }
                    break;
                case 'g':
                    if (lastNumber > g)
                    {
                        g = lastNumber;
                    }
                    break;
                case 'b':
                    if (lastNumber > b)
                    {
                        b = lastNumber;
                    }
                    break;
                default:
                    fprintf(stderr, EXCEPTION_FORMAT);
                    return 1;
            }

            lastNumber = 0;
        }

        sum += r * g * b;
    }

    printf("02b %ld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
