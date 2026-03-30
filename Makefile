C = gcc
C_FLAGS = -std=gnu2x -Wall -Werror -pedantic -ggdb -O0 -I$(INCLUDE_DIR)
C_EXT = c
OBJ_EXT = o
OBJ_DIR = obj
BIN_DIR = bin
SRC_DIR = src
INCLUDE_DIR = include
PROGRAM_DIR = programs
PROGRAM = cube
LINEAR_ALGEBRA = linalg
TRIGONOMETRY = trig
WINDOW = window
TERMINAL = terminal

all: setup $(PROGRAM)

setup:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all setup clean

$(PROGRAM): $(PROGRAM_DIR)/$(PROGRAM).$(C_EXT) \
			$(OBJ_DIR)/$(LINEAR_ALGEBRA).$(OBJ_EXT) \
			$(OBJ_DIR)/$(TRIGONOMETRY).$(OBJ_EXT) \
			$(OBJ_DIR)/$(WINDOW).$(OBJ_EXT) \
			$(OBJ_DIR)/$(TERMINAL).$(OBJ_EXT)
	$(C) $(C_FLAGS) $^ -lm -o $(BIN_DIR)/$@

$(OBJ_DIR)/$(LINEAR_ALGEBRA).$(OBJ_EXT): $(SRC_DIR)/$(LINEAR_ALGEBRA).$(C_EXT)
	$(C) $(C_FLAGS) $< -lm -c -o $@

$(OBJ_DIR)/$(TRIGONOMETRY).$(OBJ_EXT): $(SRC_DIR)/$(TRIGONOMETRY).$(C_EXT)
	$(C) $(C_FLAGS) $< -lm -c -o $@

$(OBJ_DIR)/$(WINDOW).$(OBJ_EXT): $(SRC_DIR)/$(WINDOW).$(C_EXT)
	$(C) $(C_FLAGS) $< -c -o $@

$(OBJ_DIR)/$(TERMINAL).$(OBJ_EXT): $(SRC_DIR)/$(TERMINAL).$(C_EXT)
	$(C) $(C_FLAGS) $< -c -o $@
