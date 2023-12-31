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

struct Module
{
    struct Module* next;
    int targetCount;
    char name[MODULE_NAME_CAPACITY];
    char targets[MODULE_TARGETS_CAPACITY][MODULE_NAME_CAPACITY];
    bool flipFlop;
};

struct ConjunctionModuleEntry
{
    char name[MODULE_NAME_CAPACITY];
    bool pulse;
};

struct ConjunctionModuleBucket
{
    struct ConjunctionModuleEntry* firstEntry;
    struct ConjunctionModuleBucket* nextBucket;
};

struct ConjunctionModule
{
    struct ConjunctionModuleBucket* pulseFirstBucket;
    struct ConjunctionModuleBucket pulseBuckets[MODULE_COLLECTION_ENTRIES];
    struct Module base;
};

struct FlipFlopModule
{
    struct Module base;
    bool pulse;
};

struct ModuleCollectionEntry
{
    struct Module* first;
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
typedef struct Module* Module;
typedef struct ConjunctionModuleEntry* ConjunctionModuleEntry;
typedef struct ConjunctionModuleBucket* ConjunctionModuleBucket;
typedef struct ConjunctionModule* ConjunctionModule;
typedef struct FlipFlopModule* FlipFlopModule;
typedef struct ModuleCollectionEntry* ModuleCollectionEntry;
typedef struct ModuleCollection* ModuleCollection;

void module(Module instance, String name)
{
    instance->next = NULL;
    instance->targetCount = 0;

    strcpy(instance->name, name);
}

void conjunction_module(ConjunctionModule instance, String name)
{
    module(instance, name);

    instance->base.flipFlop = false;
}

void conjunction_module_send(
    ConjunctionModule instance,
    bool pulse,
    MessageQueue queue)
{

}

void flip_flop_module(FlipFlopModule instance, String name)
{
    module(instance, name);

    instance->base.flipFlop = true;
}

void module_add_target(Module instance, String item)
{
    strcpy(instance->targets[instance->targetCount], item);

    instance->targetCount++;
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

        Module current;

        if (buffer[0] == '%')
        {
            current = malloc(sizeof(struct FlipFlopModule));

            if (!current)
            {
                fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

                return 1;
            }

            flip_flop_module(current, buffer + 1);
        }
        else
        {
            current = malloc(sizeof(struct ConjunctionModule));

            if (!current)
            {
                fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

                return 1;
            }

            if (buffer[0] = '&')
            {
                conjunction_module(current, buffer + 1);
            }
            else
            {
                conjunction_module(current, buffer);
            }
        }

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
        for (Module module = entry->first; module; module = module->next)
        {
            for (int i = 0; i < module->targetCount; i++)
            {
                Module target = module_collection_find(module->targets[i]);

                if (target && !target->flipFlop)
                {
                    ConjunctionModule conjunctionModule = target;

                    conjunction_module_set_pulse(module->name, false);
                }
            }
        }
    }

    int counts[2] = { 0, 1000 };
    struct MessageQueue queue;
    ConjunctionModule broadcaster = module_collection_find("broadcaster");

    if (!broadcaster)
    {
        fprintf(stderr, "Error: Key not found.\n");

        return 1;
    }

    message_queue(&queue);

    for (int i = 0; i < 1000; i++)
    {
        conjunction_module_send(broadcaster, false, &queue);
    }

    int result = 0;

    printf("20a %d %lf\n", result, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
