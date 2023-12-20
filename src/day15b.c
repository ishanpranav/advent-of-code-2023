// Licensed under the MIT license.

// Lens Library Part 2

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUCKETS 256
#define KEY_CAPACITY 8

struct OrderedDictionaryEntry
{
    char key[KEY_CAPACITY];
    int value;
    struct OrderedDictionaryEntry* next;
};

struct StringBuilder
{
    char* buffer;
    int length;
};

typedef char* String;
typedef struct OrderedDictionaryEntry** OrderedDictionary;
typedef struct OrderedDictionaryEntry* OrderedDictionaryEntry;
typedef struct StringBuilder* StringBuilder;

bool ordered_dictionary_set(
    OrderedDictionary instance,
    String key,
    int bucket,
    int value)
{
    OrderedDictionaryEntry* p;

    for (p = &instance[bucket]; *p; p = &(*p)->next)
    {
        if (strcmp(key, (*p)->key) == 0)
        {
            (*p)->value = value;

            return true;
        }
    }

    OrderedDictionaryEntry entry = calloc(
        1,
        sizeof(struct OrderedDictionaryEntry));

    if (!entry)
    {
        return false;
    }

    memcpy(entry->key, key, KEY_CAPACITY);

    entry->value = value;
    *p = entry;

    return true;
}

void ordered_dictionary_remove(
    OrderedDictionary instance,
    String key,
    int bucket)
{
    OrderedDictionaryEntry previous = NULL;

    for (OrderedDictionaryEntry current = instance[bucket];
        current;
        current = current->next)
    {
        if (strcmp(key, current->key) == 0)
        {
            if (previous)
            {
                previous->next = current->next;
            }
            else
            {
                instance[bucket] = current->next;
            }

            free(current);

            return;
        }

        previous = current;
    }
}

void ordered_dictionary_clear(OrderedDictionary instance)
{
    for (int bucket = 0; bucket < BUCKETS; bucket++)
    {
        OrderedDictionaryEntry current = instance[bucket];

        while (current)
        {
            OrderedDictionaryEntry next = current->next;

            free(current);

            current = next;
        }

        instance[bucket] = NULL;
    }
}

void string_builder(StringBuilder instance)
{
    instance->length = 0;
}

void string_builder_append(StringBuilder instance, char value)
{
    instance->buffer[instance->length] = value;
    instance->length++;
}

String string_builder_to_string(StringBuilder instance)
{
    instance->buffer[instance->length] = '\0';

    return instance->buffer;
}

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day15b <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    int hash = 0;
    int value = 0;
    char current;
    char buffer[KEY_CAPACITY];
    struct StringBuilder keyBuilder;
    OrderedDictionaryEntry dictionary[BUCKETS] = { 0 };
    clock_t start = clock();

    string_builder(&keyBuilder);

    keyBuilder.buffer = buffer;

    while ((current = getc(stream)) != EOF)
    {
        if (isdigit(current))
        {
            value = (value * 10) + (current - '0');

            continue;
        }

        switch (current)
        {
            case '\n':
            case '=':
                continue;

            case '-':
            {
                String key = string_builder_to_string(&keyBuilder);

                ordered_dictionary_remove(dictionary, key, hash);
            }
            continue;

            case ',':
            {
                if (value)
                {
                    String key = string_builder_to_string(&keyBuilder);

                    ordered_dictionary_set(dictionary, key, hash, value);
                }

                hash = 0;
                value = 0;

                string_builder(&keyBuilder);
            }
            continue;
        }

        hash = ((hash + current) * 17) % BUCKETS;

        string_builder_append(&keyBuilder, current);
    }

    if (value)
    {
        String key = string_builder_to_string(&keyBuilder);

        ordered_dictionary_set(dictionary, key, hash, value);
    }

    long sum = 0;

    for (int bucket = 0; bucket < BUCKETS; bucket++)
    {
        int slot = 1;

        for (OrderedDictionaryEntry entry = dictionary[bucket];
            entry;
            entry = entry->next)
        {
            sum += (bucket + 1) * slot * entry->value;
            slot++;
        }
    }

    printf("%ld : %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);
    ordered_dictionary_clear(dictionary);

    return 0;
}
