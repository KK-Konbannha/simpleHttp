CC=gcc
LD=gcc
CFLAGS=-g -c -Wall -pedantic-errors -O0 -std=gnu11 -Iinclude
LDFLAGS=
SRC_DIR=src
INC_DIR=include
OBJ_DIR=obj
PUBLIC_DIR=public
OBJECTS=$(OBJ_DIR)/main.o $(OBJ_DIR)/exp1lib.o $(OBJ_DIR)/requestHandler.o $(OBJ_DIR)/sendStatus.o $(OBJ_DIR)/httpSession.o $(OBJ_DIR)/acceptLoop.o $(OBJ_DIR)/router.o $(OBJ_DIR)/handlers.o
EXECUTABLE=main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f $(EXECUTABLE) $(OBJ_DIR)/*.o tmp.txt core

# Create obj directory if it does not exist
$(shell mkdir -p $(OBJ_DIR))
