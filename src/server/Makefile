# Noms des exécutables
TARGET = serveur

# Compilateur
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -fsanitize=address

# Bibliothèques à lier
LIBS = -lncurses -pthread -fsanitize=address -lsqlite3 -lsodium

# Fichiers source
DATA_SRC =    $(wildcard data/*.cpp)
SERVEUR_SRC =  $(filter-out main.cpp, $(wildcard *.cpp)) $(DATA_SRC)

# Fichiers objets
DATA_OBJECTS = $(DATA_SRC:.cpp=.o)
SERVEUR_OBJECTS = $(SERVEUR_SRC:.cpp=.o)

# Cibles principales
all: $(TARGET)

# Compilation de "serveur"
$(TARGET): $(SERVEUR_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SERVEUR_OBJECTS) $(LIBS)

# Règle générique pour compiler les fichiers .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Règle spécifique pour compiler les fichiers dans data/
data/%.o: data/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des fichiers objets et exécutables
clean:
	rm -f $(TARGET) $(SERVEUR_OBJECTS) $(DATA_OBJECTS) data/*.o server.log
# Supprimer la base de données
	rm -f *.db

# Exécuter "serveur" après compilation
run: $(TARGET)
	./$(TARGET)