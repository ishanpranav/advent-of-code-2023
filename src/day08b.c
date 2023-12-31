// Licensed under the MIT License.

// Haunted Wasteland Part 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 32
#define DIRECTIONS_CAPACITY 512
#define LIST_CAPACITY 1296

enum Vertex
{
    VERTEX_OFFSET = -1342,
    VERTEX_LAST = 46655 + VERTEX_OFFSET,
    VERTEX_NONE = VERTEX_LAST + 1
};

struct VertexPair
{
    int left;
    int right;
};

struct Graph
{
    struct VertexPair vertices[VERTEX_NONE];
};

struct List
{
    int items[LIST_CAPACITY];
    int count;
};

typedef char* String;
typedef enum Vertex Vertex;
typedef struct Graph* Graph;
typedef struct List* List;

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

static bool read(FILE* stream, Graph graph, List starts)
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

static int scan(Graph instance, Vertex start, String directions)
{
    int result = 0;
    String direction = directions;

    while (start % 36 != 25)
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

    struct List starts;
    char directions[DIRECTIONS_CAPACITY];
    clock_t start = clock();

    list(&starts);

    if (!fgets(directions, sizeof directions, stdin) ||
        !read(stdin, graph, &starts))
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    Vertex* first = starts.items;
    long long lcm = scan(graph, *first, directions);

    for (Vertex* p = first + 1; p < first + starts.count; p++)
    {
        lcm = math_lcm(lcm, scan(graph, *p, directions));
    }

    printf("08b %lld %lf\n", lcm, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
