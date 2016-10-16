CC=g++
FLAGS=-I. -Wall -g -O2 -std=c++11
FILES=main.o Model.o Point.o Camera.o Scene.o Linear.o
TAR=a3.tar
EXE=raytracer
all: $(FILES)
	$(CC) $(FLAGS) $(FILES) -o $(EXE)
main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp
Model.o: Model.cpp
	$(CC) $(FLAGS) -c Model.cpp
Point.o: Point.cpp
	$(CC) $(FLAGS) -c Point.cpp
Camera.o: Camera.cpp
	$(CC) $(FLAGS) -c Camera.cpp
Scene.o: Scene.cpp
	$(CC) $(FLAGS) -c Scene.cpp
Linear.o: Linear.cpp
	$(CC) $(FLAGS) -c Linear.cpp
clean:
	rm -f *.o $(TAR) $(EXE) *rounded.ply *centered.ply *.ppm
tar:
	tar -cf $(TAR) *.cpp *.h README* Makefile
