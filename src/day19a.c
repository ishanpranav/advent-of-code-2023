// Licensed under the MIT License.

// Aplenty Part 1

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 64
#define DELIMITERS ","
#define EXCEPTION_FORMAT "Error: Format.\n"
#define FUNCTION_DICTIONARY_BUCKETS 1543
#define FUNCTION_RANGES_CAPACITY 8
#define KEY_SIZE 3

enum Property
{
    PROPERTY_X = 0,
    PROPERTY_M = 1,
    PROPERTY_A = 2,
    PROPERTY_S = 3,
    PROPERTY_NONE
};

struct Range
{
    enum Property identifier;
    int comparand;
    char relation;
    char action[KEY_SIZE];
};

struct Function
{
    int count;
    struct Range ranges[FUNCTION_RANGES_CAPACITY];
};

struct FunctionDictionaryEntry
{
    struct FunctionDictionaryEntry* nextEntry;
    struct Function value;
    char key[KEY_SIZE];
};

struct FunctionDictionaryBucket
{
    struct FunctionDictionaryEntry* firstEntry;
    struct FunctionDictionaryBucket* nextBucket;
};

struct FunctionDictionary
{
    struct FunctionDictionaryBucket* firstBucket;
    struct FunctionDictionaryBucket buckets[FUNCTION_DICTIONARY_BUCKETS];
};

struct Tokenizer
{
    char* tokens;
    char* current;
};

typedef int* Dynamic;
typedef char* String;
typedef enum Property Property;
typedef struct Range* Range;
typedef struct Function* Function;
typedef struct FunctionDictionaryEntry* FunctionDictionaryEntry;
typedef struct FunctionDictionaryBucket* FunctionDictionaryBucket;
typedef struct FunctionDictionary* FunctionDictionary;
typedef struct Tokenizer* Tokenizer;

Property property(char value)
{
    switch (value)
    {
        case 'a': return PROPERTY_A;
        case 'm': return PROPERTY_M;
        case 's': return PROPERTY_S;
        case 'x': return PROPERTY_X;
        default: return 0;
    }
}

void function(Function instance)
{
    instance->count = 0;
}

Range function_new_range(Function instance)
{
    Range result = instance->ranges + instance->count;

    instance->count++;

    return result;
}

static unsigned int function_dictionary_hash(char key[])
{
    unsigned int hash = 0;

    for (int i = 0; i < KEY_SIZE; i++)
    {
        hash = (hash * 36) + key[i] - 97;
    }

    return hash % FUNCTION_DICTIONARY_BUCKETS;
}

Function function_dictionary_get(FunctionDictionary instance, char key[])
{
    unsigned int hash = function_dictionary_hash(key);

    for (FunctionDictionaryEntry entry = instance->buckets[hash].firstEntry;
        entry;
        entry = entry->nextEntry)
    {
        if (memcmp(key, entry->key, KEY_SIZE) == 0)
        {
            return &entry->value;
        }
    }

    return NULL;
}

bool function_dictionary_set(
    FunctionDictionary instance,
    char key[],
    Function value)
{
    FunctionDictionaryEntry* p;
    unsigned int hash = function_dictionary_hash(key);

    for (p = &instance->buckets[hash].firstEntry; *p; p = &(*p)->nextEntry)
    {
        if (memcmp(key, (*p)->key, KEY_SIZE) == 0)
        {
            (*p)->value = *value;

            return true;
        }
    }

    FunctionDictionaryEntry entry = malloc(sizeof * entry);

    if (!entry)
    {
        return false;
    }

    if (!instance->buckets[hash].firstEntry)
    {
        FunctionDictionaryBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;
    }

    memcpy(entry->key, key, KEY_SIZE);

    entry->value = *value;
    entry->nextEntry = NULL;
    *p = entry;

    return true;
}

void function_dictionary_clear(FunctionDictionary instance)
{
    for (FunctionDictionaryBucket bucket = instance->firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        FunctionDictionaryEntry entry = bucket->firstEntry;

        while (entry)
        {
            FunctionDictionaryEntry nextEntry = entry->nextEntry;

            free(entry);

            entry = nextEntry;
        }

        bucket->firstEntry = NULL;
    }

    instance->firstBucket = NULL;
}

void tokenizer(Tokenizer instance, String tokens)
{
    instance->tokens = tokens;
    instance->current = tokens;
}

bool tokenizer_try_peek_ahead(Tokenizer instance, char* result)
{
    if (!*instance->current || !instance->current[1])
    {
        return false;
    }

    *result = instance->current[1];

    return true;
}

char tokenizer_pop(Tokenizer instance)
{
    char result = *instance->current;

    instance->current++;

    return result;
}

static bool parse_number(Tokenizer tokenizer, int* result)
{
    *result = 0;

    while (isdigit(*tokenizer->current))
    {
        *result = (*result * 10) + (tokenizer_pop(tokenizer) - '0');
    }

    return *result > 0;
}

static bool parse_key(Tokenizer tokenizer, char result[])
{
    int i = 0;

    while (islower(*tokenizer->current))
    {
        result[i] = tokenizer_pop(tokenizer);
        i++;
    }

    while (i < KEY_SIZE)
    {
        result[i] = '\0';
        i++;
    }

    return true;
}

static bool parse_action(Tokenizer tokenizer, char result[])
{
    switch (*tokenizer->current)
    {
        case 'A':
        case 'R':
            result[0] = tokenizer_pop(tokenizer);
            return true;

        default: return parse_key(tokenizer, result);
    }
}

static bool parse_range(Tokenizer tokenizer, Range result)
{
    if (*tokenizer->current &&
        (tokenizer->current[1] == '<' || tokenizer->current[1] == '>'))
    {
        result->identifier = property(tokenizer_pop(tokenizer));
        result->relation = tokenizer_pop(tokenizer);

        if (!parse_number(tokenizer, &result->comparand) ||
            tokenizer_pop(tokenizer) != ':' ||
            !parse_action(tokenizer, result->action))
        {
            return false;
        }

        return true;
    }

    if (!parse_action(tokenizer, result->action))
    {
        return false;
    }

    result->relation = 0;

    return true;
}

static bool parse_function(Tokenizer tokenizer, char key[], Function result)
{
    if (!parse_key(tokenizer, key) || tokenizer_pop(tokenizer) != '{')
    {
        return false;
    }

    do
    {
        if (!parse_range(tokenizer, function_new_range(result)))
        {
            return false;
        }
    }
    while (tokenizer_pop(tokenizer) == ',');

    return true;
}

static bool scan(Dynamic value, FunctionDictionary dictionary)
{
    Function current = function_dictionary_get(dictionary, "in");

    while (current)
    {
        for (Range range = current->ranges;
            range < current->ranges + current->count;
            range++)
        {
            switch (range->relation)
            {
                case '<':
                    if (value[range->identifier] >= range->comparand)
                    {
                        continue;
                    }
                    break;

                case '>':
                    if (value[range->identifier] <= range->comparand)
                    {
                        continue;
                    }
                    break;
            }

            switch (range->action[0])
            {
                case 'A': return true;
                case 'R': return false;
            }

            current = function_dictionary_get(dictionary, range->action);

            break;
        }
    }

    return false;
}

int main(void)
{
    char buffer[BUFFER_SIZE];
    struct FunctionDictionary dictionary = { 0 };
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stdin))
    {
        if (buffer[0] == '\n')
        {
            break;
        }

        char key[KEY_SIZE];
        struct Function current;
        struct Tokenizer lexer;

        tokenizer(&lexer, buffer);
        function(&current);

        if (!parse_function(&lexer, key, &current))
        {
            fprintf(stderr, EXCEPTION_FORMAT);

            return 1;
        }

        if (!function_dictionary_set(&dictionary, key, &current))
        {
            fprintf(stderr, "Error: Out of memory.\n");

            return 1;
        }
    }

    long sum = 0;

    while (fgets(buffer, sizeof buffer, stdin))
    {
        int localSum = 0;
        int dynamic[PROPERTY_NONE];
        Property property = 0;

        for (String token = strtok(buffer, DELIMITERS);
            token;
            token = strtok(NULL, DELIMITERS))
        {
            char* p = strchr(token, '=');
            struct Tokenizer lexer;

            if (!p)
            {
                fprintf(stderr, EXCEPTION_FORMAT);

                return 1;
            }

            int number;

            tokenizer(&lexer, p + 1);
            parse_number(&lexer, &number);

            localSum += number;
            dynamic[property] = number;
            property++;
        }

        if (scan(dynamic, &dictionary))
        {
            sum += localSum;
        }
    }

    printf("19a %ld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    function_dictionary_clear(&dictionary);

    return 0;
}
