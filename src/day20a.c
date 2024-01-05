// Licensed under the MIT License.

// Pulse Propagation Part 1

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 32
#define DELIMITERS ", \n"
#define DICTIONARY_BUCKETS 53
#define EXCEPTION_OUT_OF_MEMORY "Error: Out of memory.\n"
#define MESSAGE_QUEUE_CAPACITY 64
#define MODULE_COLLECTION_BUCKETS 97
#define MODULE_TARGETS_CAPACITY 8
#define STRING_CAPACITY 16

struct String
{
    int length;
    char buffer[STRING_CAPACITY];
};

struct DictionaryEntry
{
    struct DictionaryEntry* nextEntry;
    struct String key;
    bool value;
};

struct DictionaryBucket
{
    struct DictionaryEntry* firstEntry;
    struct DictionaryBucket* nextBucket;
};

struct Dictionary
{
    struct DictionaryBucket* firstBucket;
    struct DictionaryBucket buckets[DICTIONARY_BUCKETS];
};

struct Message
{
    struct String source;
    struct String target;
    bool pulse;
};

struct MessageQueue
{
    int first;
    int last;
    struct Message items[MESSAGE_QUEUE_CAPACITY];
};

union ModuleType
{
    bool pulse;
    struct Dictionary* pulses;
};

struct Module
{
    struct Module* nextModule;
    union ModuleType child;
    int targetCount;
    struct String name;
    struct String targets[MODULE_TARGETS_CAPACITY];
    bool isConjunction;
};

struct ModuleCollectionBucket
{
    struct Module* firstModule;
    struct ModuleCollectionBucket* nextBucket;
};

struct ModuleCollection
{
    struct ModuleCollectionBucket* firstBucket;
    struct ModuleCollectionBucket buckets[MODULE_COLLECTION_BUCKETS];
};

typedef char* ZString;
typedef struct String* String;
typedef struct DictionaryEntry* DictionaryEntry;
typedef struct DictionaryBucket* DictionaryBucket;
typedef struct Dictionary* Dictionary;
typedef struct Message* Message;
typedef struct MessageQueue* MessageQueue;
typedef struct Module* Module;
typedef struct ModuleCollectionBucket* ModuleCollectionBucket;
typedef struct ModuleCollection* ModuleCollection;

void string(String instance, ZString buffer)
{
    instance->length = strlen(buffer);

    memcpy(instance->buffer, buffer, instance->length);
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

unsigned int string_get_hash_code(String instance)
{
    unsigned int hash = 0;

    for (int i = 0; i < instance->length; i++)
    {
        hash = (hash * 36) + instance->buffer[i] - 97;
    }

    return hash;
}

void dictionary(Dictionary instance)
{
    instance->firstBucket = NULL;

    for (int i = 0; i < DICTIONARY_BUCKETS; i++)
    {
        instance->buckets[i].firstEntry = NULL;
        instance->buckets[i].nextBucket = NULL;
    }
}

bool dictionary_try_get_value(Dictionary instance, String key, bool* result)
{
    unsigned int hash = string_get_hash_code(key) % DICTIONARY_BUCKETS;

    for (DictionaryEntry entry = instance->buckets[hash].firstEntry;
        entry;
        entry = entry->nextEntry)
    {
        if (string_equals(key, &entry->key))
        {
            *result = entry->value;

            return true;
        }
    }

    return false;
}

bool dictionary_set(Dictionary instance, String key, bool value)
{
    DictionaryEntry* p;
    unsigned int hash = string_get_hash_code(key) % DICTIONARY_BUCKETS;

    for (p = &instance->buckets[hash].firstEntry; *p; p = &(*p)->nextEntry)
    {
        if (string_equals(key, &(*p)->key))
        {
            (*p)->value = value;

            return true;
        }
    }

    DictionaryEntry entry = malloc(sizeof * entry);

    if (!entry)
    {
        return false;
    }

    if (!instance->buckets[hash].firstEntry)
    {
        DictionaryBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;
    }

    string_copy(&entry->key, key);

    entry->value = value;
    entry->nextEntry = NULL;
    *p = entry;

    return true;
}

bool dictionary_all(Dictionary instance)
{
    for (DictionaryBucket bucket = instance->firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        for (DictionaryEntry entry = bucket->firstEntry;
            entry;
            entry = entry->nextEntry)
        {
            if (!entry->value)
            {
                return false;
            }
        }
    }

    return true;
}

void dictionary_clear(Dictionary instance)
{
    for (DictionaryBucket bucket = instance->firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        DictionaryEntry entry = bucket->firstEntry;

        while (entry)
        {
            DictionaryEntry nextEntry = entry->nextEntry;

            free(entry);

            entry = nextEntry;
        }

        bucket->firstEntry = NULL;
    }

    instance->firstBucket = NULL;
}

void message_queue(MessageQueue instance)
{
    instance->first = -1;
    instance->last = -1;
}

Message message_queue_enqueue(MessageQueue instance)
{
    if (instance->first == -1)
    {
        instance->first = 0;
        instance->last = 0;
    }
    else if (instance->first && instance->last == MESSAGE_QUEUE_CAPACITY - 1)
    {
        instance->last = 0;
    }
    else
    {
        instance->last++;
    }

    return instance->items + instance->last;
}

bool message_queue_try_dequeue(MessageQueue instance, Message result)
{
    if (instance->first == -1)
    {
        return false;
    }

    *result = instance->items[instance->first];

    if (instance->first == instance->last)
    {
        instance->first = -1;
        instance->last = -1;
    }
    else if (instance->first == MESSAGE_QUEUE_CAPACITY - 1)
    {
        instance->first = 0;
    }
    else
    {
        instance->first++;
    }

    return true;
}

bool module(Module instance, bool isConjunction, ZString name)
{
    instance->isConjunction = isConjunction;
    instance->nextModule = NULL;
    instance->targetCount = 0;

    if (isConjunction)
    {
        instance->child.pulses = malloc(sizeof * instance->child.pulses);

        if (!instance->child.pulses)
        {
            return false;
        }

        dictionary(instance->child.pulses);
    }
    else
    {
        instance->child.pulse = false;
    }

    string(&instance->name, name);

    return true;
}

String module_new_target(Module instance)
{
    String result = instance->targets + instance->targetCount;

    instance->targetCount++;

    return result;
}

void module_send(Module module, MessageQueue queue, bool pulse)
{
    for (int i = 0; i < module->targetCount; i++)
    {
        Message message = message_queue_enqueue(queue);

        message->pulse = pulse;

        string_copy(&message->source, &module->name);
        string_copy(&message->target, module->targets + i);
    }
}

bool module_respond(Module instance, Message message, MessageQueue queue)
{
    if (instance->isConjunction)
    {
        if (!dictionary_set(
            instance->child.pulses,
            &message->source,
            message->pulse))
        {
            fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

            return false;
        }

        module_send(instance, queue, !dictionary_all(instance->child.pulses));
    }
    else
    {
        if (message->pulse)
        {
            return true;
        }

        instance->child.pulse = !instance->child.pulse;

        module_send(instance, queue, instance->child.pulse);
    }

    return true;
}

void finalize_module(Module instance)
{
    if (instance->isConjunction)
    {
        dictionary_clear(instance->child.pulses);
        free(instance->child.pulses);
    }
}

Module module_collection_get(ModuleCollection instance, String name)
{
    unsigned int hash = string_get_hash_code(name) % MODULE_COLLECTION_BUCKETS;

    for (Module module = instance->buckets[hash].firstModule;
        module;
        module = module->nextModule)
    {
        if (string_equals(name, &module->name))
        {
            return module;
        }
    }

    return NULL;
}

void module_collection_add(ModuleCollection instance, Module item)
{
    unsigned int hash = string_get_hash_code(&item->name) %
        MODULE_COLLECTION_BUCKETS;

    if (!instance->buckets[hash].firstModule)
    {
        ModuleCollectionBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;
    }

    item->nextModule = instance->buckets[hash].firstModule;
    instance->buckets[hash].firstModule = item;
}

int main(void)
{
    clock_t start = clock();
    char buffer[BUFFER_SIZE];
    struct ModuleCollection modules = { 0 };

    while (fgets(buffer, sizeof buffer, stdin))
    {
        char* mid = strstr(buffer, " -> ");

        if (!mid)
        {
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        *mid = '\0';

        Module next = malloc(sizeof * next);

        if (!next || !module(next, buffer[0] != '%', buffer + 1))
        {
            fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

            return 1;
        }

        module_collection_add(&modules, next);

        for (ZString token = strtok(mid + 4, DELIMITERS);
            token;
            token = strtok(NULL, DELIMITERS))
        {
            string(module_new_target(next), token);
        }
    }

    for (ModuleCollectionBucket bucket = modules.firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        for (Module module = bucket->firstModule;
            module;
            module = module->nextModule)
        {
            for (int i = 0; i < module->targetCount; i++)
            {
                Module target = module_collection_get(
                    &modules,
                    module->targets + i);

                if (!target || !target->isConjunction)
                {
                    continue;
                }

                if (!dictionary_set(target->child.pulses, &module->name, false))
                {
                    fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

                    return 1;
                }
            }
        }
    }

    struct String broadcasterKey =
    {
        .length = 10,
        .buffer = { 'r', 'o', 'a', 'd', 'c', 'a', 's', 't', 'e', 'r' }
    };
    Module broadcaster = module_collection_get(&modules, &broadcasterKey);

    if (!broadcaster)
    {
        fprintf(stderr, "Error: Key not found.\n");

        return 1;
    }

    int counts[2] =
    {
        [true] = 0,
        [false] = 1000
    };

    for (int i = 0; i < 1000; i++)
    {
        struct Message current;
        struct MessageQueue queue;

        message_queue(&queue);
        module_send(broadcaster, &queue, false);

        while (message_queue_try_dequeue(&queue, &current))
        {
            counts[current.pulse]++;

            Module target = module_collection_get(&modules, &current.target);

            if (!target)
            {
                continue;
            }

            if (!module_respond(target, &current, &queue))
            {
                fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

                return 1;
            }
        }
    }

    long result = counts[true] * counts[false];

    printf("20a %ld %lf\n", result, (double)(clock() - start) / CLOCKS_PER_SEC);

    for (ModuleCollectionBucket bucket = modules.firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        Module module = bucket->firstModule;

        while (module)
        {
            Module next = module->nextModule;

            finalize_module(module);
            free(module);

            module = next;
        }
    }

    return 0;
}
