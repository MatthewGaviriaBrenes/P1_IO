// LPTestCases.c
// Some tests cases for LP (Linear Programming) defintions.

#include "../simplex/LPDefinition.h"

// Sample case used in previous tests.
// Decision vars: x1, x2, x3
// Objective function: Maximize Z = 150*x1 + 200*x2 + 210*x3
// Constraints:
// 1. x1 + 2*x2 + 3*x3 <= 10
// 2. 2*x1 + x2 <= 8
// 3. x1 + x3 <= 5
LPDefinition* lp_test_samplecase() {
    // Decision variables: x1, x2, x3
    char test_dec_var_names[3][3] = {"x1", "x2", "x3"};
    LPDecisionVars *test_dec_vars = lp_decisionvars_create(3);
    for (size_t i = 0; i < 3; i++) {
        lp_decisionvars_set_var(test_dec_vars, i, test_dec_var_names[i]);
    }

    // Objective function: Maximize Z = 150*x1 + 200*x2 + 210*x3
    double test_obj_func_coeffs[3] = {150.0, 200.0, 210.0};
    LPCoefficients *test_coeffs = lp_coefficients_create_from_array(3, test_obj_func_coeffs);
    LPObjectiveFunc *test_obj_func = lp_objectivefunc_create(MAXIMIZE, 3, test_coeffs);

    // Constraints
    double raw_coeffs_c1[] = {1.0, 2.0, 3.0};
    double raw_coeffs_c2[] = {2.0, 1.0, 0.0};
    double raw_coeffs_c3[] = {1.0, 0.0, 1.0};
  
    LPCoefficients *test_c1_coeffs = lp_coefficients_create_from_array(3, raw_coeffs_c1);
    LPCoefficients *test_c2_coeffs = lp_coefficients_create_from_array(3, raw_coeffs_c2);
    LPCoefficients *test_c3_coeffs = lp_coefficients_create_from_array(3, raw_coeffs_c3);
  
    double test_c1_rhs = 10.0;
    double test_c2_rhs = 8.0;
    double test_c3_rhs = 5.0;

    LPConstraints *test_constraints = lp_constraints_create(3, 3);
    lp_constraints_add_constraint(test_constraints, test_c1_coeffs, LE, test_c1_rhs);
    lp_constraints_add_constraint(test_constraints, test_c2_coeffs, LE, test_c2_rhs);
    lp_constraints_add_constraint(test_constraints, test_c3_coeffs, LE, test_c3_rhs);

    LPDefinition *test_lp_def = lp_definition_create(test_dec_vars, test_obj_func, test_constraints);
    return test_lp_def;
}

// Sample case: The Belt Factory.
// Decision vars: x1, x2
// Objective function: Maximize Z = 3*x1 + 4*x2
// Constraints:
// 1. x1 + x2 <= 40
// 2. x1 + 2*x2 <= 60
LPDefinition *lp_test_beltfactory() {

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

