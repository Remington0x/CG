all: build


build: a.cpp
	g++ a.cpp -o a.out -lGLEW -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm