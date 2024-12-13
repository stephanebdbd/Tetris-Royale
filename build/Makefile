CXX=g++
CXXFLAGS=-g -Wall -Wextra -std=c++17 -O2
LIBRARIES=-lncurses
EXECUTABLE=tetris

MODEL_SOURCES=$(wildcard ../src/Model/*.cpp)
VIEW_SOURCES=$(wildcard ../src/View/*.cpp)
CONTROLLER_SOURCES=$(wildcard ../src/Controller/*.cpp)
SOURCES=../src/main.cpp $(MODEL_SOURCES) $(VIEW_SOURCES) $(CONTROLLER_SOURCES)

MODEL_OBJECTS=$(MODEL_SOURCES:../src/Model/%.cpp=Model_%.o)
VIEW_OBJECTS=$(VIEW_SOURCES:../src/View/%.cpp=View_%.o)
CONTROLLER_OBJECTS=$(CONTROLLER_SOURCES:../src/Controller/%.cpp=Controller_%.o)
OBJECTS=$(MODEL_OBJECTS) $(VIEW_OBJECTS) $(CONTROLLER_OBJECTS) main.o

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBRARIES)

main.o: ../src/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

Model_%.o: ../src/Model/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

View_%.o: ../src/View/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

Controller_%.o: ../src/Controller/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXECUTABLE)