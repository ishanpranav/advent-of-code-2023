// Licensed under the MIT License.

// Snowverload

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 64
#define DELIMITERS " \n"
#define EXCEPTION_OUT_OF_MEMORY "Error: Out of memory.\n"
#define GRAPH_BUCKETS 53
#define GRAPH_MAX_SIZE 64
#define KEY_SIZE 3
#define VERTEX_MAX_DEGREE 8

struct Vertex
{
    struct Vertex* nextVertex;
    struct Vertex* targets[VERTEX_MAX_DEGREE];
    int degree;
    char key[KEY_SIZE];
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
typedef struct GraphBucket* GraphBucket;
typedef struct Graph* Graph;

void vertex_add_target(Vertex instance, Vertex item)
{
    instance->targets[instance->degree] = item;
    instance->degree++;
}

void graph(Graph instance)
{
    instance->firstBucket = NULL;

    for (int i = 0; i < GRAPH_BUCKETS; i++)
    {
        instance->buckets[i].firstVertex = NULL;
        instance->buckets[i].nextBucket = NULL;
    }
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

Vertex graph_vertex(Graph instance, char key[])
{
    Vertex* p;
    unsigned int hash = graph_hash(key);

    for (p = &instance->buckets[hash].firstVertex; *p; p = &(*p)->nextVertex)
    {
        if (memcmp(key, (*p)->key, KEY_SIZE) == 0)
        {
            return *p;
        }
    }

    Vertex u = malloc(sizeof * u);

    if (!u)
    {
        return NULL;
    }

    if (!instance->buckets[hash].firstVertex)
    {
        GraphBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;
    }

    memcpy(u->key, key, KEY_SIZE);

    u->nextVertex = NULL;
    *p = u;

    return u;
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

static int dbgorder(Graph g)
{
    int q = 0;
    for (GraphBucket b = g->firstBucket;b;b = b->nextBucket)
        for (Vertex v = b->firstVertex;v;v = v->nextVertex)
            q++;
    return q;
}

static int dbgprint(Graph g)
{
    int q = 0;

    for (GraphBucket b = g->firstBucket;b;b = b->nextBucket)
        for (Vertex u = b->firstVertex; u; u = u->nextVertex)
        {
            printf("%c%c%c -> ", u->key[0], u->key[1], u->key[2]);
            for (Vertex* v = u->targets; v < u->targets + u->degree; v++) {
                printf("%c%c%c, ", (*v)->key[0], (*v)->key[1], (*v)->key[2]);
            }
            printf("\n");
        }

    return q;
}

int main(void)
{
    char buffer[BUFFER_SIZE];
    struct Graph g = { 0 };
    clock_t start = clock();

    graph(&g);

    while (fgets(buffer, sizeof buffer, stdin))
    {
        char* mid = strstr(buffer, ": ");

        if (!mid)
        {
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        *mid = '\0';

        Vertex u = graph_vertex(&g, buffer);

        if (!u)
        {
            fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

            return 1;
        }

        for (String target = strtok(mid + 2, DELIMITERS);
            target;
            target = strtok(NULL, DELIMITERS))
        {
            Vertex v = graph_vertex(&g, target);

            if (!v)
            {
                fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

                return 1;
            }

            vertex_add_target(u, v);
            vertex_add_target(v, u);
        }
    }

    dbgprint(&g);
    printf("Order: %d\n", dbgorder(&g));
    printf("25z %d %lf\n", 0, (double)(clock() - start) / CLOCKS_PER_SEC);
    graph_clear(&g);

    return 0;
}
