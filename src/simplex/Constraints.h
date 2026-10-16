// Constraints.h
// TODO: REWORK

#include "DecisionVars.h"

#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

// NOTE: May need to remove this for first version.
enum ConstraintType {
    LTE // Less than or equal
    //,EQ // Equal
    //,GTE  // Greater than or equal
};

typedef struct {
    enum ConstraintType type;   // Type of constraint (e.g., less than or equal)
    double *dec_vars_coeffs;     // Coefficients for the decision variables in the constraint
    double rhs;                 // Right-hand side value of the constraint
} Constraint;

typedef struct {
    size_t count;               // Count of constraints added.
    size_t max_count;           // Maximum number of constraints.
    size_t max_decision_vars;   // Number of decision variables.
    Constraint *constraints;    // Array of constraints.
} Constraints;



// Create a new Constraints struct with the given number of constraints and decision variables.
// Ensures that constraints have the correct number of coefficients for the decision variables.
Constraints *constraints_create(size_t max_constraints, size_t max_decision_vars);

// Free the memory allocated for the Constraints struct.
void constraints_free(Constraints *constraintList);

// Add a new constraint to the Constraints struct.
void constraints_add_constraint(Constraints *constraintList, const double *dec_vars_coeffs, enum ConstraintType type, double rhs);

// Set a constraint at the specified index in the Constraints struct.
void constraints_set_constraint(Constraints *constraintList, size_t index, const double *dec_vars_coeffs, enum ConstraintType type, double rhs);

// Get a constraint at the specified index in the Constraints struct.
Constraint constraints_get_constraint(const Constraints *constraintList, size_t index);


#endif