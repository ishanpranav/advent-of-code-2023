CC = gcc
CFLAGS = -O3 -pedantic -std=c99 -Wall -Wextra
LIBM = -lm
TWOS_COMPLEMENT = -fno-strict-overflow -fwrapv

all: \
	day01a day01b \
	day02a day02b \
	day03a day03b \
	day04a day04b \
	day05a day05b \
	day06a day06b \
	day07a day07b \
	day08a day08b \
	day09a day09b \
	day10a \
	day11a day11b \
	day12a day12b \
	day13a day13b \
	day14a day14b \
	day15a day15b \
	day16a day16b \
	day17a day17b \
	day18a day18b \
	day19a day19b \
	day20a day20b \
	day21a \
	day22a day22b \
	day24a day24b \
	day25z
	
day01a: src/day01a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day01b: src/day01b.c
	$(CC) $(CFLAGS) $< -o $@.o

day02a: src/day02a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day02b: src/day02b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day03a: src/day03a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day03b: src/day03b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day04a: src/day04a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day04b: src/day04b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day05a: src/day05a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day05b: src/day05b.c
	$(CC) $(CFLAGS) $(TWOS_COMPLEMENT) $< -o $@.o
	
day06a: src/day06a.c
	$(CC) $(CFLAGS) $< -o $@.o $(LIBM)
	
day06b: src/day06b.c
	$(CC) $(CFLAGS) $< -o $@.o $(LIBM)
	
day07a: src/day07a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day07b: src/day07b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day08a: src/day08a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day08b: src/day08b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day09a: src/day09a.c
	$(CC) $(CFLAGS) $< -o $@.o $(LIBM)
	
day09b: src/day09b.c
	$(CC) $(CFLAGS) $< -o $@.o $(LIBM)
	
day10a: src/day10a.c
	$(CC) $(CFLAGS) $< -o $@.o

day11a: src/day11a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day11b: src/day11b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day12a: src/day12a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day12b: src/day12b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day13a: src/day13a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day13b: src/day13b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day14a: src/day14a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day14b: src/day14b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day15a: src/day15a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day15b: src/day15b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day16a: src/day16a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day16b: src/day16b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day17a: src/day17a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day17b: src/day17b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day18a: src/day18a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day18b: src/day18b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day19a: src/day19a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day19b: src/day19b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day20a: src/day20a.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day20b: src/day20b.c
	$(CC) $(CFLAGS) $< -o $@.o
	
day21a: src/day21a.c
	$(CC) $(CFLAGS) $< -o $@.o

day22a: src/day22a.c
	$(CC) $(CFLAGS) $< -o $@.o

day22b: src/day22b.c
	$(CC) $(CFLAGS) $< -o $@.o

day24a: src/day24a.c
	$(CC) $(CFLAGS) $< -o $@.o

day24b: src/day24b.c
	$(CC) $(CFLAGS) $< -o $@.o

day25z: src/day25z.c
	$(CC) $(CFLAGS) $< -o $@.o

clean:
	rm -rf *.o
