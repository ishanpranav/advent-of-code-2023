// Licensed under the MIT License.

// Wait For It Part 2

#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#define BUFFER_SIZE 64

static bool read(FILE* stream, long long* result)
{
    char buffer[BUFFER_SIZE];

    if (!fgets(buffer, sizeof buffer, stream))
    {
        return false;
    }

    long long number = 0;

    for (char* p = buffer; *p; p++)
    {
        if (!isdigit(*p))
        {
            continue;
        }

        number = (number * 10) + (*p - '0');
    }

    *result = number;

    return true;
}

int main(void)
{
    long long t;
    long long dx;
    clock_t start = clock();

    if (!read(stdin, &t) || !read(stdin, &dx))
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    long long result = ceil((t + sqrt(t * t - 4 * dx)) / 2)
        - floor((t - sqrt(t * t - 4 * dx)) / 2)
        - 1;

    printf("06b %lld %lf\n", result, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
