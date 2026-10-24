// SPTest.c
#include "SPTest.h"

// Run for a simple case of a given LP problem using Simplex.
// Baseline run for min/max LP cases, expected to find single solution.
void sp_test_simple_case(LPDefinition *lp_def) {
    if (lp_def == NULL) {
        printf("Error: LPDefinition is NULL. Cannot run Simplex test.\n");
        return;
    }

    printf("Printing LP definition...\n");
    lp_definition_print(lp_def);

    printf("Creating Simplex tableau for the given LP definition...\n");
    // Create SPTableau from LPDefinition
    SPTableau *spt = sp_tableau_create(lp_def);
    if (spt == NULL) {
        printf("Error: Failed to create SPTableau from LPDefinition.\n");
    }

    // Print the initial tableau
    printf("Initial Tableau:\n");
    sp_tableau_print_tableau(spt);

    // Solve the LP problem step by step
    size_t step_count = 1;
    enum SPTableauStatus spt_status = SPT_STANDBY;

    // Run the Simplex tableau's step solve function to iterate through the tableau until an optimal solution is found or an error occurs.
    while (spt_status == SPT_STANDBY) {
        printf("\nStep %zu:\n", step_count);
        spt_status = sp_tableau_solve_step(spt);
        sp_tableau_print_tableau(spt);
        printf("\n");
        step_count++;
    }

    // Check the final status of the tableau after solving steps.
    switch (spt_status) {
        case SPT_OPTIMAL_SOLUTION_FOUND:
            printf("\nOptimal solution found after %zu steps.\n", step_count - 1);
            break;
        case SPT_MULTPILE_SOLUTIONS_DETECTED:
            printf("\nMultiple optimal solutions detected after %zu steps.\n", step_count - 1);
            break;
        case SPT_DEGENERATED_PROBLEM:
            printf("\nDegenerate problem detected after %zu steps.\n", step_count - 1);
            break;
        case SPT_UNBOUNDED_PROBLEM:
            printf("\nUnbounded problem detected after %zu steps.\n", step_count - 1);
            break;
        case SPT_INFEASIBLE_PROBLEM:
            printf("\nInfeasible problem detected after %zu steps.\n", step_count - 1);
            break;
        case SPT_INVALID_STATE:
            printf("\nInvalid state detected after %zu steps.\n", step_count - 1);
            break;
        default:
            printf("\nUnexpected status after %zu steps.\n", step_count - 1);
            break;
    }

    // Print the tableau's state after solving steps.
    printf("\nFinal Tableau State:\n");
    sp_tableau_print_tableau(spt);
    printf("\n");
    // Print the final BFS (Base Feasible Solution) after solving steps.
    SPBaseSolution *final_bfs = sp_tableau_get_base_sol(spt);
    if (final_bfs != NULL) {
        sp_tableau_print_base(spt, final_bfs);
        free(final_bfs);
    } else {
        printf("Error: Failed to retrieve final BFS after solving steps.\n");
    }

    // Clean up allocated resources for the SPTableau.
    sp_tableau_free(spt);

    return;
}

// TBD: Degenerate case of LP problem using Simplex.
// TBD: Multiple optimal solutions case of LP problem using Simplex.
