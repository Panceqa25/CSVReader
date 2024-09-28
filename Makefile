parametrosCompilacao=-Wall -g -lm #-Wshadow
nomePrograma=csvreader

all: $(nomePrograma)

$(nomePrograma): main.o io.o
	gcc -o $(nomePrograma) main.o io.o $(parametrosCompilacao)

main.o: main.c
	gcc -c main.c $(parametrosCompilacao)

io.o:  io.c io.h
	gcc -c io.c $(parametrosCompilacao)

clean:
	rm -f *.o *.gch

purge:
	rm -f *.o *.gch $(nomePrograma)
