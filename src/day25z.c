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
#define GRAPH_BUCKETS 3079
#define KEY_SIZE 3
#define VERTEX_MAX_DEGREE 16
#define VERTEX_QUEUE_CAPACITY 512

struct Vertex;

struct Edge
{
    struct Vertex* target;
    int capacity;
};

struct Vertex
{
    struct Vertex* nextVertex;
    struct Vertex* source;
    struct Edge edges[VERTEX_MAX_DEGREE];
    int degree;
    char key[KEY_SIZE];
};

struct VertexQueue
{
    struct Vertex* items[VERTEX_QUEUE_CAPACITY];
    int first;
    int last;
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

struct GraphIterator
{
    struct Vertex* currentVertex;
    struct GraphBucket* currentBucket;
};

struct MinCut
{
    int value;
    int reachable;
    int nonReachable;
};

typedef char* String;
typedef struct Vertex* Vertex;
typedef struct Edge* Edge;
typedef struct VertexQueue* VertexQueue;
typedef struct GraphBucket* GraphBucket;
typedef struct Graph* Graph;
typedef struct GraphIterator* GraphIterator;
typedef struct MinCut* MinCut;

void vertex_add_edge(Vertex instance, Vertex target, int capacity)
{
    instance->edges[instance->degree].target = target;
    instance->edges[instance->degree].capacity = capacity;
    instance->degree++;
}

int* vertex_capacity(Vertex instance, Vertex target)
{
    for (Edge e = instance->edges; e < instance->edges + instance->degree; e++)
    {
        if (e->target == target)
        {
            return &e->capacity;
        }
    }

    return NULL;
}

void vertex_queue(VertexQueue instance)
{
    instance->first = -1;
    instance->last = -1;
}

void vertex_queue_enqueue(VertexQueue instance, Vertex item)
{
    if (instance->first == -1)
    {
        instance->first = 0;
        instance->last = 0;
    }
    else if (instance->first && instance->last == VERTEX_QUEUE_CAPACITY - 1)
    {
        instance->last = 0;
    }
    else
    {
        instance->last++;
    }

    instance->items[instance->last] = item;
}

Vertex vertex_queue_dequeue(VertexQueue instance)
{
    if (instance->first == -1)
    {
        return NULL;
    }

    Vertex result = instance->items[instance->first];

    if (instance->first == instance->last)
    {
        instance->first = -1;
        instance->last = -1;
    }
    else if (instance->first == VERTEX_QUEUE_CAPACITY - 1)
    {
        instance->first = 0;
    }
    else
    {
        instance->first++;
    }

    return result;
}

void graph_begin(Graph instance, GraphIterator iterator)
{
    iterator->currentBucket = instance->firstBucket;

    if (iterator->currentBucket)
    {
        iterator->currentVertex = iterator->currentBucket->firstVertex;
    }
    else
    {
        iterator->currentVertex = NULL;
    }
}

bool graph_end(GraphIterator iterator)
{
    return !iterator->currentVertex && !iterator->currentBucket;
}

void graph_next(GraphIterator iterator)
{
    if (graph_end(iterator))
    {
        return;
    }

    if (iterator->currentVertex)
    {
        iterator->currentVertex = iterator->currentVertex->nextVertex;
    }

    if (!iterator->currentVertex)
    {
        iterator->currentBucket = iterator->currentBucket->nextBucket;

        if (iterator->currentBucket)
        {
            iterator->currentVertex = iterator->currentBucket->firstVertex;
        }
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
    u->degree = 0;
    *p = u;

    return u;
}

bool graph_search(Graph instance, Vertex source, Vertex target)
{
    struct GraphIterator iter;

    for (graph_begin(instance, &iter); !graph_end(&iter); graph_next(&iter))
    {
        iter.currentVertex->source = NULL;
    }

    source->source = source;

    Vertex current;
    struct VertexQueue queue;

    vertex_queue(&queue);
    vertex_queue_enqueue(&queue, source);

    while ((current = vertex_queue_dequeue(&queue)))
    {
        for (Edge e = current->edges; e < current->edges + current->degree; e++)
        {
            if (e->capacity <= 0 || e->target->source)
            {
                continue;
            }

            e->target->source = current;

            vertex_queue_enqueue(&queue, e->target);
        }
    }

    return target->source;
}

void graph_min_cut(Graph instance, Vertex source, Vertex target, MinCut result)
{
    result->value = 0;
    result->reachable = 0;
    result->nonReachable = 0;

    struct GraphIterator iter;

    for (graph_begin(instance, &iter); !graph_end(&iter); graph_next(&iter))
    {
        Vertex u = iter.currentVertex;

        for (Edge e = u->edges; e < u->edges + u->degree; e++)
        {
            e->capacity = 1;
        }
    }

    while (graph_search(instance, source, target))
    {
        int flow = INT_MAX;

        for (Vertex u = target; u != source; u = u->source)
        {
            int* capacity = vertex_capacity(u->source, u);

            if (*capacity < flow)
            {
                flow = *capacity;
            }

            *capacity -= flow;
            *vertex_capacity(u, u->source) += flow;
        }

        result->value += flow;
    }

    for (graph_begin(instance, &iter); !graph_end(&iter); graph_next(&iter))
    {
        if (iter.currentVertex->source)
        {
            result->reachable++;
        }
        else
        {
            result->nonReachable++;
        }
    }
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

long scan(Graph graph)
{
    struct MinCut result;
    struct GraphIterator u;

    for (graph_begin(graph, &u); !graph_end(&u); graph_next(&u))
    {
        struct GraphIterator v = u;

        for (graph_next(&v); !graph_end(&v); graph_next(&v))
        {
            graph_min_cut(graph, u.currentVertex, v.currentVertex, &result);

            if (result.value == 3)
            {
                return result.reachable * result.nonReachable;
            }
        }
    }

    return -1;
}

int main(void)
{
    char buffer[BUFFER_SIZE];
    struct Graph g = { 0 };
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

            vertex_add_edge(u, v, 1);
            vertex_add_edge(v, u, 1);
        }
    }

    long result = scan(&g);

    printf("25z %ld %lf\n", result, (double)(clock() - start) / CLOCKS_PER_SEC);
    graph_clear(&g);

    return 0;
}
