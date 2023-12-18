// Licensed under the MIT License.

// Hot Springs Part 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 64
#define DELIMITERS ","
#define PATTERN_BUFFER_CAPACITY 32
#define SPRING_OPERATIONAL '.'
#define SPRING_DAMAGED '#'
#define SPRING_UNKNOWN '?'
#define KEY_EMPTY -1

struct DictionaryEntry
{
    int key;
    int value;
    struct DictionaryEntry* next;
};

struct Dictionary
{
    struct DictionaryEntry* first;
    struct DictionaryEntry buckets[PATTERN_BUFFER_CAPACITY];
};

struct SpringPattern
{
    char* symbols;
    int length;
};

typedef char* String;
typedef char Spring;
typedef struct Dictionary* Dictionary;
typedef struct SpringPattern* SpringPattern;

void dictionary_increment(Dictionary instance, int key, int change)
{
    if (instance->buckets[key].key == KEY_EMPTY)
    {
        struct DictionaryEntry* first = instance->first;

        instance->first = instance->buckets + key;
        instance->buckets[key].next = first;
    }

    instance->buckets[key].key = key;
    instance->buckets[key].value += change;
}

void dictionary_clear(Dictionary instance)
{
    instance->first = NULL;

    for (int i = 0; i < PATTERN_BUFFER_CAPACITY; i++)
    {
        instance->buckets[i].key = KEY_EMPTY;
        instance->buckets[i].value = 0;
        instance->buckets[i].next = NULL;
    }
}

void spring_pattern(SpringPattern instance, Spring* symbols)
{
    instance->length = 0;
    instance->symbols = symbols;
}

void spring_pattern_append2(SpringPattern instance, Spring symbol)
{
    instance->symbols[instance->length] = symbol;
    instance->length++;
}

void spring_pattern_append3(SpringPattern instance, Spring symbol, int count)
{
    memset(instance->symbols + instance->length, symbol, count);

    instance->length += count;
}

static void read(Spring symbol, SpringPattern pattern, Dictionary current)
{
    struct Dictionary view;

    dictionary_clear(&view);

    for (struct DictionaryEntry* p = current->first; p; p = p->next)
    {
        dictionary_increment(&view, p->key, p->value);
    }

    dictionary_clear(current);

    for (struct DictionaryEntry* p = view.first; p; p = p->next)
    {
        int state = p->key;

        switch (symbol)
        {
            case SPRING_UNKNOWN:
                if (state + 1 < pattern->length)
                {
                    dictionary_increment(current, state + 1, p->value);
                }

                if (pattern->symbols[state] == SPRING_OPERATIONAL)
                {
                    dictionary_increment(current, state, p->value);
                }
                break;

            case SPRING_OPERATIONAL:
                if (state + 1 < pattern->length &&
                    pattern->symbols[state + 1] == SPRING_OPERATIONAL)
                {
                    dictionary_increment(current, state + 1, p->value);
                }

                if (pattern->symbols[state] == SPRING_OPERATIONAL)
                {
                    dictionary_increment(current, state, p->value);
                }
                break;

            case SPRING_DAMAGED:
                if (state + 1 < pattern->length &&
                    pattern->symbols[state + 1] == SPRING_DAMAGED)
                {
                    dictionary_increment(current, state + 1, p->value);
                }
                break;
        }
    }
}

static void scan(SpringPattern text, SpringPattern pattern, Dictionary current)
{
    for (Spring* p = text->symbols; p < text->symbols + text->length; p++)
    {
        read(*p, pattern, current);
    }
}

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day12a <path>\n");

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
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stream))
    {
        char* mid = strchr(buffer, ' ');

        if (!mid)
        {
            fclose(stream);
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        Spring patternBuffer[PATTERN_BUFFER_CAPACITY];
        struct Dictionary current;
        struct SpringPattern text = 
        {
            .symbols = buffer,
            .length = mid - buffer
        };
        struct SpringPattern pattern;

        spring_pattern(&pattern, patternBuffer);
        spring_pattern_append2(&pattern, SPRING_OPERATIONAL);

        for (String token = strtok(mid, DELIMITERS);
            token;
            token = strtok(NULL, DELIMITERS))
        {
            spring_pattern_append3(&pattern, SPRING_DAMAGED, atoi(token));
            spring_pattern_append2(&pattern, SPRING_OPERATIONAL);
        }

        dictionary_clear(&current);
        dictionary_increment(&current, 0, 1);
        scan(&text, &pattern, &current);

        total +=
            current.buckets[pattern.length - 1].value +
            current.buckets[pattern.length - 2].value;
    }

    printf("%d : %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
