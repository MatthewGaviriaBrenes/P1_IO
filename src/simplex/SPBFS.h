// SPBFS.h

#ifndef SPBFS_H
#define SPBFS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Struct for Simplex's Base Feasible Solution (BFS) information.
typedef struct {
    bool *var_in_bfs; // Whether the variable is in the BFS (true) or not (false)
    double *var_values; // Value of the variable

    size_t num_decision_vars; // Number of decision variables in the BFS
    size_t num_slack_vars; // Number of slack variables in the BFS
} SPBFS;

// Create base BFS entry with a given number of decision and slack variables. Returns a pointer to the created SPBFS struct.
// Includes Z value as part of the BFS (index 0, iniialized as 0.0).
SPBFS *sp_bfs_create(size_t num_decision_vars, size_t num_slack_vars);

// Free the BFS struct and its associated resources.
void sp_bfs_free(SPBFS *bfs);

// Get the number of decision variables in the BFS.
size_t sp_bfs_get_num_decision_vars(const SPBFS *bfs);

// Get the number of slack variables in the BFS.
size_t sp_bfs_get_num_slack_vars(const SPBFS *bfs);

// Get the number of variables in the BFS (Z + decision vars + slack vars).
size_t sp_bfs_get_num_all_vars(const SPBFS *bfs);

// Check if the specified variable (by tableau index) is in the BFS.
bool sp_bfs_is_var_in_bfs(const SPBFS *bfs, size_t var_index);

// Get value of variable in the BFS by its tableau index (0 for Z value).
// Returns -1 if the variable is not found in the BFS or if the BFS is NULL.
double sp_bfs_get_var_value(const SPBFS *bfs, size_t var_index);

#endif