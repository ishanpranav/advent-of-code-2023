// Licensed under the MIT License.

// Pulse Propagation Part 1

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 32
#define DELIMITERS ", "
#define EXCEPTION_FORMAT "Error: Format.\n"
#define EXCEPTION_OUT_OF_MEMORY "Error: Out of memory.\n"
#define MESSAGE_QUEUE_CAPACITY 64
#define MODULE_COLLECTION_ENTRIES 89
#define MODULE_NAME_CAPACITY 12
#define MODULE_TARGETS_CAPACITY 5

struct Message
{
    char source[MODULE_NAME_CAPACITY];
    char target[MODULE_NAME_CAPACITY];
    bool pulse;
};

struct MessageQueue
{
    int first;
    int last;
    struct Message items[MESSAGE_QUEUE_CAPACITY];
};

struct PulseEntry
{
    struct PulseEntry* nextEntry;
    char name[MODULE_NAME_CAPACITY];
    bool pulse;
};

struct PulseBucket
{
    struct PulseEntry* firstEntry;
    struct PulseBucket* nextBucket;
};

struct Module
{
    struct Module* nextModule;
    struct PulseBucket* firstBucket;
    struct PulseBucket buckets[MODULE_COLLECTION_ENTRIES];
    int targetCount;
    char name[MODULE_NAME_CAPACITY];
    char targets[MODULE_TARGETS_CAPACITY][MODULE_NAME_CAPACITY];
    bool pulse;
    bool isConjunction;
};

struct ModuleCollectionEntry
{
    struct Module* firstModule;
    struct ModuleCollectionEntry* nextEntry;
};

struct ModuleCollection
{
    struct ModuleCollectionEntry* firstEntry;
    struct ModuleCollectionEntry entries[MODULE_COLLECTION_ENTRIES];
};

typedef char* String;
typedef struct Message* Message;
typedef struct MessageQueue* MessageQueue;
typedef struct PulseEntry* PulseEntry;
typedef struct PulseBucket* PulseBucket;
typedef struct Module* Module;
typedef struct ConjunctionModuleEntry* ConjunctionModuleEntry;
typedef struct ConjunctionModuleBucket* ConjunctionModuleBucket;
typedef struct ModuleCollectionEntry* ModuleCollectionEntry;
typedef struct ModuleCollection* ModuleCollection;

int string_get_hash_code(String instance)
{
    int hash = 7;

    for (char* p = instance; *p; p++)
    {
        hash = (hash * 31) + *p;
    }

    return hash;
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

void module(Module instance, bool isConjunction, String name)
{
    instance->firstBucket = NULL;
    instance->isConjunction = isConjunction;
    instance->nextModule = NULL;
    instance->pulse = false;
    instance->targetCount = 0;

    strcpy(instance->name, name);
}

void module_send(Module instance, bool pulse, MessageQueue queue)
{
    for (int i = 0; i < instance->targetCount; i++)
    {
        struct Message message;

        message.pulse = pulse;

        strcpy(message.source, instance->name);
        strcpy(message.target, instance->targets[i]);
        message_queue_enqueue(queue, &message);
    }
}

void module_add_target(Module instance, String item)
{
    strcpy(instance->targets[instance->targetCount], item);

    instance->targetCount++;
}

bool module_set_pulse(Module instance, String name, bool value)
{
    PulseEntry* p;
    unsigned int hash = string_get_hash_code(name);

    hash %= MODULE_COLLECTION_ENTRIES;

    for (p = &instance->buckets[hash].firstEntry; *p; p = &(*p)->nextEntry)
    {
        if (strcmp(name, (*p)->name) == 0)
        {
            (*p)->pulse = value;

            return true;
        }
    }

    PulseEntry entry = malloc(sizeof * entry);

    if (!entry)
    {
        return false;
    }

    if (!instance->buckets[hash].firstEntry)
    {
        PulseBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;
    }

    strcpy(entry->name, name);

    entry->pulse = value;
    entry->nextEntry = NULL;
    *p = entry;

    return true;
}

bool module_respond(Module instance, Message message, MessageQueue queue)
{
    bool result;

    if (instance->isConjunction)
    {
        if (!module_set_pulse(instance, message->source, message->pulse))
        {
            return false;
        }

        result = false;

        for (PulseBucket bucket = instance->buckets;
            bucket;
            bucket = bucket->nextBucket)
        {
            for (PulseEntry entry = bucket->firstEntry;
                entry;
                entry = entry->nextEntry)
            {
                if (!entry->pulse)
                {
                    result = true;

                    break;
                }
            }
        }
    }
    else if (!message->pulse)
    {
        instance->pulse = !instance->pulse;
        result = instance->pulse;
    }
    else
    {
        return true;
    }

    module_send(instance, result, queue);

    return true;
}

void module_collection_add(ModuleCollection instance, Module item)
{
    Module* p;
    unsigned int hash = string_get_hash_code(item->name);

    hash %= MODULE_COLLECTION_ENTRIES;

    for (p = &instance->entries[hash].firstModule; *p; p = &(*p)->nextModule)
    {
        if (strcmp(item->name, (*p)->name) == 0)
        {
            *p = item;

            return;
        }
    }

    if (!instance->entries[hash].firstModule)
    {
        ModuleCollectionEntry first = instance->firstEntry;

        instance->entries[hash].nextEntry = first;
        instance->firstEntry = instance->entries + hash;
    }

    *p = item;
}

Module module_collection_find(ModuleCollection instance, String name)
{
    unsigned int hash = string_get_hash_code(name);

    hash %= MODULE_COLLECTION_ENTRIES;

    for (Module module = instance->entries[hash].firstModule;
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

void module_collection_finalize(ModuleCollection instance)
{
    for (ModuleCollectionEntry entry = instance->firstEntry;
        entry;
        entry = entry->nextEntry)
    {
        Module current = entry->firstModule;

        while (current)
        {
            Module next = current->nextModule;

            free(current);

            current = next;
        }

        entry->firstModule = NULL;
    }

    instance->firstEntry = NULL;
}

int main()
{
    char buffer[BUFFER_SIZE];
    clock_t start = clock();
    struct ModuleCollection modules = { 0 };

    while (fgets(buffer, sizeof buffer, stdin))
    {
        char* mid = strstr(buffer, " -> ");

        if (!mid)
        {
            fprintf(stderr, EXCEPTION_FORMAT);

            return 1;
        }

        *mid = '\0';

        Module current = malloc(sizeof * current);

        if (!current)
        {
            fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

            return 1;
        }

        String name;

        if (buffer[0] == '&' || buffer[0] == '%')
        {
            name = buffer + 1;
        }
        else
        {
            name = buffer;
        }

        module(current, buffer[0] == '%', name);
        module_collection_add(&modules, current);

        for (String target = strtok(mid + 4, DELIMITERS);
            target;
            target = strtok(NULL, DELIMITERS))
        {
            module_add_target(current, target);
        }
    }

    for (ModuleCollectionEntry entry = modules.firstEntry;
        entry;
        entry = entry->nextEntry)
    {
        for (Module module = entry->firstModule;
            module;
            module = module->nextModule)
        {
            for (int i = 0; i < module->targetCount; i++)
            {
                Module target = module_collection_find(
                    &modules,
                    module->targets[i]);

                if (!target || !target->isConjunction)
                {
                    continue;
                }

                if (!module_set_pulse(target, module->name, false))
                {
                    fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

                    return 1;
                }
            }
        }
    }

    int counts[2] = { 0, 1000 };
    Module broadcaster = module_collection_find(&modules, "broadcaster");

    if (!broadcaster)
    {
        fprintf(stderr, "Error: Key not found.\n");

        return 1;
    }

    for (int i = 0; i < 1000; i++)
    {
        struct MessageQueue queue;
        struct Message currentMessage;

        message_queue(&queue);
        module_send(broadcaster, false, &queue);

        while (message_queue_try_dequeue(&queue, &currentMessage))
        {
            counts[currentMessage.pulse]++;

            Module target = module_collection_find(
                &modules,
                currentMessage.target);

            if (!target)
            {
                continue;
            }

            module_respond(target, &currentMessage, &queue);
        }
    }

    printf("%d, %d\n", counts[0], counts[1]);

    long result = counts[0] * counts[1];

    printf("20a %ld %lf\n", result, (double)(clock() - start) / CLOCKS_PER_SEC);
    module_collection_finalize(&modules);

    return 0;
}
