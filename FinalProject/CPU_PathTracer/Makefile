# -*- MakeFile -*-

#target: dependencies
#	action
#g++ -fsanitize=address -std=c++14

ray: main.o image.o scene.o objects.o bvh.o
	g++ -fopenmp -std=c++14 -O3 main.o image.o scene.o objects.o bvh.o glad/glad.c -lSDL2 -lSDL2main -lGL -ldl -I/usr/include/SDL2/ -o ray -L oidn/lib/ -lOpenImageDenoise 

main.o: main.cpp
	g++ -fopenmp -std=c++14 -O3 main.cpp -c

image.o: image.cpp image.h
	g++ -fopenmp -std=c++14 -O3 image.cpp image.h -c

scene.o: scene.cpp
	g++ -fopenmp -std=c++14 -O3 scene.cpp -c

objects.o: objects.cpp
	g++ -fopenmp -std=c++14 -O3 objects.cpp -c

bvh.o: bvh.cpp
	g++ -fopenmp -std=c++14 -O3 bvh.cpp -c

clean:
	rm -f *.o ray