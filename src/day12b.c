// Licensed under the MIT License.

// Hot Springs Part 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 64
#define DELIMITERS ","
#define KEY_EMPTY -1
#define PATTERN_BUFFER_CAPACITY 256
#define SHORT_PATTERN_BUFFER_CAPACITY 32
#define SPRING_OPERATIONAL '.'
#define SPRING_DAMAGED '#'
#define SPRING_UNKNOWN '?'

struct DictionaryEntry
{
    int key;
    long long value;
    struct DictionaryEntry* next;
};

struct Dictionary
{
    struct DictionaryEntry* first;
    struct DictionaryEntry buckets[PATTERN_BUFFER_CAPACITY];
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

    for (int i = 0; i < PATTERN_BUFFER_CAPACITY; i++)
    {
        instance->buckets[i].key = KEY_EMPTY;
        instance->buckets[i].value = 0;
        instance->buckets[i].next = NULL;
    }
}

void dictionary_increment(Dictionary instance, int key, long long change)
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

void pattern_concat(Pattern instance, Pattern other)
{
    memcpy(
        instance->symbols + instance->length,
        other->symbols,
        other->length);

    instance->length += other->length;
}

void pattern_set(Pattern instance, int index, char item)
{
    instance->symbols[index] = item;
}

static void read(char symbol, Pattern pattern, Dictionary current)
{
    struct Dictionary view;

    dictionary_copy(&view, current);
    dictionary(current);

    for (DictionaryEntry entry = view.first; entry; entry = entry->next)
    {
        switch (symbol)
        {
            case SPRING_UNKNOWN:
                if (entry->key + 1 < pattern->length)
                {
                    dictionary_increment(current, entry->key + 1, entry->value);
                }

                if (pattern->symbols[entry->key] == SPRING_OPERATIONAL)
                {
                    dictionary_increment(current, entry->key, entry->value);
                }
                break;

            case SPRING_OPERATIONAL:
                if (entry->key + 1 < pattern->length &&
                    pattern->symbols[entry->key + 1] == SPRING_OPERATIONAL)
                {
                    dictionary_increment(current, entry->key + 1, entry->value);
                }

                if (pattern->symbols[entry->key] == SPRING_OPERATIONAL)
                {
                    dictionary_increment(current, entry->key, entry->value);
                }
                break;

            case SPRING_DAMAGED:
                if (entry->key + 1 < pattern->length &&
                    pattern->symbols[entry->key + 1] == SPRING_DAMAGED)
                {
                    dictionary_increment(current, entry->key + 1, entry->value);
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

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day12b <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    long long total = 0;
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

        char patternBuffer[PATTERN_BUFFER_CAPACITY];
        char shortPatternBuffer[SHORT_PATTERN_BUFFER_CAPACITY];
        struct Dictionary current;
        struct Pattern text =
        {
            .symbols = buffer,
            .length = mid - buffer
        };
        struct Pattern longPattern;
        struct Pattern shortPattern;

        pattern(&longPattern, patternBuffer);
        pattern(&shortPattern, shortPatternBuffer);
        pattern_append(&longPattern, SPRING_OPERATIONAL);

        for (String token = strtok(mid, DELIMITERS);
            token;
            token = strtok(NULL, DELIMITERS))
        {
            pattern_append_many(&shortPattern, SPRING_DAMAGED, atoi(token));
            pattern_append(&shortPattern, SPRING_OPERATIONAL);
        }

        for (int i = 0; i < 5; i++)
        {
            pattern_concat(&longPattern, &shortPattern);
        }

        dictionary(&current);
        dictionary_increment(&current, 0, 1);

        for (int i = 0; i < 4; i++)
        {
            scan(&text, &longPattern, &current);
            read(SPRING_UNKNOWN, &longPattern, &current);
        }

        scan(&text, &longPattern, &current);
        
        total +=
            current.buckets[longPattern.length - 1].value +
            current.buckets[longPattern.length - 2].value;
    }

    printf("%lld : %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
