INCLUDE_DIR = -I/usr/include/SDL2

all:
	cc $(INCLUDE_DIR) -o rollerTest rollerTest.c -lSDL2