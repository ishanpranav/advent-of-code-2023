// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Optimized using precalculated quadratic equation
// Time complexity: O(n) where n is the number of points

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 64
#define DELIMITERS " "
#define POINTS_CAPACITY 4

struct Point
{
    int time;
    int distance;
};

typedef char* String;
typedef struct Point Point;

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day06a <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    String token;
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    if (!fgets(buffer, sizeof buffer, stream) || !strtok(buffer, DELIMITERS))
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    int pointCount = 0;
    struct Point points[POINTS_CAPACITY];

    while ((token = strtok(NULL, DELIMITERS)))
    {
        points[pointCount].time = atoi(token);
        pointCount++;
    }

    if (!fgets(buffer, sizeof buffer, stream) || !strtok(buffer, DELIMITERS))
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    for (int i = 0; i < pointCount; i++)
    {
        token = strtok(NULL, DELIMITERS);

        if (!token)
        {
            fclose(stream);
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        points[i].distance = atoi(token);
    }

    long product = 1;

    for (int i = 0; i < pointCount; i++)
    {
        int t = points[i].time;
        int dx = points[i].distance;

        product *= ceil((t + sqrt(t * t - 4 * dx)) / 2)
            - floor((t - sqrt(t * t - 4 * dx)) / 2)
            - 1;
    }

    printf("%ld : %lf\n", product, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
