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
    LPCoefficients *coeffs;      // LPCoefficients for the decision variables in the constraint
} LPObjectiveFunc;

// Create a new LPObjectiveFunc struct with the given optimization type, number of decision variables, and coefficients.
LPObjectiveFunc *lp_objectivefunc_create(enum ObjectiveFunctionType type, size_t num_dec_vars, LPCoefficients *coeffs);

// Free the memory allocated for the LPObjectiveFunc struct and its components.
void lp_objectivefunc_free(LPObjectiveFunc *obj_func);

// Print the details of the objective function for debugging purposes.
// DecisionVariable struct can be passed to display the coefficients alongside their corresponding variable names. If NULL, will print generic variable names.
void lp_objectivefunc_print(const LPObjectiveFunc *obj_func, const LPDecisionVars *decision_vars);


#endif