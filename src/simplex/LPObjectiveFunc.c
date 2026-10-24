// LPObjectiveFunction.c

#include "LPObjectiveFunc.h"


// Create a new LPObjectiveFunc struct with the given optimization type, number of decision variables, and coefficients.
LPObjectiveFunc *lp_objectivefunc_create(enum ObjectiveFunctionType type, size_t num_dec_vars, LPCoefficients *coeffs) {
    if (type < MAXIMIZE || type > MINIMIZE) {
        printf("Error: Invalid objective function type specified.\n");
        return NULL;
    }
    if (num_dec_vars < MINIMUM_DECISION_VARS || num_dec_vars > MAXIMUM_DECISION_VARS) {
        printf("Error: Invalid number of decision variables specified. Must be between %zu and %zu.\n", MINIMUM_DECISION_VARS, MAXIMUM_DECISION_VARS);
        return NULL;
    }
    if (!coeffs) {
        printf("Error: Invalid coefficients specified.\n");
        return NULL;
    }
    if (coeffs->length != num_dec_vars) {
        printf("Error: LPCoefficients length does not match the number of decision variables for objective function.\n");
        return NULL;
    }
    LPObjectiveFunc *obj_func = (LPObjectiveFunc *)malloc(sizeof(LPObjectiveFunc));
    if (!obj_func) {
        printf("Error: Memory allocation failed for LPObjectiveFunc struct.\n");
        return NULL;
    }

    obj_func->coeffs = lp_coefficients_create(num_dec_vars);
    if (!obj_func->coeffs) {
        printf("Error: Memory allocation failed for coefficients in LPObjectiveFunc struct.\n");
        free(obj_func);
        return NULL;
    }
    obj_func->type = type;

    // Copy the coefficients
    for (size_t i = 0; i < num_dec_vars; i++) {
        obj_func->coeffs->coefficients[i] = coeffs->coefficients[i];
    }
    obj_func->num_dec_vars = num_dec_vars;
    return obj_func;
}

// Free the memory allocated for the LPObjectiveFunc struct and its components.
void lp_objectivefunc_free(LPObjectiveFunc *obj_func) {
    if (obj_func) {
        lp_coefficients_free(obj_func->coeffs); // Free the LPCoefficients struct
        free(obj_func); // Free the LPObjectiveFunc struct itself
    }
}

// Print the details of the objective function for debugging purposes.
void lp_objectivefunc_print(const LPObjectiveFunc *obj_func, const LPDecisionVars *decision_vars) {
    if (obj_func == NULL) {
        printf("Error: LPObjectiveFunc struct is NULL.\n");
        return;
    }
    if (decision_vars && decision_vars->length != obj_func->coeffs->length) {
        printf("Error: LPDecisionVars count does not match the number of coefficients in the objective function.\n");
        return;
    }
    printf("--- Objective Function ---\n");
    switch (obj_func->type) {
        case MAXIMIZE:
            printf("Maximize: \n");
            break;
        case MINIMIZE:
            printf("Minimize: \n");
            break;
        default:
            printf("Error: Unknown objective function type\n");
            return;
    }
    printf("Z = ");
    // Print the coefficients for the objective function
    lp_coefficients_print(obj_func->coeffs, decision_vars);
    printf("\n\n");
}