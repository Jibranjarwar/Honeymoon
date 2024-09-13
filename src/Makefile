
SOURCES = Main.cpp Window.cpp Gameobject.cpp
SRC_DIR = scripts/

SRC_FILES = $(addprefix $(SRC_DIR), $(SOURCES))

all:
	g++ -I include -L lib -o main $(SRC_FILES) -lmingw32 -lSDL2main -lSDL2