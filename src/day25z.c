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
#define GRAPH_EDGES_CAPACITY 64
#define KEY_SIZE 3

struct Vertex
{
    struct Vertex* nextVertex;
    int height;
    int excess;
    char key[KEY_SIZE];
};

struct Edge
{
    struct Vertex* u;
    struct Vertex* v;
    int flow;
    int capacity;
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
    struct Vertex** vertices;
    struct Edge edges[GRAPH_EDGES_CAPACITY];
    int size;
};

typedef char* String;
typedef struct Vertex* Vertex;
typedef struct Edge* Edge;
typedef struct GraphBucket* GraphBucket;
typedef struct Graph* Graph;

void graph(Graph instance)
{
    instance->firstBucket = NULL;
    instance->size = 0;

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

Edge graph_new_edge(Graph instance)
{
    Edge result = instance->edges + instance->size;

    instance->size++;

    return result;
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

    u->height = 0;
    u->excess = 0;
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

static void dbgedge(Graph g, Vertex u, Vertex v, int capacity)
{
    Edge e = graph_new_edge(g);
    e->capacity = capacity;
    e->flow = 0;
    e->u = u;
    e->v = v;
}

static void dbgpreflow(Graph g, Vertex s)
{
    s->height = dbgorder(g);

    // 
    for (int i = 0; i < g->size; i++)
    {
        // If current edge goes from source 
        if (g->edges[i].u == s)
        {
            // Flow is equal to capacity 
            g->edges[i].flow = g->edges[i].capacity;

            // Initialize excess flow for adjacent v 
            g->edges[i].v->excess += g->edges[i].flow;

            // Add an edge from v to s in residual graph with 
            // capacity equal to 0 
            Edge e = graph_new_edge(g);

            e->flow = -g->edges[i].flow;
            e->capacity = 0;
            e->u = g->edges[i].v;
            e->v = s;
        }
    }
}

static Vertex dbgoverflow(Graph g)
{
    for (int i = 1; i < 5; i++)
    {
        if (g->vertices[i]->excess > 0)
        {
            return g->vertices[i];
        }
    }

    return NULL;
}

int math_min(int a, int b)
{
    if (a < b)
    {
        return a;
    }

    return b;
}

static void dbguprevef(Graph g, int i, int flow)
{
    Vertex u = g->edges[i].v;
    Vertex v = g->edges[i].u;

    for (int j = 0; j < g->size; j++)
    {
        if (g->edges[j].v == v && g->edges[j].u == u)
        {
            g->edges[j].flow -= flow;
            return;
        }
    }

    Edge e = graph_new_edge(g);

    e->flow = 0;
    e->capacity = flow;
    e->u = u;
    e->v = v;
}

static bool dbgpush(Graph g, Vertex u)
{
    // to which flow can be pushed 
    for (int i = 0; i < g->size; i++)
    {
        // Checks u of current edge is same as given 
        // overflowing vertex 
        if (g->edges[i].u == u)
        {
            // if flow is equal to capacity then no push 
            // is possible 
            if (g->edges[i].flow == g->edges[i].capacity)
                continue;

            // Push is only possible if height of adjacent 
            // is smaller than height of overflowing vertex 
            if (u->height > g->edges[i].v->height)
            {
                // Flow to be pushed is equal to minimum of 
                // remaining flow on edge and excess flow. 
                int flow = math_min(g->edges[i].capacity - g->edges[i].flow,
                    u->excess);

                // Reduce excess flow for overflowing vertex 
                u->excess -= flow;

                // Increase excess flow for adjacent 
                g->edges[i].v->excess += flow;

                // Add residual flow (With capacity 0 and negative 
                // flow) 
                g->edges[i].flow += flow;

                dbguprevef(g, i, flow);

                return true;
            }
        }
    }
    return false;
}

static void dbgrelabel(Graph g, Vertex u)
{
    // Initialize minimum height of an adjacent 
    int maxHeight = INT_MAX;

    // Find the adjacent with minimum height 
    for (int i = 0; i < g->size; i++)
    {
        if (g->edges[i].u == u)
        {
            // if flow is equal to capacity then no 
            // relabeling 
            if (g->edges[i].flow == g->edges[i].capacity)
                continue;

            // Update minimum height 
            if (g->edges[i].v->height < maxHeight)
            {
                maxHeight = g->edges[i].v->height;

                // updating height of u 
                u->height = maxHeight + 1;
            }
        }
    }
}

static void dbgmaxflow(Graph g, Vertex s)
{
    dbgpreflow(g, s);

    Vertex u;

    while ((u = dbgoverflow(g)))
    {
        if (!dbgpush(g, u))
        {
            dbgrelabel(g, u);
        }
    }
}

int main(void)
{
    // char buffer[BUFFER_SIZE];
    struct Graph g = { 0 };
    clock_t start = clock();

    graph(&g);

    // while (fgets(buffer, sizeof buffer, stdin))
    // {
    //     char* mid = strstr(buffer, ": ");

    //     if (!mid)
    //     {
    //         fprintf(stderr, "Error: Format.\n");

    //         return 1;
    //     }

    //     *mid = '\0';

    //     printf("%s -> ", buffer);

    //     Vertex u = graph_vertex(&g, buffer);

    //     if (!u)
    //     {
    //         fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

    //         return 1;
    //     }

    //     for (String target = strtok(mid + 2, DELIMITERS);
    //         target;
    //         target = strtok(NULL, DELIMITERS))
    //     {
    //         Vertex v = graph_vertex(&g, target);

    //         if (!v)
    //         {
    //             fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

    //             return 1;
    //         }

    //         Edge e = graph_new_edge(&g);

    //         e->u = u;
    //         e->v = v;
    //         e->flow = 0;
    //         e->capacity = 1;

    //         printf("%s, ", target);
    //     }

    //     printf("\n");
    // }

    printf("25z %d %lf\n", 0, (double)(clock() - start) / CLOCKS_PER_SEC);
    //graph_clear(&g);

    Vertex u0 = graph_vertex(&g, "00");
    Vertex u1 = graph_vertex(&g, "01");
    Vertex u2 = graph_vertex(&g, "02");
    Vertex u3 = graph_vertex(&g, "03");
    Vertex u4 = graph_vertex(&g, "04");
    Vertex u5 = graph_vertex(&g, "05");
    Vertex vertices[] = { u0, u1, u2, u3, u4, u5 };

    g.vertices = vertices;

    dbgedge(&g, u0, u1, 16);
    dbgedge(&g, u0, u2, 13);
    dbgedge(&g, u1, u2, 10);
    dbgedge(&g, u2, u1, 4);
    dbgedge(&g, u1, u3, 12);
    dbgedge(&g, u2, u4, 14);
    dbgedge(&g, u3, u2, 9);
    dbgedge(&g, u3, u5, 20);
    dbgedge(&g, u4, u3, 7);
    dbgedge(&g, u4, u5, 4);
    dbgmaxflow(&g, u0);
    printf("Order: %d\n", dbgorder(&g));
    printf("max flow is: %d", u5->excess);

    return 0;
}
