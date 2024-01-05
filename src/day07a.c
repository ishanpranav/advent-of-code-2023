// Licensed under the MIT License.

// Camel Cards Part 1

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 16
#define DELIMITERS " "
#define EXCEPTION_FORMAT "Error: Format.\n"
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
    int frequencies[CARD_NONE];
    int count;
    int maxFrequency;
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

typedef const void* Object;
typedef char* String;
typedef enum Card Card;
typedef enum HandType HandType;
typedef struct Hand* Hand;
typedef struct Player* Player;
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

int player_compare(Object left, Object right)
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

    const struct Player* leftPlayer = left;
    const struct Player* rightPlayer = right;
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

Player player_list_new_player(PlayerList instance)
{
    Player result = instance->items + instance->count;

    instance->count++;

    return result;
}

void player_list_sort(PlayerList instance)
{
    qsort(
        instance->items,
        instance->count,
        sizeof * instance->items,
        player_compare);
}

int main(void)
{
    struct PlayerList players;
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    player_list(&players);

    while (fgets(buffer, sizeof buffer, stdin))
    {
        String token = strtok(buffer, DELIMITERS);

        if (!token)
        {
            fprintf(stderr, EXCEPTION_FORMAT);

            return 1;
        }

        struct Hand hand = { 0 };
        Player player = player_list_new_player(&players);

        for (int i = 0; i < HAND_SIZE; i++)
        {
            Card drawn = card(token[i]);

            if (drawn == CARD_NONE)
            {
                fprintf(stderr, EXCEPTION_FORMAT);

                return 1;
            }

            hand_add(&hand, drawn);

            player->cards[i] = drawn;
        }

        HandType handType = hand_get_type(&hand);

        token = strtok(NULL, DELIMITERS);

        if (!token || handType == HAND_TYPE_NONE)
        {
            fprintf(stderr, EXCEPTION_FORMAT);

            return 1;
        }

        player->handType = handType;
        player->bid = atoi(token);
    }

    player_list_sort(&players);

    long sum = 0;

    for (int i = 0; i < players.count; i++)
    {
        sum += (players.count - i) * players.items[i].bid;
    }

    printf("07a %ld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
