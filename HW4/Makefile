CC=g++
CFLAGS=-std=c++14 -O3
LINKS=-lSDL2 -lSDL2main -lGL -ldl -I/usr/include/SDL2/
FILE1=BlankScreenSDL
FILE2=Cube3D
FILE3=CubeLit
FILE4=CubeLit1VBO
FILE5=TriangleColored
FILE6=ModelLoad_pointlight
FILE7=ModelLoad_dirLight
FILE8=ModelLoad_dirLight_Gouraud
FILE9=ModelLoad
FILE10=ModelLoad_dirLight_blinnPhong

all: $(FILE1) $(FILE2) $(FILE3) $(FILE4) $(FILE5) $(FILE6) $(FILE7) $(FILE8) $(FILE9) $(FILE10)

$(FILE1): glad/glad.c $(FILE1).cpp
	$(CC) $(CFLAGS) glad/glad.c $(FILE1).cpp $(LINKS) -o $(FILE1)

$(FILE2): glad/glad.c $(FILE2).cpp
	$(CC) $(CFLAGS) glad/glad.c $(FILE2).cpp $(LINKS) -o $(FILE2)

$(FILE3): glad/glad.c $(FILE3).cpp
	$(CC) $(CFLAGS) glad/glad.c $(FILE3).cpp $(LINKS) -o $(FILE3)

$(FILE4): glad/glad.c $(FILE4).cpp
	$(CC) $(CFLAGS) glad/glad.c $(FILE4).cpp $(LINKS) -o $(FILE4)

$(FILE5): glad/glad.c $(FILE5).cpp
	$(CC) $(CFLAGS) glad/glad.c $(FILE5).cpp $(LINKS) -o $(FILE5)

$(FILE6): glad/glad.c $(FILE6).cpp
	$(CC) $(CFLAGS) glad/glad.c $(FILE6).cpp $(LINKS) -o $(FILE6)

$(FILE7): glad/glad.c $(FILE7).cpp
	$(CC) $(CFLAGS) glad/glad.c $(FILE7).cpp $(LINKS) -o $(FILE7)

$(FILE8): glad/glad.c $(FILE8).cpp
	$(CC) $(CFLAGS) glad/glad.c $(FILE8).cpp $(LINKS) -o $(FILE8)

$(FILE9): glad/glad.c $(FILE9).cpp
	$(CC) $(CFLAGS) glad/glad.c $(FILE9).cpp $(LINKS) -o $(FILE9)

$(FILE10): glad/glad.c $(FILE10).cpp
	$(CC) $(CFLAGS) glad/glad.c $(FILE10).cpp $(LINKS) -o $(FILE10)

clean:
	rm -f $(FILE1) $(FILE2) $(FILE3) $(FILE4) $(FILE5) $(FILE6) $(FILE7) $(FILE8) $(FILE9) $(FILE10)