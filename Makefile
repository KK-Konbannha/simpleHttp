CC=gcc
C++=g++
LD=g++
CFLAGS=-g -c -Wall -pedantic-errors -O0 -std=gnu11 -I../
LDFLAGS=
OBJECTS=main.o exp1lib.o requestHandler.o sendStatus.o httpSession.o acceptLoop.o
EXECUTABLE=main

all: $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(C++) $(CFLAGS) $< -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f ${EXECUTABLE} *.o tmp.txt core

