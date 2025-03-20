
SOURCES = Main.cpp Window.cpp Collision.cpp Gameobject.cpp Gamescreen.cpp FileManager.cpp Serilization.cpp Camera.cpp 
SRC_DIR = scripts/

SRC_FILES = $(addprefix $(SRC_DIR), $(SOURCES))

all:
	g++ -I include -Iheaders -Iheaders/ImGUI -L lib -o main $(SRC_FILES) -limgui -lmingw32 -lcomdlg32 -lSDL2main -lSDL2 -lSDL2_image