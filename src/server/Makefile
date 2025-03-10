# Noms des exécutables
TARGET0 = tetris
TARGET1 = serveur

# Compilateur
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -fsanitize=address

# Bibliothèques à lier
LIBS = -lncurses -pthread -fsanitize=address

# Fichiers source
TETRIS_SRC = main.cpp Game.cpp Grid.cpp Tetramino.cpp Timer.cpp Score.cpp Color.cpp TetraminoDisplacement.cpp
#SERVEUR_SRC = $(filter-out main.cpp FriendList.cpp chat.cpp chatManager.cpp chatRoom.cpp, $(wildcard *.cpp))
SERVEUR_SRC = $(filter-out main.cpp, $(wildcard *.cpp))

# Fichiers objets
TETRIS_OBJECTS = $(TETRIS_SRC:.cpp=.o)
SERVEUR_OBJECTS = $(SERVEUR_SRC:.cpp=.o)

#Sinon compile que target0
all: $(TARGET0) $(TARGET1)

# Compilation de "tetris"
$(TARGET0): $(TETRIS_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET0) $(TETRIS_OBJECTS) $(LIBS)

# Compilation de "serveur"
$(TARGET1): $(SERVEUR_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET1) $(SERVEUR_OBJECTS) $(LIBS)

# Règle générique pour compiler les fichiers .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des fichiers objets et exécutables
clean:
	rm -f $(TARGET0) $(TETRIS_OBJECTS) $(TARGET1) $(SERVEUR_OBJECTS) server.log
	> users.txt

# Exécuter "serveur" après compilation
run: $(TARGET1)
	./$(TARGET1)