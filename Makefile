all:
	gcc.exe -Isrc/include -L./src/lib/ -o main.exe main.c -lSDL3
