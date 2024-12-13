CXX=g++
CXXFLAGS=-g -Wall -Wextra -std=c++17 -O2
LIBRARIES=-lncurses
SOURCES=main.cpp Model/User.cpp Model/Game.cpp Model/Grid.cpp Model/Tetrimino.cpp Controller/Controller.cpp View/Board.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tetris

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBRARIES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)