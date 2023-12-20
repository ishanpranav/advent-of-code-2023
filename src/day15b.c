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
    struct OrderedDictionaryEntry* nextEntry;
    char key[KEY_CAPACITY];
    int value;
};

struct OrderedDictionaryBucket
{
    struct OrderedDictionaryEntry* firstEntry;
    struct OrderedDictionaryBucket* nextBucket;
    struct OrderedDictionaryBucket* previousBucket;
};

struct OrderedDictionary
{
    struct OrderedDictionaryBucket* firstBucket;
    struct OrderedDictionaryBucket buckets[BUCKETS];
};

struct StringBuilder
{
    char* buffer;
    int length;
};

typedef char* String;
typedef struct OrderedDictionaryEntry* OrderedDictionaryEntry;
typedef struct OrderedDictionaryBucket* OrderedDictionaryBucket;
typedef struct OrderedDictionary* OrderedDictionary;
typedef struct StringBuilder* StringBuilder;

bool ordered_dictionary_set(
    OrderedDictionary instance,
    String key,
    int hash,
    int value)
{
    OrderedDictionaryEntry* p;

    for (p = &instance->buckets[hash].firstEntry; *p; p = &(*p)->nextEntry)
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

    if (!instance->buckets[hash].firstEntry)
    {
        OrderedDictionaryBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;

        if (first)
        {
            first->previousBucket = instance->buckets + hash;
        }
    }

    memcpy(entry->key, key, KEY_CAPACITY);

    entry->value = value;
    *p = entry;

    return true;
}

void ordered_dictionary_remove(OrderedDictionary instance, String key, int hash)
{
    OrderedDictionaryEntry previous = NULL;

    for (OrderedDictionaryEntry current = instance->buckets[hash].firstEntry;
        current;
        current = current->nextEntry)
    {
        if (strcmp(key, current->key) == 0)
        {
            if (previous)
            {
                previous->nextEntry = current->nextEntry;
            }
            else if (current->nextEntry)
            {
                instance->buckets[hash].firstEntry = current->nextEntry;
            }
            else
            {
                if (instance->firstBucket == instance->buckets + hash)
                {
                    instance->firstBucket = instance->buckets[hash].nextBucket;
                }

                if (instance->buckets[hash].nextBucket)
                {
                    instance->buckets[hash].nextBucket->previousBucket =
                        instance->buckets[hash].previousBucket;
                }

                if (instance->buckets[hash].previousBucket)
                {
                    instance->buckets[hash].previousBucket->nextBucket =
                        instance->buckets[hash].nextBucket;
                }

                instance->buckets[hash].firstEntry = NULL;
            }

            free(current);

            break;
        }

        previous = current;
    }
}

void ordered_dictionary_clear(OrderedDictionary instance)
{
    for (OrderedDictionaryBucket bucket = instance->firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        OrderedDictionaryEntry entry = bucket->firstEntry;

        while (entry)
        {
            OrderedDictionaryEntry nextEntry = entry->nextEntry;

            free(entry);

            entry = nextEntry;
        }

        bucket->firstEntry = NULL;
    }

    instance->firstBucket = NULL;
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
    struct OrderedDictionary dictionary = { 0 };
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

                ordered_dictionary_remove(&dictionary, key, hash);
            }
            continue;

            case ',':
            {
                if (value)
                {
                    String key = string_builder_to_string(&keyBuilder);

                    if (!ordered_dictionary_set(&dictionary, key, hash, value))
                    {
                        fclose(stream);
                        fprintf(stderr, "Error: Out of memory.\n");

                        return 1;
                    }
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

        if (!ordered_dictionary_set(&dictionary, key, hash, value))
        {
            fclose(stream);
            fprintf(stderr, "Error: Out of memory.\n");

            return 1;
        }
    }

    long sum = 0;

    for (OrderedDictionaryBucket bucket = dictionary.firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        int slot = 1;

        for (OrderedDictionaryEntry entry = bucket->firstEntry;
            entry;
            entry = entry->nextEntry)
        {
            sum += (bucket - dictionary.buckets + 1) * slot * entry->value;
            slot++;
        }
    }

    printf("%ld : %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);
    ordered_dictionary_clear(&dictionary);

    return 0;
}
