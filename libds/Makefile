CC=gcc
CFLAGS=-g -O3 -ansi -Wall -Wextra -pedantic -fPIC -lpthread
HEADERS=hashmap.h linkedlist.h queue.h vector.h
OBJS=hashmap.o linkedlist.o queue.o vector.o
LDFLAGS=-I. -L. -lds -lpthread

libds.so: $(OBJS)
	$(CC) -shared $(CFLAGS) $(OBJS) -o libds.so

examples: ex-hashmaps ex-vectors ex-lists ex-queue

libds.a: $(OBJS)
	ar rcs libds.a $(OBJS)

ds.h: $(HEADERS)
	cat $(HEADERS) > ds.h

hashmap.o: hashmap.c hashmap.h vector.h
	$(CC) $(CFLAGS) -c hashmap.c -o hashmap.o

linkedlist.o: linkedlist.c linkedlist.h
	$(CC) $(CFLAGS) -c linkedlist.c -o linkedlist.o

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c -o queue.o

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) -c vector.c -o vector.o

ex-%: libds.so examples/%.c
	$(CC) $(CFLAGS) examples/$*.c $(LDFLAGS) -o ex-$*

clean:
	rm -f ex-{hashmaps,vectors,lists,queue}
	rm -f libds.{a,so}
	rm -f *.o

