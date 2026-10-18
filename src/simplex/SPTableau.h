// SPTableau.h

#include "LPDefinition.h"

#ifndef SPTABLEAU_H
#define SPTABLEAU_H

typedef struct {

    LPDefinition *lp_def; // Pointer to the definition of the linear programming problem (LPDefinition struct)
    double **tableau; // 2D array representing the simplex tableau. Will be initialized by constructor funciton.

    size_t num_rows;  // Number of rows in the tableau
    size_t num_cols;  // Number of columns in the tableau

    // Counters for variables in the tableau. Also useful for index-based access.
    size_t num_decision_vars; // Number of decision variables
    size_t num_constraints;   // Number of constraints
    size_t num_slack_vars;      // Number of slack variables
    size_t num_surplus_vars;     // Number of surplus variables [for v2]
    size_t num_artificial_vars; // Number of artificial variables [for v2]

    // Additional fields for tracking the state of the tableau
    bool multiple_solutions; // Flag indicating if multiple optimal solutions exist. Defaults to false (no multiple solutions).
    size_t degeneration_level; // Level of degeneration in the tableau. Defaults to 0 (no degeneration), increases over iterations if degeneracy is detected.

} SPTableau;

// Create the tableaul for the given LPDefinition struct. Returns a pointer to the created SPTableau struct.
SPTableau *sp_tableau_create(LPDefinition *lp_def);


#endif