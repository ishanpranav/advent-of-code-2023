// Licensed under the MIT License.

// Snowverload

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 64
#define DELIMITERS " \n"
#define GRAPH_BUCKETS 1
#define KEY_SIZE 3
#define VERTEX_MAX_DEGREE 8

struct Vertex
{
    struct Vertex* nextVertex;
    int degree;
    char key[KEY_SIZE];
    char targets[VERTEX_MAX_DEGREE][KEY_SIZE];
};

struct GraphBucket
{
    struct Vertex* firstVertex;
    struct GraphBucket* nextBucket;
};

struct Graph
{
    struct GraphBucket* firstBucket;
    struct GraphBucket buckets[GRAPH_BUCKETS];
};

typedef char* String;
typedef struct Vertex* Vertex;
typedef struct Graph* Graph;
typedef struct GraphBucket* GraphBucket;

void vertex(Vertex instance, char key[])
{
    memcpy(instance->key, key, KEY_SIZE);

    instance->degree = 0;
    instance->nextVertex = NULL;
}

void vertex_add_target(Vertex instance, char key[])
{
    for (int d = 0; d < instance->degree; d++)
    {
        if (memcmp(instance->targets[d], key, KEY_SIZE) == 0)
        {
            return;
        }
    }

    memcpy(instance->targets[instance->degree], key, KEY_SIZE);

    instance->degree++;
}

static unsigned int graph_hash(char key[])
{
    unsigned int hash = 0;

    for (int i = 0; i < KEY_SIZE; i++)
    {
        hash = (hash * 36) + key[i] - 97;
    }

    return hash % GRAPH_BUCKETS;
}

Vertex graph_get_vertex(Graph instance, char key[])
{
    unsigned int hash = graph_hash(key) % GRAPH_BUCKETS;

    for (Vertex u = instance->buckets[hash].firstVertex; u; u = u->nextVertex)
    {
        if (memcmp(u->key, key, KEY_SIZE) == 0)
        {
            return u;
        }
    }

    return NULL;
}

void graph_add_vertex(Graph instance, Vertex u)
{
    unsigned int hash = graph_hash(u->key) % GRAPH_BUCKETS;

    if (!instance->buckets[hash].firstVertex)
    {
        GraphBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;
    }

    u->nextVertex = instance->buckets[hash].firstVertex;
    instance->buckets[hash].firstVertex = u;
}

void graph_clear(Graph instance)
{
    for (GraphBucket bucket = instance->firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        Vertex u = bucket->firstVertex;

        while (u)
        {
            Vertex v = u->nextVertex;

            free(u);

            u = v;
        }

        bucket->firstVertex = NULL;
    }

    instance->firstBucket = NULL;
}

int main(void)
{
    char buffer[BUFFER_SIZE];
    struct Graph graph = { 0 };
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stdin))
    {
        char* mid = strstr(buffer, ": ");

        if (!mid)
        {
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        *mid = '\0';

        Vertex u = malloc(sizeof * u);

        if (!u)
        {
            fprintf(stderr, "Error: Out of memory.\n");

            return 1;
        }

        vertex(u, buffer);
        graph_add_vertex(&graph, u);

        for (String target = strtok(mid + 2, DELIMITERS);
            target;
            target = strtok(NULL, DELIMITERS))
        {
            vertex_add_target(u, target);
        }
    }

    for (GraphBucket bucket = graph.firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        for (Vertex u = bucket->firstVertex; u; u = u->nextVertex)
        {
            for (int d = 0; d < u->degree; d++)
            {
                Vertex v = graph_get_vertex(&graph, u->targets[d]);

                if (!v)
                {
                    Vertex v = malloc(sizeof * v);

                    if (!v)
                    {
                        fprintf(stderr, "Error: Out of memory.\n");

                        return 1;
                    }

                    vertex(v, u->targets[d]);
                    graph_add_vertex(&graph, v);
                }

                vertex_add_target(v, u->key);
            }
        }
    }

    printf("25z %d %lf\n", 0, (double)(clock() - start) / CLOCKS_PER_SEC);
    graph_clear(&graph);

    return 0;
}
