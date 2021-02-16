# -*- MakeFile -*-

#target: dependencies
#	action

image: main.o image.o pixel.o
	g++ main.o image.o pixel.o -o image

main.o: main.cpp
	g++ main.cpp -c

image.o: image.cpp
	g++ image.cpp -c

pixel.o: pixel.cpp
	g++ pixel.cpp -c

clean:
	rm -f *.o image