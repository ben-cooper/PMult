CFLAGS = -Wall -Wextra -Werror -Wpedantic --std=c99

all: pmult threadpmult forkpmult clean

debug: CFLAGS += -g
debug: all

fast: CFLAGS += -O3
fast: all

pmult: pmult.o
	gcc -o pmult $^

threadpmult: threadpmult.o
	gcc -lpthread -o threadpmult $^
	
forkpmult: forkpmult.o
	gcc -o forkpmult $^
	
clean : 
	rm -f *.o

remove :
	rm -f pmult threadpmult forkpmult
