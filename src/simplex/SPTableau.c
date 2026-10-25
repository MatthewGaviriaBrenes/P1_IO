// SPTableau.c

#include "SPTableau.h"
#include <math.h>

// Threshold for degeneration level to prevent infinite loops in the simplex algorithm.
const size_t SPT_DEGENERATION_THRESHOLD = 10; 

// Feasibility tolerance for checking if a solution satisfies the constraints of the LP problem.
// Needed to prevent false negatives due to floating-point precision errors during calculations.
static const double SPT_FEASIBILITY_TOLERANCE = 1e-9;

// Create the tableaul for the given LPDefinition struct. Returns a pointer to the created SPTableau struct.
SPTableau *sp_tableau_create(LPDefinition *lp_def) {
    if (lp_def == NULL) {
        printf("Error: LPDefinition struct is NULL.\n");
        return NULL;
    }

    SPTableau *spt = (SPTableau *)malloc(sizeof(SPTableau));
    if (!spt) {
        printf("Error: Memory allocation failed for SPTableau struct.\n");
        return NULL;
    }

    // Set up default values for the SPTableau struct field before initializing it.
    spt->lp_def = lp_def;
    spt->tableau = NULL;
    spt->num_rows = spt->num_cols = 0;
    spt->num_decision_vars = spt->num_constraints = 0;
    spt->num_slack_vars = spt->num_surplus_vars = spt->num_artificial_vars = 0;
    spt->degeneration_level = 0;

    // Initialize the tableau and aborth if any step fails.
    if (!sp_tableau_init_baseline(spt) ||
        !sp_tableau_init_objective_func_row(spt) ||
        !sp_tableau_init_constraints_rows(spt)) {
        printf("Error: Failed to initialize the tableau.\n");
        sp_tableau_free(spt);
        return NULL;
    }

    return spt;
}

// Helper initialization function: Create the baseline tableau given the LP definition's baseline information.
bool sp_tableau_init_baseline(SPTableau *spt) {
    if (spt == NULL || spt->lp_def == NULL || spt->lp_def->constraints == NULL) {
        printf("Error: Tableau or its LP definition is NULL.\n");
        return false;
    }

    // Initialize the tableau dimensions based on the LP definition
    spt->num_decision_vars = spt->lp_def->num_decision_vars;
    spt->num_constraints = spt->lp_def->num_constraints;

    spt->num_slack_vars = 0;
    spt->num_surplus_vars = 0; // For v2, if needed
    spt->num_artificial_vars = 0; // For v2, if needed


    // Loop to calculate slack, surplus, and artificial variables based on the constraints
    for (size_t i = 0; i < spt->num_constraints; i++) {
        switch (spt->lp_def->constraints->types[i]) {
            case LE:
                spt->num_slack_vars++;
                break;
            case GE:
                spt->num_surplus_vars++;
                spt->num_artificial_vars++;
                break;
            case EQ:
                spt->num_artificial_vars++;
                break;
            default:    
                printf("Error: Unknown constraint type encountered during tableau initialization.\n");
                return false;
        }
    }

    // Rows in the tableau: One for each constraint + one for the objective function (row 0).
    // Columns in the tableau: One for the Z value (col. 0) + one for each variable (decision, slack, surplus, artificial) + one for the RHS value (last column).
    spt->num_rows = 1 + spt->num_constraints;
    spt->num_cols = 2 + spt->num_decision_vars + spt->num_slack_vars + spt->num_surplus_vars + spt->num_artificial_vars;

    // Allocate memory for the tableau 2D array
    spt->tableau = (double **)malloc(spt->num_rows * sizeof(double *));
    if (!spt->tableau) {
        printf("Error: Memory allocation failed for tableau rows.\n");
        return false;
    }
    // Allocate memory for each row (and column by extension) in the tableau
    for (size_t i = 0; i < spt->num_rows; i++) {
        spt->tableau[i] = (double *)calloc(spt->num_cols, sizeof(double));
        if (!spt->tableau[i]) {
            printf("Error: Memory allocation failed for tableau columns.\n");
            for (size_t row = 0; row < i; row++) {
                free(spt->tableau[row]);
            }
            free(spt->tableau);
            spt->tableau = NULL;
            return false;
        }
    }
    return true;
}

// Helper initialization function: Set up the tableau's objective function row based on the LP definition's objective function.
// Must be called after sp_tableau_init_baseline.
bool sp_tableau_init_objective_func_row(SPTableau *spt) {
    if (spt == NULL ||  spt->lp_def == NULL ||
        spt->lp_def->objective_function == NULL ||
        spt->lp_def->objective_function->coeffs == NULL) {
        printf("Error: Tableau or its LP definition or objective function is NULL.\n");
        return false;
    }

    // Initialize the objective function row (row 0) in the tableau
    // The first column (col. 0) is for the Z value, which is initialized to 1.
    spt->tableau[0][0] = 1.0; // Z value

    // Fill in the coefficients of the decision variables in the objective function row
    // Objective function's coefficients enter negated into the tableau, regardless of LP scenario (maximization or minimization).
    for (size_t j = 0; j < spt->num_decision_vars; j++) {
        spt->tableau[0][1 + j] =
            -spt->lp_def->objective_function->coeffs->coefficients[j]; // Negate for maximization
    }

    // Get indices for first slack, surplus, and artificial variables in the tableau
    size_t slack_start_col = 1 + spt->num_decision_vars;   // Slack vars go after decision vars
    size_t surplus_start_col = slack_start_col + spt->num_slack_vars;  // Suprlus vars go after slack vars (for v2)
    size_t artificial_start_col = surplus_start_col + spt->num_surplus_vars; // Artificial vars go after surplus vars (for v2)

    for (size_t j = slack_start_col; j < artificial_start_col + spt->num_artificial_vars; j++) {
        spt->tableau[0][j] = 0.0;
    }

    // The last column (RHS value) in the objective function row is also initialized to 0
    spt->tableau[0][spt->num_cols - 1] = 0.0;
    return true;
}

// Helper function to add the constraints rows to the tableau. This function is called during the initialization of the tableau.
// Must be called after sp_tableau_init_baseline.
bool sp_tableau_init_constraints_rows(SPTableau *spt) {
    if (spt == NULL || spt->lp_def == NULL || spt->lp_def->constraints == NULL) {
        printf("Error: Tableau or its LP definition or constraints is NULL.\n");
        return false;
    }

    // Variables of each type occupy their own contiguous block of columns.
    size_t slack_start_col = 1 + spt->num_decision_vars;
    size_t surplus_start_col = slack_start_col + spt->num_slack_vars;
    size_t artificial_start_col = surplus_start_col + spt->num_surplus_vars;

    // Counter to ensure that slack/surplus/artificial vars are placed in the 
    // correct columns for each constraint row.
    size_t slack_index = 0;
    size_t surplus_index = 0;
    size_t artificial_index = 0;

    for (size_t i = 0; i < spt->num_constraints; i++) {
        // Column 0 is for the Z value, which is initialized to 0 for all constraint rows.
        spt->tableau[1 + i][0] = 0.0;

        // Fill in the coefficients of the decision variables for each constraint row
        for (size_t j = 0; j < spt->num_decision_vars; j++) {
            spt->tableau[1 + i][1 + j] = spt->lp_def->constraints->coeff_sets[i]->coefficients[j];
        }

        // Fill in the slack, surplus, and artificial variables based on the constraint type.
        switch (spt->lp_def->constraints->types[i]) {
            case LE:
                spt->tableau[1 + i][slack_start_col + slack_index++] = 1.0;
                break;
            case GE:
                spt->tableau[1 + i][surplus_start_col + surplus_index++] = -1.0;
                spt->tableau[1 + i][artificial_start_col + artificial_index++] = 1.0;
                break;
            case EQ:
                spt->tableau[1 + i][artificial_start_col + artificial_index++] = 1.0;
                break;
            default:
                printf("Error: Unknown constraint type encountered during constraints rows initialization.\n");
                return false;
        }

        // Fill in the RHS value for each constraint row
        spt->tableau[1 + i][spt->num_cols - 1] = spt->lp_def->constraints->rhs_values[i];
    }
    return true;
}

// Free the memory allocated for the SPTableau struct and its associated resources.
// Caller must ensure that the LPDefinition struct is freed separately if it was dynamically allocated.
void sp_tableau_free(SPTableau *spt) {
    if (spt == NULL) {
        return;
    }

    // Free the tableau's data
    if (spt->tableau != NULL) {
        for (size_t i = 0; i < spt->num_rows; i++) {
            free(spt->tableau[i]);
        }
        free(spt->tableau);
    }
    free(spt);
}

// Get the current BFS (Base Feasible Solution) from the tableau.
// Returns a pointer to the created SPBaseSolution struct.
// Caller is responsible for freeing the memory allocated for the SPBaseSolution struct after use.
SPBaseSolution *sp_tableau_get_base_sol(const SPTableau *spt) {
    if (spt == NULL || spt->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return NULL;
    }

    SPBaseSolution *spbs = sp_base_solution_create(spt->num_decision_vars, spt->num_slack_vars);
    if (!spbs) {
        printf("Error: Failed to create BFS from tableau.\n");
        return NULL;
    }

    // Add the Z value from the tableau to the BFS struct at index 0.
    spbs->vars_in_base[0] = true; // Z is always in the BFS
    spbs->var_values[0] = spt->tableau[0][spt->num_cols - 1]; 

    // Add the values of the decision and slack variables from the tableau to the BFS struct.
    size_t num_spbs_vars = 1 + spt->num_decision_vars + spt->num_slack_vars; 

    for (size_t col = 1; col < num_spbs_vars; col++) { 
        spbs->vars_in_base[col] = sp_tableau_is_base_column(spt, col);     // Mark if the variable is in the BFS
        spbs->var_values[col] = spbs->vars_in_base[col] ? sp_tableau_get_base_var_value(spt, col) : 0.0; // If not a base variable, set value to 0.0
    }
    return spbs;
}

// Validate a provided BFS against the tableau's constraints.
// Does not confirm optimal solution.
bool sp_tableau_validate_base(const SPTableau *spt, const SPBaseSolution *spbs) {
    if (spt == NULL || spt->tableau == NULL || spbs == NULL) {
        printf("Error: Tableau or its data or BFS is NULL.\n");
        return false;
    }

    // 1 - BFS variable counts must match the tableau's variable counts.
    if (spbs->num_decision_vars != spt->num_decision_vars 
        || spbs->num_slack_vars != spt->num_slack_vars) {
        printf("Error: BFS variable counts do not match tableau's variable counts.\n");
        return false;
    }

    // 2 - Value of all decision variables in the BFS must be non-negative.
    for (size_t col = 1; col <= spt->num_decision_vars; col++) {
        if (spbs->var_values[col] < -SPT_FEASIBILITY_TOLERANCE) {
            printf("Error: Decision variable at index %zu in BFS is negative.\n", col);
            return false;
        }
    }

    // 3 - Validate all constraints are satisfied by the BFS.

    LPConstraints *constraints = spt->lp_def->constraints;

    // Loop to validate each constraint against the BFS value. 
    // Constraint index (ci).
    for (size_t ci = 0; ci < spt->num_constraints; ci++) {
        // Evaluate the variables in the BFS against the constraint's coefficients and RHS value.
        double evaluated_result = 0.0;
        LPCoefficients *constraint_coeffs = constraints->coeff_sets[ci];
        // Loop through each decision variable in the BFS to evaluate the constraint.
        for (size_t j = 0; j < spt->num_decision_vars; j++) {
            evaluated_result += spbs->var_values[1 + j] * constraint_coeffs->coefficients[j];
        }

        // Compared evaluated value against the constraint's RHS value based on the constraint type (LE, GE, EQ).
        bool valid = false;
        switch (constraints->types[ci]) {
            case LE:
                valid = evaluated_result <= constraints->rhs_values[ci] + SPT_FEASIBILITY_TOLERANCE;
                break;
            // Validation for >= and == constraints will be reserved for v2.
            // case GE:
            //     valid = evaluated_result >= constraints->rhs_values[ci] - SPT_FEASIBILITY_TOLERANCE;
            //     break;
            // case EQ:
            //     valid = fabs(evaluated_result - constraints->rhs_values[ci]) <= SPT_FEASIBILITY_TOLERANCE;
            //     break;
            default:
                printf("Error: Unknown constraint type encountered during BFS validation.\n");
                return false;
        }
        if (!valid) {
            printf("Error: Constraint %zu is not satisfied.\n", (ci + 1));
            return false;
        }
    }
    return true;
}

// Returns a snapshot of the tableau's current state as a 2D array of doubles.
// To be used if intermediate tableau states need to be displayed.
// Caller is responsible for freeing the memory allocated for the snapshot after use.
double **sp_tableau_take_snapshot(const SPTableau *spt) {
    if (spt == NULL || spt->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return NULL;
    }

    double **snapshot = malloc(spt->num_rows * sizeof(*snapshot));
    if (!snapshot) {
        printf("Error: Memory allocation failed for tableau snapshot rows.\n");
        return NULL;
    }

    for (size_t i = 0; i < spt->num_rows; i++) {
        snapshot[i] = malloc(spt->num_cols * sizeof(*snapshot[i]));
        if (!snapshot[i]) {
            printf("Error: Memory allocation failed for tableau snapshot columns.\n");
            for (size_t row = 0; row < i; row++) {
                free(snapshot[row]);
            }
            free(snapshot);
            return NULL;
        }
        for (size_t j = 0; j < spt->num_cols; j++) {
            snapshot[i][j] = spt->tableau[i][j];
        }
    }
    return snapshot;
}

// Check if the specified column in the tableau is a base column (i.e., corresponds to a basic variable).
// Returns true if the column is a base column, false otherwise.
bool sp_tableau_is_base_column(const SPTableau *spt, size_t col_index) {
    if (spt == NULL || spt->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return false;
    }
    if (col_index >= spt->num_cols) {
        printf("Error: Column index out of bounds when checking for base column.\n");
        return false;
    }

    // Index check to ensure that the column index is a feasible base variable column
    // Base variables must be either decision or slack variables. 
    // Z column (index 0) and RHS column are not base variables.
    if (col_index < 1 || col_index >= (1 + spt->num_decision_vars + spt->num_slack_vars)) {
        return false; // Not a valid base variable column
    }

    // A base column has exactly one entry of 1 and all other entries are 0.
    size_t count_of_ones = 0;
    for (size_t i = 0; i < spt->num_rows; i++) {
        if (spt->tableau[i][col_index] == 1.0) {
            count_of_ones++;
        } else if (spt->tableau[i][col_index] != 0.0) {
            return false; // If any entry is not 0 or 1, it's not a base column
        }
    }
    return count_of_ones == 1; // True if exactly one entry is 1
}

// Check if the specified variable (by tableau index) in the tableau is a base variable.
// Returns true if the variable is a base variable, false otherwise.
bool sp_tableau_is_base_variable(const SPTableau *spt, size_t var_index) {
    if (spt == NULL || spt->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return false;
    }
    if (var_index >= spt->num_cols) {
        printf("Error: Variable index out of bounds when checking for base variable.\n");
        return false;
    }
    // A base variable corresponds to a base column in the tableau.
    return sp_tableau_is_base_column(spt, var_index);
}

// Get the value of a base variable.
// Return -1 if the variable is not a base variable or if the tableau is NULL. Otherwise, return the value of the base variable.
double sp_tableau_get_base_var_value(const SPTableau *spt, size_t var_index) {
    if (spt == NULL || spt->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return -1.0;
    }
    if (var_index >= spt->num_cols) {
        printf("Error: Variable index out of bounds when getting base variable value.\n");
        return -1.0;
    }

    // Check if the specified variable is a base variable
    if (!sp_tableau_is_base_column(spt, var_index)) {
        printf("Error: Specified variable is not a base variable.\n");
        return -1.0;
    }

    // Find the row corresponding to the base variable and return its value from the RHS column
    for (size_t i = 1; i < spt->num_rows; i++) { // Start from row 1 to skip the objective function row
        if (spt->tableau[i][var_index] == 1.0) {
            return spt->tableau[i][spt->num_cols - 1]; // Return the value from the RHS column
        }
    }
    return -1.0; // Should not reach here if the variable is a base variable
}

// Get the current value of Z in the tableau (row 0, column 0).
double sp_tableau_get_z_value(const SPTableau *spt) {
    if (spt == NULL || spt->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return 0.0; // Return 0.0 as a default value in case of error
    }
    return spt->tableau[0][spt->num_cols - 1]; // Return the value of Z from the last column of the objective function row
}

// Get the next variable entering the basis (pivot column) based on the current tableau state.
// Returns the index of the pivot column, or 0 if no valid pivot column is found (solution found).
size_t sp_tableau_get_pivot_column(const SPTableau *spt) {
    if (spt == NULL || spt->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return 0;
    }

    // Initialize pivot column to zero (Z column), will be updated to the index of the pivot column.
    // If no valid pivot column is found, assume that the solution is optimal and return 0.
    size_t select_pivot_col = 0; 
    double best_value = 0.0; 

    for (size_t col_index = 1; col_index < spt->num_cols - 1; col_index++) { // Exclude Z column (0) and RHS column (last)
        double value = spt->tableau[0][col_index];

        // If the coefficient is zero and no other pivot column has been found yet, 
        // consider it as a potential pivot column for multiple solutions unless it is already in the base.
        if (value == 0.0) {
            if (!sp_tableau_is_base_column(spt, col_index)) {
                select_pivot_col = col_index; 
            }
        }
        // Maximization: Look for the most negative coefficient in the objective function row (row 0).
        else if (spt->lp_def->objective_function->type == MAXIMIZE) {
            if (value < best_value) {
                best_value = value;
                select_pivot_col = col_index;
            }
        } 
        // Minimization: Look for the most positive coefficient in the objective function row (row 0).
        else {
            if (value > best_value) {
                best_value = value;
                select_pivot_col = col_index;
            }
        }
    }
    return select_pivot_col; // Returns 0 if no negative coefficients found (optimal solution reached)
}

// Calculate minimum ratio for the incoming variable (pivot column) to determine the leaving variable (pivot row).
// Returns an array of doubles containing the minimum ratios for each tableau row (including objetive row) for indexing purposes.
// Objective row defaults to -1 (not selectable).
// If the pivot column value is negative or zero, the ratio is set to -1 (indicating no valid ratio).
double *sp_tableau_calculate_min_ratios(const SPTableau *spt, size_t pivot_col_index) {
    if (spt == NULL || spt->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return NULL;
    }
    if (pivot_col_index >= spt->num_cols) {
        printf("Error: Pivot column index out of bounds when calculating minimum ratios.\n");
        return NULL;
    }

    double *min_ratios = malloc(spt->num_rows * sizeof(double));
    if (!min_ratios) {
        printf("Error: Memory allocation failed for minimum ratios array.\n");
        return NULL;
    }

    for (size_t row = 0; row < spt->num_rows; row++) {
        // First row (row 0) is the objective function row, default to ratio -1.0 to skip it.
        if (row == 0){
            min_ratios[row] = -1.0;
            continue;
        }
        // For other rows, calculate the minimum ratio.
        double pivot_col_value = spt->tableau[row][pivot_col_index];
        double rhs_value = spt->tableau[row][spt->num_cols - 1];

        // If the pivot column value is positive, calculate the ratio; otherwise, set it to -1 (indicating no valid ratio).
        if (pivot_col_value > 0) {
            min_ratios[row] = rhs_value / pivot_col_value;
        } else {
            min_ratios[row] = -1.0; // Invalid ratio (negative or division by zero)
        }
    }
    return min_ratios;
}

// Get the next variable leaving the basis (pivot row) based on the current tableau state.
// Must be called after sp_tableau_get_pivot_column and sp_tableau_calculate_min_ratios.
// Returns the index of the pivot row, or 0 if no valid pivot row is found (unbound problem).
size_t sp_tableau_get_pivot_row(const SPTableau *spt, size_t pivot_col_index, double *min_ratios) {
    if (spt == NULL || spt->tableau == NULL || min_ratios == NULL) {
        printf("Error: Tableau, its data, or minimum ratios array is NULL.\n");
        return SIZE_MAX;
    }
    // Ensure that the pivot column index is within the valid range (no Z or RHS columns).
    if (pivot_col_index < 1 || pivot_col_index >= spt->num_cols - 1) {
        printf("Error: Pivot column index out of bounds when getting pivot row.\n");
        return SIZE_MAX;
    }

    size_t pivot_row_index = 0; // Initialize to 0 to indicate no valid pivot row found.
    double min_ratio = -1.0; // Initialize to -1 to find the minimum positive ratio

    for (size_t row = 0; row < spt->num_rows; row++) {
        double new_ratio = min_ratios[row];
        // Only consider valid ratios (non-negative)
        if (new_ratio >= 0) {
            if (min_ratio < 0 || new_ratio < min_ratio) {
                min_ratio = new_ratio;
                pivot_row_index = row;
            }
            // If two rows have the same minimum ratio, apply tie-breaking rules to select the pivot row.
            if (min_ratio == new_ratio) {
                double current_row_coeff  = spt->tableau[pivot_row_index][pivot_col_index];
                double entering_row_coeff  = spt->tableau[row][pivot_col_index];
                pivot_row_index = (entering_row_coeff < current_row_coeff) ? row : pivot_row_index;
            }

        }
    }
    return pivot_row_index;
}

// Canonize the pivot row in the tableau to ensure a pivot value of 1 in the pivot cell, and updates the pivot row accordingly.
void sp_tableau_canonize_pivot_row(SPTableau *spt, size_t pivot_row_index, size_t pivot_col_index) {
    if (spt == NULL || spt->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return;
    }
    if (pivot_row_index >= spt->num_rows || pivot_col_index >= spt->num_cols) {
        printf("Error: Pivot row or column index out of bounds when normalizing pivot row.\n");
        return;
    }
    double pivot_value = spt->tableau[pivot_row_index][pivot_col_index];
    if (pivot_value == 0.0) {
        printf("Error: Pivot value is zero, cannot normalize pivot row.\n");
        return;
    }
    if (pivot_value == 1.0) {
        return; // Already canonized, no need to normalize again.
    }
    

    // Get normalization factor to ensure a pivot value of 1 in the pivot cell
    double normalization_factor = 1.0 / pivot_value;

    // Normalize the pivot row by dividing each element in the row by the pivot value
    for (size_t pcol = 1; pcol < spt->num_cols; pcol++) {
        spt->tableau[pivot_row_index][pcol] *= normalization_factor;
    }
}

// Canonize the non-pivot rows to ensure that all other entries in the pivot column are zero.
// Must be called after sp_tableau_canonize_pivot_row. 
void sp_tableau_canonize_nonpivot_rows(SPTableau *spt, size_t pivot_row_index, size_t pivot_col_index) {
    if (spt == NULL || spt->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return;
    }
    if (pivot_row_index < 1 || pivot_row_index >= spt->num_rows) {
        printf("Error: Pivot row index out of bounds when normalizing non-pivot rows.\n");
        return;
    }
    if (pivot_col_index < 1 || pivot_col_index >= spt->num_cols - 1) {
        printf("Error: Pivot column index out of bounds when normalizing non-pivot rows.\n");
        return;
    }

    // Normalize each non-pivot row to ensure that all other entries in the pivot column are zero.
    for (size_t np_row = 0; np_row < spt->num_rows; np_row++) {
        if (np_row == pivot_row_index) {
            continue;   // Skip the pivot row
        }
        // If pivot column value in non-pivot row is zero, skip the row (already canonized).
        if (spt->tableau[np_row][pivot_col_index] == 0) {
            continue;
        }

        // Elimination factor: Get negative of the pivot column value in the non-pivot row to eliminate it (turn it to zero).
        // Also used to scale the pivot row for elimination in the non-pivot row.
        double eliminiation_factor = -spt->tableau[np_row][pivot_col_index];

        // Add the scaled pivot row to the non-pivot row to eliminate the pivot column value in the non-pivot row.
        for (size_t np_col = 1; np_col < spt->num_cols; np_col++) {
            spt->tableau[np_row][np_col] += eliminiation_factor * spt->tableau[pivot_row_index][np_col];
        }
    }
}

// Perform a single step of the simplex algorithm on the tableau.
enum SPTableauStatus sp_tableau_solve_step(SPTableau *spt) {
    if (spt == NULL || spt->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return SPT_INVALID_STATE;
    }

    // Do not allow the simplex algorithm to continue if the degeneration level exceeds a threshold.
    if (spt->degeneration_level >= SPT_DEGENERATION_THRESHOLD) {
        printf("Error: Severe degeneration detected. Problem is stuck in a degenerate state and cannot be resolved.\n");
        return SPT_DEGENERATED_LOOP; // Degeneration level exceeded threshold, problem is stuck.
    }

    // Step 1 - Get the pivot column (variable entering the base).
    // If no valid pivot column is found, validate current base solution to confirm if valid (BFS) and optimal solution.
    size_t pivot_col_index = sp_tableau_get_pivot_column(spt);
    
    if (pivot_col_index == 0) {
        SPBaseSolution *spbs = sp_tableau_get_base_sol(spt);
        if (spbs == NULL) {
            printf("Error: Failed to retrieve current base solution for validation.\n");
            return SPT_INVALID_STATE; // Failed to retrieve BFS, cannot confirm optimal solution.
        }
        bool validBFS = sp_tableau_validate_base(spt, spbs);
        if (!validBFS) {
            printf("Error: Current base solution is invalid. Cannot proceed with Simplex step.\n");
            free(spbs);
            return SPT_INVALID_STATE; // Current BFS is invalid, cannot proceed.
        }
        printf("Optimal solution reached. No valid pivot column found.\n");
        free(spbs);
        return SPT_OPTIMAL_SOLUTION_FOUND; // No valid pivot column found, optimal solution reached.
    }
    
    // Step 2 - Find the variable leaving the base (pivot row) based on the minimum ratio test.
    // If no valid pivot row is found, the problem is unbounded.
    double *min_ratios = sp_tableau_calculate_min_ratios(spt, pivot_col_index);

    if (min_ratios == NULL) {
        printf("Error: Failed to allocate memory to calculate minimum ratios for pivot column.\n");
        return SPT_INVALID_STATE; //
    }

    size_t pivot_row_index = sp_tableau_get_pivot_row(spt, pivot_col_index, min_ratios);

    if (pivot_row_index == 0) {
        printf("Unbounded problem detected. No valid pivot row found.\n");
        free(min_ratios);
        return SPT_UNBOUNDED_PROBLEM; 
    }
    
    // If entering variable has a ratio of zero, the problem is degenerate. Increment degeneration level and continue to next step.
    // Otherwise, reset degeneration level to zero if not degenerate (or degeneration was mitigated).
    if (min_ratios[pivot_row_index] == 0.0) {
        printf("Warning: Degeneration detected.\n");
        spt->degeneration_level++;
    } else {
        if (spt->degeneration_level > 0) {
            printf("Degeneration mitigated. Resetting degeneration level.\n");
        }
        spt->degeneration_level = 0;
    }

    free(min_ratios);

    // Step 3 - Canonize the pivot row to ensure a pivot value of 1 in the pivot cell and update pivot row.
    sp_tableau_canonize_pivot_row(spt, pivot_row_index, pivot_col_index);
    // Step 4 - Canonize the non-pivot rows to ensure identity matrix for variable entering the base (pivot column).
    sp_tableau_canonize_nonpivot_rows(spt, pivot_row_index, pivot_col_index);

    // Step 5 - Check if a new variable can enter the base (pivot column) in a future step.

    // Get current base to confirm if it is a potentially valid BFS after the pivot operation and potential optimal solution.
    // Return error state if cannot acquire the current base solution for validation.
    SPBaseSolution *spbs = sp_tableau_get_base_sol(spt);
    if (spbs == NULL) {
        printf("Error: Failed to retrieve BFS for validation after pivot operation.\n");
        return SPT_INVALID_STATE; // Failed to retrieve BFS, cannot validate.
    }
    bool valid_bfs = sp_tableau_validate_base(spt, spbs);

    // Check if another variable can enter the base in future steps.
    // If no new variable can enter the base, confirm if the current BFS is valid, thus confirming an optimal solution has been reached.
    // If a new variable can enter the base, check if multiple optimal solutions exist (coefficient is zero in itsobjective row).
    size_t future_pivot_col_index = sp_tableau_get_pivot_column(spt);
    if (future_pivot_col_index == 0) {
        if (valid_bfs) {
            printf("Optimal solution reached. No new variable can enter the BFS.\n");
            free(spbs);
            return SPT_OPTIMAL_SOLUTION_FOUND; 
        } else {
            printf("Error: Current BFS is invalid. Cannot confirm optimal solution.\n");
            free(spbs);
            return SPT_INVALID_STATE;
        }
    } 
    if (spt->tableau[0][future_pivot_col_index] == 0.0 && valid_bfs) {
        printf("Multiple solutions detected. Non-base variable has coefficient zero in the objective function row.\n");
        free(spbs);
        return SPT_MULTPILE_SOLUTIONS_DETECTED; 
    } 

    // If no optimal solution has been reached yet, check degeneration level to determine if the problem is stuck in a degenerate state.
    if (spt->degeneration_level >= SPT_DEGENERATION_THRESHOLD) {
        printf("Error: Severe degeneration detected. Problem is stuck in a degenerate state and cannot be resolved.\n"); 
        free(spbs);
        return SPT_DEGENERATED_LOOP;
    }
    if (spt->degeneration_level > 0) {
        printf("Warning: Current degeneration level: %zu\n", spt->degeneration_level);
        free(spbs);
        return SPT_DEGENERATED_PROBLEM; 
    }

    // No optimal solution has been reached yet and the problem is not degenerate.
    free(spbs);
    return SPT_STANDBY; 
}

// Print the current state of the tableau to the console for debugging and visualization purposes.
void sp_tableau_print_tableau(const SPTableau *spt) {
    if (spt == NULL || spt->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return;
    }

    size_t slack_var_count = 1;
    size_t surplus_var_count = 1;
    size_t artificial_var_count = 1;

    // Printing column headers
    printf("Z\t");
    for (size_t var_index = 1; var_index < spt->num_cols - 1; var_index++) {
        // Printing decision variable names (first four characters)
        if (var_index <= spt->num_decision_vars) {
            // Get the name of the decision variable and truncate it to the first four characters
            char *short_var_name = strndup(spt->lp_def->decision_variables->names[var_index - 1], 4); // Get first four characters of the variable name
            printf("%s\t", short_var_name);
            free(short_var_name);
        }
        // Printing slack variable names (s1, s2, s3, ...)
        else if (var_index <= spt->num_decision_vars + spt->num_slack_vars) {
            printf("s%zu\t", slack_var_count++);
        }
        // Print surplus variable names (e.g., e1, e2, e3, ...) for v2 if needed
        else if (var_index <= spt->num_decision_vars + spt->num_slack_vars + spt->num_surplus_vars) {
            printf("e%zu\t", surplus_var_count++);
        }
        // Print artificial variable names (e.g., a1, a2, a3, ...) for v2 if needed
        else {
            printf("a%zu\t", artificial_var_count++);
        }
    }
    printf("RHS\n");

    // Printing values
    for (size_t row = 0; row < spt->num_rows; row++) {
        for (size_t col = 0; col < spt->num_cols; col++) {
            // Format value output as integer if it is a whole number, otherwise print with 4 decimal places
            if (spt->tableau[row][col] == (int)spt->tableau[row][col]) {
                printf("%d", (int)spt->tableau[row][col]);
            } else {
                printf("%0.4f", spt->tableau[row][col]);
            }
            // Add tab spacing between columns, except for the last column
            if (col < spt->num_cols - 1) {
                printf("\t");
            }
        }
        printf("\n");
    }
}

// Print the current state of the BS (Base Solution) for the provided SPTableau.
// Name of decision variables are included in the output if included in the SPT's LP definition.
void sp_tableau_print_base(const SPTableau *spt) {
    if (spt == NULL) {
        printf("Error: Tableau or base solution is NULL.\n");
        return;
    }
    if (!spt->lp_def || !spt->lp_def->decision_variables) {
        printf("Error: LP definition or decision variables are NULL.\n");
        return;
    }

    SPBaseSolution *spbs = sp_tableau_get_base_sol(spt);
    if (!spbs) {
        printf("Error: Failed to retrieve current base solution for printing.\n");
        return;
    }
    sp_base_solution_print(spbs, spt->lp_def->decision_variables);
    
    bool is_bfs = sp_tableau_validate_base(spt, spbs);
    printf("--- Is BFS?: %s ---\n", (is_bfs ? "Yes" : "No"));

    free(spbs);
}