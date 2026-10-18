// LPDecisionVars.h

#ifndef LPDECISIONVARS_H
#define LPDECISIONVARS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const size_t MINIMUM_DECISION_VARS; // Minimum length for a decision variable name
extern const size_t MAXIMUM_DECISION_VARS; // Maximum length for a decision variable name

typedef struct {
    char **names; // Array of decision variable names
    size_t length;    // Number of decision variables
} LPDecisionVars;

// Create a new LPDecisionVars struct with the given length.
LPDecisionVars* lp_decisionvars_create(size_t length);

// Free the memory allocated for the LPDecisionVars struct.
void lp_decisionvars_free(LPDecisionVars *vars);

// Set the name of a decision variable at the specified index.
void lp_decisionvars_set_var(LPDecisionVars *vars, size_t index, const char *name);

// Get the name of a decision variable at the specified index.
const char* lp_decisionvars_get_var(LPDecisionVars *vars, size_t index);

// Print names of decision variables for debugging purposes.
void lp_decisionvars_print(LPDecisionVars *vars);


#endif 