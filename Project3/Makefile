# -*- MakeFile -*-

#target: dependencies
#	action

ray: main.o image.o scene.o
	g++ main.o image.o scene.o -o ray

main.o: main.cpp
	g++ main.cpp -c

image.o: image.cpp image.h
	g++ image.cpp image.h -c

scene.o: scene.cpp
	g++ scene.cpp -c

clean:
	rm -f *.o image