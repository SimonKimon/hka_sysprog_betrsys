CC=gcc
CFLAGS=-I.
DEPS = Aufgabe_3.c
OBJ = Aufgabe_3.o

Aufgabe_3: $(OBJ)
	$(CC) $(OBJ) -o $@ $(CFLAGS)

Aufgabe_3.o: $(DEPS)
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o Aufgabe_3