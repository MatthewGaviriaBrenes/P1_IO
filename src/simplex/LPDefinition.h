// LPDefintion.h
// TODO: REWORK

#include "DecisionVars.h"
#include "Constraints.h"

#ifndef LPDEFINITION_H
#define LPDEFINITION_H

// LP problem type enumeration
enum LPOptimizationType {       
    LP_MAXIMIZATION,
    LP_MINIMIZATION
};

typedef struct {
    enum LPOptimizationType objective;   // Type of constraint (e.g., less than or equal)
    double *dec_vars_coeffs;     // Coefficients for the decision variables in the constraint
} ObjectiveFunction;

typedef struct {
    // Define the structure for LPDefinition here
    // For example, you might have fields for the objective function, constraints, etc.

    size_t num_decision_vars;   // Number of decision variables
    size_t num_constraints;     // Number of constraints
    size_t num_slack_vars;      // Number of slack variables (one per less-than-or-equal constraint)

    //TODO: Add more fields as needed for the LP definition, such as arrays for coefficients, bounds, etc.
    ObjectiveFunction *objective_function;      // The objective function

    

} LPModel;


LPModel *lp_model_create(enum LPOptimizationType objective, size_t)



#endif

