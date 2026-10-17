// Constraints.c

#include "Constraints.h"

const size_t MINIMUM_CONSTRAINTS = 2; // Minimum number of constraints
const size_t MAXIMUM_CONSTRAINTS = 25; // Maximum number of constraints


// Free the memory allocated for the coefficients array and resets the length to 0.
void coefficients_set_free(CoefficientsSet *set) {
    if (set != NULL) {
        free(set->coefficients);
        set->coefficients = NULL;
        set->length = 0;
    }
}


// Create a new Constraints struct with the given number of constraints and decision variables.
// Ensures that constraints have the correct number of coefficients for the decision variables.
Constraints *constraints_create(size_t constraints_count, size_t num_decision_vars) {
    if (constraints_count < MINIMUM_CONSTRAINTS || constraints_count > MAXIMUM_CONSTRAINTS) {
        printerr("Error: Invalid number of constraints specified. Must be between %zu and %zu.\n", MINIMUM_CONSTRAINTS, MAXIMUM_CONSTRAINTS);
        return NULL;
    }
    if (num_decision_vars == 0) {
        printerr("Error: Invalid number of decision variables specified.\n");
        return NULL;
    }
    if (num_decision_vars < MINIMUM_DECISION_VARS || num_decision_vars > MAXIMUM_DECISION_VARS) {
        printerr("Error: Invalid number of decision variables specified. Must be between %zu and %zu.\n", MINIMUM_DECISION_VARS, MAXIMUM_DECISION_VARS);
        return NULL;
    }
    Constraints *constraintList = (Constraints *)malloc(sizeof(Constraints));
    if (!constraintList) {
        return NULL;
    }

    constraintList->count = 0;
    constraintList->max_count = constraints_count;
    constraintList->num_decision_vars = num_decision_vars;

    // Allocate memory for the arrays that will store the constraint information.
    constraintList->coeffs = (double **)malloc(constraints_count * sizeof(double *));
    if (!constraintList->coeffs) {
        free(constraintList);
        return NULL;
    }
    for (size_t i = 0; i < constraints_count; i++) {
        constraintList->coeffs[i] = (double *)malloc(num_decision_vars * sizeof(double));
        if (!constraintList->coeffs[i]) {
            for (size_t j = 0; j < i; j++) {
                free(constraintList->coeffs[j]);
            }
            free(constraintList->coeffs);
            free(constraintList);
            return NULL;
        }
    }
    constraintList->types = (enum ConstraintType *)malloc(constraints_count * sizeof(enum ConstraintType));
    if (!constraintList->types) {
        for (size_t i = 0; i < constraints_count; i++) {
            free(constraintList->coeffs[i]);
        }
        free(constraintList->coeffs);
        free(constraintList);
        return NULL;
    }
    constraintList->rhs_values = (double *)malloc(constraints_count * sizeof(double));
    if (!constraintList->rhs_values) {
        for (size_t i = 0; i < constraints_count; i++) {
            free(constraintList->coeffs[i]);
        }
        free(constraintList->coeffs);
        free(constraintList->types);
        free(constraintList);
        return NULL;
    }

    return constraintList;
}

// Free the memory allocated for the Constraints struct.
// Pointer to decision variables should be freed separately if it was dynamically allocated.
void constraints_free(Constraints *constraintList) {
    if (constraintList) {
        for (size_t i = 0; i < constraintList->max_count; i++) {
            free(constraintList->coeffs[i]);    // Free each row of coefficients
        }
        free(constraintList->coeffs);   // Free the array of coefficient pointers
        free(constraintList->types);    // Free the array of constraint types
        free(constraintList->rhs_values);   // Free the array of RHS values
        free(constraintList);   // Free the struct itself
    }
}

// Add a new constraint to the Constraints struct. New constraint is added at the end of the list, and count is incremented.
void constraints_add_constraint(Constraints *constraintList, CoefficientSet coeffSet, 
    enum ConstraintType type, double rhs_value) {
    if (constraintList == NULL) {
        printerr("Error: Constraint list is NULL.\n");
        return;
    }
    if (constraintList->count >= constraintList->max_count) {
        printerr("Error: Cannot add more constraints. Maximum limit reached.\n");
        return;
    }
    if (coeffSet.length != constraintList->num_decision_vars) {
        printerr("Error: Count of coefficients in set does not match number of expected decision variables.\n");
        return;
    }
    if (coeffSet.coefficients == NULL) {
        printerr("Error: Coefficients array in CoefficientSet is NULL.\n");
        return;
    }
    if (type != LTE && type != EQ && type != GTE) {
        printerr("Error: Invalid constraint type specified.\n");
        return;
    }
    
    // Copy the coefficients from the provided CoefficientSet to the new constraint's coefficients array
    for (size_t i = 0; i < coeffSet.length; i++) {
        constraintList->coeffs[constraintList->count][i] = coeffSet.coefficients[i];
    }
    constraintList->types[constraintList->count] = type;
    constraintList->rhs_values[constraintList->count] = rhs_value;
    constraintList->count++;

}

// Set the coefficients for a specific constraint at the given index.
void constraints_set_coeffs(Constraints *constraintList, size_t index, 
    CoefficientSet newCoeffs, enum ConstraintType newType, double newRHS) {
    if (constraintList == NULL) {
        printerr("Error: Constraint list is NULL.\n");
        return;
    }
    if (index >= constraintList->count) {
        printerr("Error: Index out of bounds when setting coefficients for constraint.\n");
        return;
    }
    if (newCoeffs.length != constraintList->num_decision_vars) {
        printerr("Error: Count of new coefficients in set does not match number of expected decision variables.\n");
        return;
    }
    if (newCoeffs.coefficients == NULL) {
        printerr("Error: Array of new coefficients is NULL.\n");
        return;
    }
    if (newType != LTE && newType != EQ && newType != GTE) {
        printerr("Error: Invalid constraint type specified.\n");
        return;
    }

    // Overwrite the constraint's coefficients, type and RHS values at the specified index with the new values
    for (size_t i = 0; i < newCoeffs.length; i++) {
        constraintList->coeffs[index][i] = newCoeffs.coefficients[i];
    }
    constraintList->types[index] = newType;
    constraintList->rhs_values[index] = newRHS;
}

// Get the coefficients for a specific constraint at the given index.
// Returns a CoefficientsSet struct containing the coefficients and their count.
// Caller should free the coefficients array in the returned CoefficientsSet when done using it.
CoefficientsSet constraints_get_constraint_coeffs(const Constraints *constraintList, size_t index) {
    CoefficientsSet result = {0, NULL};
    if (constraintList == NULL) {
        printerr("Error: Constraint list is NULL.\n");
        return result;
    }
    if (index >= constraintList->count) {
        printerr("Error: Index out of bounds when getting coefficients for constraint.\n");
        return result;
    }

    double *coefficients = (double *)malloc(constraintList->num_decision_vars * sizeof(double));

    if (coefficients == NULL) {
        printerr("Error: Memory allocation failed for constraint coefficients.\n");
        return result;
    }

    for (size_t i = 0; i < constraintList->num_decision_vars; i++) {
        coefficients[i] = constraintList->coeffs[index][i];
    }

    result.length = constraintList->num_decision_vars;
    result.coefficients = coefficients;

    return result;
}

// Get the type of constraint at the specified index.
enum ConstraintType constraints_get_constraint_type(const Constraints *constraintList, size_t index) {
    if (constraintList == NULL) {
        printerr("Error: Constraint list is NULL.\n");
        return ERROR;
    }
    if (index >= constraintList->count) {
        printerr("Error: Index out of bounds when getting constraint type.\n");
        return ERROR;
    }
    return constraintList->types[index];
}

// Get the right-hand side (RHS) value of the constraint at the specified index.
double constraints_get_constraint_rhs(const Constraints *constraintList, size_t index) {
    if (constraintList == NULL) {
        printerr("Error: Constraint list is NULL.\n");
        return 0.0; // Return a default value or handle error as needed
    }
    if (index >= constraintList->count) {
        printerr("Error: Index out of bounds when getting constraint RHS value.\n");
        return 0.0; // Return a default value or handle error as needed
    }
    return constraintList->rhs_values[index];
}

// Print the details of a specific constraint at the given index for debugging purposes.
// DecisionVariable struct can be passed to display the coefficients alongside their corresponding variable names. If NULL, will print generic variable names.
void constraints_print_constraint_definiton(const Constraints *constraintList, size_t index, const DecisionVariables *decision_vars) {
    if (constraintList == NULL) {
        printerr("Error: Constraint list is NULL.\n");
        return;
    }
    if (index >= constraintList->count) {
        printerr("Error: Index out of bounds when printing constraint definition.\n");
        return;
    }
    if (decision_vars && decision_vars->size != constraintList->num_decision_vars) {
        printerr("Error: Set of decision variable names does not match the number of decision variables in the constraint list.\n");
        return;
    }
    if (decision_vars && decision_vars->names == NULL) {
        printerr("Error: Decision variable names array is NULL. Cannot be used for printing constraint definition.\n");
        return;
    }

    printf("Constraint %zu:\n", index);
    printf("Coefficients: ");

    // Flag to track printing of the first non-zero coefficient to handle formatting of operators (+/-)
    bool firstNonZeroCoeffPrinted = false; 

    for (size_t i = 0; i < constraintList->num_decision_vars; i++) {
        if (constraintList->coeffs[index][i] == 0) {
                continue;
        }
        // Print the coefficient with appropriate sign and formatting after the first non-zero coefficient has been printed.
        if (firstNonZeroCoeffPrinted) {
            printf("%c ", (constraintList->coeffs[index][i] >= 0) ? '+' : '-');
            
            if (constraintList->coeffs[index][i] < 0) {
                printf("%.2f", -constraintList->coeffs[index][i]);
            } else {
                printf("%.2f", constraintList->coeffs[index][i]);
            }
        }
        // Printing the first non-zero coefficient without a leading operator.
        else {
            if (constraintList->coeffs[index][i] < 0) {
                printf("-%.2f", -constraintList->coeffs[index][i]);
            } else {
                printf("%.2f", constraintList->coeffs[index][i]);
            }
            firstNonZeroCoeffPrinted = true;
        }

        // Print the corresponding decision variable name if available, otherwise print a generic variable name.
        if (decision_vars && decision_vars->names[i] != NULL) {
            printf(" * %s ", decision_vars->names[i]);
        } else {
            printf(" * x%zu ", i);
        }
    }
    switch (constraintList->types[index]) {
        case LTE:
            printf("<= %.2f\n", constraintList->rhs_values[index]);
            break;
        case EQ:
            printf("= %.2f\n", constraintList->rhs_values[index]);
            break;
        case GTE:
            printf(">= %.2f\n", constraintList->rhs_values[index]);
            break;
        default:
            printf("Unknown\n");
            break;
    }
}

// Print all constraints in the Constraints struct for debugging purposes.
// DecisionVariable struct can be passed to display the coefficients alongside their corresponding variable names. If NULL, will print generic variable names.
void constrainst_print_all_constraints(const Constraints *constraintList, const DecisionVariables *decision_vars) {
    if (constraintList == NULL) {
        printerr("Error: Constraint list is NULL.\n");
        return;
    }
    if (decision_vars && decision_vars->size != constraintList->num_decision_vars) {
        printerr("Error: Set of decision variable names does not match the number of decision variables in the constraint list.\n");
        return;
    }

    printf("All Constraints:\n");
    for (size_t i = 0; i < constraintList->count; i++) {
        constraints_print_constraint_definiton(constraintList, i, decision_vars);
    }
}