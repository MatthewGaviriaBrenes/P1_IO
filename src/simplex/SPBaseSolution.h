// SPBaseSolution.h

#ifndef SPspbs_H
#define SPspbs_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Struct for Simplex's Base Solution (BS) information.
// Contains list of decision and slack variables, their values and whether they are in the base or not.
// Not to be confused with BFS (Base Feasible Solution), Simplex Tableau is used to validate a BS as a BFS.
typedef struct {
    bool *vars_in_base; // Whether the variable is in the base (true) or not (false)
    double *var_values; // Value of the variable

    size_t num_decision_vars; // Number of decision variables in the BS
    size_t num_slack_vars; // Number of slack variables in the BS
} SPBaseSolution;

// Create base BS entry with a given number of decision and slack variables. Returns a pointer to the created SPBaseSolution struct.
// Includes Z value as part of the BS (index 0, iniialized as 0.0).
SPBaseSolution *sp_base_sol_create(size_t num_decision_vars, size_t num_slack_vars);

// Free the BS struct and its associated resources.
void sp_base_sol_free(SPBaseSolution *spbs);

// Get the number of decision variables in the BS.
size_t sp_base_sol_get_num_decision_vars(const SPBaseSolution *spbs);

// Get the number of slack variables in the BS.
size_t sp_base_sol_get_num_slack_vars(const SPBaseSolution *spbs);

// Get the total number of variables in the SPBaseSolution struct (Z + decision vars + slack vars).
// Useful for functions iterating through the struct's arrays.
size_t sp_base_sol_get_num_vars(const SPBaseSolution *spbs);

// Get the number of variables that are in the BS.
size_t sp_base_sol_get_num_base_vars(const SPBaseSolution *spbs);

// Check if the specified variable (by tableau index) is in the BS.
bool sp_base_sol_is_vars_in_base(const SPBaseSolution *spbs, size_t var_index);

// Get value of variable in the BS by its tableau index (0 for Z value).
// Returns -1 if the variable is not found in the BS or if the BS is NULL.
double sp_base_sol_get_var_value(const SPBaseSolution *spbs, size_t var_index);

// Check if the BS is degenerate (i.e., has one basic variable with value zero).
bool sp_base_sol_is_degenerate(const SPBaseSolution *spbs);

#endif