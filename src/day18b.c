// Licensed under the MIT License.

// Lavaduct Lagoon Part 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 16
#define DELIMITERS " "

typedef char* String;

long long math_pick_theorem_a(long long b, long long i)
{
    return i + (b / 2) - 1;
}

static String read(String buffer)
{
    if (!strtok(buffer, DELIMITERS) || !strtok(NULL, DELIMITERS))
    {
        return NULL;
    }

    String token = strtok(NULL, DELIMITERS);

    if (!token)
    {
        return NULL;
    }

    char* first = strchr(token, '#');

    if (!first)
    {
        return NULL;
    }

    char* last = strchr(first, ')');

    if (!last)
    {
        return NULL;
    }

    *last = '\0';

    return first + 1;
}

int main()
{
    long long b = 0;
    long long i = 0;
    long long x = 0;
    long long y = 0;
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stdin))
    {
        String token = read(buffer);

        if (!token)
        {
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        long n = strtol(token, NULL, 16);
        int length = n >> 4;

        switch (n % 16)
        {
            case 0:
                x -= length;
                break;

            case 1:
                y -= length;
                i -= x * length;
                break;

            case 2:
                x += length;
                break;

            case 3:
                y += length;
                i += x * length;
                break;
        }

        b += length;
    }

    long long area = math_pick_theorem_a(b, i) + 2;

    printf("18b %lld %lf\n", area, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
