// Licensed under the MIT license.

// Lens Library Part 2

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define EXCEPTION_OUT_OF_MEMORY "Error: Out of memory.\n"
#define ORDERED_DICTIONARY_BUCKETS 256
#define STRING_CAPACITY 8

struct String
{
    int length;
    char buffer[STRING_CAPACITY];
};

struct OrderedDictionaryEntry
{
    struct OrderedDictionaryEntry* nextEntry;
    int value;
    struct String key;
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
    struct OrderedDictionaryBucket buckets[ORDERED_DICTIONARY_BUCKETS];
};

typedef struct String* String;
typedef struct OrderedDictionaryEntry* OrderedDictionaryEntry;
typedef struct OrderedDictionaryBucket* OrderedDictionaryBucket;
typedef struct OrderedDictionary* OrderedDictionary;

void string(String instance)
{
    instance->length = 0;
}

void string_append(String instance, char value)
{
    instance->buffer[instance->length] = value;
    instance->length++;
}

void string_copy(String destination, String source)
{
    destination->length = source->length;

    memcpy(destination->buffer, source->buffer, source->length);
}

bool string_equals(String instance, String other)
{
    if (instance->length != other->length)
    {
        return false;
    }

    return memcmp(instance->buffer, other->buffer, instance->length) == 0;
}

bool ordered_dictionary_set(
    OrderedDictionary instance,
    String key,
    int hash,
    int value)
{
    OrderedDictionaryEntry* p;

    for (p = &instance->buckets[hash].firstEntry; *p; p = &(*p)->nextEntry)
    {
        if (string_equals(key, &(*p)->key))
        {
            (*p)->value = value;

            return true;
        }
    }

    OrderedDictionaryEntry entry = malloc(sizeof * entry);

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

    string_copy(&entry->key, key);

    entry->value = value;
    entry->nextEntry = NULL;
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
        if (string_equals(key, &current->key))
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

int main(void)
{
    int hash = 0;
    int value = 0;
    char current;
    struct String key;
    struct OrderedDictionary dictionary = { 0 };
    clock_t start = clock();

    string(&key);

    while ((current = getc(stdin)) != EOF)
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
                ordered_dictionary_remove(&dictionary, &key, hash);
            }
            continue;

            case ',':
            {
                if (value)
                {
                    if (!ordered_dictionary_set(&dictionary, &key, hash, value))
                    {
                        fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

                        return 1;
                    }
                }

                hash = 0;
                value = 0;

                string(&key);
            }
            continue;
        }

        hash = ((hash + current) * 17) % ORDERED_DICTIONARY_BUCKETS;

        string_append(&key, current);
    }

    if (value)
    {
        if (!ordered_dictionary_set(&dictionary, &key, hash, value))
        {
            fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

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

    printf("15b %ld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    ordered_dictionary_clear(&dictionary);

    return 0;
}
