// LPDefintion.c

#include "LPDefinition.h"

// Create a new baseline LPDefinition (definition of Linear Problem) with the given number of decision variables and constraints. 
// This function initializes the LPDefinition struct with default values and allocates memory for its components.
// Objective function and constraints will be set to NULL initially, and should be set later using appropriate setter functions.
LPDefinition *lp_definition_create(
    LPDecisionVars *decision_vars, 
    LPObjectiveFunc *objective_func, 
    LPConstraints *constraints
) {
    if (decision_vars == NULL) {
        printf("Error: LPDecisionVars struct is NULL.\n");
        return NULL;
    }
    if (objective_func == NULL) {
        printf("Error: LPObjectiveFunc struct is NULL.\n");
        return NULL;
    }
    if (constraints == NULL) {
        printf("Error: LPConstraints struct is NULL.\n");
        return NULL;
    }

    // Number of decision variables and constraints must match across the LPDefinition components.
    if (decision_vars->length != objective_func->num_dec_vars || 
        decision_vars->length != constraints->num_dec_vars) {
        printf("Error: Number of decision variables used for objective function and/or constraints does not match.\n");
        printf("Expected: %zu, Objective Function: %zu, LPConstraints: %zu\n", 
            decision_vars->length, objective_func->num_dec_vars, constraints->num_dec_vars);
        return NULL;
    }

    LPDefinition *lp_def = (LPDefinition *)malloc(sizeof(LPDefinition));
    if (!lp_def) {
        printf("Error: Memory allocation failed for LPDefinition struct.\n");
        return NULL;
    }

    lp_def->num_decision_vars = decision_vars->length;
    lp_def->num_constraints = constraints->count;

    lp_def->decision_variables = decision_vars;
    lp_def->objective_function = objective_func;
    lp_def->constraints = constraints;

    return lp_def;
}

// Free the memory allocated for the LPDefinition struct and its components.
// Caller is responsible for freeing the decision variables, objective function, and constraints separately if they were dynamically allocated.
void lp_definition_free(LPDefinition *lp_def) {
    if (lp_def) {
        free(lp_def);
    }
}

// Print the details of the LPDefinition for debugging purposes.
void lp_definition_print(const LPDefinition *lp_def) {
    if (lp_def == NULL) {
        printf("Error: LPDefinition struct is NULL.\n");
        return;
    }

    printf("--- LP Definition ---\n");
    printf("Number of Decision Variables: %zu\n", lp_def->num_decision_vars);
    printf("Number of LPConstraints: %zu\n", lp_def->num_constraints);

    // Print decision variables
    if (lp_def->decision_variables) {
        lp_decisionvars_print(lp_def->decision_variables);
    } else {
        printf("Warning: LPDecisionVars struct is NULL.\n");
    }

    // Print objective function
    if (lp_def->objective_function) {
        lp_objectivefunc_print(lp_def->objective_function, lp_def->decision_variables);
    } else {
        printf("Warning: LPObjectiveFunc struct is NULL.\n");
    }

    // Print constraints
    if (lp_def->constraints) {
        constrainst_print_all_constraints(lp_def->constraints, lp_def->decision_variables);
    } else {
        printf("Warning: LPConstraints struct is NULL.\n");
    }
}