// Licensed under the MIT License.

// Pulse Propagation Part 1

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 1000

union U1
{
    bool pulse;
};

struct Module
{
    union U1 impl;
    bool isConjunction;
};

int main()
{
    clock_t start = clock();
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, sizeof buffer, stdin))
    {
        char* mid = strstr(buffer, " -> ");

        if (!mid)
        {
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        *mid = '\0';

        struct Module module;

        module.isConjunction = buffer[0] == '%';

        printf("%s -> %s", buffer + 1, mid + 4);
        module_collection_add(&modules, module);
    }

    long result = 0;
    printf("20a %ld %lf\n", result, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
