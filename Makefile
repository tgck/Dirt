# Makefile for dirt on MacOSX

CC        = gcc
CFLAGS    = -g -Wall -O3 -std=gnu99 -DCHANNELS=2
LDFLAGS   = -lm -llo -lsndfile -lsamplerate -ljack
INCLUDES  = -I/opt/local/include
LIBS      = -L/opt/local/lib
DESTDIR   = /usr/local

all: dirt

clean:
	rm -f *.o *~ dirt dirt-analyse

dirt: dirt.o jack.o audio.o file.o server.o Makefile
	$(CC) dirt.o jack.o audio.o file.o server.o $(CFLAGS) $(INCLUDES) $(LIBS) $(LDFLAGS) -o dirt 

dirt-analyse: dirt.o jack.o audio.o file.o server.o pitch.o Makefile
	$(CC) dirt.o jack.o audio.o file.o server.o pitch.o $(CFLAGS) $(LDFLAGS) -o dirt-analyse

test : test.c Makefile
	$(CC) test.c -llo -o test

install: dirt
	install -d $(DESTDIR)/bin/
	install -m 0755 dirt $(DESTDIR)/bin/dirt


