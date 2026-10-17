// Constraints.h
// TODO: REWORK

#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include "DecisionVars.h"

extern const size_t MINIMUM_CONSTRAINTS; // Minimum number of constraints
extern const size_t MAXIMUM_CONSTRAINTS; // Maximum number of constraints


enum ConstraintType {
    ERROR = -1, // Error value for invalid constraint type or error in function execution
    LTE = 0, // Less than or equal 
    EQ = 1,  // Equal (to be used in v2)
    GTE = 2  // Greater than or equal (to be used in v2)
};

// Use this struct to store and retrieve coefficients for constraints.
// Added to prevent memory issues.
typedef struct {
    size_t length;                      // Count of constraints added.
    double *coefficients;                // Array of coefficients for the constraint.
} CoefficientsSet;


typedef struct {
    size_t count;                       // Count of constraints added.
    size_t max_count;                   // Maximum number of constraints.
    size_t num_decision_vars;          // Number of decision variables in each constraint.

    // Constraint definitions use multiple arrays to ensure indexing.
    double **coeffs;                // Pointer to 2D array of coefficients for each constraint.
    enum ConstraintType *types;     // Array of constraint types (LTE, EQ, GTE) for each constraint.
    double *rhs_values;             // Array of right-hand side (RHS) values for each constraint.
} Constraints;


// Free the memory allocated for the coefficients array and resets the length to 0.
void coefficients_set_free(CoefficientsSet *set);

// Create a new Constraints struct with the given number of constraints and decision variables.
// Ensures that constraints have the correct number of coefficients for the decision variables.
Constraints *constraints_create(size_t constraints_count, size_t num_decision_vars);

// Free the memory allocated for the Constraints struct.
// Pointer to decision variables should be freed separately if it was dynamically allocated.
void constraints_free(Constraints *constraintList);

// Add a new constraint to the Constraints struct. New constraint is added at the end of the list, and count is incremented.
void constraints_add_constraint(Constraints *constraintList, CoefficientSet coeffSet, enum ConstraintType type, double rhs_value);

// Set the coefficients for a specific constraint at the given index.
void constraints_set_coeffs(Constraints *constraintList, size_t index, CoefficientSet coeffSet, enum ConstraintType newType, double newRHS);

// Get the coefficients for a specific constraint at the given index.
// Returns a CoefficientsSet struct containing the coefficients and the count of entries.
// Caller should free the coefficients array in the returned CoefficientsSet when done using it.
CoefficientsSet constraints_get_constraint_coeffs(const Constraints *constraintList, size_t index);

// Get the type of constraint at the specified index.
enum ConstraintType constraints_get_constraint_type(const Constraints *constraintList, size_t index);

// Get the right-hand side (RHS) value of the constraint at the specified index.
double constraints_get_constraint_rhs(const Constraints *constraintList, size_t index);

// Print the details of a specific constraint at the given index for debugging purposes.
// DecisionVariable struct can be passed to display the coefficients alongside their corresponding variable names. If NULL, will print generic variable names.
void constraints_print_constraint_definiton(const Constraints *constraintList, size_t index, const DecisionVariables *decision_vars);

// Print all constraints in the Constraints struct for debugging purposes.
// DecisionVariable struct can be passed to display the coefficients alongside their corresponding variable names. If NULL, will print generic variable names.
void constrainst_print_all_constraints(const Constraints *constraintList, const DecisionVariables *decision_vars);

#endif