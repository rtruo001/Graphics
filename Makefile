## CS130 Fall 2013: Computer Graphics
## Lab 1 Makefile
##
## This file does not need to be modified
#########################################

LIBS = -lglut -lGLU -lGL -lXmu -lXext -lXi -lX11 -lm
CC = g++

## FOR MACS
MAC_COMMANDS = -framework OpenGL -framework GLUT

## Global header files
INCLUDE =

## Object files and executables
MAIN_OUT = a.out

## Requirements for each command
MAIN_REQS = main.cpp

## Targets to compile for each command
MAIN_TARGETS = main.cpp

all: main

mac: main_macs

## Main 
main: $(MAIN_REQS)
	$(CC) $(MAIN_TARGETS) $(LIBS) -o $(MAIN_OUT)

main_macs: $(MAIN_REQS)
	$(CC) $(MAIN_TARGETS) $(MAC_COMMANDS) -o $(MAIN_OUT)

clean:
	rm -f *~ *.o *.out
