// Licensed under the MIT License.

// Pulse Propagation Part 1

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUCKETS 131
#define BUFFER_SIZE 32
#define DELIMITERS ", \n"
#define KEY_CAPACITY 12
#define MESSAGE_QUEUE_CAPACITY 64
#define TARGETS_CAPACITY 8

struct DictionaryEntry
{
    struct DictionaryEntry* nextEntry;
    char key[KEY_CAPACITY];
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
    struct DictionaryBucket buckets[BUCKETS];
};

struct Message
{
    char source[KEY_CAPACITY];
    char target[KEY_CAPACITY];
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
    char name[KEY_CAPACITY];
    char targets[TARGETS_CAPACITY][KEY_CAPACITY];
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
    struct ModuleCollectionBucket buckets[BUCKETS];
};

typedef char* String;
typedef struct DictionaryEntry* DictionaryEntry;
typedef struct DictionaryBucket* DictionaryBucket;
typedef struct Dictionary* Dictionary;
typedef struct Message* Message;
typedef struct MessageQueue* MessageQueue;
typedef struct Module* Module;
typedef struct ModuleCollectionBucket* ModuleCollectionBucket;
typedef struct ModuleCollection* ModuleCollection;

unsigned int string_get_hash_code(String instance)
{
    unsigned int hash = 0;

    for (char* p = instance; *p; p++)
    {
        hash = (hash * 36) + *p - 97;
    }

    return hash;
}

void dictionary(Dictionary instance)
{
    instance->firstBucket = NULL;

    for (int i = 0; i < BUCKETS; i++)
    {
        instance->buckets[i].firstEntry = NULL;
        instance->buckets[i].nextBucket = NULL;
    }
}

bool dictionary_try_get_value(Dictionary instance, String key, bool* result)
{
    unsigned int hash = string_get_hash_code(key) % BUCKETS;

    for (DictionaryEntry entry = instance->buckets[hash].firstEntry;
        entry;
        entry = entry->nextEntry)
    {
        if (strcmp(key, entry->key) == 0)
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
    unsigned int hash = string_get_hash_code(key) % BUCKETS;

    for (p = &instance->buckets[hash].firstEntry; *p; p = &(*p)->nextEntry)
    {
        if (strcmp(key, (*p)->key) == 0)
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

    strcpy(entry->key, key);

    entry->value = value;
    entry->nextEntry = NULL;
    *p = entry;

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

void module(Module instance, bool isConjunction, String name)
{
    instance->isConjunction = isConjunction;
    instance->nextModule = NULL;
    instance->targetCount = 0;

    if (isConjunction)
    {
        instance->child.pulses = malloc(sizeof * instance->child.pulses);

        dictionary(instance->child.pulses);
    }
    else
    {
        instance->child.pulse = false;
    }

    strcpy(instance->name, name);
}

void message_queue(MessageQueue instance)
{
    instance->first = -1;
    instance->last = -1;
}

void message_queue_enqueue(MessageQueue instance, Message item)
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

    instance->items[instance->last] = *item;
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

bool module_all_pulses(Module instance)
{
    for (DictionaryBucket bucket = instance->child.pulses->firstBucket;
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

void module_add_target(Module instance, String item)
{
    strcpy(instance->targets[instance->targetCount], item);

    instance->targetCount++;
}

void module_send(Module module, MessageQueue queue, bool pulse)
{
    for (int i = 0; i < module->targetCount; i++)
    {
        struct Message message;

        message.pulse = pulse;

        strcpy(message.source, module->name);
        strcpy(message.target, module->targets[i]);
        message_queue_enqueue(queue, &message);
    }
}

Module module_collection_get(ModuleCollection instance, String name)
{
    unsigned int hash = string_get_hash_code(name) % BUCKETS;

    for (Module module = instance->buckets[hash].firstModule;
        module;
        module = module->nextModule)
    {
        if (strcmp(name, module->name) == 0)
        {
            return module;
        }
    }

    return NULL;
}

bool module_collection_add(ModuleCollection instance, Module item)
{
    Module* p;
    unsigned int hash = string_get_hash_code(item->name) % BUCKETS;

    for (p = &instance->buckets[hash].firstModule; *p; p = &(*p)->nextModule)
    {
        if (strcmp(item->name, (*p)->name) == 0)
        {
            return false;
        }
    }

    if (!instance->buckets[hash].firstModule)
    {
        ModuleCollectionBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;
    }

    *p = item;

    return true;
}

static void scan(Module module, Message message, MessageQueue queue)
{
    if (module->isConjunction)
    {
        dictionary_set(module->child.pulses, message->source, message->pulse);
        module_send(module, queue, !module_all_pulses(module));
    }
    else if (!message->pulse)
    {
        module->child.pulse = !module->child.pulse;

        module_send(module, queue, module->child.pulse);
    }
}

int main()
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

        module(next, buffer[0] != '%', buffer + 1);
        module_collection_add(&modules, next);

        for (String target = strtok(mid + 4, DELIMITERS);
            target;
            target = strtok(NULL, DELIMITERS))
        {
            module_add_target(next, target);
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
                    module->targets[i]);

                if (target && target->isConjunction)
                {
                    dictionary_set(target->child.pulses, module->name, false);
                }
            }
        }
    }

    Module broadcaster = module_collection_get(&modules, "roadcaster");

    if (!broadcaster)
    {
        fprintf(stderr, "Error: Key not found.");

        return 1;
    }

    int counts[2] = { 1000, 0 };

    for (int i = 0; i < 1000; i++)
    {
        struct Message current;
        struct MessageQueue queue;

        message_queue(&queue);
        module_send(broadcaster, &queue, false);

        while (message_queue_try_dequeue(&queue, &current))
        {
            counts[current.pulse]++;

            Module target = module_collection_get(&modules, current.target);

            if (target)
            {
                scan(target, &current, &queue);
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
            if (module->isConjunction)
            {
                dictionary_clear(module->child.pulses);
                free(module->child.pulses);
            }

            Module next = module->nextModule;

            free(module);

            module = next;
        }
    }
    
    return 0;
}
