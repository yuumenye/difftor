$(CC) := g++

all: difftor

difftor: main.o tree.o
	$(CC) $^ -o $@

main.o: main.cpp
	$(CC) main.cpp -c

tree.o: tree.cpp
	$(CC) tree.cpp -c

clean:
	rm -rf *.o difftor
