# Feature To-Do List

This list is not definitive, may change at any time.
This will be used for guidance on what needs to be implemented.


## Simplex Solver Logic
### Data structures for LP elements
* [X] - Decision variables names
* [X] - Objective function
* [X] - Constraint definition
* [X] - Constraints list

NOTES: 
Data Structure for LP is meant to represent the base mathematical models for LP problems.
TBD: 
* Whether to include slack variables in LP definitions (e.g. Constraints).
* Or use the Constraints struct's array for constraint types to infer slack variables and their indices.

### Data structures for Simplex tableau
* [ ] - Slack variables for constraints (where applicable)
* [ ] - Simplex tableau baseline
* [ ] - Simplex tableau matrix
* [ ] - Reserve indices for objective row (row 0), objective var (col 0) and RHS (added last col)
* [ ] - Flag for degenerated problem
* [ ] - Flag for unbound problem

### Simplex Tableau - Indexed access
* [ ] - Handle indexed access for constraints (rows) and variables (columns).
* [ ] - Handle indexed access for decision vars and slack vars.
* [ ] - Check if variable is in BFS.
* [ ] - Get indices of variables in BFS. (May include 0 for Z).
* [ ] - Get value of var in BFS.
* [ ] - Check if column is identity.
* [ ] - Get position of identity cell (i.e. cell with 1) for variable column in BFS.
* [ ] - Check if issue is solved (no new variables can enter BFS).

### Simplex Tableau - Solver Functionality
* [ ] - Check if issue is solved (no new variables can enter BFS)
* [ ] - Get column index of variable that enter BFS.
* [ ] - Calculate minimum-ratio values.
* [ ] - Handle unbounded problem.
* [ ] - Get row index of variable that exits BFS (and set pivot).
* [ ] - Normalize pivot row (multiply/divide pivot row's coefficients to get 1 in pivot cell)
* [ ] - Zeroing of pivot column (add/subtract pivot row's coefficients into other rows' coefficients to get zero coefficients in pivot column except pivot cell.)
* [ ] - >>> TO BE UPDATED...

## GUI (GTK + Glade)
* [ ] - Main window.
* [ ] - Receive user inputs for number of decision variables and constraints.
* [ ] - Custom names for decision variables.
* [ ] - Define objective function.
* [ ] - Define constraints.
* [ ] - Let user start execution of program when ready.
* [ ] - Error case: Incomplete information.
* [ ] - Error error case: Invalid information.
* [ ] - Error error case: Unbounded case.
* [ ] - Notify user that run is complete.
* [ ] - OPTIONAL: Let user select destination for generated file (default to working directory.)
* [ ] - >>> TO BE UPDATED...
 
## LaTeX Generator
* [ ] - >>> TO BE UPDATED...
