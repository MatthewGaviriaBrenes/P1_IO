// LPDefintion.h
// TODO: REWORK

#ifndef LPDEFINITION_H
#define LPDEFINITION_H

// LP problem type enumeration
enum LPOptimizationType {       
    LP_MAXIMIZATION,
    LP_MINIMIZATION
};

typedef struct {
    enum LPOptimizationType objective;   // Type of constraint (e.g., less than or equal)
    size_t num_dec_vars;           // Number of decision variables in the objective function
    double *dec_vars_coeffs;     // Coefficients for the decision variables in the constraint
} ObjectiveFunction;

typedef struct {
    // Define the structure for LPDefinition here
    // For example, you might have fields for the objective function, constraints, etc.

    size_t num_decision_vars;   // Number of decision variables
    size_t num_constraints;     // Number of constraints
    size_t num_slack_vars;      // Number of slack variables (one per less-than-or-equal constraint)

    //TODO: Add more fields as needed for the LP definition, such as arrays for coefficients, bounds, etc.
    DecisionVariables *decision_variables;      // The decision variables
    ObjectiveFunction *objective_function;      // The objective function
    Constraints *constraints;                    // The constraints of the LP problem
    

} LPDefinition;

// Create a new baseline LPDefinition (definition of Linear Problem) with the given number of decision variables and constraints. 
// This function initializes the LPDefinition struct with default values and allocates memory for its components.
// Objective function and constraints will be set to NULL initially, and should be set later using appropriate setter functions.
LPDefinition *lp_definition_create(enum LPOptimizationType objective_type, size_t num_decision_vars, size_t num_constraints);



#endif

