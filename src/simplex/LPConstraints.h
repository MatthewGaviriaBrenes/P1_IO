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


enum LPConstraintType {
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
    LPCoefficients **coeff_sets;          // Array of CoeffSet pointers for each constraint.
    enum LPConstraintType *types;     // Array of constraint types (LTE, EQ, GTE) for each constraint.
    double *rhs_values;             // Array of right-hand side (RHS) values for each constraint.

} LPConstraints;

// Create a new LPConstraints struct with the given number of constraints and decision variables.
// Ensures that constraints have the correct number of coefficients for the decision variables.
LPConstraints *lp_constraints_create(size_t count, size_t num_dec_vars);

// Free the memory allocated for the LPConstraints struct.
// Pointer to decision variables should be freed separately if it was dynamically allocated.
void lp_constraints_free(LPConstraints *constraintList);

// Add a new constraint to the LPConstraints struct. New constraint is added at the end of the list, and count is incremented.
// LPCoefficients struct is used to pass the coefficients for the new constraint. 
// Caller is responsible for ensuring that the coefficients array in the LPCoefficients struct is valid and has the correct length.
void lp_constraints_add_constraint(LPConstraints *constraintList, const LPCoefficients *coeffs, enum LPConstraintType type, double rhs_value);

// Update the coefficients, type, and RHS value of an existing constraint at the specified index.
// Can only be used to update an existing constraint. New constraints should be added using lp_constraints_add_constraint.
// Caller is responsible for ensuring that the coefficients array in the LPCoefficients struct is valid and has the correct length.
void lp_constraints_set_coeffs(LPConstraints *constraintList, size_t index, const LPCoefficients *newCoeffs, enum LPConstraintType newType, double newRHS);

// Get the coefficients for a specific constraint at the given index.
// Returns a CoefficientsSet struct containing the coefficients and the count of entries.
// Caller should free the coefficients array in the returned CoefficientsSet when done using it.
LPCoefficients lp_constraints_get_constraint_coeffs(const LPConstraints *constraintList, size_t index);

// Get the type of constraint at the specified index.
enum LPConstraintType lp_constraints_get_constraint_type(const LPConstraints *constraintList, size_t index);

// Get the right-hand side (RHS) value of the constraint at the specified index.
double lp_constraints_get_constraint_rhs(const LPConstraints *constraintList, size_t index);

// Print the details of a specific constraint at the given index for debugging purposes.
// Caller is responsible for passing a valid LPDecisionVars struct if they want to display variable names. 
// If LPDecisionVars struct is NULL, will print generic variable names.
void lp_constraints_print_constraint_definiton(const LPConstraints *constraintList, size_t index, const LPDecisionVars *decision_vars);

// Print all constraints in the LPConstraints struct for debugging purposes.
// DecisionVariable struct can be passed to display the coefficients alongside their corresponding variable names. If NULL, will print generic variable names.
void constrainst_print_all_constraints(const LPConstraints *constraintList, const LPDecisionVars *decision_vars);

#endif