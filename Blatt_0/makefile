CC=gcc
CFLAGS=-I.
DEPS = Aufgabe_3.c
OBJ = Aufgabe_3.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Aufgabe_3: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
