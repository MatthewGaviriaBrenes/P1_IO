// decisionVars.c

#include "DecisionVars.h"

const size_t MINIMUM_DECISION_VARS = 2;  // Minimum length for a decision variable name
const size_t MAXIMUM_DECISION_VARS = 25; // Maximum length for a decision variable name

// Create a new DecisionVariables struct with the given size.
DecisionVariables* decision_vars_create(size_t size) {
    if (size < MINIMUM_DECISION_VARS || size > MAXIMUM_DECISION_VARS) {
        printf("Error: Invalid number of decision variables specified. Must be between %zu and %zu.\n", MINIMUM_DECISION_VARS, MAXIMUM_DECISION_VARS);
        return NULL;
    }

    DecisionVariables *vars = (DecisionVariables *)malloc(sizeof(DecisionVariables));
    if (!vars) {
        return NULL;
    }

    vars->length = size;
    vars->names = (char **)malloc(size * sizeof(char *));
    if (!vars->names) {
        free(vars);
        return NULL;
    }
    for (size_t i = 0; i < size; i++) {
        vars->names[i] = NULL;  // Initialize names to NULL
    }
    return vars;
}

// Free the memory allocated for the DecisionVariables struct.
void decision_vars_free(DecisionVariables *vars) {
    if (vars) {
        for (size_t i = 0; i < vars->length; i++) {
            free(vars->names[i]); // Free each name
        }
        free(vars->names); // Free the array of names
        free(vars); // Free the struct itself
    }
}

// Set the name of a decision variable at the specified index.
void decision_vars_set_var(DecisionVariables *vars, size_t index, const char *name) {
    if (index >= 0 && index < vars->length) {
        free(vars->names[index]); // Free existing name if any
        vars->names[index] = strdup(name); // Duplicate the new name
    }
}

// Get the name of a decision variable at the specified index.
const char* decision_vars_get_var(DecisionVariables *vars, size_t index) {
    if (index >= 0 && index < vars->length) {
        return vars->names[index];
    }
    return NULL; // Return NULL if index is out of bounds
}

// Print names of decision variables for debugging purposes.
void decision_vars_print(DecisionVariables *vars) {
    printf("--- Decision Variables ---\n");
    for (int i = 0; i < vars->length; i++) {
        if (vars->names[i]) {
            printf("Variable %d: %s\n", i, vars->names[i]);
        } else {
            printf("Variable %d: (null)\n", i);
        }
    }
    printf("\n");
}