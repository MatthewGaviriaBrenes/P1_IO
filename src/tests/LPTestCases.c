// LPTestCases.c
// Some tests cases for LP (Linear Programming) defintions.

#include "../simplex/LPDefinition.h"

// Test case - PyV Door and Window Factory
// Decision vars: doors, windows
// Objective function: Minimize Z = 3*doors + 5*windows
// Constraints:
// 1. 1*doors <= 4
// 2. 2*windows <= 12
// 3. 3*doors + 2*windows <= 18
// doors, windows >= 0
LPDefinition* lp_test_max_pyv() {
    // Decision variables: doors, windows
    char test_dec_var_names[2][10] = {"doors", "windows"};
    LPDecisionVars *test_dec_vars = lp_decisionvars_create(2);
    for (size_t i = 0; i < 2; i++) {
        lp_decisionvars_set_var(test_dec_vars, i, test_dec_var_names[i]);
    }


    // Objective function: Minimize Z = 3*doors + 5*windows
    double test_obj_func_coeffs[2] = {3.0, 5.0};
    LPCoefficients *test_coeffs = lp_coefficients_create_from_array(2, test_obj_func_coeffs);
    LPObjectiveFunc *test_obj_func = lp_objectivefunc_create(MINIMIZE, 2, test_coeffs);

    // Constraints
    double raw_coeffs_c1[] = {4.0, 0.0};
    double raw_coeffs_c2[] = {0.0, 2.0};
    double raw_coeffs_c3[] = {3.0, 2.0};
  
    LPCoefficients *test_c1_coeffs = lp_coefficients_create_from_array(3, raw_coeffs_c1);
    LPCoefficients *test_c2_coeffs = lp_coefficients_create_from_array(3, raw_coeffs_c2);
    LPCoefficients *test_c3_coeffs = lp_coefficients_create_from_array(3, raw_coeffs_c3);
  
    double test_c1_rhs = 4.0;
    double test_c2_rhs = 12.0;
    double test_c3_rhs = 18.0;

    LPConstraints *test_constraints = lp_constraints_create(3, 3);
    lp_constraints_add_constraint(test_constraints, test_c1_coeffs, LE, test_c1_rhs);
    lp_constraints_add_constraint(test_constraints, test_c2_coeffs, LE, test_c2_rhs);
    lp_constraints_add_constraint(test_constraints, test_c3_coeffs, LE, test_c3_rhs);

    LPDefinition *test_lp_def = lp_definition_create(test_dec_vars, test_obj_func, test_constraints);
    return test_lp_def;
}

// Sample case: The Seat Belt Factory.
// Decision vars: standard, deluxe
// Objective function: Maximize Z = 3*standard + 4*deluxe
// Constraints:
// 1. standard + deluxe <= 40
// 2. standard + 2*deluxe <= 60
// standard, deluxe >= 0
LPDefinition *lp_test_max_seatbeltfactory() {

    // Decision variables: x1, x2
    char test_dec_var_names[2][10] = {"standard", "deluxe"};
    LPDecisionVars *test_dec_vars = lp_decisionvars_create(2);
    for (size_t i = 0; i < 2; i++) {
        lp_decisionvars_set_var(test_dec_vars, i, test_dec_var_names[i]);
    }

    // Objective function: Maximize Z = 3*x1 + 4*x2
    double test_obj_func_coeffs[2] = {3.0, 4.0};
    LPCoefficients *test_coeffs = lp_coefficients_create_from_array(2, test_obj_func_coeffs);
    LPObjectiveFunc *test_obj_func = lp_objectivefunc_create(MAXIMIZE, 2, test_coeffs);

    // Constraints
    double raw_coeffs_c1[] = {1.0, 1.0};
    double raw_coeffs_c2[] = {1.0, 2.0};

    LPCoefficients *test_c1_coeffs = lp_coefficients_create_from_array(2, raw_coeffs_c1);
    LPCoefficients *test_c2_coeffs = lp_coefficients_create_from_array(2, raw_coeffs_c2);

    double test_c1_rhs = 40.0;
    double test_c2_rhs = 60.0;

    LPConstraints *test_constraints = lp_constraints_create(2, 2);
    lp_constraints_add_constraint(test_constraints, test_c1_coeffs, LE, test_c1_rhs);
    lp_constraints_add_constraint(test_constraints, test_c2_coeffs, LE, test_c2_rhs);
    
    LPDefinition *test_lp_def = lp_definition_create(test_dec_vars, test_obj_func, test_constraints);
    return test_lp_def;
}

// Sample case: Geppetto's Toy Factory.
// Decision vars: toy_soldiers, toy_trains
// Objective function: Maximize Z = 3/x1 + 2/x2
// Constraints:
// 1. 2*toy_soldiers + toy_trains <= 100
// 2. toy_soldiers + toy_trains <= 80
// 3. toy_soldiers <= 40
// toy_soldiers, toy_trains >= 0
LPDefinition *lp_test_max_geppetto() {
    // Decision variables: toy_soldiers, toy_trains
    char test_dec_var_names[2][16] = {"toy_soldiers", "toy_trains"};
    LPDecisionVars *test_dec_vars = lp_decisionvars_create(2);
    for (size_t i = 0; i < 2; i++) {
        lp_decisionvars_set_var(test_dec_vars, i, test_dec_var_names[i]);
    }

    // Objective function: Maximize Z = 3*x1 + 2*x2
    double test_obj_func_coeffs[2] = {3.0, 2.0};
    LPCoefficients *test_coeffs = lp_coefficients_create_from_array(2, test_obj_func_coeffs);
    LPObjectiveFunc *test_obj_func = lp_objectivefunc_create(MAXIMIZE, 2, test_coeffs);

    // Constraints
    double raw_coeffs_c1[] = {2.0, 1.0};
    double raw_coeffs_c2[] = {1.0, 1.0};
    double raw_coeffs_c3[] = {1.0, 0.0};

    LPCoefficients *test_c1_coeffs = lp_coefficients_create_from_array(2, raw_coeffs_c1);
    LPCoefficients *test_c2_coeffs = lp_coefficients_create_from_array(2, raw_coeffs_c2);
    LPCoefficients *test_c3_coeffs = lp_coefficients_create_from_array(2, raw_coeffs_c3);

    double test_c1_rhs = 100.0;
    double test_c2_rhs = 80.0;
    double test_c3_rhs = 40.0;

    LPConstraints *test_constraints = lp_constraints_create(3, 2);
    lp_constraints_add_constraint(test_constraints, test_c1_coeffs, LE, test_c1_rhs);
    lp_constraints_add_constraint(test_constraints, test_c2_coeffs, LE, test_c2_rhs);
    lp_constraints_add_constraint(test_constraints, test_c3_coeffs, LE, test_c3_rhs);

    LPDefinition *test_lp_def = lp_definition_create(test_dec_vars, test_obj_func, test_constraints);
    return test_lp_def;
}


// Sample case: Dakota's Furniture Factory.
// Decision vars: desks, tables, chairs
// Objective function: Maximize Z = 60*desks + 30*tables + 20*chairs
// Constraints:
// 1. 8*desks + 6*tables + 1*chairs <= 48   - Woodworking hours
// 2. 4*desks + 2*tables + 1.5*chairs <= 20 - Carpentry hours
// 3. 2*desks + 1.5*tables + 0.5*chairs <= 8 - Finishing hours
// 4. tables <= 5 - Demand for tables
// desks, tables, chairs >= 0
LPDefinition *lp_test_max_dakota_tables() {
    // Decision variables: desks, tables, chairs
    char test_dec_var_names[3][10] = {"desks", "tables", "chairs"};
    LPDecisionVars *test_dec_vars = lp_decisionvars_create(3);
    for (size_t i = 0; i < 3; i++) {
        lp_decisionvars_set_var(test_dec_vars, i, test_dec_var_names[i]);
    }

    // Objective function: Maximize Z = 60*desks + 30*tables + 20*chairs
    double test_obj_func_coeffs[3] = {60.0, 30.0, 20.0};
    LPCoefficients *test_coeffs = lp_coefficients_create_from_array(3, test_obj_func_coeffs);
    LPObjectiveFunc *test_obj_func = lp_objectivefunc_create(MAXIMIZE, 3, test_coeffs);

    // Constraints
    double raw_coeffs_c1[] = {8.0, 6.0, 1.0};
    double raw_coeffs_c2[] = {4.0, 2.0, 1.5};
    double raw_coeffs_c3[] = {2.0, 1.5, 0.5};
    double raw_coeffs_c4[] = {0.0, 1.0, 0.0};

    LPCoefficients *test_c1_coeffs = lp_coefficients_create_from_array(3, raw_coeffs_c1);
    LPCoefficients *test_c2_coeffs = lp_coefficients_create_from_array(3, raw_coeffs_c2);
    LPCoefficients *test_c3_coeffs = lp_coefficients_create_from_array(3, raw_coeffs_c3);
    LPCoefficients *test_c4_coeffs = lp_coefficients_create_from_array(3, raw_coeffs_c4);

    double test_c1_rhs = 48.0;
    double test_c2_rhs = 20.0;
    double test_c3_rhs = 8.0;
    double test_c4_rhs = 5.0;

    LPConstraints *test_constraints = lp_constraints_create(4, 3);
    lp_constraints_add_constraint(test_constraints, test_c1_coeffs, LE, test_c1_rhs);
    lp_constraints_add_constraint(test_constraints, test_c2_coeffs, LE, test_c2_rhs);
    lp_constraints_add_constraint(test_constraints, test_c3_coeffs, LE, test_c3_rhs);
    lp_constraints_add_constraint(test_constraints, test_c4_coeffs, LE, test_c4_rhs);

    LPDefinition *test_lp_def = lp_definition_create(test_dec_vars, test_obj_func, test_constraints);
    return test_lp_def;
}

// Sample minimization case.
// Decision vars: x1, x2
// Objective function: Minimize Z = 2x1 - 3x2
// Constraints:
// 1. x1 + x2 <= 4
// 2. x1 - x2 <= 6
// x1, x2 >= 0
LPDefinition* lp_test_min_sample_case() {
    // Decision variables: x1, x2
    char test_dec_var_names[2][10] = {"x1", "x2"};
    LPDecisionVars *test_dec_vars = lp_decisionvars_create(2);
    for (size_t i = 0; i < 2; i++) {
        lp_decisionvars_set_var(test_dec_vars, i, test_dec_var_names[i]);
    }

    // Objective function: Minimize Z = 2x1 - 3x2
    double test_obj_func_coeffs[2] = {2.0, -3.0};
    LPCoefficients *test_coeffs = lp_coefficients_create_from_array(2, test_obj_func_coeffs);
    LPObjectiveFunc *test_obj_func = lp_objectivefunc_create(MINIMIZE, 2, test_coeffs);

    // Constraints
    double raw_coeffs_c1[] = {1.0, 1.0};
    double raw_coeffs_c2[] = {1.0, -1.0};

    LPCoefficients *test_c1_coeffs = lp_coefficients_create_from_array(2, raw_coeffs_c1);
    LPCoefficients *test_c2_coeffs = lp_coefficients_create_from_array(2, raw_coeffs_c2);

    double test_c1_rhs = 4.0;
    double test_c2_rhs = 6.0;

    LPConstraints *test_constraints = lp_constraints_create(2, 2);
    lp_constraints_add_constraint(test_constraints, test_c1_coeffs, LE, test_c1_rhs);
    lp_constraints_add_constraint(test_constraints, test_c2_coeffs, LE, test_c2_rhs);

    LPDefinition *test_lp_def = lp_definition_create(test_dec_vars, test_obj_func, test_constraints);
    return test_lp_def;
}

// Sample case for unbounded problem - Bakery
// Decision vars: baguettes, sweet_bread, baking_soda, flour
// Objective function: Maximize Z = 36*baguettes + 30*sweet_bread - 3*baking_soda - 4*flour
// Constraints:
// 1. baguettes + sweet_bread - baking_soda <= 5
// 2. 6*baguettes + 5*sweet_bread - flour <= 10
// baguettes, sweet_bread, baking_soda, flour >= 0
LPDefinition* lp_test_unbounded_bakery() {
    // Decision variables: x1, x2
    char test_dec_var_names[4][16] = {"baguettes", "sweet_bread", "baking_soda", "flour"};
    LPDecisionVars *test_dec_vars = lp_decisionvars_create(4);
    for (size_t i = 0; i < 4; i++) {
        lp_decisionvars_set_var(test_dec_vars, i, test_dec_var_names[i]);
    }

    // Objective function: Maximize Z = 36*baguettes + 30*sweet_bread - 3*baking_soda - 4*flour
    double test_obj_func_coeffs[4] = {36.0, 30.0, -3.0, -4.0};
    LPCoefficients *test_coeffs = lp_coefficients_create_from_array(4, test_obj_func_coeffs);
    LPObjectiveFunc *test_obj_func = lp_objectivefunc_create(MAXIMIZE, 4, test_coeffs);

    // Constraints
    double raw_coeffs_c1[] = {1.0, 1.0, -1.0, 0.0};
    double raw_coeffs_c2[] = {6.0, 5.0, 0.0, -1.0};

    LPCoefficients *test_c1_coeffs = lp_coefficients_create_from_array(4, raw_coeffs_c1);
    LPCoefficients *test_c2_coeffs = lp_coefficients_create_from_array(4, raw_coeffs_c2);

    double test_c1_rhs = 5.0;
    double test_c2_rhs = 10.0;

    LPConstraints *test_constraints = lp_constraints_create(2, 4);
    lp_constraints_add_constraint(test_constraints, test_c1_coeffs, LE, test_c1_rhs);
    lp_constraints_add_constraint(test_constraints, test_c2_coeffs, LE, test_c2_rhs);

    LPDefinition *test_lp_def = lp_definition_create(test_dec_vars, test_obj_func, test_constraints);
    return test_lp_def;
}

// Sample case for degenerate problem.
// Decision vars: x1, x2
// Objective function: Maximize Z = 2x1 + x2
// Constraints:
// 1. 3x1 + x2 <= 6
// 2. x1 - x2 <= 2
// 3. x2 <= 3
// x1, x2 >= 0
LPDefinition* lp_test_sample_degenerate_case() {
    // Decision variables: x1, x2
    char test_dec_var_names[2][10] = {"x1", "x2"};
    LPDecisionVars *test_dec_vars = lp_decisionvars_create(2);
    for (size_t i = 0; i < 2; i++) {
        lp_decisionvars_set_var(test_dec_vars, i, test_dec_var_names[i]);
    }

    // Objective function: Maximize Z = 2x1 + x2
    double test_obj_func_coeffs[2] = {2.0, 1.0};
    LPCoefficients *test_coeffs = lp_coefficients_create_from_array(2, test_obj_func_coeffs);
    LPObjectiveFunc *test_obj_func = lp_objectivefunc_create(MAXIMIZE, 2, test_coeffs);

    // Constraints
    double raw_coeffs_c1[] = {3.0, 1.0};
    double raw_coeffs_c2[] = {1.0, -1.0};
    double raw_coeffs_c3[] = {0.0, 1.0};

    LPCoefficients *test_c1_coeffs = lp_coefficients_create_from_array(2, raw_coeffs_c1);
    LPCoefficients *test_c2_coeffs = lp_coefficients_create_from_array(2, raw_coeffs_c2);
    LPCoefficients *test_c3_coeffs = lp_coefficients_create_from_array(2, raw_coeffs_c3);

    double test_c1_rhs = 6.0;
    double test_c2_rhs = 2.0;
    double test_c3_rhs = 3.0;

    LPConstraints *test_constraints = lp_constraints_create(3, 2);
    lp_constraints_add_constraint(test_constraints, test_c1_coeffs, LE, test_c1_rhs);
    lp_constraints_add_constraint(test_constraints, test_c2_coeffs, LE, test_c2_rhs);
    lp_constraints_add_constraint(test_constraints, test_c3_coeffs, LE, test_c3_rhs);

    LPDefinition *test_lp_def = lp_definition_create(test_dec_vars, test_obj_func, test_constraints);
    return test_lp_def;
}

// Sample case for case with multiple optimal solutions.
// Decision vars: x1, x2
// Objective function: Maximize Z = 4x1 + 14x2
// Constraints:
// 1. 2x1 + 7x2 <= 21
// 2. 7x1 + 2x2 <= 21
// x1, x2 >= 0
LPDefinition * lp_test_sample_multisolutions_case() {
    // Decision variables: x1, x2
    char test_dec_var_names[2][10] = {"x1", "x2"};
    LPDecisionVars *test_dec_vars = lp_decisionvars_create(2);
    for (size_t i = 0; i < 2; i++) {
        lp_decisionvars_set_var(test_dec_vars, i, test_dec_var_names[i]);
    }

    // Objective function: Maximize Z = 4*x1 + 14*x2
    double test_obj_func_coeffs[2] = {4.0, 14.0};
    LPCoefficients *test_coeffs = lp_coefficients_create_from_array(2, test_obj_func_coeffs);
    LPObjectiveFunc *test_obj_func = lp_objectivefunc_create(MAXIMIZE, 2, test_coeffs);

    // Constraints
    double raw_coeffs_c1[] = {2.0, 7.0};
    double raw_coeffs_c2[] = {7.0, 2.0};

    LPCoefficients *test_c1_coeffs = lp_coefficients_create_from_array(2, raw_coeffs_c1);
    LPCoefficients *test_c2_coeffs = lp_coefficients_create_from_array(2, raw_coeffs_c2);

    double test_c1_rhs = 21.0;
    double test_c2_rhs = 21.0;

    LPConstraints *test_constraints = lp_constraints_create(2, 2);
    lp_constraints_add_constraint(test_constraints, test_c1_coeffs, LE, test_c1_rhs);
    lp_constraints_add_constraint(test_constraints, test_c2_coeffs, LE, test_c2_rhs);

    LPDefinition *test_lp_def = lp_definition_create(test_dec_vars, test_obj_func, test_constraints);
    return test_lp_def;
}


