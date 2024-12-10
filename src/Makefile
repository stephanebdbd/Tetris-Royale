CXX=g++
CXXFLAGS=-g -Wall -Wextra -std=c++17 -O2
ALLEGROLIBS=-lallegro -lallegro_main -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_image -lallegro_audio -lallegro_acodec
SOURCES=main.cpp Model/User.cpp Model/Game.cpp Model/Grid.cpp Model/Tetrimino.cpp Controller/Controller.cpp View/Board.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tetris

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(ALLEGROLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)