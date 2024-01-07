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
#define KEY_SIZE 3
#define VERTEX_MAX_DEGREE 16
#define VERTEX_QUEUE_CAPACITY 30000

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
typedef struct MinCut* MinCut;

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

    u->source = NULL;
    u->nextVertex = NULL;
    u->degree = 0;
    *p = u;

    return u;
}

bool graph_search(Graph instance, Vertex source, Vertex target)
{
    for (GraphBucket bucket = instance->firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        for (Vertex u = bucket->firstVertex; u; u = u->nextVertex)
        {
            u->source = NULL;
        }
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

    for (GraphBucket bucket = instance->firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        for (Vertex u = bucket->firstVertex; u; u = u->nextVertex)
        {
            for (Edge e = u->edges; e < u->edges + u->degree; e++)
            {
                e->capacity = 1;
            }
        }
    }

    while (graph_search(instance, source, target))
    {
        int flow = INT_MAX;

        for (Vertex u = target; u != source; u = u->source)
        {
            int reverse = *vertex_capacity(u->source, u);

            if (reverse < flow)
            {
                flow = reverse;
            }
        }

        result->value += flow;

        Vertex v;

        for (Vertex u = target; u != source; u = v)
        {
            v = u->source;

            int* cp = vertex_capacity(v, u);
            *cp -= flow;
            cp = vertex_capacity(u, v);
            *cp += flow;
        }
    }

    for (GraphBucket bucket = instance->firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        for (Vertex u = bucket->firstVertex; u; u = u->nextVertex)
        {
            if (u->source)
            {
                result->reachable++;
            }
            else
            {
                result->nonReachable++;
            }
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
    Vertex vcx[10000];
    int sp = 0;
    for (GraphBucket b = graph->firstBucket;b;b = b->nextBucket)
        for (Vertex u = b->firstVertex; u; u = u->nextVertex)
        {
            vcx[sp] = u;
            sp++;
        }

    for (int i = 0; i < sp - 1; i++)
    {
        for (int j = i + 1; j < sp; j++)
        {
            graph_min_cut(graph, vcx[i], vcx[j], &result);

            if (result.value == 3)
            {
                return result.reachable * result.nonReachable;
            }
        }
    }

    return 0;
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

            vertex_add_edge(u, v, 1);
            vertex_add_edge(v, u, 1);
        }
    }

    long product = scan(&g);

    printf("25z %ld %lf\n", product, (double)(clock() - start) / CLOCKS_PER_SEC);
    graph_clear(&g);

    return 0;
}
