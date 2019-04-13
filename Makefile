CFLAGS = -Wall -Wextra -Werror -Wpedantic --std=c99

all: pmult threadpmult forkpmult bigmp clean

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

bigmp: bigmp.o
	gcc -lgmp -lpthread -o bigmp $^
	
clean : 
	rm -f *.o

remove :
	rm -f pmult threadpmult forkpmult bigmp
