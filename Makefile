CC = gcc
TARGET = simplex_solver.exe

SRC = src/main.c \
	  src/simplex/LPDecisionVars.c \
	  src/simplex/LPCoefficients.c \
	  src/simplex/LPObjectiveFunc.c \
	  src/simplex/LPConstraints.c \
	  src/simplex/LPDefinition.c \
	  src/simplex/SPTableau.c \
	  src/simplex/SPBFS.c \
	  src/tests/LPTestCases.c 

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: clean