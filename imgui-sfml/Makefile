ifeq ($(OS),Windows_NT)
	RM := del /Q /F
	MKDIR := mkdir
	SLASH := \\
	SHELL := cmd.exe
else
	RM := rm -rf
	MKDIR := mkdir -p
	SLASH := /
	SHELL := /bin/bash
endif

CXX      := g++
AR       := ar
CXXFLAGS := -std=c++17 -O2 -march=native -s -I./include/

BIN_DIR := bin
SRC_DIR := src
IMGUI_SRC := imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_tables.cpp imgui-SFML.cpp

IMGUI_OBJ := $(patsubst %.cpp,$(BIN_DIR)$(SLASH)%.o,$(IMGUI_SRC))
STATIC_LIB := $(BIN_DIR)$(SLASH)libimgui-sfml.a

.PHONY: all clean demobuild

all: $(STATIC_LIB)

$(STATIC_LIB): $(IMGUI_OBJ)
	$(AR) rcs $@ $^

$(BIN_DIR)$(SLASH)%.o: $(SRC_DIR)$(SLASH)%.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I./include/

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

clean:
	$(RM) $(BIN_DIR)

demobuild:
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)$(SLASH)imgui_demo.cpp -o $(BIN_DIR)$(SLASH)imgui_demo.o 
	$(AR) rcs $(BIN_DIR)$(SLASH)libimgui-demo.a $(BIN_DIR)$(SLASH)imgui_demo.o
