// SPTableau.h

#include "LPDefinition.h"
#include "SPBaseSolution.h"
#include "SPMultiSolutions.h"

#ifndef SPTABLEAU_H
#define SPTABLEAU_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Enum to output status of the Simplex Tableau after solving steps. 
// Used to indicate the current state of the tableau and the solution.
enum SPTableauStatus {
    SPT_STANDBY = 0,                    // Optimal solution has not been found yet. The tableau is still being processed.
    SPT_OPTIMAL_SOLUTION_FOUND = 2,     // Optimal solution found. The tableau has reached an optimal state.
    SPT_MULTPILE_SOLUTIONS_DETECTED = 3,  // Multiple optimal solutions found. The tableau has reached an optimal state, but there are multiple solutions.
    SPT_DEGENERATED_PROBLEM = 4,         // Degenerate problem detected.
    SPT_UNBOUNDED_PROBLEM = 5,          // Unbounded problem detected.
    SPT_INFEASIBLE_PROBLEM = 6,         // Infeasible problem detected [for v2].
    SPT_INVALID_STATE = 7               // Invalid problem detected (for other errors).
};


// Struct for Simplex Tableau (SPTableau) information.
typedef struct {
    LPDefinition *lp_def; // Pointer to the definition of the linear programming problem (LPDefinition struct) for the Simplex Tableau.
    double **tableau; // 2D array representing the Simplex tableau. Will be initialized by constructor funciton.

    size_t num_rows;  // Number of rows in the tableau
    size_t num_cols;  // Number of columns in the tableau

    // Counters for variables and constraints in the tableau. Also useful for index-based access.
    size_t num_decision_vars; 
    size_t num_constraints;   
    size_t num_slack_vars;      
    size_t num_surplus_vars;     // for v2
    size_t num_artificial_vars; // for v2

    // Additional fields for tracking the state of the tableau
    size_t degeneration_level;  // If problem is degenerated, counts steps the tableau stays degenerated. May be used to handle potential cycling. Defaults to 0.
} SPTableau;

// Create the tableaul for the given LPDefinition struct. Returns a pointer to the created SPTableau struct.
SPTableau *sp_tableau_create(LPDefinition *lp_def);

// Helper initialization function: Create the baseline tableau given the LP definition's baseline information.
// Returns boolean to confirm successful initialization.
bool sp_tableau_init_baseline(SPTableau *spt);

// Helper initialization function: Set up the tableau's objective function row based on the LP definition's objective function.
// Returns boolean to confirm successful initialization.
// Must be called after sp_tableau_init_baseline.
bool sp_tableau_init_objective_func_row(SPTableau *spt);

// Helper function to add the constraints rows to the tableau. This function is called during the initialization of the tableau.
// Must be called after sp_tableau_init_baseline.
bool sp_tableau_init_constraints_rows(SPTableau *spt);

// Free the memory allocated for the SPTableau struct and its associated resources.
// Caller must ensure that the LPDefinition struct is freed separately if it was dynamically allocated.
void sp_tableau_free(SPTableau *spt);

// Get the current base solution for the tableau. 
// Returns a pointer to the SPBaseSolution struct representing the current BFS.
// Caller is responsible for freeing the memory allocated for the SPBaseSolution struct after use.
SPBaseSolution *sp_tableau_get_base_sol(const SPTableau *spt);

// Validate a provided Base solution against the tableau's constraints to confirm if it is a BFS (Bease Feasible Solution).
// A valid BFS must satisfy all constraints of the tableau.
// Does not confirm optimal solution.
bool sp_tableau_validate_base(const SPTableau *spt, const SPBaseSolution *spbs);

// Returns a snapshot of the tableau's current state as a 2D array of doubles.
// To be used if intermediate tableau states need to be displayed.
// Caller is responsible for freeing the memory allocated for the snapshot after use.
double **sp_tableau_take_snapshot(const SPTableau *spt);

// Check if the specified column in the tableau is a base column (i.e., corresponds to a basic variable).
// Returns true if the column is a base column, false otherwise.
bool sp_tableau_is_base_column(const SPTableau *spt, size_t col_index);

// Check if the specified variable (by tableau index) in the tableau is a base variable.
// Returns true if the variable is a base variable, false otherwise.
bool sp_tableau_is_base_variable(const SPTableau *spt, size_t var_index);

// Count the number of base variables in the tableau.
// Returns the count of base variables in the tableau.
// Use alongside sp_tableau_get_base_vars_indices to get the list of base variables and their count.
size_t sp_tableau_get_base_vars_count(const SPTableau *spt);

// Get the indices of the base variables in the tableau.
// Returns a dynamically allocated array of size_t containing the indices of the base variables.
// Caller is responsible for freeing the memory allocated for the array after use.
// Returned list can be used to check the base variables' values.
size_t *sp_tableau_get_base_vars_indices(const SPTableau *spt);

// Get the value of a base variable.
// Return -1 if the variable is not a base variable or if the tableau is NULL. Otherwise, return the value of the base variable.
double sp_tableau_get_base_var_value(const SPTableau *spt, size_t var_index);

// Get the current value of Z in the tableau (row 0, column 0).
double sp_tableau_get_z_value(const SPTableau *spt);

// Get the next variable entering the basis (pivot column) based on the current tableau state.
// Returns the index of the pivot column, or 0 if no valid pivot column is found (solution found).
size_t sp_tableau_get_pivot_column(const SPTableau *spt);

// Calculate minimum ratio for the incoming variable (pivot column) to determine the leaving variable (pivot row).
// Returns an array of doubles containing the minimum ratios for each tableau row (including objetive row) for indexing purposes.
// Objective row defaults to -1 (not selectable).
// If the pivot column value is negative or zero, the ratio is set to -1 (indicating no valid ratio).
double *sp_tableau_calculate_min_ratios(const SPTableau *spt, size_t pivot_col_index);

// Get the next variable leaving the basis (pivot row) based on the current tableau state.
// Must be called after sp_tableau_get_pivot_column and sp_tableau_calculate_min_ratios.
// Returns the index of the pivot row, or 0 if no valid pivot row is found (unbound problem).
size_t sp_tableau_get_pivot_row(const SPTableau *spt, size_t pivot_col_index, double *min_ratios);

// Canonize the pivot row in the tableau to ensure a pivot value of 1 in the pivot cell, and updates the pivot row accordingly.
void sp_tableau_canonize_pivot_row(SPTableau *spt, size_t pivot_row_index, size_t pivot_col_index);

// Canonize the non-pivot rows to ensure that all other entries in the pivot column are zero.
// Must be called after sp_tableau_canonize_pivot_row. 
void sp_tableau_canonize_nonpivot_rows(SPTableau *spt, size_t pivot_row_index, size_t pivot_col_index);

// Perform a single step of the simplex algorithm on the tableau.
enum SPTableauStatus sp_tableau_solve_step(SPTableau *spt);

// Print the current state of the tableau to the console for debugging and visualization purposes.
void sp_tableau_print_tableau(const SPTableau *spt);

// Print the current state of the BS (Base Solution) for the provided SPTableau.
// Name of decision variables are included in the output if included in the SPT's LP definition.
void sp_tableau_print_base(const SPTableau *spt, const SPBaseSolution *spbs);


//TODO: List of functions to add later...
/* 
- TBD...
*/


#endif