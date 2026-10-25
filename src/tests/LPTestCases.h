// LP Test Cases
// File with test cases for LP definitions.

#ifndef LP_TEST_CASES_H
#define LP_TEST_CASES_H

#include "../simplex/LPDefinition.h"

// Test case - PyV Door and Window Factory
// Decision vars: doors, windows
// Objective function: Minimize Z = 3*doors + 5*windows
// Constraints:
// 1. 1*doors <= 4
// 2. 2*windows <= 12
// 3. 3*doors + 2*windows <= 18
// doors, windows >= 0
LPDefinition* lp_test_max_pyv();


// Sample case: The Seat Belt Factory.
// Decision vars: standard, deluxe
// Objective function: Maximize Z = 3*standard + 4*deluxe
// Constraints:
// 1. standard + deluxe <= 40
// 2. standard + 2*deluxe <= 60
// standard, deluxe >= 0
LPDefinition *lp_test_max_seatbeltfactory();


// Sample case: Geppetto's Toy Factory.
// Decision vars: toy_soldiers, toy_trains
// Objective function: Maximize Z = 3*toy_soldiers + 2*toy_trains
// Constraints:
// 1. 2*toy_soldiers + toy_trains <= 100 - Finishing hours
// 2. toy_soldiers + toy_trains <= 80 - Carpentry hours
// 3. toy_soldiers <= 40 - Demand for toy soldiers
// toy_soldiers, toy_trains >= 0
LPDefinition *lp_test_max_geppetto();


// Sample case: Dakota Tables.
// Decision vars: desks, tables, chairs
// Objective function: Maximize Z = 60*desks + 30*tables + 20*chairs
// Constraints:
// 1. 8*desks + 6*tables + 1*chairs <= 48   - Woodworking hours
// 2. 4*desks + 2*tables + 1.5*chairs <= 20 - Carpentry hours
// 3. 2*desks + 1.5*tables + 0.5*chairs <= 8 - Finishing hours
// 4. tables <= 5 - Demand for tables
// desks, tables, chairs >= 0
LPDefinition *lp_test_max_dakota_tables();

// Sample minimization case.
// Decision vars: x1, x2
// Objective function: Minimize Z = 2x1 - 3x2
// Constraints:
// 1. x1 + x2 <= 4
// 2. x1 - x2 <= 6
// x1, x2 >= 0
LPDefinition* lp_test_min_sample_case();

// Sample case for unbounded problem - Bakery
// Decision vars: baguettes, sweet_bread, baking_soda, flour
// Objective function: Maximize Z = 36*baguettes + 30*sweet_bread - 3*baking_soda - 4*flour
// Constraints:
// 1. baguettes + sweet_bread - baking_soda <= 5
// 2. 6*baguettes + 5*sweet_bread - flour <= 10
// baguettes, sweet_bread, baking_soda, flour >= 0
LPDefinition* lp_test_unbounded_bakery();


// Sample case for degenerate problem.
// Decision vars: x1, x2
// Objective function: Maximize Z = 2x1 + x2
// Constraints:
// 1. 3x1 + x2 <= 6
// 2. x1 - x2 <= 2
// 3. x2 <= 3
// x1, x2 >= 0
LPDefinition* lp_test_sample_degenerate_case();

// Sample case for case with multiple optimal solutions.
// Decision vars: x1, x2
// Objective function: Maximize Z = 4x1 + 14x2
// Constraints:
// 1. 2x1 + 7x2 <= 21
// 2. 7x1 + 2x2 <= 21
// x1, x2 >= 0
LPDefinition * lp_test_sample_multisolutions_case();

#endif 