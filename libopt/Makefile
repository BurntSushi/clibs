CC=gcc
CFLAGS=-g -O3 -ansi -Wall -Wextra -pedantic -fPIC
HEADERS=opt.h
OBJS=opt.o parse.o
LDFLAGS=
LDLIBS=-I. -L. -lopt

libopt.so: $(OBJS)
	$(CC) -shared $(CFLAGS) $(OBJS) -o libopt.so

libopt.a: $(OBJS)
	ar rcs libopt.a $(OBJS)

opt.o: opt.c opt.h
	$(CC) $(CFLAGS) -c opt.c -o opt.o

parse.o: parse.c opt.h
	$(CC) $(CFLAGS) -c parse.c -o parse.o

examples: ex-contrived ex-simple

ex-%: libopt.so examples/%.c
	$(CC) $(LDFLAGS) -o ex-$* $(CFLAGS) examples/$*.c $(LDLIBS)

clean:
	rm -f ex-*
	rm -f libopt.{a,so}
	rm -f *.o

