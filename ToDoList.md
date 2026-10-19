# Feature To-Do List

This list is not definitive, may change at any time.
This will be used for guidance on what needs to be implemented.


## Simplex Solver Logic
### Data structures for LP elements
* [X] - Decision variables names
* [X] - Objective function
* [X] - Constraint definition
* [X] - LPConstraints list

NOTES: 
Data Structure for LP is meant to represent the base mathematical models for LP problems.
Slack variables and other non-decision variables can be inferred by type of LP constraint (<=, =, >=).

### Data structures for Simplex tableau
* [X] - Slack variables for constraints (where applicable)
* [X] - Simplex tableau baseline
* [X] - Simplex tableau matrix
* [X] - Reserve indices for objective row (row 0), objective var (col 0) and RHS (added last col)
* [X] - Flag for degenerated problem
* [X] - Flag for unbound problem

### Simplex Tableau - Indexed access
* [X] - Handle indexed access for constraints (rows) and variables (columns).
* [X] - Handle indexed access for decision vars and slack vars.
* [X] - Check if variable is in BFS.
* [X] - Get indices of variables in BFS. (May include 0 for Z).
* [X] - Get value of var in BFS.
* [X] - Check if column is in base.
* [X] - Get list of variables in base by column indices.

### Simplex Tableau - Solver Functionality
* [ ] - Check if issue is solved (no new variables can enter BFS).
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
