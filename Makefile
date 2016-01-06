CC= gcc
CFLAGS= -O3 -ggdb -rdynamic
LDFLAGS=
CPPFLAGS=

SOURCES= main.c data.c dump.c int.c list.c
HEADERS= data.h dump.h int.h list.h
OUTPUT= main

$(OUTPUT): $(SOURCES) $(HEADERS)
	$(CC) -o $(OUTPUT) $(CPPFLAGS) $(CFLAGS) $(SOURCES) $(LDFLAGS)
