// SPMultiSolutions.c

#include "SPMultiSolutions.h"

// Create a SPMultisolutions instance with two BFS.
// The two BFS must have the same counts of variables.
// Caller must ensure that both BFS are valid and represent optimal solutions.
SPMultiSolutions* sp_multi_solutions_create(SPBaseSolution *bfs_a, SPBaseSolution *bfs_b) {
    if (bfs_a == NULL || bfs_b == NULL) {
        printf("Error: One or both of the provided BFS are NULL.\n");
        return NULL; 
    }
    if (bfs_a->num_decision_vars != bfs_b->num_decision_vars || bfs_a->num_slack_vars != bfs_b->num_slack_vars) {
        printf("Error: The two BFS must have the same number of decision and slack variables.\n");
        return NULL; 
    }
    if (bfs_a->var_values[0] != bfs_b->var_values[0]) {
        printf("Error: The two BFS must have the same objective function value (Z).\n");
        return NULL; 
    }

    SPMultiSolutions *multi_solutions = (SPMultiSolutions *)malloc(sizeof(SPMultiSolutions));
    if (multi_solutions == NULL) {
        printf("Error: Memory allocation failed for SPMultiSolutions struct.\n");
        return NULL; 
    }

    multi_solutions->bfs_a = bfs_a;
    multi_solutions->bfs_b = bfs_b;

    return multi_solutions;
}

// Free the SPMultiSolutions instance and its associated BFS.
// Caller is responsible for freeing the memory allocated for the BFS structs after use.
void sp_multi_solutions_free(SPMultiSolutions *multi_solutions) {
    if (multi_solutions == NULL) {
        return;
    }

    multi_solutions->bfs_a = NULL;
    multi_solutions->bfs_b = NULL; 

    // Free the SPMultiSolutions struct itself
    free(multi_solutions);
}

// Calculate the midpoint BFS (bfs_c) between the two optimal solutions (bfs_a and bfs_b).
// Alpha (a) is a value between 0 and 1 that determines the position of the midpoint.
// Formula: bfs_c = a(bfs_a) + (1-a)(bfs_b)
SPBaseSolution* sp_multi_solutions_calculate_midpoint(SPMultiSolutions *multi_solutions, double alpha) {
    if (multi_solutions == NULL || multi_solutions->bfs_a == NULL || multi_solutions->bfs_b == NULL) {
        printf("Error: One or both of the provided BFS are NULL.\n");
        return NULL; // Invalid input
    }

    if (alpha < 0.0 || alpha > 1.0) {
        printf("Error: Alpha must be between 0 and 1.\n");
        return NULL; // Invalid alpha value
    }

    size_t num_decision_vars = multi_solutions->bfs_a->num_decision_vars;
    size_t num_slack_vars = multi_solutions->bfs_a->num_slack_vars;

    SPBaseSolution *midpoint_bfs = sp_base_solution_create(num_decision_vars, num_slack_vars);
    if (midpoint_bfs == NULL) {
        printf("Error: Memory allocation failed for midpoint BFS.\n");
        return NULL; // Memory allocation failed
    }

    // Copy the Z value from either BFS (they should be the same) into the midpoint BFS
    midpoint_bfs->var_values[0] = multi_solutions->bfs_a->var_values[0];
    midpoint_bfs->vars_in_base[0] = multi_solutions->bfs_a->vars_in_base[0];

    size_t num_vars = 1 + num_decision_vars + num_slack_vars; // Total variables in BFS: Z + decision vars + slack vars


    // Calculate the midpoint values between the two BFS for decision and slack variables.
    for (size_t var = 1; var < num_vars; var++) {
        midpoint_bfs->var_values[var] = alpha * multi_solutions->bfs_a->var_values[var] + (1 - alpha) * multi_solutions->bfs_b->var_values[var];
        // Determine if the variable is in the base for the midpoint BFS based on the two BFS
        midpoint_bfs->vars_in_base[var] = (multi_solutions->bfs_a->vars_in_base[var] || multi_solutions->bfs_b->vars_in_base[var]);
    }

    return midpoint_bfs;
}