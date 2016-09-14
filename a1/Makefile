CC=g++
FLAGS=-I. -Wall -g -O2 -std=c++11
FILES=main.o Model.o Point.o
TAR=a1.tar
all: $(FILES)
	$(CC) $(FLAGS) $(FILES) -o geonorm
main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp
Model.o: Model.cpp
	$(CC) $(FLAGS) -c Model.cpp
Point.o: Point.cpp
	$(CC) $(FLAGS) -c Point.cpp
clean:
	rm -f *.o $(TAR) geonorm *rounded.ply *centered.ply
tar:
	tar -cf $(TAR) *.cpp *.h README* Makefile
