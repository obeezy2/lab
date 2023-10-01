CC = gcc
CFLAGS = -Wall -ansi -pedantic -g
PROG_NAME = a.out

SOURCES = $(wildcard assembler/*.c) \
	  data_structure/list.c \
	  data_structure/tree.c \
	  lexicalAnalysis/lexicalAnalysis.c \
	  preAssembly/preAssembler.c \
	  output/output.c \
	  main.c \
	  $(wildcard structs/*.c)

OBJECTS = $(SOURCES:.c=.o)

all: $(PROG_NAME)

$(PROG_NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(PROG_NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(PROG_NAME)
