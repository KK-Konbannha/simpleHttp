SRC_DIR=src
INC_DIR=include
OBJ_DIR=obj
BIN_DIR=bin

CC=gcc
LD=gcc

CFLAGS=-c -Wall -pedantic-errors -O0 -g3 -std=gnu11 -I$(INC_DIR)
LDFLAGS=-pthread


OBJECTS = $(OBJ_DIR)/main.o $(OBJ_DIR)/lib.o \
					$(OBJ_DIR)/request_handler.o $(OBJ_DIR)/http_session.o \
					$(OBJ_DIR)/router.o $(OBJ_DIR)/handler.o \
					$(OBJ_DIR)/send_status.o  $(OBJ_DIR)/auth.o \
					$(OBJ_DIR)/accept_loop/default_loop.o \
					$(OBJ_DIR)/accept_loop/select_loop.o \
					$(OBJ_DIR)/accept_loop/thread_loop.o \
					$(OBJ_DIR)/accept_loop/process_loop.o \
					$(OBJ_DIR)/accept_loop/epoll_loop.o
EXECUTABLE=$(BIN_DIR)/main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/accept_loop/%.o: $(SRC_DIR)/accept_loop/%.c
	$(CC) $(CFLAGS) $< -o $@


clean:
	-rm -f $(EXECUTABLE) $(OBJECTS)

.PHONY: all clean

# Create obj and bin directory if it does not exist
$(shell mkdir -p $(BIN_DIR))
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(OBJ_DIR)/accept_loop)
