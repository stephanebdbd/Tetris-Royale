# Noms des exécutables
TARGET = serveur

# Compilateur
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -fsanitize=address

# Bibliothèques à lier
LIBS = -lncurses -pthread -fsanitize=address -lsqlite3 -lsodium

# Fichiers source
DATA_SRC =    $(wildcard data/*.cpp)
COMMON_SRC = $(wildcard ../common/*.cpp)
SERVEUR_SRC = $(filter-out main.cpp, $(wildcard *.cpp)) $(DATA_SRC) $(COMMON_SRC)

# Fichiers objets
DATA_OBJECTS = $(DATA_SRC:.cpp=.o)
SERVEUR_OBJECTS = $(SERVEUR_SRC:.cpp=.o)

#Sinon compile que target0
all: $(TARGET0) $(TARGET)

# Compilation de "serveur"
$(TARGET): $(SERVEUR_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SERVEUR_OBJECTS) $(LIBS)

# Règle générique pour compiler les fichiers .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des fichiers objets et exécutables
clean:
	rm -f $(TARGET) $(SERVEUR_OBJECTS) $(DATA_OBJECTS) data/*.o server.log
	rm -f *.o
	rm -f *.txt

# Exécuter "serveur" après compilation
run: $(TARGET)
	./$(TARGET)