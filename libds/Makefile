CC=gcc
HEADERS=hashmap.h linkedlist.h queue.h vector.h
OBJS=hashmap.o linkedlist.o queue.o vector.o
CFLAGS=-g -O3 -ansi -Wall -Wextra -pedantic -fPIC -lpthread -I.
LDFLAGS=-L.
LDLIBS=-lds -lpthread

libds.so: $(OBJS)
	$(CC) -shared $(CFLAGS) $(OBJS) -o libds.so

libds.a: $(OBJS)
	ar rcs libds.a $(OBJS)

ds.h: $(HEADERS)
	cat $(HEADERS) | sed 's/#include "vector.h"//' > ds.h

hashmap.o: hashmap.c hashmap.h vector.h

linkedlist.o: linkedlist.c linkedlist.h

queue.o: queue.c queue.h

vector.o: vector.c vector.h

examples: ex-hashmaps ex-vectors ex-lists ex-queue

ex-hashmaps: libds.so ds.h examples/hashmaps.o
	$(CC) $(LDFLAGS) examples/hashmaps.o $(LDLIBS) -o ex-hashmaps

ex-vectors: libds.so ds.h examples/vectors.o
	$(CC) $(LDFLAGS) examples/vectors.o $(LDLIBS) -o ex-vectors

ex-lists: libds.so ds.h examples/lists.o
	$(CC) $(LDFLAGS) examples/lists.o $(LDLIBS) -o ex-lists

ex-queue: libds.so ds.h examples/queue.o
	$(CC) $(LDFLAGS) examples/queue.o $(LDLIBS) -o ex-queue

clean:
	rm -f ex-{hashmaps,vectors,lists,queue}
	rm -f libds.{a,so}
	rm -f *.o examples/*.o

