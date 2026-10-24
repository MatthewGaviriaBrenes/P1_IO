CC = gcc
TARGET = simplex_solver.exe

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

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: clean