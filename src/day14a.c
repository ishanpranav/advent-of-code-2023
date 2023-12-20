// Licensed under the MIT License.

// The Floor Will Be Lava Part 1

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 8

typedef char* String;

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day14a <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    long total = 0;
    int i = 0;
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stream))
    {
        int j = 0;
        char current;

        for (char* p = buffer;*p;p++)
        {
            if (*p == 0) 
            {

            }
        }

        i++;
    }

    printf("%ld : %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
