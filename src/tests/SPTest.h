// SPTest.h
// Header file to define test cases of Simplex Problem (SP).

#include "../simplex/LPDefinition.h"
#include "../simplex/SPTableau.h"

#ifndef SPTEST_H
#define SPTEST_H

// Base function to run a Simplex test case for a given LPDefinition.
// To be used as reference for running Simplex tests for various LP cases (maximization, minimization, unbounded, degenerate, multiple solutions, etc.).
// Also, for reference to integrate with other program's modules (GUI, LaTeX) as they are developed. Currently using console output for demonstration purposes.
void sp_test_base_run(LPDefinition *lp_def);


// Helper function for multiple solutions test case.
// 1. Receives a SPTableau that reached a multiple solution state, 
// 2. Gets the current BFS, calculates another BFS
// 3. Creates a SPMultipleSolutions struct to hold both BFSs and calculate some midpoint solutions.
void sp_test_get_multiple_solutions(SPTableau *spt);

#endif