CC = gcc
TARGET = simplex_solver.exe

SRC = src/main.c \
	  src/simplex/DecisionVars.c \
	  src/simplex/Coefficients.c \
	  src/simplex/ObjectiveFunction.c \
	  src/simplex/Constraints.c \
	  src/simplex/LPDefinition.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: clean