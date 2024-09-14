
SOURCES = Main.cpp Window.cpp Gameobject.cpp
SRC_DIR = scripts/
IMGUI_SOURCES = $(wildcard scripts/ImGUI/*.cpp)

SRC_FILES = $(addprefix $(SRC_DIR), $(SOURCES)) $(IMGUI_SOURCES)

all:
	g++ -I include -Iheaders -Iheaders/ImGUI -L lib -o main $(SRC_FILES) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image