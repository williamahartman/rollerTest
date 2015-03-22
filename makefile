INCLUDE_DIR = -I/usr/include/SDL2
LINUX_ARGS = -lSDL2
WINDOWS_ARGS = -DWINDOWS -lmingw32 -lSDL2main -lSDL2 

ARGS =

ifeq ($(OS),Windows_NT)
    ARGS = $(WINDOWS_ARGS)
else
	ARGS = $(LINUX_ARGS)
endif

all:
	cc $(INCLUDE_DIR) -o rollerTest rollerTest.c $(ARGS)
