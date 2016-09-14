CC=g++
FLAGS=-I. -Wall -g -O2
FILES=main.o
TAR=Test.tar
all: $(FILES)
	$(CC) $(FLAGS) $(FILES) -o a1
main.o: main.cpp Pixel.t
	$(CC) $(FLAGS) -c main.cpp
clean:
	rm -f *.o $(TAR) test
tar:
	tar -cf $(TAR) *.cpp *.h *.t Makefile