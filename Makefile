
CC=gcc
CCFLAGS=-Wall -Werror -pedantic

all :
	$(CC) $(CCFLAGS) volatileRLMap.c -o volatileRLMap

clean :
	rm -f volatileRLMap

.PHONY: $(PHONY) all clean
