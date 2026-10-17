// decisionVars.h
// Header file for decisionVars.c
// Data structure to manage decision variable names in a linear programming problem.

#ifndef DECISIONVARS_H
#define DECISIONVARS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const size_t MINIMUM_DECISION_VARS; // Minimum length for a decision variable name
extern const size_t MAXIMUM_DECISION_VARS; // Maximum length for a decision variable name

typedef struct {
    char **names; // Array of decision variable names
    size_t length;    // Number of decision variables
} DecisionVariables;

// Create a new DecisionVariables struct with the given length.
DecisionVariables* decision_vars_create(size_t length);

// Free the memory allocated for the DecisionVariables struct.
void decision_vars_free(DecisionVariables *vars);

// Set the name of a decision variable at the specified index.
void decision_vars_set_var(DecisionVariables *vars, size_t index, const char *name);

// Get the name of a decision variable at the specified index.
const char* decision_vars_get_var(DecisionVariables *vars, size_t index);

// Print names of decision variables for debugging purposes.
void decision_vars_print(DecisionVariables *vars);


#endif 