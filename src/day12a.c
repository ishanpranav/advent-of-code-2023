// Licensed under the MIT License.

// Hot Springs Part 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 64
#define DELIMITERS ","
#define KEY_EMPTY -1
#define SHORT_PATTERN_BUFFER_CAPACITY 32

struct DictionaryEntry
{
    int key;
    int value;
    struct DictionaryEntry* next;
};

struct Dictionary
{
    struct DictionaryEntry* first;
    struct DictionaryEntry buckets[SHORT_PATTERN_BUFFER_CAPACITY];
};

struct Pattern
{
    char* symbols;
    int length;
};

typedef char* String;
typedef struct DictionaryEntry* DictionaryEntry;
typedef struct Dictionary* Dictionary;
typedef struct Pattern* Pattern;

void dictionary(Dictionary instance)
{
    instance->first = NULL;

    for (int i = 0; i < SHORT_PATTERN_BUFFER_CAPACITY; i++)
    {
        instance->buckets[i].key = KEY_EMPTY;
        instance->buckets[i].value = 0;
        instance->buckets[i].next = NULL;
    }
}

void dictionary_increment(Dictionary instance, int key, int change)
{
    if (instance->buckets[key].key == KEY_EMPTY)
    {
        DictionaryEntry first = instance->first;

        instance->first = instance->buckets + key;
        instance->buckets[key].next = first;
    }

    instance->buckets[key].key = key;
    instance->buckets[key].value += change;
}

void dictionary_copy(Dictionary destination, Dictionary source)
{
    dictionary(destination);
    
    for (DictionaryEntry entry = source->first; entry; entry = entry->next)
    {
        dictionary_increment(destination, entry->key, entry->value);
    }
}

void pattern(Pattern instance, char* symbols)
{
    instance->length = 0;
    instance->symbols = symbols;
}

void pattern_append(Pattern instance, char symbol)
{
    instance->symbols[instance->length] = symbol;
    instance->length++;
}

void pattern_append_many(Pattern instance, char symbol, int count)
{
    memset(instance->symbols + instance->length, symbol, count);

    instance->length += count;
}

static void read(char symbol, Pattern pattern, Dictionary current)
{
    struct Dictionary view;

    dictionary_copy(&view, current);
    dictionary(current);

    for (DictionaryEntry p = view.first; p; p = p->next)
    {
        int state = p->key;

        switch (symbol)
        {
            case '?':
                if (state + 1 < pattern->length)
                {
                    dictionary_increment(current, state + 1, p->value);
                }

                if (pattern->symbols[state] == '.')
                {
                    dictionary_increment(current, state, p->value);
                }
                break;

            case '.':
                if (state + 1 < pattern->length &&
                    pattern->symbols[state + 1] == '.')
                {
                    dictionary_increment(current, state + 1, p->value);
                }

                if (pattern->symbols[state] == '.')
                {
                    dictionary_increment(current, state, p->value);
                }
                break;

            case '#':
                if (state + 1 < pattern->length &&
                    pattern->symbols[state + 1] == '#')
                {
                    dictionary_increment(current, state + 1, p->value);
                }
                break;
        }
    }
}

static void scan(Pattern text, Pattern pattern, Dictionary current)
{
    for (char* p = text->symbols; p < text->symbols + text->length; p++)
    {
        read(*p, pattern, current);
    }
}

int main(void)
{
    int total = 0;
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stdin))
    {
        char* mid = strchr(buffer, ' ');

        if (!mid)
        {
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        char shortPatternBuffer[SHORT_PATTERN_BUFFER_CAPACITY];
        struct Dictionary current;
        struct Pattern text =
        {
            .symbols = buffer,
            .length = mid - buffer
        };
        struct Pattern shortPattern;

        pattern(&shortPattern, shortPatternBuffer);
        pattern_append(&shortPattern, '.');

        for (String token = strtok(mid, DELIMITERS);
            token;
            token = strtok(NULL, DELIMITERS))
        {
            pattern_append_many(&shortPattern, '#', atoi(token));
            pattern_append(&shortPattern, '.');
        }

        dictionary(&current);
        dictionary_increment(&current, 0, 1);
        scan(&text, &shortPattern, &current);

        total +=
            current.buckets[shortPattern.length - 1].value +
            current.buckets[shortPattern.length - 2].value;
    }

    printf("12a %d %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
