// SPMultiSolutions.h
#ifndef SPMULTISOLUTIONS_H
#define SPMULTISOLUTIONS_H

#include "SPBaseSolution.h"

typedef struct SPMultiSolutions {
    SPBaseSolution *bfs_a;      // First optimal solution
    SPBaseSolution *bfs_b;      // Second optimal solution
} SPMultiSolutions;

// Create a SPMultisolutions instance with two BFS.
// The two BFS must have the same counts of variables and same objective function value (Z).
// Caller must ensure that both BFS are valid and represent optimal solutions.
SPMultiSolutions* sp_multi_solutions_create(SPBaseSolution *bfs_a, SPBaseSolution *bfs_b);

// Free the SPMultiSolutions instance and its associated BFS.
// Caller is responsible for freeing the memory allocated for the BFS structs after use.
void sp_multi_solutions_free(SPMultiSolutions *multi_solutions);

// Calculate the midpoint BFS (bfs_c) between the two optimal solutions (bfs_a and bfs_b).
// Alpha (a) is a value between 0 and 1 that determines the position of the midpoint.
// Formula: bfs_c = a(bfs_a) + (1-a)(bfs_b)
SPBaseSolution* sp_multi_solutions_calculate_midpoint(SPMultiSolutions *multi_solutions, double alpha);

#endif