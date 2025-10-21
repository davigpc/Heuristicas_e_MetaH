# Nome do executável final
TARGET_NAME = trabalho1
TARGET = bin/$(TARGET_NAME)

# Compilador e flags
CXX = g++
# -Iinclude tells the compiler to look for headers in the include/ directory
# -O3 is optimization level 3
CXXFLAGS = -std=c++11 -Wall -O3 -Iinclude

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Arquivos fonte (.cpp files in src/)
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Arquivos objeto (.o files will be placed in obj/)
# Replaces src/%.cpp with obj/%.o for each source file
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Regra principal: criar o executável
all: $(TARGET)

# Regra para linkar os objetos e criar o executável na pasta bin/
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR) # Create bin/ if it doesn't exist
	$(CXX) $(OBJECTS) -o $(TARGET)
	@echo "Executável $(TARGET) criado com sucesso."

# Regra para compilar cada .cpp em um .o na pasta obj/
# $< is the source file (.cpp)
# $@ is the target file (.o)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR) # Create obj/ if it doesn't exist
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para limpar os arquivos gerados
clean:
	@echo "Limpando arquivos gerados..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) # Remove obj/ and bin/ directories

.PHONY: all clean