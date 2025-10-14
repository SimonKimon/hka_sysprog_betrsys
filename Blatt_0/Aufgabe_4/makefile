CC=gcc
CFLAGS=-I.
DEPS = Aufgabe_4.c
OBJ = Aufgabe_4.o

Aufgabe_4: $(OBJ)
	$(CC) $(OBJ) -o $@ $(CFLAGS)

Aufgabe_4.o: $(DEPS)
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o Aufgabe_4