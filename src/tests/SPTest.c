// SPTest.c
#include "SPTest.h"
#include "../simplex/presentation.h"

// Base function to run a Simplex test case for a given LPDefinition.
// To be used as reference for running Simplex tests for various LP cases (maximization, minimization, unbounded, degenerate, multiple solutions, etc.).
// Also, for reference to integrate with other program's modules (GUI, LaTeX) as they are developed. Currently using console output for demonstration purposes.
void sp_test_base_run(LPDefinition *lp_def, const char *problem_name){
    if (lp_def == NULL) {
        printf("Error: LPDefinition is NULL. Cannot run Simplex test.\n");
        return;
    }

    //===========================
    char tex_filename[128];
    presentation_gen_filename(tex_filename, sizeof(tex_filename));

    FILE *tex_file = presentation_create_file(tex_filename);
    if (tex_file == NULL) {
        return;
    }

    presentation_preamble(tex_file);
    presentation_cover(tex_file);
    presentation_simplex_slide(tex_file);
    presentation_original_problem(tex_file, problem_name, lp_def);
    //===========================


    // 1 - Print the LP definition for reference before running the Simplex algorithm.
    printf("Printing LP definition for %s...\n", problem_name);
    lp_definition_print(lp_def);

    // 2 - Create the Simplex Tableau (SPTableau) from the LP definition.
    printf("Creating Simplex tableau for the given LP definition...\n");
    SPTableau *spt = sp_tableau_create(lp_def);
    if (spt == NULL) {
        printf("Error: Failed to create SPTableau from LPDefinition.\n");
        return;
    }
    presentation_tableau(tex_file, spt, "Tabla Simplex inicial");

    // 3 - Print the SP Tableau's initial state before running the Simplex algorithm.
    printf("Initial Tableau:\n");
    sp_tableau_print_tableau(spt);

    // 4 - Use the SP Tableau's step solve function to iterate through the tableau until an optimal solution is found or an error occurs.
    size_t step_count = 1;
    enum SPTableauStatus spt_status = SPT_STANDBY;

    while (spt_status == SPT_STANDBY || spt_status == SPT_DEGENERATED_PROBLEM) {
        printf("\nRunning step %zu of Simplex resolver...\n", step_count);
        spt_status = sp_tableau_solve_step(spt); 
        char slide_title[64];
        
        snprintf(slide_title, sizeof(slide_title),
        "Tabla despues del paso %zu", step_count);

        presentation_tableau(tex_file, spt, slide_title);

        sp_tableau_print_tableau(spt);
        printf("\n");
        step_count++;
    }

    // 5 - Check if the final state of the tableau indicates multiple solutions, optimal solution, or an error state.
    // If multiple solutions are detected, use the helper function (sp_test_get_multiple_solutions) to retrieve two base solutions and calculate midpoint solutions.
    if (spt_status == SPT_MULTPILE_SOLUTIONS_DETECTED) {
        sp_test_get_multiple_solutions(tex_file, spt);
    } else {
        presentation_solution(tex_file, spt, spt_status, NULL);presentation_solution(tex_file, spt, spt_status, NULL);
        switch (spt_status) {
            case SPT_OPTIMAL_SOLUTION_FOUND:
                printf("\nOptimal solution found after %zu steps. See below for details on found base solution:\n", step_count - 1);
                break;
            case SPT_DEGENERATED_LOOP:
                printf("\nProblem is stuck in a degenerated loop after %zu steps. See below for details on current base solution:\n", step_count - 1);
                break;
            case SPT_UNBOUNDED_PROBLEM:
                printf("\nUnbounded problem detected after %zu steps. See below for details on current base solution:\n", step_count - 1);
                break;
            case SPT_INFEASIBLE_PROBLEM:
                printf("\nInfeasible problem detected after %zu steps. See below for details on current base solution:\n", step_count - 1);
                break;
            case SPT_INVALID_STATE:
                printf("\nInvalid state detected after %zu steps. No \n", step_count - 1);
                break;
            default:
                printf("\nUnexpected status after %zu steps.\n", step_count - 1);
                break;
        }
        printf("\nTableau's current base solution:\n");
        sp_tableau_print_base(spt);
    }

    //===========================
    presentation_end(tex_file);
    fclose(tex_file);

    presentation_compile(tex_filename);
    //===========================

    // Clean up allocated resources for the Simplex Tableau.
    sp_tableau_free(spt);

    return;
}

// Helper function for multiple solutions test case.
// 1. Receives a SPTableau that reached a multiple solution state, 
// 2. Gets the current BFS, calculates another BFS
// 3. Creates a SPMultipleSolutions struct to hold both BFSs and calculate some midpoint solutions.
void sp_test_get_multiple_solutions(FILE *tex_file, SPTableau *spt) {
    if (spt == NULL) {
        printf("Error: SPTableau is NULL. Cannot retrieve multiple solutions.\n");
        return;
    } 

    // Get current BFS (bfs_a) from the tableau after multiple solutions are detected.
    SPBaseSolution *bfs_a = sp_tableau_get_base_sol(spt);

    if (bfs_a == NULL) {
        printf("Error: Failed to retrieve current BFS from tableau for multiple solutions test.\n");
        return;
    }

    // Resolve again to get another BFS (bfs_b) to generate multiple solutions between bfs_a and bfs_b.
    printf("Running resolver again to find another BFS for multiple solutions test...\n");
    enum SPTableauStatus second_run = sp_tableau_solve_step(spt);
    if (second_run != SPT_MULTPILE_SOLUTIONS_DETECTED) {
        printf("Error: Second run of Simplex did not detect multiple solutions as expected. Status: %d\n", second_run);
        sp_base_solution_free(bfs_a);
        return;
    }
    else {
        SPBaseSolution *bfs_b = sp_tableau_get_base_sol(spt);
        if (bfs_b == NULL) {
            printf("Error: Failed to retrieve second BFS from tableau for multiple solutions test.\n");
            sp_base_solution_free(bfs_a);
            return;
        }
        printf("\n=== Multiple Solutions LP Problem Details ===\n");
        printf("\nBase Solution A:\n");
        sp_base_solution_print(bfs_a, spt->lp_def->decision_variables);
        printf("\nBase Solution B:\n");
        sp_base_solution_print(bfs_b, spt->lp_def->decision_variables);

        // Create a SPMultiSolutions struct to hold both BFSs and calculate midpoint solutions.
        SPMultiSolutions *multi_sols = sp_multi_solutions_create(bfs_a, bfs_b);
        if (multi_sols == NULL) {
            printf("Error: Failed to create SPMultipleSolutions struct for multiple solutions test.\n");
            sp_base_solution_free(bfs_a);
            sp_base_solution_free(bfs_b);
            return;
        }
        
        //===============================
        presentation_solution(tex_file,spt,SPT_MULTPILE_SOLUTIONS_DETECTED, multi_sols);
        //===============================

        // Example alphas to calculate midpoint solutions between bfs_a and bfs_b.
        double alpha_1 = 0.5;   // Midpoint between bfs_a and bfs_b
        double alpha_2 = 0.25;  // Closer to bfs_a
        double alpha_3 = 0.75;  // Closer to bfs_b

        SPBaseSolution *midpoint_1 = sp_multi_solutions_calculate_midpoint(multi_sols, alpha_1);
        SPBaseSolution *midpoint_2 = sp_multi_solutions_calculate_midpoint(multi_sols, alpha_2);
        SPBaseSolution *midpoint_3 = sp_multi_solutions_calculate_midpoint(multi_sols, alpha_3);

        printf("\nMidpoint Solution (alpha = %.2f):\n", alpha_1);
        sp_base_solution_print(midpoint_1, spt->lp_def->decision_variables);
        printf("\nMidpoint Solution (alpha = %.2f):\n", alpha_2);
        sp_base_solution_print(midpoint_2, spt->lp_def->decision_variables);
        printf("\nMidpoint Solution (alpha = %.2f):\n", alpha_3);
        sp_base_solution_print(midpoint_3, spt->lp_def->decision_variables);

        // Clean up allocated resources for the three midpoint base solutions.
        sp_base_solution_free(midpoint_1);
        sp_base_solution_free(midpoint_2);
        sp_base_solution_free(midpoint_3);

        // Free the SPMultipleSolutions struct.
        sp_multi_solutions_free(multi_sols);

        // Clean up allocated resources for the two base solutions.
        sp_base_solution_free(bfs_a);
        sp_base_solution_free(bfs_b);
    }
}

// TBD: Degenerate case of LP problem using Simplex.
// TBD: Multiple optimal solutions case of LP problem using Simplex.

