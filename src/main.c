// main.c

#include "simplex/LPDefinition.h"
#include "simplex/SPTableau.h"
#include "simplex/SPBFS.h"

// Test cases for LPDefinition and its components
#include "tests/LPTestCases.h"

int main() {
    LPDefinition *belt_factory_lp_def = lp_test_beltfactory();
    lp_definition_print(belt_factory_lp_def);

    SPTableau *spt_belt_factory = sp_tableau_create(belt_factory_lp_def);
    if (spt_belt_factory != NULL) {
        sp_tableau_print_tableau(spt_belt_factory);
        
        printf("After Step 1 to resolve...\n");
        sp_tableau_solve_step(spt_belt_factory);
        sp_tableau_print_tableau(spt_belt_factory);

        printf("After Step 2 to resolve...\n");
        sp_tableau_solve_step(spt_belt_factory);
        sp_tableau_print_tableau(spt_belt_factory);

        printf("After Step 3 to resolve...\n");
        sp_tableau_solve_step(spt_belt_factory);
        sp_tableau_print_tableau(spt_belt_factory);

        printf("Printing BFS so far...\n");
        SPBFS *test_bfs  = sp_tableau_get_bfs(spt_belt_factory);
        if (test_bfs != NULL) {
            sp_tableau_print_bfs(spt_belt_factory, test_bfs);
            sp_bfs_free(test_bfs);
        } else {
            printf("Error: Failed to retrieve BFS from tableau.\n");
        }

        // Free SPTableau for the Belt Factory case
        sp_tableau_free(spt_belt_factory);
    } else {
        printf("Error: Failed to create SPTableau for the Belt Factory case.\n");
    }

    lp_definition_free(belt_factory_lp_def);

    return 0;
}