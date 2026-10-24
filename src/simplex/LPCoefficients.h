// LPCoefficients.h

#include "LPDecisionVars.h"

#ifndef LPCOEFFICIENTS_H
#define LPCOEFFICIENTS_H

// Struct to hold the coefficients of decision variables a linear programming problem's objective function or constraints.
typedef struct {
    size_t length;                      // Count of constraints added.
    double *coefficients;                // Array of coefficients for the constraint.
} LPCoefficients;

// Create a new CoefficientsSet struct with the given length and allocate memory for the coefficients array.
// Initializes the coefficients array to zero.
LPCoefficients *lp_coefficients_create(size_t length);

// Free the memory allocated for the coefficients array and resets the length to 0.
// Use to delete the coefficient set when it is no longer needed to prevent memory leaks.
void lp_coefficients_free(LPCoefficients *set);

// Create a new CoefficientsSet struct with the given length and copy the provided coefficients into the new struct.
// Caller is responsible for freeing the coefficients array in the returned CoefficientsSet when done using it.
LPCoefficients *lp_coefficients_create_from_array(size_t length, const double *coefficients);

// Print the coefficients in the LPCoefficients struct for debugging purposes.
// DecisionVariable struct can be passed to display the coefficients alongside their corresponding variable names. If NULL, will print generic variable names.
// Does not add line break. Caller should print a line break afterwards if necessary.
void lp_coefficients_print(const LPCoefficients *set, const LPDecisionVars *decision_vars);

#endif
