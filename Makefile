CC = gcc
CCFLAGS = -ansi -Wall -Wextra -Os

BINARY = graphsim
OBJECTS = graph.o simulate.o parser.o main.o

all: $(BINARY)

$(BINARY): $(OBJECTS)
	@ $(CC) $(CCFLAGS) *.o -o $(BINARY) -lm -lpthread

%.o: %.c
	@ $(CC) $(CCFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@ rm -f *.o $(BINARY)
