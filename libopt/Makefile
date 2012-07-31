CC=gcc
HEADERS=opt.h
OBJS=opt.o parse.o
CFLAGS=-g -O3 -ansi -Wall -Wextra -pedantic -fPIC -I.
LDFLAGS=-L.
LDLIBS=-lopt

libopt.so: $(OBJS) $(HEADERS)
	$(CC) -shared $(CFLAGS) $(OBJS) -o libopt.so

libopt.a: $(OBJS)
	ar rcs libopt.a $(OBJS)

opt.o: opt.c opt.h

parse.o: parse.c opt.h

examples: ex-contrived ex-simple

ex-contrived: libopt.so opt.h examples/contrived.o
	$(CC) $(LDFLAGS) examples/contrived.o $(LDLIBS) -o ex-contrived

ex-simple: libopt.so opt.h examples/simple.o
	$(CC) $(LDFLAGS) examples/simple.o $(LDLIBS) -o ex-simple

clean:
	rm -f ex-*
	rm -f libopt.{a,so}
	rm -f *.o examples/*.o

