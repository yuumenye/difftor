$(CC) := g++

all: difftor

difftor: main.o tree.o draw.o difftor.o optimizer.o read.o
	$(CC) $^ -o $@ -lm

main.o: main.cpp
	$(CC) main.cpp -c

tree.o: tree.cpp
	$(CC) tree.cpp -c

draw.o: draw.cpp
	$(CC) draw.cpp -c

difftor.o: difftor.cpp
	$(CC) difftor.cpp -c

optimizer.o: optimizer.cpp
	$(CC) optimizer.cpp -c

read.o: read.cpp
	$(CC) read.cpp -c

clean:
	rm -rf *.o difftor
