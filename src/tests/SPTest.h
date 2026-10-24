// SPTest.h
// Header file to define test cases of Simplex Problem (SP).

#ifndef SPTEST_H
#define SPTEST_H

#include "../simplex/LPDefinition.h"
#include "../simplex/SPTableau.h"

// Run for a simple case of LP problem using Simplex.
// Baseline run for min/max LP cases, expected to find single solution.
void sp_test_simple_case(LPDefinition *lp_def);

// TBD: Unbounded case of LP problem using Simplex.
// TBD: Degenerate case of LP problem using Simplex.
// TBD: Multiple optimal solutions case of LP problem using Simplex.

#endif