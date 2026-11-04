
CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0)

TARGET = simplex_solver.exe
GUI_TARGET = simplex_gui

SRC = src/main.c \
      src/simplex/LPDecisionVars.c \
      src/simplex/LPCoefficients.c \
      src/simplex/LPObjectiveFunc.c \
      src/simplex/LPConstraints.c \
      src/simplex/LPDefinition.c \
      src/simplex/SPTableau.c \
      src/simplex/SPBaseSolution.c \
      src/simplex/SPMultiSolutions.c \
      src/tests/LPTestCases.c \
      src/tests/SPTest.c

SIMPLEX_SRC = \
      src/simplex/LPDecisionVars.c \
      src/simplex/LPCoefficients.c \
      src/simplex/LPObjectiveFunc.c \
      src/simplex/LPConstraints.c \
      src/simplex/LPDefinition.c \
      src/simplex/SPTableau.c \
      src/simplex/SPBaseSolution.c \
      src/simplex/SPMultiSolutions.c\
      src/tex/presentation.c

$(GUI_TARGET): src/main.c $(SIMPLEX_SRC)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -o $(GUI_TARGET) \
	    src/main.c $(SIMPLEX_SRC) $(GTK_LIBS) -lm

clean:
	rm -f $(TARGET) $(GUI_TARGET)

.PHONY: clean

