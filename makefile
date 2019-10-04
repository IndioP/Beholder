#inspirado num tutorial do stackoverflow
#https://stackoverflow.com/questions/30573481/path-include-and-src-directory-makefile
CXX = g++
EXE = saida.exe

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS += -std=c++11
CFLAGS += -Wall
LDFLAGS += -Llib
LDLIBS += `pkg-config --cflags --libs opencv`

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)
