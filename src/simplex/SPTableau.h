// SPTableau.h

#include "LPDefinition.h"

#ifndef SPTABLEAU_H
#define SPTABLEAU_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// SPBaseVariableInfo struct
// Struct to represent information about a base variable (decision variable or slack variable) in the simplex tableau.
// Surplus and artificial variables are not considered base variables (may review this for v2).
// To be used when outputting information about a base variable in the tableau (e.g. Getting optimal solution).
typedef struct {
    size_t decision_var_index; // Index of the decision variable in the tableau
    size_t slack_var_index;   // Index of the slack variable in the tableau
    double value; // Coefficient of the variable in the constraint
} SPBaseVarInfo;

typedef struct {
    LPDefinition *lp_def; // Pointer to the definition of the linear programming problem (LPDefinition struct)
    double **tableau; // 2D array representing the simplex tableau. Will be initialized by constructor funciton.

    size_t num_rows;  // Number of rows in the tableau
    size_t num_cols;  // Number of columns in the tableau

    // Counters for variables in the tableau. Also useful for index-based access.
    size_t num_decision_vars; 
    size_t num_constraints;   
    size_t num_slack_vars;      
    size_t num_surplus_vars;     // for v2
    size_t num_artificial_vars; // for v2
    // Additional fields for tracking the state of the tableau
    bool multiple_solutions;    // Multiple solutions flag (true if multiple solutions exist, false otherwise). Defaults to false.
    bool is_unbounded;          // Unbounded flag (true if the problem is unbounded, false by default). Stop further steps if true.
    size_t degeneration_level;  // If problem is degenerated, counts steps the tableau stays degenerated. May be used to handle potential cycling. Defaults to 0.
} SPTableau;

// Create the tableaul for the given LPDefinition struct. Returns a pointer to the created SPTableau struct.
SPTableau *sp_tableau_create(LPDefinition *lp_def);

// Helper initialization function: Create the baseline tableau given the LP definition's baseline information.
// Returns boolean to confirm successful initialization.
bool sp_tableau_init_baseline(SPTableau *tableau);

// Helper initialization function: Set up the tableau's objective function row based on the LP definition's objective function.
// Returns boolean to confirm successful initialization.
// Must be called after sp_tableau_init_baseline.
bool sp_tableau_init_objective_func_row(SPTableau *tableau);

// Helper function to add the constraints rows to the tableau. This function is called during the initialization of the tableau.
// Must be called after sp_tableau_init_baseline.
bool sp_tableau_init_constraints_rows(SPTableau *tableau);

// Free the memory allocated for the SPTableau struct and its associated resources.
// Caller must ensure that the LPDefinition struct is freed separately if it was dynamically allocated.
void sp_tableau_free(SPTableau *tableau);

// Returns a snapshot of the tableau's current state as a 2D array of doubles.
// To be used if intermediate tableau states need to be displayed.
// Caller is responsible for freeing the memory allocated for the snapshot after use.
double **sp_tableau_take_snapshot(const SPTableau *tableau);

// Check if the specified column in the tableau is a base column (i.e., corresponds to a basic variable).
// Returns true if the column is a base column, false otherwise.
bool sp_tableau_is_base_column(const SPTableau *tableau, size_t col_index);

// Check if the specified variable (column) in the tableau is a base variable.
// Returns true if the variable is a base variable, false otherwise.
bool sp_tableau_is_base_variable(const SPTableau *tableau, size_t var_index);

// Count the number of base variables in the tableau.
// Returns the count of base variables in the tableau.
size_t sp_tableau_count_base_variables(const SPTableau *tableau);

// Get the indices of the base variables in the tableau.
// Returns a dynamically allocated array of size_t containing the indices of the base variables.
// Caller is responsible for freeing the memory allocated for the array after use.
// Returned list can be used to check the base variables' values.
size_t *sp_tableau_get_base_vars_indices(const SPTableau *tableau);

// Get the value of the specified base variable in the tableau (by index). 
// The variable index should correspond to a base variable.
SPBaseVarInfo sp_tableau_get_base_var_value(const SPTableau *tableau, size_t index);

//TODO: List of functions to add later...
/* 
- Function to find if a variable is a base variable in the tableau (i.e., corresponds to a basic variable).
- Function to find next variable entering the basis (pivot column) based on the current tableau state.
- Function to find next variable leaving the basis (pivot row) based on the current tableau state
- Function to perform pivot operation on the tableau given a pivot row and pivot column.
- Function to check for optimality of the current tableau state.
- Function to extract the optimal solution from the tableau once optimality is reached.
- TBD...
*/


#endif