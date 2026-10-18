// LPDefintion.h

#ifndef LPDEFINITION_H
#define LPDEFINITION_H

#include "LPDecisionVars.h"
#include "LPObjectiveFunc.h"
#include "LPCoefficients.h"
#include "LPConstraints.h"

typedef struct {
    // Define the structure for LPDefinition here
    // For example, you might have fields for the objective function, constraints, etc.

    size_t num_decision_vars;   // Number of decision variables
    size_t num_constraints;     // Number of constraints
    
    LPDecisionVars *decision_variables;      // The decision variables
    LPObjectiveFunc *objective_function;      // The objective function
    LPConstraints *constraints;                   // The constraints of the LP problem
} LPDefinition;

// Create a new baseline LPDefinition (definition of Linear Problem) with the given number of decision variables and constraints. 
// This function initializes the LPDefinition struct with default values and allocates memory for its components.
// Objective function and constraints will be set to NULL initially, and should be set later using appropriate setter functions.
LPDefinition *lp_definition_create(
    LPDecisionVars *decision_vars, 
    LPObjectiveFunc *objective_func, 
    LPConstraints *constraints
);

// Free the memory allocated for the LPDefinition struct and its components.
// Caller is responsible for freeing the decision variables, objective function, and constraints separately if they were dynamically allocated.
void lp_definition_free(LPDefinition *lp_def);

// Print the details of the LPDefinition for debugging purposes.
void lp_definition_print(const LPDefinition *lp_def);



#endif

