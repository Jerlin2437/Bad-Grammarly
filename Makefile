CC        = gcc
CFLAGS    = -Wall -Wextra -g -std=c99 -fsanitize=address,undefined -Wvla -DREALMALLOC

spchk: spchk.o dictionary.o
	$(CC) $(CFLAGS) spchk.o dictionary.o -o spchk

spchk.o: spchk.c dictionary.h
	$(CC) $(CFLAGS) -c spchk.c

dictionary.o: dictionary.c dictionary.h
	$(CC) $(CFLAGS) -c dictionary.c
clean:
	rm -f *.o spchk