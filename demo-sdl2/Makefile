OBJDIR = build

SDL2_DIR = /Library/Frameworks/SDL2.framework

CC := clang

# todo separate/abstract path list
CFLAGS :=  -ggdb3 -O0 --std=c99 -Wall -I$(SDL2_DIR)/Headers $(SDL2_DIR)/SDL2 -I../src -lmain -L../build/src

HDRS := 

# add source files here
SRCS := main.c reader.c

INCLUDES = 

# generate names of object files
OBJS := $(SRCS:.c=.o)

# name of executable
EXEC := main

# default recipe
all: $(EXEC)
 
$(EXEC): $(OBJS) $(HDRS) Makefile
	$(CC) -v -o $@ $(OBJS) $(CFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean
