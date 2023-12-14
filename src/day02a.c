// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Based on B.H.'s algorithm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 256

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

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day02a <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    char buffer[BUFFER_SIZE];
    long sum = 0;
    long game = 1;
    String delimiters = ";, ";
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stream))
    {
        char* start = strchr(buffer, ':');

        if (!start || !strtok(start, delimiters))
        {
            fclose(stream);
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        String token;
        int lastNumber = 0;
        PossibleResult lastPossibleResult = POSSIBLE_RESULT_POSSIBLE;

        while ((token = strtok(NULL, delimiters)))
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
            fclose(stream);
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        if (lastPossibleResult == POSSIBLE_RESULT_POSSIBLE)
        {
            sum += game;
        }

        game++;
    }

    printf("%ld : %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
