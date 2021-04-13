CC=g++
CFLAGS=-std=c++14
LINKS=-lSDL2 -lSDL2main -lGL -ldl -I/usr/include/SDL2/

app: application.o scene.o objects.o
	$(CC) $(CFLAGS) objects.o scene.o application.o glad/glad.c $(LINKS) -o app

application.o: application.cpp
	$(CC) $(CFLAGS) application.cpp -c

scene.o: scene.cpp
	$(CC) $(CFLAGS) scene.cpp scene.h -c

objects.o: objects.cpp
	$(CC) $(CFLAGS) objects.cpp objects.h -c

clean:
	rm -f *.o app