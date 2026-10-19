// SPTableau.c

#include "SPTableau.h"

// Create the tableaul for the given LPDefinition struct. Returns a pointer to the created SPTableau struct.
SPTableau *sp_tableau_create(LPDefinition *lp_def) {
    if (lp_def == NULL) {
        printf("Error: LPDefinition struct is NULL.\n");
        return NULL;
    }

    SPTableau *tableau = (SPTableau *)malloc(sizeof(SPTableau));
    if (!tableau) {
        printf("Error: Memory allocation failed for SPTableau struct.\n");
        return NULL;
    }

    // Set up default values for the tableau struct field before initializing it.
    tableau->lp_def = lp_def;
    tableau->tableau = NULL;
    tableau->num_rows = tableau->num_cols = 0;
    tableau->num_decision_vars = tableau->num_constraints = 0;
    tableau->num_slack_vars = tableau->num_surplus_vars = tableau->num_artificial_vars = 0;
    tableau->multiple_solutions = false;
    tableau->is_unbounded = false;
    tableau->degeneration_level = 0;

    // Initialize the tableau and aborth if any step fails.
    if (!sp_tableau_init_baseline(tableau) ||
        !sp_tableau_init_objective_func_row(tableau) ||
        !sp_tableau_init_constraints_rows(tableau)) {
        printf("Error: Failed to initialize the tableau.\n");
        sp_tableau_free(tableau);
        return NULL;
    }

    return tableau;
}

// Helper initialization function: Create the baseline tableau given the LP definition's baseline information.
bool sp_tableau_init_baseline(SPTableau *tableau) {
    if (tableau == NULL || tableau->lp_def == NULL || tableau->lp_def->constraints == NULL) {
        printf("Error: Tableau or its LP definition is NULL.\n");
        return false;
    }

    // Initialize the tableau dimensions based on the LP definition
    tableau->num_decision_vars = tableau->lp_def->num_decision_vars;
    tableau->num_constraints = tableau->lp_def->num_constraints;

    tableau->num_slack_vars = 0;
    tableau->num_surplus_vars = 0; // For v2, if needed
    tableau->num_artificial_vars = 0; // For v2, if needed


    // Loop to calculate slack, surplus, and artificial variables based on the constraints
    for (size_t i = 0; i < tableau->num_constraints; i++) {
        switch (tableau->lp_def->constraints->types[i]) {
            case LE:
                tableau->num_slack_vars++;
                break;
            case GE:
                tableau->num_surplus_vars++;
                tableau->num_artificial_vars++;
                break;
            case EQ:
                tableau->num_artificial_vars++;
                break;
            default:    
                printf("Error: Unknown constraint type encountered during tableau initialization.\n");
                return false;
        }
    }

    // Rows in the tableau: one for each constraint + one for the objective function (row 0).
    // Columns in the tableau: one for the Z value (col. 0) + one for each variable (decision, slack, surplus, artificial) + one for the RHS value (last column).
    tableau->num_rows = 1 + tableau->num_constraints; // +1 for the objective function row
    tableau->num_cols = 2 + tableau->num_decision_vars + tableau->num_slack_vars + tableau->num_surplus_vars + tableau->num_artificial_vars; // +1 for the RHS value

    // Allocate memory for the tableau 2D array
    tableau->tableau = (double **)malloc(tableau->num_rows * sizeof(double *));
    if (!tableau->tableau) {
        printf("Error: Memory allocation failed for tableau rows.\n");
        return false;
    }
    // Allocate memory for each row (and column by extension) in the tableau
    for (size_t i = 0; i < tableau->num_rows; i++) {
        tableau->tableau[i] = (double *)calloc(tableau->num_cols, sizeof(double));
        if (!tableau->tableau[i]) {
            printf("Error: Memory allocation failed for tableau columns.\n");
            for (size_t row = 0; row < i; row++) {
                free(tableau->tableau[row]);
            }
            free(tableau->tableau);
            tableau->tableau = NULL;
            return false;
        }
    }
    return true;
}

// Helper initialization function: Set up the tableau's objective function row based on the LP definition's objective function.
// Must be called after sp_tableau_init_baseline.
bool sp_tableau_init_objective_func_row(SPTableau *tableau) {
    if (tableau == NULL || tableau->lp_def == NULL ||
        tableau->lp_def->objective_function == NULL ||
        tableau->lp_def->objective_function->coeffs == NULL) {
        printf("Error: Tableau or its LP definition or objective function is NULL.\n");
        return false;
    }

    // Initialize the objective function row (row 0) in the tableau
    // The first column (col. 0) is for the Z value, which is initialized to 1.
    tableau->tableau[0][0] = 1.0; // Z value

    // Fill in the coefficients of the decision variables in the objective function row
    // Objective function's coefficients enter negated into the tableau, regardless of LP scenario (maximization or minimization).
    for (size_t j = 0; j < tableau->num_decision_vars; j++) {
        tableau->tableau[0][1 + j] =
            -tableau->lp_def->objective_function->coeffs->coefficients[j]; // Negate for maximization
    }

    // Get indices for first slack, surplus, and artificial variables in the tableau
    size_t slack_start_col = 1 + tableau->num_decision_vars;   // Slack vars go after decision vars
    size_t surplus_start_col = slack_start_col + tableau->num_slack_vars;  // Suprlus vars go after slack vars (for v2)
    size_t artificial_start_col = surplus_start_col + tableau->num_surplus_vars; // Artificial vars go after surplus vars (for v2)

    for (size_t j = slack_start_col; j < artificial_start_col + tableau->num_artificial_vars; j++) {
        tableau->tableau[0][j] = 0.0;
    }

    // The last column (RHS value) in the objective function row is also initialized to 0
    tableau->tableau[0][tableau->num_cols - 1] = 0.0;
    return true;
}

// Helper function to add the constraints rows to the tableau. This function is called during the initialization of the tableau.
// Must be called after sp_tableau_init_baseline.
bool sp_tableau_init_constraints_rows(SPTableau *tableau) {
    if (tableau == NULL || tableau->lp_def == NULL || tableau->lp_def->constraints == NULL) {
        printf("Error: Tableau or its LP definition or constraints is NULL.\n");
        return false;
    }

    // Variables of each type occupy their own contiguous block of columns.
    size_t slack_start_col = 1 + tableau->num_decision_vars;
    size_t surplus_start_col = slack_start_col + tableau->num_slack_vars;
    size_t artificial_start_col = surplus_start_col + tableau->num_surplus_vars;

    // Counter to ensure that slack/surplus/artificial vars are placed in the 
    // correct columns for each constraint row.
    size_t slack_index = 0;
    size_t surplus_index = 0;
    size_t artificial_index = 0;

    for (size_t i = 0; i < tableau->num_constraints; i++) {
        // Column 0 is for the Z value, which is initialized to 0 for all constraint rows.
        tableau->tableau[1 + i][0] = 0.0;

        // Fill in the coefficients of the decision variables for each constraint row
        for (size_t j = 0; j < tableau->num_decision_vars; j++) {
            tableau->tableau[1 + i][1 + j] = tableau->lp_def->constraints->coeff_sets[i]->coefficients[j];
        }

        // Fill in the slack, surplus, and artificial variables based on the constraint type.
        switch (tableau->lp_def->constraints->types[i]) {
            case LE:
                tableau->tableau[1 + i][slack_start_col + slack_index++] = 1.0;
                break;
            case GE:
                tableau->tableau[1 + i][surplus_start_col + surplus_index++] = -1.0;
                tableau->tableau[1 + i][artificial_start_col + artificial_index++] = 1.0;
                break;
            case EQ:
                tableau->tableau[1 + i][artificial_start_col + artificial_index++] = 1.0;
                break;
            default:
                printf("Error: Unknown constraint type encountered during constraints rows initialization.\n");
                return false;
        }

        // Fill in the RHS value for each constraint row
        tableau->tableau[1 + i][tableau->num_cols - 1] = tableau->lp_def->constraints->rhs_values[i];
    }
    return true;
}

// Free the memory allocated for the SPTableau struct and its associated resources.
// Caller must ensure that the LPDefinition struct is freed separately if it was dynamically allocated.
void sp_tableau_free(SPTableau *tableau) {
    if (tableau == NULL) {
        return;
    }

    // Free the tableau's data
    if (tableau->tableau != NULL) {
        for (size_t i = 0; i < tableau->num_rows; i++) {
            free(tableau->tableau[i]);
        }
        free(tableau->tableau);
    }
    free(tableau);
}

// Returns a snapshot of the tableau's current state as a 2D array of doubles.
// To be used if intermediate tableau states need to be displayed.
// Caller is responsible for freeing the memory allocated for the snapshot after use.
double **sp_tableau_take_snapshot(const SPTableau *tableau) {
    if (tableau == NULL || tableau->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return NULL;
    }

    double **snapshot = malloc(tableau->num_rows * sizeof(*snapshot));
    if (!snapshot) {
        printf("Error: Memory allocation failed for tableau snapshot rows.\n");
        return NULL;
    }

    for (size_t i = 0; i < tableau->num_rows; i++) {
        snapshot[i] = malloc(tableau->num_cols * sizeof(*snapshot[i]));
        if (!snapshot[i]) {
            printf("Error: Memory allocation failed for tableau snapshot columns.\n");
            for (size_t row = 0; row < i; row++) {
                free(snapshot[row]);
            }
            free(snapshot);
            return NULL;
        }
        for (size_t j = 0; j < tableau->num_cols; j++) {
            snapshot[i][j] = tableau->tableau[i][j];
        }
    }
    return snapshot;
}

// Check if the specified column in the tableau is a base column (i.e., corresponds to a basic variable).
// Returns true if the column is a base column, false otherwise.
bool sp_tableau_is_base_column(const SPTableau *tableau, size_t col_index) {
    if (tableau == NULL || tableau->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return false;
    }
    if (col_index >= tableau->num_cols) {
        printf("Error: Column index out of bounds when checking for base column.\n");
        return false;
    }

    // Index check to ensure that the column index is a feasible base variable column
    // Base variables must be either decision or slack variables. 
    // Z column (index 0) and RHS column are not base variables.
    if (col_index < 1 || col_index >= (1 + tableau->num_decision_vars + tableau->num_slack_vars)) {
        return false; // Not a valid base variable column
    }

    // A base column has exactly one entry of 1 and all other entries are 0.
    size_t count_of_ones = 0;
    for (size_t i = 0; i < tableau->num_rows; i++) {
        if (tableau->tableau[i][col_index] == 1.0) {
            count_of_ones++;
        } else if (tableau->tableau[i][col_index] != 0.0) {
            return false; // If any entry is not 0 or 1, it's not a base column
        }
    }
    return count_of_ones == 1; // True if exactly one entry is 1
}

// Check if the specified variable (column) in the tableau is a base variable.
// Returns true if the variable is a base variable, false otherwise.
bool sp_tableau_is_base_variable(const SPTableau *tableau, size_t var_index) {
    if (tableau == NULL || tableau->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return false;
    }
    if (var_index >= tableau->num_cols) {
        printf("Error: Variable index out of bounds when checking for base variable.\n");
        return false;
    }
}

// Count the number of base variables in the tableau.
// Returns the count of base variables in the tableau.
size_t sp_tableau_count_base_variables(const SPTableau *tableau) {
    if (tableau == NULL || tableau->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return 0;
    }

    size_t base_variable_count = 0;
    for (size_t j = 1; j < tableau->num_cols - 1; j++) { // Exclude Z column (0) and RHS column (last)
        if (sp_tableau_is_base_column(tableau, j)) {
            base_variable_count++;
        }
    }
    return base_variable_count;
}

// Get the indices of the base variables in the tableau.
// Returns a dynamically allocated array of size_t containing the indices of the base variables.
// Caller is responsible for freeing the memory allocated for the array after use.
size_t *sp_tableau_get_base_vars_indices(const SPTableau *tableau) {
    if (tableau == NULL || tableau->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return NULL;
    }

    size_t base_variable_count = sp_tableau_count_base_variables(tableau);
    size_t *base_var_indices = malloc(base_variable_count * sizeof(size_t));
    if (!base_var_indices) {
        printf("Error: Memory allocation failed for base variable indices array.\n");
        return NULL;
    }

    size_t index = 0;
    for (size_t j = 1; j < tableau->num_cols - 1; j++) { // Exclude Z column (0) and RHS column (last)
        if (sp_tableau_is_base_column(tableau, j)) {
            base_var_indices[index++] = j;
        }
    }
    return base_var_indices;
}

// Get the value of the specified base variable in the tableau (by index). 
// The variable index should correspond to a base variable.
SPBaseVarInfo sp_tableau_get_base_var_value(const SPTableau *tableau, size_t index) {
    SPBaseVarInfo result = {0, 0, 0.0}; // Initialize with default values

    if (tableau == NULL || tableau->tableau == NULL) {
        printf("Error: Tableau or its data is NULL.\n");
        return result;
    }
    if (index >= tableau->num_cols) {
        printf("Error: Variable index out of bounds when getting base variable value.\n");
        return result;
    }
    if (index < 1 || index >= (1 + tableau->num_decision_vars + tableau->num_slack_vars)) {
        printf("Error: Variable index is not a valid base variable column.\n");
        return result;
    }
    if (!sp_tableau_is_base_column(tableau, index)) {
        printf("Error: Specified variable index does not correspond to a base variable.\n");
        return result;
    }

    // Determine the type of variable based on its index and set the appropriate field in the result struct.
    if (index < 1 + tableau->num_decision_vars) {
        result.decision_var_index = index - 1; // Adjust for Z column at index 0
    } else if (index < 1 + tableau->num_decision_vars + tableau->num_slack_vars) {
        result.slack_var_index = index - (1 + tableau->num_decision_vars); // Adjust for decision vars
    }

    // Find the row corresponding to the base variable to get the variable's value (RHS).
    for (size_t i = 1; i < tableau->num_rows; i++) {
        if (tableau->tableau[i][index] == 1.0) {
            result.value = tableau->tableau[i][tableau->num_cols - 1]; // RHS value
            return result;
        }
    }

    printf("Error: Base variable not found in the tableau.\n");
    return result; // Return default values if not found
}
