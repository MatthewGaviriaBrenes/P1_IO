// LPObjectiveFunction.c

#include "LPObjectiveFunc.h"


// Create a new ObjectiveFunction struct with the given optimization type, number of decision variables, and coefficients.
ObjectiveFunction *objective_function_create(enum ObjectiveFunctionType type, size_t num_dec_vars, Coefficients *coeffs) {
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
        printf("Error: Coefficients length does not match the number of decision variables for objective function.\n");
        return NULL;
    }
    ObjectiveFunction *obj_func = (ObjectiveFunction *)malloc(sizeof(ObjectiveFunction));
    if (!obj_func) {
        printf("Error: Memory allocation failed for ObjectiveFunction struct.\n");
        return NULL;
    }

    obj_func->coeffs = coefficients_create(num_dec_vars);
    if (!obj_func->coeffs) {
        printf("Error: Memory allocation failed for coefficients in ObjectiveFunction struct.\n");
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

// Free the memory allocated for the ObjectiveFunction struct and its components.
void objective_function_free(ObjectiveFunction *obj_func) {
    if (obj_func) {
        coefficients_free(obj_func->coeffs); // Free the Coefficients struct
        free(obj_func); // Free the ObjectiveFunction struct itself
    }
}

// Print the details of the objective function for debugging purposes.
void objective_function_print(const ObjectiveFunction *obj_func, const DecisionVariables *decision_vars) {
    if (obj_func == NULL) {
        printf("Error: ObjectiveFunction struct is NULL.\n");
        return;
    }
    if (decision_vars && decision_vars->length != obj_func->coeffs->length) {
        printf("Error: DecisionVariables count does not match the number of coefficients in the objective function.\n");
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
    coefficients_print(obj_func->coeffs, decision_vars);
    printf("\n\n");
}