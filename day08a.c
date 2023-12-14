// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Custom implementation
// Graph: use base-36 encoding to index into the hashtable
// This results in somewhat fragmented memory but saves time

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 32
#define DIRECTIONS_CAPACITY 512
#define DIRECTION_LEFT 'L'
#define DIRECTION_RIGHT 'R'
#define VERTEX_START 13330
#define VERTEX_END 46655

struct VertexPair
{
    int left;
    int right;
};

struct Graph
{
    struct VertexPair vertices[VERTEX_END + 1];
};

typedef char* String;
typedef int Vertex;
typedef char Direction;
typedef struct Graph* Graph;

void graph_add(Graph graph, Vertex vertex, Vertex left, Vertex right)
{
    graph->vertices[vertex].left = left;
    graph->vertices[vertex].right = right;
}

static bool parse(char buffer[], char window[], Vertex* result)
{
    memcpy(window, buffer, 3);

    window[3] = '\0';

    Vertex vertex = strtol(window, NULL, 36);

    if (vertex < 0)
    {
        return false;
    }

    *result = vertex;

    return true;
}

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day8a <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    int total = 0;
    char buffer[BUFFER_SIZE];
    Direction directions[DIRECTIONS_CAPACITY];
    struct Graph graph;
    clock_t start = clock();

    if (!fgets(directions, DIRECTIONS_CAPACITY, stream) ||
        !fgets(buffer, BUFFER_SIZE, stream))
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    while (fgets(buffer, BUFFER_SIZE, stream))
    {
        char window[4];
        Vertex vertex;
        Vertex left;
        Vertex right;

        if (strlen(buffer) < 16 ||
            !parse(buffer, window, &vertex) ||
            !parse(buffer + 7, window, &left) ||
            !parse(buffer + 12, window, &right))
        {
            fclose(stream);
            fprintf(stderr, "Erorr: Format.\n");

            return 1;
        }

        graph_add(&graph, vertex, left, right);
    }

    Vertex current = VERTEX_START;
    Direction* direction = directions;

    while (current != VERTEX_END)
    {
        switch (*direction) 
        {
        case DIRECTION_LEFT:
            current = graph.vertices[current].left;
            direction++;
            total++;
            break;
        case DIRECTION_RIGHT:
            current = graph.vertices[current].right;
            direction++;
            total++;
            break;
        default:
            direction = directions;
            break;
        }
    }

    printf("%d : %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
