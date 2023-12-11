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
        printf("Usage: day6a <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    if (!fgets(buffer, sizeof buffer, stream))
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    int pointCount = 0;
    struct Point points[POINTS_CAPACITY];

    strtok(buffer, DELIMITERS);

    String token = strtok(NULL, DELIMITERS);

    for (pointCount = 0; pointCount < POINTS_CAPACITY && token; pointCount++)
    {
        points[pointCount].time = atoi(token);
        token = strtok(NULL, DELIMITERS);
    }

    if (!fgets(buffer, sizeof buffer, stream) || !strtok(buffer, DELIMITERS))
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    token = strtok(NULL, DELIMITERS);

    for (int i = 0; i < pointCount && token; i++)
    {
        points[i].distance = atoi(token);
        token = strtok(NULL, DELIMITERS);
    }

    long product = 1;

    for (int i = 0; i < pointCount; i++)
    {
        Point point = points[i];
        int t = point.time;
        int dx = point.distance;

        product *= ceil((t + sqrt(t * t - 4 * dx)) / 2)
            - floor((t - sqrt(t * t - 4 * dx)) / 2)
            - 1;
    }

    printf("%ld : %lf\n", product, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
