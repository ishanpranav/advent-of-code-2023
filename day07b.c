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

enum BCard
{
    B_CARD_JOKER,
    B_CARD_TWO,
    B_CARD_THREE,
    B_CARD_FOUR,
    B_CARD_FIVE,
    B_CARD_SIX,
    B_CARD_SEVEN,
    B_CARD_EIGHT,
    B_CARD_NINE,
    B_CARD_TEN,
    B_CARD_QUEEN,
    B_CARD_KING,
    B_CARD_ACE,
    B_CARD_NONE
};

struct BHand
{
    int count;
    int frequencies[B_CARD_NONE];
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

struct BPlayer
{
    enum BCard cards[HAND_SIZE];
    enum HandType handType;
    int bid;
};

struct BPlayerList
{
    struct BPlayer items[PLAYER_LIST_CAPACITY];
    int count;
};

typedef char* String;
typedef enum BCard BCard;
typedef struct BHand* BHand;
typedef enum HandType HandType;
typedef struct BPlayer BPlayer;
typedef struct BPlayerList* BPlayerList;

BCard b_card(char symbol)
{
    if (symbol >= '2' && symbol <= '9')
    {
        return symbol - '1';
    }

    switch (symbol)
    {
    case 'J': return B_CARD_JOKER;
    case 'T': return B_CARD_TEN;
    case 'Q': return B_CARD_QUEEN;
    case 'K': return B_CARD_KING;
    case 'A': return B_CARD_ACE;
    }

    return B_CARD_NONE;
}

int b_player_compare(const void* left, const void* right)
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

    const BPlayer* leftPlayer = left;
    const BPlayer* rightPlayer = right;
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

void b_player_list(BPlayerList instance)
{
    instance->count = 0;
}

void b_player_list_add(BPlayerList instance, BPlayer item)
{
    int count = instance->count;

    instance->items[count] = item;
    instance->count = count + 1;
}

void b_player_list_sort(BPlayerList instance)
{
    qsort(
        instance->items,
        instance->count,
        sizeof(struct BPlayer),
        b_player_compare);
}

void b_hand_add(BHand instance, BCard item)
{
    int frequency = instance->frequencies[item];

    if (!frequency)
    {
        instance->count++;
    }

    frequency++;
    instance->frequencies[item] = frequency;
}

HandType b_hand_get_type(BHand instance)
{
    BCard mode = B_CARD_TWO;

    for (BCard card = B_CARD_THREE; card < B_CARD_NONE; card++)
    {
        if (instance->frequencies[card] > instance->frequencies[mode])
        {
            mode = card;
        }
    }

    int jokers = instance->frequencies[B_CARD_JOKER];

    if (jokers && jokers < HAND_SIZE) 
    {
        instance->count--;
        instance->frequencies[B_CARD_JOKER] = 0;
        instance->frequencies[mode] += jokers;
    }

    switch (instance->count)
    {
    case 1: return HAND_TYPE_FIVE_OF_A_KIND;
    case 2:
        switch (instance->frequencies[mode])
        {
        case 3: return HAND_TYPE_FULL_HOUSE;
        case 4: return HAND_TYPE_FOUR_OF_A_KIND;
        }
        break;
    case 3:
        switch (instance->frequencies[mode])
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
        printf("Usage: day7b <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    struct BPlayerList players;
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    b_player_list(&players);

    while (fgets(buffer, sizeof buffer, stream))
    {
        String token = strtok(buffer, DELIMITERS);

        if (!token)
        {
            fclose(stream);
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        BPlayer player;
        struct BHand current = { 0 };

        for (int i = 0; i < HAND_SIZE; i++)
        {
            BCard drawn = b_card(token[i]);

            if (drawn == B_CARD_NONE)
            {
                fclose(stream);
                fprintf(stderr, "Error: Format.\n");

                return 1;
            }

            b_hand_add(&current, drawn);

            player.cards[i] = drawn;
        }

        HandType handType = b_hand_get_type(&current);

        token = strtok(NULL, DELIMITERS);

        if (!token || handType == HAND_TYPE_NONE)
        {
            fclose(stream);
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        player.handType = handType;
        player.bid = atoi(token);

        b_player_list_add(&players, player);
    }

    b_player_list_sort(&players);

    long sum = 0;

    for (int i = 0; i < players.count; i++)
    {
        sum += (players.count - i) * players.items[i].bid;
    }

    printf("%ld : %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
