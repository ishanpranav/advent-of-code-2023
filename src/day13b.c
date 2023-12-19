// Licensed under the MIT License.

// Point of Incidence Part 2

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define DIMENSION 32

typedef char* String;
typedef unsigned long BitVector;
typedef unsigned long* BitMatrix;

bool bit_vector_is_pow_2(BitVector instance)
{
    return instance > 0 && (instance & (instance - 1)) == 0;
}

void bit_matrix_clear(BitMatrix instance)
{
    memset(instance, 0, DIMENSION * sizeof(unsigned long));
}

static bool mid(BitMatrix matrix, int index)
{
    int left = index;
    int right = index + 1;
    bool smudged = false;

    while (left >= 0 && matrix[right])
    {
        if (matrix[left] != matrix[right])
        {
            if (smudged || !bit_vector_is_pow_2(matrix[left] ^ matrix[right]))
            {
                return false;
            }

            smudged = true;
        }

        left--;
        right++;
    }

    return smudged;
}

static int realize(BitMatrix matrix)
{
    for (int i = 0; matrix[i + 1]; i++)
    {
        if (mid(matrix, i))
        {
            return i + 1;
        }
    }

    return 0;
}

static int realize_xy(BitMatrix x, BitMatrix y)
{
    int result = realize(y);

    if (result)
    {
        return 100 * result;
    }

    return realize(x);
}

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day13b <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    int i = 0;
    int total = 0;
    BitVector x[DIMENSION] = { 0 };
    BitVector y[DIMENSION] = { 0 };
    char buffer[DIMENSION + 2];
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stream))
    {
        if (buffer[0] == '\n')
        {
            i = 0;
            total += realize_xy(x, y);

            bit_matrix_clear(x);
            bit_matrix_clear(y);
            continue;
        }

        char current;

        for (int j = 0; (current = buffer[j]) && j < DIMENSION; j++)
        {
            switch (current)
            {
                case '#':
                    y[i] = (y[i] << 1) | 1;
                    x[j] = (x[j] << 1) | 1;
                    break;

                case '.':
                    y[i] <<= 1;
                    x[j] <<= 1;
                    break;
            }
        }

        i++;
    }

    total += realize_xy(x, y);

    printf("%d : %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
