# Noms des exécutables
TARGET0 = tetris
TARGET1 = serveur

# Compilateur
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -fsanitize=address

# Bibliothèques à lier
LIBS = -lncurses -pthread -fsanitize=address -lsqlite3 -lsodium

# Fichiers source
TETRIS_SRC = main.cpp Game.cpp Grid.cpp Tetramino.cpp Timer.cpp Score.cpp Color.cpp TetraminoDisplacement.cpp
DATA_SRC = data/DataManager.cpp data/database.cpp data/query_result.cpp data/security.cpp data/chatRoom.cpp data/chat.cpp
SERVEUR_SRC = $(filter-out main.cpp, $(wildcard *.cpp)) $(DATA_SRC)

# Fichiers objets
TETRIS_OBJECTS = $(TETRIS_SRC:.cpp=.o)
DATA_OBJECTS = $(DATA_SRC:.cpp=.o)
SERVEUR_OBJECTS = $(SERVEUR_SRC:.cpp=.o)

# Cibles principales
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

# Règle spécifique pour compiler les fichiers dans data/
data/%.o: data/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des fichiers objets et exécutables
clean:
	rm -f $(TARGET0) $(TETRIS_OBJECTS) $(TARGET1) $(SERVEUR_OBJECTS) $(DATA_OBJECTS) data/*.o server.log 

# Exécuter "serveur" après compilation
run: $(TARGET1)
	./$(TARGET1)
