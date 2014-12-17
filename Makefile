CC=gcc
CFLAGS=-I. $(OPTFLAGS) -DMEMORY_SIZE=65536
LIBS=$(OPTLIBS)
SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

%.o: %.c $(SOURCES)
	$(CC) -c -o $@ $< $(CFLAGS)

altair8800: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)
