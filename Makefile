EXECUTABLES = myls

CC = gcc
LIBS = libfdr.a
CFLAGS = -O0 -g -w -std=c99

LIBFDROBJS = dllist.o jval.o

all: $(EXECUTABLES)

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c -w $*.c

myls: main.c header.h libfdr.a
	$(CC) $(CFLAGS) -o myls main.c $(LIBS) -lpthread -lm

libfdr.a: $(LIBFDROBJS)
	ar ru libfdr.a $(LIBFDROBJS)
	ranlib libfdr.a

clean:
	rm -f core *.o $(EXECUTABLES)
