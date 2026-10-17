// Constraints.c

#include "Constraints.h"

const size_t MINIMUM_CONSTRAINTS = 2; // Minimum number of constraints
const size_t MAXIMUM_CONSTRAINTS = 25; // Maximum number of constraints


// Create a new Constraints struct with the given number of constraints and decision variables.
// Ensures that constraints have the correct number of coefficients for the decision variables.
Constraints *constraints_create(size_t constraints_count, size_t num_dec_vars) {
    if (constraints_count < MINIMUM_CONSTRAINTS || constraints_count > MAXIMUM_CONSTRAINTS) {
        printf("Error: Invalid number of constraints specified. Must be between %zu and %zu.\n", MINIMUM_CONSTRAINTS, MAXIMUM_CONSTRAINTS);
        return NULL;
    } 
    if (num_dec_vars < MINIMUM_DECISION_VARS || num_dec_vars > MAXIMUM_DECISION_VARS) {
        printf("Error: Invalid number of decision variables specified. Must be between %zu and %zu.\n", MINIMUM_DECISION_VARS, MAXIMUM_DECISION_VARS);
        return NULL;
    }

    Constraints *constraintList = (Constraints *)malloc(sizeof(Constraints));
    if (!constraintList) {
        printf("Error: Memory allocation failed for Constraints struct.\n");
        return NULL;
    }

    constraintList->count = 0;
    constraintList->max_count = constraints_count;
    constraintList->num_dec_vars = num_dec_vars;

    constraintList->coeff_sets = (Coefficients **)malloc(constraints_count * sizeof(Coefficients *));
    if (!constraintList->coeff_sets) {
        printf("Error: Memory allocation failed for coefficient sets.\n");
        free(constraintList);
        return NULL;
    }

    constraintList->types = (enum ConstraintType *)malloc(constraints_count * sizeof(enum ConstraintType));
    if (!constraintList->types) {
        printf("Error: Memory allocation failed for constraint types.\n");
        free(constraintList->coeff_sets);
        free(constraintList);
        return NULL;
    }

    constraintList->rhs_values = (double *)malloc(constraints_count * sizeof(double));
    if (!constraintList->rhs_values) {
        printf("Error: Memory allocation failed for RHS values.\n");
        free(constraintList->types);
        free(constraintList->coeff_sets);
        free(constraintList);
        return NULL;
    }

    return constraintList;

}

// Free the memory allocated for the Constraints struct.
// Pointer to decision variables should be freed separately if it was dynamically allocated.
void constraints_free(Constraints *constraintList) {
    if (constraintList) {
        for (size_t i = 0; i < constraintList->count; i++) {
            coefficients_free(constraintList->coeff_sets[i]); // Free each Coefficients struct
            free(constraintList->coeff_sets[i]); // Free the Coefficients struct itself
        }
        free(constraintList->coeff_sets); // Free the array of Coefficients pointers
        free(constraintList->types);      // Free the array of constraint types
        free(constraintList->rhs_values);  // Free the array of RHS values
        free(constraintList);              // Free the Constraints struct itself
    }
}

// Add a new constraint to the Constraints struct. New constraint is added at the end of the list, and count is incremented.
// Coefficients struct is used to pass the coefficients for the new constraint. 
// Caller is responsible for ensuring that the coefficients array in the Coefficients struct is valid and has the correct length.
void constraints_add_constraint(Constraints *constraintList, const Coefficients *coeffs, enum ConstraintType type, double rhs_value) {
    if (constraintList == NULL) {
        printf("Error: Constraints struct is NULL.\n");
        return;
    }
    if (coeffs == NULL) {
        printf("Error: Coefficients struct is NULL.\n");
        return;
    }
    if (coeffs->coefficients == NULL) {
        printf("Error: Array of coefficients in Coefficients struct is NULL.\n");
        return;
    }
    if (type < LE || type > GE) {
        printf("Error: Invalid constraint type specified.\n");
        return;
    }
    if (constraintList->count >= constraintList->max_count) {
        printf("Error: Cannot add more constraints. Maximum limit reached.\n");
        return;
    }
    if (coeffs->length != constraintList->num_dec_vars) {
        printf("Error: Coefficients length does not match the number of decision variables.\n");
        return;
    }

    Coefficients *newCoeffs = coefficients_create_from_array(coeffs->length, coeffs->coefficients);
    if (newCoeffs == NULL) {
        printf("Error: Memory allocation failed for new coefficients.\n");
        return;
    }

    // Store the coefficients, type, and RHS value for the new constraint
    constraintList->coeff_sets[constraintList->count] = newCoeffs;
    constraintList->types[constraintList->count] = type;
    constraintList->rhs_values[constraintList->count] = rhs_value;

    // Increment the count of constraints
    constraintList->count++;
}

// Update the coefficients, type, and RHS value of an existing constraint at the specified index.
// Can only be used to update an existing constraint. New constraints should be added using constraints_add_constraint.
// Caller is responsible for ensuring that the coefficients array in the Coefficients struct is valid and has the correct length.
void constraints_set_coeffs(Constraints *constraintList, size_t index, const Coefficients *newCoeffs, enum ConstraintType newType, double newRHS) {
    if (constraintList == NULL) {
        printf("Error: Constraints struct is NULL.\n");
        return;
    }
    if (index >= constraintList->count) {
        printf("Error: Index out of bounds when setting coefficients for constraint.\n");
        return;
    }
    if (newCoeffs == NULL) {
        printf("Error: Coefficients struct is NULL.\n");
        return;
    }
    if (newCoeffs->coefficients == NULL) {
        printf("Error: Array of coefficients in Coefficients struct is NULL.\n");
        return;
    }
    if (newCoeffs->length != constraintList->num_dec_vars) {
        printf("Error: Coefficients length does not match the number of decision variables.\n");
        return;
    }
    if (newType < LE || newType > GE) {
        printf("Error: Invalid constraint type specified.\n");
        return;
    }
    // Overwrite the existing coefficients, type, and RHS value for the constraint at the specified index.
    for (size_t i = 0; i < newCoeffs->length; i++) {
        constraintList->coeff_sets[index]->coefficients[i] = newCoeffs->coefficients[i];
    }
    constraintList->types[index] = newType;
    constraintList->rhs_values[index] = newRHS;
}

// Get the coefficients for a specific constraint at the given index.
// Returns a CoefficientsSet struct containing the coefficients and the count of entries.
// Caller should free the coefficients array in the returned CoefficientsSet when done using it.
Coefficients constraints_get_constraint_coeffs(const Constraints *constraintList, size_t index) {
    Coefficients result = {0, NULL}; // Initialize result with default values

    if (constraintList == NULL) {
        printf("Error: Constraints struct is NULL.\n");
        return result; 
    }
    if (index >= constraintList->count) {
        printf("Error: Index out of bounds when getting coefficients for constraint.\n");
        return result;
    }
    
    // Allocate memory to copy the coefficients for the specified constraint to return them by value.
    const Coefficients *constraintCoeffs = constraintList->coeff_sets[index];
    result.length = constraintCoeffs->length;
    result.coefficients = (double *)malloc(result.length * sizeof(double));

    if (!result.coefficients) {
        printf("Error: Memory allocation failed for coefficients array.\n");
        result.length = 0; // Reset length to 0 on failure
        return result;
    }
    
    memcpy(result.coefficients, constraintCoeffs->coefficients, result.length * sizeof(double));
    return result;
}

// Get the type of constraint at the specified index.
enum ConstraintType constraints_get_constraint_type(const Constraints *constraintList, size_t index) {
    if (constraintList == NULL) {
        printf("Error: Constraints struct is NULL.\n");
        return ERROR; // Return an error value for invalid constraint type
    }
    if (index >= constraintList->count) {
        printf("Error: Index out of bounds when getting constraint type.\n");
        return ERROR; // Return an error value for invalid constraint type
    }
    return constraintList->types[index];
}

// Get the right-hand side (RHS) value of the constraint at the specified index.
double constraints_get_constraint_rhs(const Constraints *constraintList, size_t index) {
   if (constraintList == NULL) {
        printf("Error: Constraints struct is NULL.\n");
        return 0.0; // Return a default value for invalid RHS
    }
    if (index >= constraintList->count) {
        printf("Error: Index out of bounds when getting constraint RHS value.\n");
        return 0.0; // Return a default value for invalid RHS
    }
    return constraintList->rhs_values[index];
}

// Print the details of a specific constraint at the given index for debugging purposes.
// DecisionVariable struct can be passed to display the coefficients alongside their corresponding variable names. If NULL, will print generic variable names.
// Caller is responsible for passing a valid DecisionVariables struct if they want to display variable names. If NULL, will print generic variable names.
void constraints_print_constraint_definiton(const Constraints *constraintList, size_t index, const DecisionVariables *decision_vars) {
    if (constraintList == NULL) {
        printf("Error: Constraints struct is NULL.\n");
        return;
    }
    if (index >= constraintList->count) {
        printf("Error: Index out of bounds when printing constraint definition.\n");
        return;
    }
    printf("Constraint %zu:  ", index + 1);
    bool firstNonZeroCoeffPrinted = false; // Flag to track if the first non-zero coefficient has been printed for formatting purposes.

    // Print the coefficients for the constraint
    coefficients_print(constraintList->coeff_sets[index], decision_vars); 

    // Print the constraint type (operator) and RHS value.
    switch (constraintList->types[index]) {
        case LE:
            printf("<= ");
            break;
        case EQ:
            printf("= ");
            break;
        case GE:
            printf(">= ");
            break;
        default:
            printf("Unknown\n");   
            return;     // Skip printing RHS value if constraint type is unknown.
    }
    printf("%.2f\n", constraintList->rhs_values[index]);
}

// Print all constraints in the Constraints struct for debugging purposes.
// DecisionVariable struct can be passed to display the coefficients alongside their corresponding variable names. If NULL, will print generic variable names.
void constrainst_print_all_constraints(const Constraints *constraintList, const DecisionVariables *decision_vars) {
    if (constraintList == NULL) {
        printf("Error: Constraints struct is NULL.\n");
        return;
    }
    printf("--- Constraints ---\n");
    for (size_t i = 0; i < constraintList->count; i++) {
        constraints_print_constraint_definiton(constraintList, i, decision_vars);
    }
    printf("\n");
}