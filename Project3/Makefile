# -*- MakeFile -*-

#target: dependencies
#	action
#g++ -fsanitize=address -std=c++14

ray: main.o image.o scene.o objects.o
	g++ -std=c++14 -O3 main.o image.o scene.o objects.o -o ray

main.o: main.cpp
	g++ -std=c++14 -O3 main.cpp -c

image.o: image.cpp image.h
	g++ -std=c++14 -O3 image.cpp image.h -c

scene.o: scene.cpp
	g++ -std=c++14 -O3 scene.cpp -c

objects.o: objects.cpp
	g++ -std=c++14 -O3 objects.cpp -c

clean:
	rm -f *.o image