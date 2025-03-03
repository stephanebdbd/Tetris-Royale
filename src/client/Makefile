TARGET = client

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -fsanitize=address

LIBS = -lncurses -fsanitize=address

SRC = $(wildcard *.cpp)

OBJECTS = $(SRC:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)
	> messages.json

run: $(TARGET)
	./$(TARGET)