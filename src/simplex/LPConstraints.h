// LPConstraints.h

#ifndef LPCONSTRAINTS_H
#define LPCONSTRAINTS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "LPCoefficients.h"
#include "LPDecisionVars.h"

extern const size_t MINIMUM_CONSTRAINTS; // Minimum number of constraints
extern const size_t MAXIMUM_CONSTRAINTS; // Maximum number of constraints


enum ConstraintType {
    ERROR = -1, // Error value for invalid constraint type or error in function execution
    LE = 0, // Less than or equal 
    EQ = 1,  // Equal (to be used in v2)
    GE = 2  // Greater than or equal (to be used in v2)
};

typedef struct {
    size_t count;                       // Count of constraints added.
    size_t max_count;                   // Maximum number of constraints.
    size_t num_dec_vars;          // Number of decision variables in each constraint.

    // Constraint definitions use multiple arrays to ensure indexing.
    Coefficients **coeff_sets;          // Array of CoeffSet pointers for each constraint.
    enum ConstraintType *types;     // Array of constraint types (LTE, EQ, GTE) for each constraint.
    double *rhs_values;             // Array of right-hand side (RHS) values for each constraint.

} Constraints;

// Create a new Constraints struct with the given number of constraints and decision variables.
// Ensures that constraints have the correct number of coefficients for the decision variables.
Constraints *constraints_create(size_t constraints_count, size_t num_dec_vars);

// Free the memory allocated for the Constraints struct.
// Pointer to decision variables should be freed separately if it was dynamically allocated.
void constraints_free(Constraints *constraintList);

// Add a new constraint to the Constraints struct. New constraint is added at the end of the list, and count is incremented.
// Coefficients struct is used to pass the coefficients for the new constraint. 
// Caller is responsible for ensuring that the coefficients array in the Coefficients struct is valid and has the correct length.
void constraints_add_constraint(Constraints *constraintList, const Coefficients *coeffs, enum ConstraintType type, double rhs_value);

// Update the coefficients, type, and RHS value of an existing constraint at the specified index.
// Can only be used to update an existing constraint. New constraints should be added using constraints_add_constraint.
// Caller is responsible for ensuring that the coefficients array in the Coefficients struct is valid and has the correct length.
void constraints_set_coeffs(Constraints *constraintList, size_t index, const Coefficients *newCoeffs, enum ConstraintType newType, double newRHS);

// Get the coefficients for a specific constraint at the given index.
// Returns a CoefficientsSet struct containing the coefficients and the count of entries.
// Caller should free the coefficients array in the returned CoefficientsSet when done using it.
Coefficients constraints_get_constraint_coeffs(const Constraints *constraintList, size_t index);

// Get the type of constraint at the specified index.
enum ConstraintType constraints_get_constraint_type(const Constraints *constraintList, size_t index);

// Get the right-hand side (RHS) value of the constraint at the specified index.
double constraints_get_constraint_rhs(const Constraints *constraintList, size_t index);

// Print the details of a specific constraint at the given index for debugging purposes.
// Caller is responsible for passing a valid DecisionVariables struct if they want to display variable names. 
// If DecisionVariables struct is NULL, will print generic variable names.
void constraints_print_constraint_definiton(const Constraints *constraintList, size_t index, const DecisionVariables *decision_vars);

// Print all constraints in the Constraints struct for debugging purposes.
// DecisionVariable struct can be passed to display the coefficients alongside their corresponding variable names. If NULL, will print generic variable names.
void constrainst_print_all_constraints(const Constraints *constraintList, const DecisionVariables *decision_vars);

#endif