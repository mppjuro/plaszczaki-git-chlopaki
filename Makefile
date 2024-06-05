# Kompilator
CXX = g++

# flagi
CXXFLAGS = -Wall -Wextra -std=c++17
SDLFLAGS = -lSDL2 -lSDL2_ttf

# Sciezki
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
DATA_DIR = data
DATA_INPUT_DIR = $(DATA_DIR)/input
DATA_RESULTS_DIR = $(DATA_DIR)/results
DATA_JUNK_DIR = $(DATA_DIR)/junk


# Pliki zrodlowe
GENERATOR_SRCS = $(wildcard $(SRC_DIR)/generator/*.cpp)
PROBLEM1_SRCS = $(wildcard $(SRC_DIR)/problem1/*.cpp)
PROBLEM2_SRCS = $(wildcard $(SRC_DIR)/problem2/*.cpp)

# Pliki obiektow 
# w naszym przypadku troche useless, ale jakbysmy pisali kod bardziej modularnie to by sie przydalo,
# mozna wyjebac ale nie przeszkadza (wtedy trzeba pozmieniac reguly kompilacji)
GENERATOR_OBJS = $(GENERATOR_SRCS:$(SRC_DIR)/generator/%.cpp=$(BUILD_DIR)/generator/%.o)
PROBLEM1_OBJS = $(PROBLEM1_SRCS:$(SRC_DIR)/problem1/%.cpp=$(BUILD_DIR)/problem1/%.o)
PROBLEM2_OBJS = $(PROBLEM2_SRCS:$(SRC_DIR)/problem2/%.cpp=$(BUILD_DIR)/problem2/%.o)

# Nazwy binarek
GENERATOR_EXEC = $(BIN_DIR)/generator
PROBLEM1_EXEC = $(BIN_DIR)/problem1
PROBLEM2_EXEC = $(BIN_DIR)/problem2

# Domyslny target (make bez argumentow)
all: create_dirs $(GENERATOR_EXEC) $(PROBLEM1_EXEC) $(PROBLEM2_EXEC)

# Tworzenie folderow
create_dirs:
	@mkdir -p $(BUILD_DIR)/generator
	@mkdir -p $(BUILD_DIR)/problem1
	@mkdir -p $(BUILD_DIR)/problem2
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(DATA_INPUT_DIR)
	@mkdir -p $(DATA_RESULTS_DIR)
	@mkdir -p $(DATA_JUNK_DIR)

# Faktyczna kompilacja
$(GENERATOR_EXEC): $(GENERATOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(PROBLEM1_EXEC): $(PROBLEM1_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SDLFLAGS)

$(PROBLEM2_EXEC): $(PROBLEM2_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/generator/%.o: $(SRC_DIR)/generator/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< 

$(BUILD_DIR)/problem1/%.o: $(SRC_DIR)/problem1/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(SDLFLAGS)

$(BUILD_DIR)/problem2/%.o: $(SRC_DIR)/problem2/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Czyszczenie
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/*

# Oznaczenie targetow w ktorych nic sie nie tworzy
.PHONY: all clean
