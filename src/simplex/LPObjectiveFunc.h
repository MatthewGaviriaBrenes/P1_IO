// LPObjectiveFunction.h

#ifndef LPOBJECTIVEFUNCTION_H
#define LPOBJECTIVEFUNCTION_H

#include "LPDecisionVars.h"
#include "LPCoefficients.h"

enum ObjectiveFunctionType {       
    MAXIMIZE,
    MINIMIZE
};

typedef struct {
    enum ObjectiveFunctionType type;      // Type of the objective function (maximization or minimization)
    size_t num_dec_vars;                // Number of decision variables in the objective function
    Coefficients *coeffs;      // Coefficients for the decision variables in the constraint
} ObjectiveFunction;

// Create a new ObjectiveFunction struct with the given optimization type, number of decision variables, and coefficients.
ObjectiveFunction *objective_function_create(enum ObjectiveFunctionType type, size_t num_dec_vars, Coefficients *coeffs);

// Free the memory allocated for the ObjectiveFunction struct and its components.
void objective_function_free(ObjectiveFunction *obj_func);

// Print the details of the objective function for debugging purposes.
// DecisionVariable struct can be passed to display the coefficients alongside their corresponding variable names. If NULL, will print generic variable names.
void objective_function_print(const ObjectiveFunction *obj_func, const DecisionVariables *decision_vars);


#endif