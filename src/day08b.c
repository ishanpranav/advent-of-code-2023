// Licensed under the MIT License.

// Haunted Wasteland Part 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 32
#define DIRECTIONS_CAPACITY 512
#define DIRECTION_LEFT 'L'
#define DIRECTION_RIGHT 'R'
#define LIST_CAPACITY 1024
#define VERTEX_OFFSET -13330
#define VERTEX_STOP_MOD 25
#define VERTEX_LAST 33325

struct VertexPair
{
    int left;
    int right;
};

struct Graph
{
    struct VertexPair vertices[VERTEX_LAST + 1];
};

struct List
{
    int items[LIST_CAPACITY];
    int count;
};

typedef int Vertex;
typedef char Direction;
typedef struct Graph* Graph;
typedef struct List* List;
typedef bool (*Predicate)(Vertex vertex);

long long math_gcd(long long a, long long b)
{
    while (b > 0)
    {
        long long r = a % b;

        a = b;
        b = r;
    }

    return a;
}

long long math_lcm(long long a, long long b)
{
    return (a / math_gcd(a, b)) * b;
}

void graph_add(Graph instance, Vertex vertex, Vertex left, Vertex right)
{
    instance->vertices[vertex].left = left;
    instance->vertices[vertex].right = right;
}

int graph_walk(
    Graph instance,
    Vertex start,
    Direction directions[],
    Predicate predicate)
{
    int result = 0;
    Direction* direction = directions;

    while (!predicate(start))
    {
        switch (*direction)
        {
            case DIRECTION_LEFT:
                start = instance->vertices[start].left;
                direction++;
                result++;
                break;
            case DIRECTION_RIGHT:
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

void list(List instance)
{
    instance->count = 0;
}

void list_add(List instance, Vertex item)
{
    instance->items[instance->count] = item;
    instance->count++;
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

static bool scan(FILE* stream, Graph graph, List starts)
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

        if (buffer[2] == 'A')
        {
            list_add(starts, vertex);
        }

        graph_add(graph, vertex, left, right);
    }

    return true;
}

static bool stop(Vertex vertex)
{
    return vertex % 36 == VERTEX_STOP_MOD;
}

int main()
{
    struct Graph graph;
    struct List starts;
    Direction directions[DIRECTIONS_CAPACITY];
    clock_t start = clock();

    list(&starts);

    if (!fgets(directions, sizeof directions, stdin) ||
        !scan(stdin, &graph, &starts))
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    Vertex* first = starts.items;
    long long lcm = graph_walk(&graph, *first, directions, stop);

    for (Vertex* p = first + 1; p < first + starts.count; p++)
    {
        lcm = math_lcm(lcm, graph_walk(&graph, *p, directions, stop));
    }

    printf("%lld : %lf\n", lcm, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
