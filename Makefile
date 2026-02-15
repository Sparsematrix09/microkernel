CC=gcc
CFLAGS=-Wall -Wextra

all:
	$(CC) main.c memory/allocator.c threading/thread.c -o kernel

clean:
	rm -f kernel