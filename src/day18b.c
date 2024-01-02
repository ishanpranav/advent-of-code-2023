// Licensed under the MIT License.

// Lavaduct Lagoon Part 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 16

long long math_pick_theorem_a(long long b, long long i)
{
    return i + (b / 2) - 1;
}

int main(void)
{
    long long b = 0;
    long long i = 0;
    long long x = 0;
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stdin))
    {
        char* token = strchr(buffer, '#');

        if (!token)
        {
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        long n = strtol(token + 1, NULL, 16);
        int length = n >> 4;

        switch (n & 0xf)
        {
            case 0:
                x -= length;
                break;

            case 1:
                i -= x * length;
                break;

            case 2:
                x += length;
                break;

            case 3:
                i += x * length;
                break;
        }

        b += length;
    }

    long long area = math_pick_theorem_a(b, i) + 2;

    printf("18b %lld %lf\n", area, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
