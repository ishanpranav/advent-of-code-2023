// Licensed under the MIT License.

// Aplenty Part 2

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 64
#define CALL_STACK_CAPACITY 8
#define DELIMITERS ","
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

struct Interval
{
    int min;
    int max;
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

struct Call
{
    struct Function* function;
    struct Interval value[PROPERTY_NONE];
};

struct CallStack
{
    struct Call items[CALL_STACK_CAPACITY];
    int count;
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

typedef char* String;
typedef enum Property Property;
typedef struct Interval* Interval;
typedef struct Interval* BDynamic;
typedef struct Range* Range;
typedef struct Function* Function;
typedef struct Call* Call;
typedef struct CallStack* CallStack;
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

void b_dynamic(BDynamic instance)
{
    for (Property property = 0; property < PROPERTY_NONE; property++)
    {
        instance[property].min = 1;
        instance[property].max = 4001;
    }
}

void call_stack(CallStack instance)
{
    instance->count = 0;
}

void call_stack_push(CallStack instance, Call item)
{
    instance->items[instance->count] = *item;
    instance->count++;
}

bool call_stack_try_pop(CallStack instance, Call result)
{
    if (!instance->count)
    {
        return false;
    }

    instance->count--;
    *result = instance->items[instance->count];

    return true;
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

static long long scan(
    Call current,
    CallStack stack,
    FunctionDictionary dictionary)
{
    for (Range range = current->function->ranges;
        range < current->function->ranges + current->function->count;
        range++)
    {
        struct Call next;
        int comparand = range->comparand;

        switch (range->relation)
        {
            case '<':
                if (current->value[range->identifier].min >= comparand)
                {
                    continue;
                }

                if (current->value[range->identifier].max > comparand)
                {
                    next = *current;
                    next.value[range->identifier].min = comparand;
                    current->value[range->identifier].max = comparand;

                    call_stack_push(stack, &next);
                }
                break;

            case '>':
                if (current->value[range->identifier].max <= comparand + 1)
                {
                    continue;
                }

                if (current->value[range->identifier].min <= comparand)
                {
                    next = *current;
                    next.value[range->identifier].max = comparand + 1;
                    current->value[range->identifier].min = comparand + 1;

                    call_stack_push(stack, &next);
                }
                break;
        }

        switch (range->action[0])
        {
            case 'A':
            {
                long long product = 1;

                for (Property i = 0; i < PROPERTY_NONE; i++)
                {
                    product *= current->value[i].max - current->value[i].min;
                }

                return product;
            }

            case 'R': return 0;
        }

        next = *current;
        next.function = function_dictionary_get(dictionary, range->action);

        call_stack_push(stack, &next);

        break;
    }

    return 0;
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
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        if (!function_dictionary_set(&dictionary, key, &current))
        {
            fprintf(stderr, "Error: Out of memory.\n");

            return 1;
        }
    }

    long long total = 0;
    struct Call current;
    struct CallStack stack;

    current.function = function_dictionary_get(&dictionary, "in");

    b_dynamic(current.value);
    call_stack(&stack);
    call_stack_push(&stack, &current);

    while (call_stack_try_pop(&stack, &current))
    {
        total += scan(&current, &stack, &dictionary);
    }

    printf("19b %lld %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
    function_dictionary_clear(&dictionary);

    return 0;
}
