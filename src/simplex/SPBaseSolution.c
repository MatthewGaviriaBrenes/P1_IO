// SPBaseSolution.c

#include "SPBaseSolution.h"

// Create base solution entry with a given number of decision and slack variables. Returns a pointer to the created SPBaseSolution struct.
// Includes Z value as part of the BS (index 0, iniialized as 0.0).
SPBaseSolution *sp_base_solution_create(size_t num_decision_vars, size_t num_slack_vars) {
    if (num_decision_vars == 0) {
        printf("Error: Cannot create BS with zero decision and slack variables.\n");
        return NULL;
    }
    SPBaseSolution *spbs = (SPBaseSolution *)malloc(sizeof(SPBaseSolution));
    if (!spbs) {
        printf("Error: Memory allocation failed for SPBaseSolution struct.\n");
        return NULL;
    }

    spbs->num_decision_vars = num_decision_vars;
    spbs->num_slack_vars = num_slack_vars;

    // Number of variables in the BS: 1 (Z column) + number of decision variables + number of slack variables
    size_t spbs_total_vars = 1 + spbs->num_decision_vars + spbs->num_slack_vars; // Z column + decision vars + slack vars

    // Allocate memory for the arrays to hold the BS information (Z, decision vars, slack vars)
    spbs->vars_in_base = (bool *)malloc(spbs_total_vars * sizeof(bool));
    spbs->var_values = (double *)malloc(spbs_total_vars * sizeof(double));

    if (spbs->vars_in_base == NULL) {
        printf("Error: Memory allocation failed for BS variable membership array.\n");
        free(spbs);
        return NULL;
    }
    if (spbs->var_values == NULL) {
        printf("Error: Memory allocation failed for BS variable values array.\n");
        free(spbs->vars_in_base);
        free(spbs);
        return NULL;
    }

    for (size_t col = 0; col < spbs_total_vars; col++) {
        spbs->vars_in_base[col] = false; // Initialize all other variables as not in the BS
        spbs->var_values[col] = 0.0; // Initialize all other variable values to 0.0
    }
    return spbs;
}

// Free the BS struct and its associated resources.
void sp_base_solution_free(SPBaseSolution *spbs) {
    if (spbs) {
        free(spbs->vars_in_base);
        free(spbs->var_values);
        free(spbs);
    }
}

// Get the number of decision variables in the BS.
size_t sp_base_solution_get_num_decision_vars(const SPBaseSolution *spbs) {
    if (spbs == NULL) {
        printf("Error: Base Solution struct is NULL.\n");
        return 0;
    }
    return spbs->num_decision_vars;
}
// Get the number of slack variables in the BS.
size_t sp_base_solution_get_num_slack_vars(const SPBaseSolution *spbs) {
    if (spbs == NULL) {
        printf("Error: Base Solution struct is NULL.\n");
        return 0;
    }
    return spbs->num_slack_vars;
}

// Get the number of variables in the BaseSolution struct that are in the base.
// Uses the var_in_base array (bool) to count the number of variables that are marked as being in the base.
size_t sp_base_solution_get_num_base_vars(const SPBaseSolution *spbs) {
    if (spbs == NULL) {
        printf("Error: Base Solution struct is NULL.\n");
        return 0;
    }
    size_t count = 0;
    size_t num_vars = 1 + spbs->num_decision_vars + spbs->num_slack_vars; // Total variables in BS definition: Z + decision vars + slack vars
    for (size_t i = 0; i < num_vars; i++) {
        if (spbs->vars_in_base[i]) {
            count++;
        }
    }
    return count;
}

// Get the total number of variables in the SPBaseSolution struct (Z + decision vars + slack vars).
// Useful for functions iterating through the struct's arrays.
size_t sp_base_solution_get_num_vars(const SPBaseSolution *spbs) {
    if (spbs == NULL) {
        printf("Error: Base Solution struct is NULL.\n");
        return 0;
    }
    // Total variables in BS: Z + decision vars + slack vars
    return 1 + spbs->num_decision_vars + spbs->num_slack_vars;
}

// Check if the specified variable (by tableau index) is in the BS.
bool sp_base_solution_is_vars_in_base(const SPBaseSolution *spbs, size_t var_index) {
    if (spbs == NULL) {
        printf("Error: Base Solution struct is NULL.\n");
        return false;
    }
    size_t num_vars_in_bfs = sp_base_solution_get_num_vars(spbs);
    if (var_index >= num_vars_in_bfs) {
        printf("Error: Variable index out of bounds when checking for Base Solution membership.\n");
        return false;
    }
    return spbs->vars_in_base[var_index];
}

// Get value of variable in the BS by its tableau index (0 for Z value).
// Returns -1 if the variable is not found in the BS or if the BS is NULL.
double sp_base_solution_get_var_value(const SPBaseSolution *spbs, size_t var_index) {
    if (spbs == NULL) {
        printf("Error: Base Solution struct is NULL.\n");
        return -1.0;
    }
    size_t num_vars_in_bfs = sp_base_solution_get_num_vars(spbs);
    if (var_index >= num_vars_in_bfs) {
        printf("Error: Variable index out of bounds when retrieving Base Solution variable value.\n");
        return -1.0;
    }
    return spbs->var_values[var_index];
}

// Check if the BS is degenerate (i.e., has one basic variable with value zero).
bool sp_base_solution_is_degenerate(const SPBaseSolution *spbs) {
    if (spbs == NULL) {
        printf("Error: Base Solution struct is NULL.\n");
        return false;
    }

    size_t num_vars = sp_base_solution_get_num_vars(spbs);
    size_t count_zero_basic_vars = 0;

    for (size_t i = 1; i < num_vars; i++) { // Start from index 1 to skip Z value
        if (spbs->vars_in_base[i] && spbs->var_values[i] == 0.0) {
            count_zero_basic_vars++;
        }
    }

    return count_zero_basic_vars > 0; // Return true if there is at least one basic variable with value zero
}

// Print the current state of the BS (Base Solution) for the provided SPBaseSolution.
// Name of decision variables are included in the output if included in the SPT's LP definition.
// Otherwise, pass NULL for dec_vars to print generic decision variable names (x1, x2, ...).
void sp_base_solution_print(const SPBaseSolution *spbs, const LPDecisionVars *dec_vars) {
    if (spbs == NULL) {
        printf("Error: Base Solution struct is NULL.\n");
        return;
    }
    if (spbs->num_decision_vars != dec_vars->length) {
        printf("Error: Mismatch between Base Solution decision variable count and provided decision variable names.\n");
        return;
    }

    size_t num_vars = sp_base_solution_get_num_vars(spbs);
    printf("--- Base Solution ---\n");
    for (size_t var = 0; var < num_vars; var++) {
        if (var == 0) {
            printf("Z: %0.4f\n", spbs->var_values[var]);
        } else if (spbs->vars_in_base[var]) {
            if (var <= spbs->num_decision_vars) {
                // Decision variable
                const char *var_name = dec_vars ? dec_vars->names[var - 1] : NULL;
                if (var_name) {
                    printf("%s: %0.4f\n", var_name, spbs->var_values[var]);
                } else {
                    printf("x%zu: %0.4f\n", var, spbs->var_values[var]);
                }
            } else {
                // Slack variable
                size_t slack_index = var - spbs->num_decision_vars;
                printf("slack-%zu: %0.4f\n", slack_index, spbs->var_values[var]);
            }
        }
    }
}
