CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -fsanitize=address

OBJS = ClientChat.o Client.o ClientDisplay.o ClientNetwork.o Color.o Controller.o main.o SFMLGame.o Button.o Text.o

all: client

client: $(OBJS)
	$(CXX) $(CXXFLAGS) -o client $(OBJS) -lncurses $(SFML_LIBS) -fsanitize=address

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o client
