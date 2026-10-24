// LPCoefficients.c

#include "LPCoefficients.h"

// Create a new CoefficientsSet struct with the given length and allocate memory for the coefficients array.
// Initializes the coefficients array to zero.
LPCoefficients *lp_coefficients_create(size_t length) {
    LPCoefficients *set = (LPCoefficients *)malloc(sizeof(LPCoefficients));
    if (!set) {
        return NULL;
    }

    if (length < MINIMUM_DECISION_VARS || length > MAXIMUM_DECISION_VARS) {
        printf("Error: Invalid number of coefficients specified. Must be between %zu and %zu.\n", MINIMUM_DECISION_VARS, MAXIMUM_DECISION_VARS);
        free(set);
        return NULL;
    }
    
    set = (LPCoefficients *)malloc(sizeof(LPCoefficients));
    if (!set) {
        return NULL;
    }

    set->length = length;
    set->coefficients = (double *)calloc(length, sizeof(double)); // Initialize coefficients to zero
    if (!set->coefficients) {
        free(set);
        return NULL;
    }

    return set;
}

// Free the memory allocated for the coefficients array and resets the length to 0.
// Use to delete the coefficient set when it is no longer needed to prevent memory leaks.
void lp_coefficients_free(LPCoefficients *set) {
    if (set) {
        free(set->coefficients);
        free(set);
    }
}

// Create a new LPCoefficients struct with the given length and copy the provided coefficients into the new struct.
// Caller is responsible for freeing the coefficients array in the returned CoefficientsSet when done using it.
LPCoefficients *lp_coefficients_create_from_array(size_t length, const double *coefficients) {
    if (coefficients == NULL) {
        printf("Error: LPCoefficients array is NULL.\n");
        return NULL;
    }
    if (length < MINIMUM_DECISION_VARS || length > MAXIMUM_DECISION_VARS) {
        printf("Error: Invalid number of coefficients specified. Must be between %zu and %zu.\n", MINIMUM_DECISION_VARS, MAXIMUM_DECISION_VARS);
        return NULL;
    }

    LPCoefficients *set = (LPCoefficients *)malloc(sizeof(LPCoefficients));
    if (!set) {
        printf("Error: Memory allocation failed for LPCoefficients struct.\n");
        return NULL;
    }
    set->length = length;
    set->coefficients = (double *)malloc(length * sizeof(double));
    
    if (!set->coefficients) {
        printf("Error: Memory allocation failed for array of coefficients for LPCoefficients struct.\n");
        free(set);
        return NULL;
    }

    // Copy the provided coefficients into the new struct
    memcpy(set->coefficients, coefficients, length * sizeof(double));

    return set;
}

// Print the coefficients in the LPCoefficients struct for debugging purposes.
// DecisionVariable struct can be passed to display the coefficients alongside their corresponding variable names. If NULL, will print generic variable names.
// Does not add line break. Caller should print a line break afterwards if necessary.
void lp_coefficients_print(const LPCoefficients *set, const LPDecisionVars *decision_vars) {
    if (set == NULL) {
        printf("Error: LPCoefficients struct is NULL.\n");
        return;
    }
    if (decision_vars && decision_vars->length != set->length) {
        printf("Error: LPDecisionVars count does not match the number of coefficients in the LPCoefficients struct.\n");
        return;
    }

    bool firstNonZeroCoeffPrinted = false; // Flag to track if the first non-zero coefficient has been printed for formatting purposes.

    for (size_t i = 0; i < set->length; i++) {
        if (firstNonZeroCoeffPrinted) {
            if (set->coefficients[i] >= 0) {
                printf("+ %.2f", set->coefficients[i]);
            } else {
                printf("- %.2f", -set->coefficients[i]);
            }
        } else {
            if (set->coefficients[i] != 0) {
                firstNonZeroCoeffPrinted = true;
                printf("%.2f", set->coefficients[i]);
            } else {
                continue;   // Do not print zero coefficients if a non-zero coefficient has not been printed yet.
            }
        }
        // Add the decision variable name from the LPDecisionVars struct if provided, otherwise use a generic variable name.
        if (decision_vars && decision_vars->names[i]) {
            printf(" * %s ", decision_vars->names[i]);
        } else {
            printf(" * x%zu ", i + 1); // Generic variable name if LPDecisionVars struct is NULL or name is not set.
        }
    }
}