// Licensed under the MIT License.

// Haunted Wasteland Part 1

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 32
#define DIRECTIONS_CAPACITY 512

enum Vertex
{
    VERTEX_OFFSET = -13330,
    VERTEX_LAST = 46655 + VERTEX_OFFSET,
    VERTEX_NONE = VERTEX_LAST + 1
};

struct VertexPair
{
    enum Vertex left;
    enum Vertex right;
};

struct Graph
{
    struct VertexPair vertices[VERTEX_NONE];
};

typedef char* String;
typedef enum Vertex Vertex;
typedef struct Graph* Graph;

void graph_add(Graph instance, Vertex vertex, Vertex left, Vertex right)
{
    instance->vertices[vertex].left = left;
    instance->vertices[vertex].right = right;
}

static bool parse(char buffer[], char window[], Vertex* result)
{
    memcpy(window, buffer, 3);

    window[3] = '\0';

    Vertex vertex = strtol(window, NULL, 36) + VERTEX_OFFSET;

    if (vertex < 0)
    {
        return false;
    }

    *result = vertex;

    return true;
}

static bool read(FILE* stream, Graph graph)
{
    char buffer[BUFFER_SIZE];

    if (!fgets(buffer, sizeof buffer, stream))
    {
        return false;
    }

    while (fgets(buffer, sizeof buffer, stream))
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
            return false;
        }

        graph_add(graph, vertex, left, right);
    }

    return true;
}

static int scan(Graph instance, Vertex start, String directions)
{
    int result = 0;
    char* direction = directions;

    while (start != VERTEX_LAST)
    {
        switch (*direction)
        {
            case 'L':
                start = instance->vertices[start].left;
                direction++;
                result++;
                break;
            case 'R':
                start = instance->vertices[start].right;
                direction++;
                result++;
                break;
            default:
                direction = directions;
                break;
        }
    }

    return result;
}

int main()
{
    Graph graph = malloc(sizeof * graph);

    if (!graph)
    {
        fprintf(stderr, "Error: Out of memory.\n");

        return 1;
    }

    char directions[DIRECTIONS_CAPACITY];
    clock_t start = clock();

    if (!fgets(directions, sizeof directions, stdin) || !read(stdin, graph))
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    int result = scan(graph, 0, directions);

    printf("08a %d %lf\n", result, (double)(clock() - start) / CLOCKS_PER_SEC);
    free(graph);

    return 0;
}
