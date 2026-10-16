# Simplex Solver v1

A simple program to resolve Linear Programming issues using the Simplex algorithm with tableaus.


## User Interface
GTK-based Graphics User Interface (GUI) that receives the following information for the LP scenario from the user: 
- Problem name
- Amount of decision variables
- Amount of constraints
- Names for decision variables (defaults to x_## prefix)
- Definition of the objective function
- Definitions for each constraint

## Program Features
- Resolves maximize and minimize LP problems using the Simplex algorithm and tableaus. As of this version of the program, it will solve issues with less-than-equal (<=) constraints.
- Detection of multiple solutions: Calculate a second solution, results from both solutions are used to set up equation to calculate other solutions in-between.
- Detection of unbounded problem: Stops execution and reports situation to the user.
- Detection of degenerated problem: Reports degeneration, provide details on degeneration and ratio tie-breaking. 
- Show tableaus for each step (if requested by user).

## Output of results
The program outputs the results in a LaTeX file and a PDF file (compiled from the LaTeX file). Additional metadata about the execution is included:
- Cover page.
- About Simplex algorithm: History, properties, etc.
- Original problem: Name given by user, mathematical model (objective function and constraints using user-given variable names).
- Initial Simplex tableau.
- Steps' tableaus (if requested by user).
- Final tableau.
- Solution details, including explanations for special cases (multiple solutions, degenerated problems, unbounded problems.)

## Language

C

## Requirements

- Linux operating system
- GCC
- GNU Make
- GTK
- Glade
- Pdflatex
- evince

## Project Structure

- `src/`- Source code
- `src/gui`- Source code for GUI interface.
- `src/simplex`- Source code for Simplex Solver's logic.
- `src/tex`- Source code for LaTeX generator.
- `Makefile` - Makefile to build executable with Make.

## How to build

Use GNU Make to build the project using the project's Makefile:

`make`

## How to run

...

[TO BE UPDATED]