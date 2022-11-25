CC=gcc

.PHONY: all clean

all: task

task: main.c
	$(CC) -O2 $< -o $@

clean:
	rm task