INCLUDE_DIR = -I/usr/local/include/SDL2/

all:
	cc $(INCLUDE_DIR) -o rollerTest rollerTest.c -lSDL2