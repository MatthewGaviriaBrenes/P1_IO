// SPBFS.c

#include "SPBFS.h"

// Create base BFS entry with a given number of decision and slack variables. Returns a pointer to the created SPBFS struct.
// Includes Z value as part of the BFS (index 0, iniialized as 0.0).
SPBFS *sp_bfs_create(size_t num_decision_vars, size_t num_slack_vars) {
    if (num_decision_vars == 0) {
        printf("Error: Cannot create BFS with zero decision and slack variables.\n");
        return NULL;
    }
    SPBFS *bfs = (SPBFS *)malloc(sizeof(SPBFS));
    if (!bfs) {
        printf("Error: Memory allocation failed for SPBFS struct.\n");
        return NULL;
    }

    bfs->num_decision_vars = num_decision_vars;
    bfs->num_slack_vars = num_slack_vars;

    // Number of variables in the BFS: 1 (Z column) + number of decision variables + number of slack variables
    size_t bfs_total_vars = 1 + bfs->num_decision_vars + bfs->num_slack_vars; // Z column + decision vars + slack vars

    // Allocate memory for the arrays to hold the BFS information (Z, decision vars, slack vars)
    bfs->var_in_bfs = (bool *)malloc(bfs_total_vars * sizeof(bool));
    bfs->var_values = (double *)malloc(bfs_total_vars * sizeof(double));

    if (!bfs->var_in_bfs) {
        printf("Error: Memory allocation failed for BFS variable membership array.\n");
        free(bfs);
        return NULL;
    }
    if (!bfs->var_values) {
        printf("Error: Memory allocation failed for BFS variable values array.\n");
        free(bfs->var_in_bfs);
        free(bfs);
        return NULL;
    }

    for (size_t col = 0; col < bfs_total_vars; col++) {
        bfs->var_in_bfs[col] = false; // Initialize all other variables as not in the BFS
        bfs->var_values[col] = 0.0; // Initialize all other variable values to 0.0
    }
    return bfs;
}

// Free the BFS struct and its associated resources.
void sp_bfs_free(SPBFS *bfs) {
    if (bfs) {
        free(bfs->var_in_bfs);
        free(bfs->var_values);
        free(bfs);
    }
}

// Get the number of decision variables in the BFS.
size_t sp_bfs_get_num_decision_vars(const SPBFS *bfs) {
    if (bfs == NULL) {
        printf("Error: BFS struct is NULL.\n");
        return 0;
    }
    return bfs->num_decision_vars;
}
// Get the number of slack variables in the BFS.
size_t sp_bfs_get_num_slack_vars(const SPBFS *bfs) {
    if (bfs == NULL) {
        printf("Error: BFS struct is NULL.\n");
        return 0;
    }
    return bfs->num_slack_vars;
}
// Get the total number of variables in the SPBFS struct (Z + decision vars + slack vars).
// Useful for functions iterating through the struct's arrays.
size_t sp_bfs_get_num_all_vars(const SPBFS *bfs) {
    if (bfs == NULL) {
        printf("Error: BFS struct is NULL.\n");
        return 0;
    }
    // Total variables in BFS: Z + decision vars + slack vars
    return 1 + bfs->num_decision_vars + bfs->num_slack_vars;
}

// Get the number of variables that are in the BFS.
size_t sp_bfs_get_num_vars_in_base(const SPBFS *bfs) {
    if (bfs == NULL) {
        printf("Error: BFS struct is NULL.\n");
        return 0;
    }
    size_t count = 0;
    size_t num_all_vars = 1 + bfs->num_decision_vars + bfs->num_slack_vars; // Total variables in BFS definition: Z + decision vars + slack vars
    for (size_t i = 0; i < num_all_vars; i++) {
        if (bfs->var_in_bfs[i]) {
            count++;
        }
    }
    return count;
}

// Get the number of variables that are in the BFS.
size_t sp_bfs_get_num_vars_in_base(const SPBFS *bfs);

// Check if the specified variable (by tableau index) is in the BFS.
bool sp_bfs_is_var_in_bfs(const SPBFS *bfs, size_t var_index) {
    if (bfs == NULL) {
        printf("Error: BFS struct is NULL.\n");
        return false;
    }
    size_t num_vars_in_bfs = sp_bfs_get_num_all_vars(bfs);
    if (var_index >= num_vars_in_bfs) {
        printf("Error: Variable index out of bounds when checking for BFS membership.\n");
        return false;
    }
    return bfs->var_in_bfs[var_index];
}

// Get value of variable in the BFS by its tableau index (0 for Z value).
// Returns -1 if the variable is not found in the BFS or if the BFS is NULL.
double sp_bfs_get_var_value(const SPBFS *bfs, size_t var_index) {
    if (bfs == NULL) {
        printf("Error: BFS struct is NULL.\n");
        return -1.0;
    }
    size_t num_vars_in_bfs = sp_bfs_get_num_all_vars(bfs);
    if (var_index >= num_vars_in_bfs) {
        printf("Error: Variable index out of bounds when retrieving BFS variable value.\n");
        return -1.0;
    }
    return bfs->var_values[var_index];
}
