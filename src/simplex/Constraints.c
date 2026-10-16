#include "Constraints.h"

// Create a new Constraints struct with the given number of constraints and decision variables.
// Ensures that constraints have the correct number of coefficients for the decision variables.
Constraints *constraints_create(size_t max_constraints, size_t max_decision_vars) {
    Constraints *newList = (Constraints *)malloc(sizeof(Constraints));
    if (!newList) {
        fprintf(stderr, "Memory allocation failed for Constraints struct.\n");
        exit(EXIT_FAILURE);
    }

    newList->count = 0;
    newList->max_count = max_constraints;
    newList->max_decision_vars = max_decision_vars;
    newList->constraints = NULL;

    // Allocate memory for the array of constraints. Free memory if allocation fails.
    newList->constraints = (Constraint *)malloc(max_constraints * sizeof(Constraint));
    if (!newList->constraints) {
        fprintf(stderr, "Memory allocation failed for constraints array.\n");
        free(newList);
        exit(EXIT_FAILURE);
    }

    return newList;
}

// Free the memory allocated for the Constraints struct.
void constraints_free(Constraints *constraintList) {
    if (constraintList) {
        // Free each individual constraint's coefficients
        for (size_t i = 0; i < constraintList->count; i++) {
            free(constraintList->constraints[i].dec_vars_coeffs);
        }
        // Free the array of constraints
        free(constraintList->constraints);
        // Free the Constraints struct itself
        free(constraintList);
    }
}

// Add a new constraint to the Constraints struct.
void constraints_add_constraint(Constraints *constraintList,const double *dec_vars_coeffs, enum ConstraintType type, double rhs) {
    if (constraintList == NULL || dec_vars_coeffs == NULL) {
        fprintf(stderr, "Invalid constraint input.\n");
        return;
    }

    if (constraintList->count >= constraintList->max_count) {
        fprintf(stderr, "Cannot add more constraints. Maximum limit reached.\n");
        return;
    }

    Constraint *constraint = &constraintList->constraints[constraintList->count];

    constraint->type = type;
    constraint->rhs = rhs;
    constraint->dec_vars_coeffs = malloc(
        constraintList->max_decision_vars *
        sizeof *constraint->dec_vars_coeffs
    );

    if (constraint->dec_vars_coeffs == NULL) {
        fprintf(stderr, "Memory allocation failed for coefficients.\n");
        return;
    }

    for (size_t i = 0; i < constraintList->max_decision_vars; i++) {
        constraint->dec_vars_coeffs[i] = dec_vars_coeffs[i];
    }

    constraintList->count++;
}

// Set a constraint at the specified index in the Constraints struct.
void constraints_set_constraint(Constraints *constraintList, size_t index, const double *dec_vars_coeffs, enum ConstraintType type, double rhs) {
    if (constraintList == NULL || dec_vars_coeffs == NULL) {
        fprintf(stderr, "Invalid constraint input.\n");
        return;
    }

    if (index >= constraintList->count) {
        fprintf(stderr, "Index out of bounds. Cannot set constraint at index %zu.\n", index);
        return;
    }

    Constraint *constraint = &constraintList->constraints[index];

    // Free the existing coefficients array before setting new values
    free(constraint->dec_vars_coeffs);

    constraint->type = type;
    constraint->rhs = rhs;
    constraint->dec_vars_coeffs = malloc(
        constraintList->max_decision_vars *
        sizeof *constraint->dec_vars_coeffs
    );

    if (constraint->dec_vars_coeffs == NULL) {
        fprintf(stderr, "Memory allocation failed for new constraint's decision variables' coefficients.\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < constraintList->max_decision_vars; i++) {
        constraint->dec_vars_coeffs[i] = dec_vars_coeffs[i];
    }
}

// Get a constraint at the specified index in the Constraints struct.
Constraint constraints_get_constraint(const Constraints *constraintList, size_t index) {
    if (constraintList == NULL) {
        fprintf(stderr, "Invalid constraint list input.\n");
        return;
    }

    if (index >= constraintList->count) {
        fprintf(stderr, "Index out of bounds. Cannot get constraint at index %zu.\n", index);
        return;
    }
    return constraintList->constraints[index];
}

void constraints_print_constraint(const Constraints *constraintList, size_t index) {
    if (constraintList == NULL) {
        fprintf(stderr, "Invalid constraint list input.\n");
        return;
    }

    if (index >= constraintList->count) {
        fprintf(stderr, "Index out of bounds. Cannot print constraint at index %zu.\n", index);
        return;
    }

    Constraint *constraint = &constraintList->constraints[index];

    printf("Constraint %zu: ", index);
    for (size_t i = 0; i < constraintList->max_decision_vars; i++) {
        printf("%lf ", constraint->dec_vars_coeffs[i]);
    }
    printf("%s %lf\n", (constraint->type == LTE) ? "<=" : "Unknown", constraint->rhs);
}



