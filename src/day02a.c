// Licensed under the MIT License.

// Cube Conundrum Part 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 256
#define DELIMITERS ";, "
#define EXCEPTION_FORMAT "Error: Format.\n"

enum PossibleResult
{
    POSSIBLE_RESULT_IMPOSSIBLE,
    POSSIBLE_RESULT_POSSIBLE,
    POSSIBLE_RESULT_FORMAT_EXCEPTION
};

typedef char* String;
typedef enum PossibleResult PossibleResult;

static PossibleResult possible(char token, int number)
{
    switch (token)
    {
        case 'r':
            if (number > 12)
            {
                return POSSIBLE_RESULT_IMPOSSIBLE;
            }
            break;
        case 'g':
            if (number > 13)
            {
                return POSSIBLE_RESULT_IMPOSSIBLE;
            }
            break;
        case 'b':
            if (number > 14)
            {
                return POSSIBLE_RESULT_IMPOSSIBLE;
            }
            break;
        default:
            return POSSIBLE_RESULT_FORMAT_EXCEPTION;
    }

    return POSSIBLE_RESULT_POSSIBLE;
}

int main(void)
{
    char buffer[BUFFER_SIZE];
    long sum = 0;
    long game = 1;
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
        int lastNumber = 0;
        PossibleResult lastPossibleResult = POSSIBLE_RESULT_POSSIBLE;

        while ((token = strtok(NULL, DELIMITERS)))
        {
            if (!lastNumber)
            {
                lastNumber = atoi(token);

                continue;
            }

            lastPossibleResult = possible(token[0], lastNumber);
            lastNumber = 0;

            if (lastPossibleResult != POSSIBLE_RESULT_POSSIBLE)
            {
                break;
            }
        }

        if (lastPossibleResult == POSSIBLE_RESULT_FORMAT_EXCEPTION)
        {
            fprintf(stderr, EXCEPTION_FORMAT);

            return 1;
        }

        if (lastPossibleResult == POSSIBLE_RESULT_POSSIBLE)
        {
            sum += game;
        }

        game++;
    }

    printf("02a %ld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
