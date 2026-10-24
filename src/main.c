// main.c

#include "simplex/LPDefinition.h"
#include "simplex/SPTableau.h"
#include "tests/LPTestCases.h"
#include "tests/SPTest.h"

int main() {

    // Test cases - Maximization case - Geppetto's Toy Factory 
    printf("=== Running Simplex case for Geppetto's Toy Factory problem (basic maximization) ===\n");
    LPDefinition *geppetto_lp_def = lp_test_max_geppetto();
    if (geppetto_lp_def == NULL) {
        printf("Error: Failed to create LPDefinition for Geppetto's Toy Factory test case.\n");
        return 1;
    }
    sp_test_simple_case(geppetto_lp_def);
    lp_definition_free(geppetto_lp_def);

    printf("Press any key to continue...\n");
    getchar(); // Waits for user to hit Enter

    // Test case - Minimization case - Base minimization case
    printf("\n=== Running Simplex case for a sample minimization problem ===\n");
    LPDefinition *min_case_lp_def = lp_test_sample_min_case();
    if (min_case_lp_def == NULL) {
        printf("Error: Failed to create LPDefinition for sample minimization test case.\n");
        return 1;
    }
    sp_test_simple_case(min_case_lp_def);
    lp_definition_free(min_case_lp_def);

    printf("Press any key to continue...\n");
    getchar(); // Waits for user to hit Enter

    // Test case - Unbounded problem - Sample unbounded case
    printf("\n=== Running Simplex case for a sample unbounded problem ===\n");
    LPDefinition *unbounded_bakery_lp_def = lp_test_sample_unbounded_bakery();
    sp_test_simple_case(unbounded_bakery_lp_def);
    lp_definition_free(unbounded_bakery_lp_def);

    printf("Press any key to continue...\n");
    getchar(); // Waits for user to hit Enter

    // TBD: Degenerate case - Sample degenerate case
    // TBD: Multiple solutions case - Sample multiple solutions case


    

    return 0;
}