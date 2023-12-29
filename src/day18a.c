// Licensed under the MIT License.

// Lavaduct Lagoon Part 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 16
#define DELIMITERS " "

typedef char* String;

long math_pick_theorem_a(long b, long i)
{
    return i + (b / 2) - 1;
}

static String read(String buffer, char* direction)
{
    String token = strtok(buffer, DELIMITERS);

    if (!token)
    {
        return NULL;
    }

    *direction = token[0];
    token = strtok(NULL, DELIMITERS);

    if (!token)
    {
        return NULL;
    }

    return token;
}

int main()
{
    int b = 0;
    int i = 0;
    int x = 0;
    int y = 0;
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stdin))
    {
        char direction;
        String token = read(buffer, &direction);

        if (!token)
        {
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        int length = atoi(token);

        switch (direction)
        {
            case 'U':
                y += length;
                i += x * length;
                break;

            case 'D':
                y -= length;
                i -= x * length;
                break;

            case 'L':
                x += length;
                break;

            case 'R':
                x -= length;
                break;
        }

        b += length;
    }

    long area = math_pick_theorem_a(b, i) + 2;

    printf("18a %ld %lf\n", area, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
