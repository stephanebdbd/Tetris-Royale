CLIENT_DIR = ../src/client
SERVER_DIR = ../src/server

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -fsanitize=address

all: client server

client:
	$(MAKE) -C $(CLIENT_DIR) SFML_LIBS="-lsfml-graphics -lsfml-window -lsfml-system"

server:
	$(MAKE) -C $(SERVER_DIR)

clean:
	$(MAKE) -C $(CLIENT_DIR) clean
	$(MAKE) -C $(SERVER_DIR) clean
