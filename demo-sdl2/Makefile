OBJDIR = build

SDL2_DIR = /Library/Frameworks/SDL2.framework

CC := clang

CFLAGS :=  -ggdb3 -O0 --std=c99 -Wall -L. -lm -I$(SDL2_DIR)/Headers $(SDL2_DIR)/SDL2 -I../src

HDRS := 

# add source files here
SRCS := main.c reader.c ../src/gd.c

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
