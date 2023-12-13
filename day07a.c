// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Custom implementation

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 16
#define DELIMITERS " "
#define HAND_SIZE 5
#define PLAYER_LIST_CAPACITY 1024

enum Card
{
    CARD_TWO,
    CARD_THREE,
    CARD_FOUR,
    CARD_FIVE,
    CARD_SIX,
    CARD_SEVEN,
    CARD_EIGHT,
    CARD_NINE,
    CARD_TEN,
    CARD_JACK,
    CARD_QUEEN,
    CARD_KING,
    CARD_ACE,
    CARD_NONE
};

struct Hand
{
    int count;
    int maxFrequency;
    int frequencies[CARD_NONE];
};

enum HandType
{
    HAND_TYPE_HIGH_CARD,
    HAND_TYPE_ONE_PAIR,
    HAND_TYPE_TWO_PAIR,
    HAND_TYPE_THREE_OF_A_KIND,
    HAND_TYPE_FULL_HOUSE,
    HAND_TYPE_FOUR_OF_A_KIND,
    HAND_TYPE_FIVE_OF_A_KIND,
    HAND_TYPE_NONE
};

struct Player
{
    enum Card cards[HAND_SIZE];
    enum HandType handType;
    int bid;
};

struct PlayerList
{
    struct Player items[PLAYER_LIST_CAPACITY];
    int count;
};

typedef char* String;
typedef enum Card Card;
typedef struct Hand* Hand;
typedef enum HandType HandType;
typedef struct Player Player;
typedef struct PlayerList* PlayerList;

Card card(char symbol)
{
    if (symbol >= '2' && symbol <= '9')
    {
        return symbol - '2';
    }

    switch (symbol)
    {
    case 'T': return CARD_TEN;
    case 'J': return CARD_JACK;
    case 'Q': return CARD_QUEEN;
    case 'K': return CARD_KING;
    case 'A': return CARD_ACE;
    }

    return CARD_NONE;
}

int player_compare(const void* left, const void* right)
{
    if (!left && !right)
    {
        return 0;
    }

    if (!left)
    {
        return -1;
    }

    if (!right)
    {
        return 1;
    }

    const Player* leftPlayer = left;
    const Player* rightPlayer = right;
    int handDifference = rightPlayer->handType - leftPlayer->handType;

    if (handDifference)
    {
        return handDifference;
    }

    for (int i = 0; i < HAND_SIZE; i++)
    {
        int cardDifference = rightPlayer->cards[i] - leftPlayer->cards[i];

        if (cardDifference)
        {
            return cardDifference;
        }
    }

    return 0;
}

void player_list(PlayerList instance)
{
    instance->count = 0;
}

void player_list_add(PlayerList instance, Player item)
{
    int count = instance->count;

    instance->items[count] = item;
    instance->count = count + 1;
}

void player_list_sort(PlayerList instance)
{
    qsort(
        instance->items,
        instance->count,
        sizeof(struct Player),
        player_compare);
}

void hand_add(Hand instance, Card item)
{
    int frequency = instance->frequencies[item];

    if (!frequency)
    {
        instance->count++;
    }

    frequency++;
    instance->frequencies[item] = frequency;

    if (frequency > instance->maxFrequency)
    {
        instance->maxFrequency = frequency;
    }
}

HandType hand_get_type(Hand instance)
{
    switch (instance->count)
    {
    case 1: return HAND_TYPE_FIVE_OF_A_KIND;
    case 2:
        switch (instance->maxFrequency)
        {
        case 3: return HAND_TYPE_FULL_HOUSE;
        case 4: return HAND_TYPE_FOUR_OF_A_KIND;
        }
        break;
    case 3:
        switch (instance->maxFrequency)
        {
        case 2: return HAND_TYPE_TWO_PAIR;
        case 3: return HAND_TYPE_THREE_OF_A_KIND;
        }
        break;
    case 4: return HAND_TYPE_ONE_PAIR;
    case 5: return HAND_TYPE_HIGH_CARD;
    }

    return HAND_TYPE_NONE;
}

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day7a <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    struct PlayerList players;
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    player_list(&players);

    while (fgets(buffer, sizeof buffer, stream))
    {
        String token = strtok(buffer, DELIMITERS);

        if (!token)
        {
            fclose(stream);
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        Player player;
        struct Hand hand = { 0 };

        for (int i = 0; i < HAND_SIZE; i++)
        {
            Card drawn = card(token[i]);

            if (drawn == CARD_NONE)
            {
                fclose(stream);
                fprintf(stderr, "Error: Format.\n");

                return 1;
            }

            hand_add(&hand, drawn);

            player.cards[i] = drawn;
        }

        HandType handType = hand_get_type(&hand);

        token = strtok(NULL, DELIMITERS);

        if (!token || handType == HAND_TYPE_NONE)
        {
            fclose(stream);
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        player.handType = handType;
        player.bid = atoi(token);

        player_list_add(&players, player);
    }

    player_list_sort(&players);

    long sum = 0;

    for (int i = 0; i < players.count; i++)
    {
        sum += (players.count - i) * players.items[i].bid;
    }

    printf("%ld : %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
