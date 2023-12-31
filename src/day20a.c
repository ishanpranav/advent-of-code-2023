// Licensed under the MIT License.

// Pulse Propagation Part 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 32
#define DELIMITERS ", "
#define EXCEPTION_FORMAT "Error: Format.\n"
#define EXCEPTION_OUT_OF_MEMORY "Error: Out of memory.\n"
#define GRAPH_MAX_DEGREE 5

enum Vertex
{
    VERTEX_OFFSET = -369,
    VERTEX_LAST = 24187,
    VERTEX_NONE = VERTEX_LAST + 1
};

struct Graph
{
    int edges[VERTEX_NONE][GRAPH_MAX_DEGREE];
};

union IModule
{
    struct ConjunctionModule conjunction;
    struct FlipFlopModule flipFlop;
};

struct Module
{
    char type;
    union IModule implementation;
};

struct ConjunctionModule
{

};

struct FlipFlopModule
{

};

struct Set
{
    long values[VERTEX_NONE];
};

typedef char* String;
typedef enum Vertex Vertex;
typedef struct Graph* Graph;
typedef struct Dictionary* Dictionary;

Vertex vertex_from_string(String value)
{
    if (strcmp(value, "output") == 0)
    {
        return VERTEX_LAST;
    }

    return strtol(value, NULL, 36) + VERTEX_OFFSET;
}

void graph_add_vertex(Graph instance, Vertex item)
{
    instance->vertices[instance->vertexCount] = item;
    instance->vertexCount++;
}

void graph_add_edge(Graph instance, Vertex source, Vertex target)
{
    int i = 0;

    while (instance->edges[source][i])
    {
        i++;
    }

    instance->edges[source][i] = target;
}

int main()
{
    Graph graph = malloc(sizeof * graph);

    if (!graph)
    {
        fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

        return 1;
    }

    Dictionary flipFlops = malloc(sizeof * flipFlops);

    if (!flipFlops)
    {
        fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

        return 1;
    }

    Dictionary conjunctions = malloc(sizeof * conjunctions);

    if (!conjunctions)
    {
        fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

        return 1;
    }

    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    dictionary(flipFlops);
    dictionary(conjunctions);

    while (fgets(buffer, sizeof buffer, stdin))
    {
        Vertex source;
        char* mid = strstr(buffer, " -> ");

        if (!mid)
        {
            fprintf(stderr, EXCEPTION_FORMAT);

            return 1;
        }

        *mid = '\0';

        switch (buffer[0])
        {
            case '%':
                source = vertex_from_string(buffer + 1);
                break;

            case '&':
                source = vertex_from_string(buffer + 1);
                break;

            default:
                source = 0;
                break;
        }

        graph_add_vertex(source, source);

        for (String token = strtok(mid + 4, DELIMITERS);
            token;
            token = strtok(NULL, DELIMITERS))
        {
            graph_add_edge(graph, source, vertex_from_string(token));
        }


    }

    int result = 0;

    printf("20a %d %lf\n", result, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
