#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "presentation.h"

// Creates and opens the LaTeX file to write.
 
FILE *presentation_create_file(const char *filename)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        fprintf(stderr, "Error: Could not create TEX file.\n");
        return NULL;
    }

    return file;
}

// Generates a filename based on the current date and time.
 
void presentation_gen_filename(char *buffer, size_t bufferSize)
{
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);

    strftime(
        buffer,
        bufferSize,
        "presentation_%Y%m%d_%H%M%S.tex",
        localTime
    );
}

//Writes the Beamer preamble.
 
void presentation_preamble(FILE *file)
{
    fprintf(file, "\\documentclass{beamer}\n");

    fprintf(file, "\\usepackage[utf8]{inputenc}\n");
    fprintf(file, "\\usepackage[T1]{fontenc}\n");
    fprintf(file, "\\usepackage{xcolor}\n");
    fprintf(file, "\\usepackage{listings}\n");
    fprintf(file, "\\usepackage{tikz}\n");
    fprintf(file, "\\usepackage{pgfplots}\n");
    fprintf(file, "\\pgfplotsset{compat=1.18}\n");
    fprintf(file, "\\usepackage{amsmath}\n");
    fprintf(file, "\\usepackage{booktabs}\n");
    fprintf(file, "\\usepackage{array}\n\n");

    fprintf(file, "\\usetheme{Madrid}\n\n");

    //Token category colors.
     
    fprintf(file, "\\definecolor{keywordcolor}{RGB}{0,0,180}\n");
    fprintf(file, "\\definecolor{identifiercolor}{RGB}{0,120,0}\n");
    fprintf(file, "\\definecolor{numbercolor}{RGB}{180,80,0}\n");
    fprintf(file, "\\definecolor{stringcolor}{RGB}{160,0,120}\n");
    fprintf(file, "\\definecolor{operatorcolor}{RGB}{120,0,0}\n");
    fprintf(file, "\\definecolor{delimitercolor}{RGB}{80,80,80}\n");
    fprintf(file, "\\definecolor{errorcolor}{RGB}{220,0,0}\n\n");

    fprintf(file, "\\title{Project 01 - Simplex}\n");
    fprintf(file, "\\author{Matthew Gaviria Brenes - 2023179864 \\ Esteban Fonseca Montoya - 201028937 \\ Josué Meza Solano - 2024240640}\n");
    fprintf(file, "\\institute{Investigacion de Operaciones\\\\Semestre: II, 2026}\n");
    fprintf(file, "\\date{\\today}\n\n");

    fprintf(file, "\\begin{document}\n\n");
}

// Generates the cover slide.
 
void presentation_cover(FILE *file)
{
    fprintf(file, "\\begin{frame}\n");
    fprintf(file, "\\titlepage\n");
    fprintf(file, "\\end{frame}\n\n");
}

void presentation_simplex_slide(FILE *file)
{
    if (file == NULL) {
        return;
    }

    fprintf(file, "\\begin{frame}{Historia y propiedades del algoritmo Simplex}\n");
    fprintf(file, "\\small\n");
    fprintf(file, "\\begin{itemize}\n");
    fprintf(file,
        "    \\item George B. Dantzig developed the Simplex method in 1947 to "
        "solve linear programming problems.\n");
    fprintf(file,
        "    \\item The method seeks an optimal solution among the basic feasible "
        "solutions, associated with the vertices of the feasible region.\n");
    fprintf(file,
        "    \\item In each iteration, a variable enters the base and another "
        "leaves, resulting in a new basic feasible solution. This is known as pivot operation.\n");
    fprintf(file,
        "    \\item In bounded problems, iterations advance toward an "
        "improvement of the objective; in degenerate cases, the value may "
        "remain the same.\n");
    fprintf(file,
        "    \\item It can detect optimality, unboundedness, and, "
        "according to the formulation, infeasibility.\n");
    fprintf(file,
        "    \\item It is efficient in many practical problems, although its "
        "worst-case theoretical complexity is exponential.\n");
    fprintf(file, "\\end{itemize}\n");
    fprintf(file, "\\end{frame}\n\n");
}

// Escapes characters that have a special meaning in LaTeX.
static void presentation_tex_text(FILE *file, const char *text)
{
    if (text == NULL) {
        return;
    }

    for (size_t i = 0; text[i] != '\0'; ++i) {
        switch (text[i]) {
            case '\\': fputs("\\textbackslash{}", file); break;
            case '&':  fputs("\\&", file); break;
            case '%':  fputs("\\%", file); break;
            case '$':  fputs("\\$", file); break;
            case '#':  fputs("\\#", file); break;
            case '_':  fputs("\\_", file); break;
            case '{':  fputs("\\{", file); break;
            case '}':  fputs("\\}", file); break;
            case '~':  fputs("\\textasciitilde{}", file); break;
            case '^':  fputs("\\textasciicircum{}", file); break;
            default:   fputc((unsigned char)text[i], file); break;
        }
    }
}

void presentation_tableau(FILE *file,
    const SPTableau *tableau, const char *title)
{
    if (file == NULL || tableau == NULL || tableau->tableau == NULL ||
        tableau->num_rows == 0 || tableau->num_cols < 2) {
        return;
    }

    const size_t rhs_column = tableau->num_cols - 1;

    fprintf(file, "\\begin{frame}{");
    presentation_tex_text(
        file,
        title != NULL ? title : "Simplex Tableau"
    );
    fprintf(file, "}\n");
    fprintf(file, "\\scriptsize\n");
    fprintf(file, "\\centering\n");
    fprintf(file, "\\resizebox{\\textwidth}{!}{%%\n");

    //One column for label row and one for each column of the tableau
    fprintf(file, "\\begin{tabular}{c");
    for (size_t col = 0; col < tableau->num_cols; ++col) {
        fprintf(file, "c");
    }
    fprintf(file, "}\n");

    fprintf(file, "\\toprule\n");
    fprintf(file, "Row & $Z$");

    for (size_t col = 1; col < rhs_column; ++col) {
        fprintf(file, " & ");

        if (col <= tableau->num_decision_vars) {
            const char *name = NULL;

            if (tableau->lp_def != NULL &&
                tableau->lp_def->decision_variables != NULL &&
                tableau->lp_def->decision_variables->names != NULL) {
                name = tableau->lp_def->decision_variables->names[col - 1];
            }

            if (name != NULL) {
                fprintf(file, "\\texttt{");
                presentation_tex_text(file, name);
                fprintf(file, "}");
            } else {
                fprintf(file, "$x_{%zu}$", col);
            }
        } else if (col <= tableau->num_decision_vars +
                          tableau->num_slack_vars) {
            fprintf(file, "$S_{%zu}$", col - tableau->num_decision_vars);
        } else if (col <= tableau->num_decision_vars +
                          tableau->num_slack_vars +
                          tableau->num_surplus_vars) {
            fprintf(file, "$E_{%zu}$",
                    col - tableau->num_decision_vars -
                    tableau->num_slack_vars);
        } else {
            fprintf(file, "$A_{%zu}$",
                    col - tableau->num_decision_vars -
                    tableau->num_slack_vars -
                    tableau->num_surplus_vars);
        }
    }

    fprintf(file, " & RHS \\\\\n");
    fprintf(file, "\\midrule\n");

    for (size_t row = 0; row < tableau->num_rows; ++row) {
        if (row == 0) {
            fprintf(file, "$Z$");
        } else {
            fprintf(file, "$R_{%zu}$", row);
        }

        for (size_t col = 0; col < tableau->num_cols; ++col) {
            fprintf(file, " & %.4g", tableau->tableau[row][col]);
        }

        fprintf(file, " \\\\\n");
    }

    fprintf(file, "\\bottomrule\n");
    fprintf(file, "\\end{tabular}}\n");
    fprintf(file, "\\end{frame}\n\n");
}

static void presentation_print_variable(FILE *file,
    const LPDefinition *problem, size_t index)
{
    const char *name = NULL;

    if (problem->decision_variables != NULL &&
        problem->decision_variables->names != NULL) {
        name = problem->decision_variables->names[index];
    }

    if (name != NULL && name[0] != '\0') {
        fputs("\\text{\\texttt{", file);
        presentation_tex_text(file, name);
        fputs("}}", file);
    } else {
        fprintf(file, "x_{%zu}", index + 1);
    }
}

static void presentation_print_expression(FILE *file, const LPDefinition *problem,
    const double *coefficients)
{
    int printed_term = 0;

    for (size_t i = 0; i < problem->num_decision_vars; ++i) {
        double coefficient = coefficients[i];

        if (coefficient == 0.0) {
            continue;
        }

        if (printed_term) {
            fputs(coefficient < 0.0 ? " - " : " + ", file);
        } else if (coefficient < 0.0) {
            fputc('-', file);
        }

        fprintf(file, "%.6g\\,", coefficient < 0.0
                                      ? -coefficient
                                      : coefficient);
        presentation_print_variable(file, problem, i);
        printed_term = 1;
    }

    if (!printed_term) {
        fputc('0', file);
    }
}

void presentation_original_problem(FILE *file, 
    const char *problem_name, const LPDefinition *problem)
{
    if (file == NULL ||
        problem == NULL ||
        problem->objective_function == NULL ||
        problem->objective_function->coeffs == NULL ||
        problem->objective_function->coeffs->coefficients == NULL ||
        problem->constraints == NULL) {
        return;
    }

    const char *optimization =
        problem->objective_function->type == MAXIMIZE ? "\\max" : "\\min";

    fputs("\\begin{frame}[allowframebreaks]{Original Problem}\n", file);
    fputs("\\small\n\\textbf{Problem Name:} ", file);

    presentation_tex_text(
        file,
        problem_name != NULL ? problem_name : "Problem Name Pending"
    );

    fputs("\n\\[\n\\begin{array}{rcl}\n", file);
    fprintf(file, "%s\\quad Z & = & ", optimization);

    presentation_print_expression(
        file,
        problem,
        problem->objective_function->coeffs->coefficients
    );

    fputs(" \\\\\n\\multicolumn{3}{l}{\\text{subject to}} \\\\\n", file);

    for (size_t row = 0; row < problem->constraints->count; ++row) {
        const LPCoefficients *constraint =
            problem->constraints->coeff_sets[row];

        if (constraint == NULL || constraint->coefficients == NULL) {
            continue;
        }

        presentation_print_expression(
            file,
            problem,
            constraint->coefficients
        );

        switch (problem->constraints->types[row]) {
            case LE:
                fputs(" & \\leq & ", file);
                break;
            case EQ:
                fputs(" & = & ", file);
                break;
            case GE:
                fputs(" & \\geq & ", file);
                break;
            default:
                fputs(" & ? & ", file);
                break;
        }

        fprintf(file, "%.6g \\\\\n", problem->constraints->rhs_values[row]);
    }

    for (size_t i = 0; i < problem->num_decision_vars; ++i) {
        presentation_print_variable(file, problem, i);
        fputs(" & \\geq & 0 \\\\\n", file);
    }

    fputs("\\end{array}\n\\]\n\\end{frame}\n\n", file);
}

static void presentation_print_base_solution(FILE *file, const SPTableau *tableau,
    const SPBaseSolution *solution, const char *label)
{
    if (solution == NULL) {
        fprintf(file, "\\item No solution available to display.\n");
        return;
    }

    fprintf(file, "\\item \\textbf{%s}\n", label);
    fprintf(file, "\\begin{itemize}\n");
    fprintf(file, "    \\item Objective Function Value: "
                  "$Z = %.6g$.\n",
            solution->var_values[0]);

    for (size_t i = 0; i < solution->num_decision_vars; ++i) {
        fprintf(file, "    \\item $");
        presentation_print_variable(file, tableau->lp_def, i);
        fprintf(file, " = %.6g$%s\n",
                solution->var_values[i + 1],
                solution->vars_in_base[i + 1]
                    ? " (basic)"
                    : " (non-basic)");
    }

    for (size_t i = 0; i < solution->num_slack_vars; ++i) {
        size_t index = 1 + solution->num_decision_vars + i;
        fprintf(file, "    \\item $S_{%zu} = %.6g$%s\n",
                i + 1,
                solution->var_values[index],
                solution->vars_in_base[index]
                    ? " (basic)"
                    : " (non-basic)");
    }

    fprintf(file, "\\end{itemize}\n");
}

void presentation_solution(FILE *file, const SPTableau *tableau,
    enum SPTableauStatus status, const SPMultiSolutions *multiple_solutions)
{
    if (file == NULL || tableau == NULL) {
        return;
    }

    fprintf(file, "\\begin{frame}{Problem Solution}\n");
    fprintf(file, "\\small\n");
    fprintf(file, "\\begin{itemize}\n");

    switch (status) {
        case SPT_OPTIMAL_SOLUTION_FOUND: {
            fprintf(file, "\\item An optimal solution was found.\n");
            SPBaseSolution *solution = sp_tableau_get_base_sol(tableau);
            presentation_print_base_solution(
                file, tableau, solution, "Optimal Solution");
            sp_base_solution_free(solution);
            break;
        }

        case SPT_MULTPILE_SOLUTIONS_DETECTED:
            fprintf(file, "\\item Multiple optimal solutions detected.\n");
            if (multiple_solutions != NULL) {
                presentation_print_base_solution(
                    file, tableau, multiple_solutions->bfs_a,
                    "First optimal solution");
                presentation_print_base_solution(
                    file, tableau, multiple_solutions->bfs_b,
                    "Second optimal solution");
                fprintf(file,
                    "\\item Any convex combination of both solutions "
                    "is also optimal.\n");
            } else {
                fprintf(file,
                    "\\item Solver detected multiplicity, but the two "
                    "solutions were not received for display.\n");
            }
            break;

        case SPT_UNBOUNDED_PROBLEM:
            fprintf(file,
                "\\item Unbounded Problem: No finite optimal value.\n");
            break;

        case SPT_INFEASIBLE_PROBLEM:
            fprintf(file,
                "\\item The problem is infeasible: no solution exists "
                "that satisfies all constraints.\n");
            break;

        case SPT_DEGENERATED_PROBLEM: {
            fprintf(file,
                "\\item Degeneracy detected during an iteration. "
                "This state does not confirm by itself that the algorithm "
                "has terminated or that an optimal solution exists.\n");
            SPBaseSolution *solution = sp_tableau_get_base_sol(tableau);
            presentation_print_base_solution(
                file, tableau, solution, "Current basic solution");
            sp_base_solution_free(solution);
            break;
        }

        case SPT_DEGENERATED_LOOP:
            fprintf(file,
                "\\item The algorithm reached degeneracy limit and "
                "stopped without confirming an optimal solution.\n");
            break;

        case SPT_STANDBY:
            fprintf(file,
                "\\item The algorithm has not terminated. The current "
                "state should not be presented as the optimal solution.\n");
            break;

        case SPT_INVALID_STATE:
        default:
            fprintf(file,
                "\\item It was not possible to determine a valid solution "
                "due to an invalid state of the problem or the tableau.\n");
            break;
    }

    fprintf(file, "\\end{itemize}\n");
    fprintf(file, "\\end{frame}\n\n");
}

//Generates the final slide.
 
void presentation_end_slide(FILE *file)
{
    fprintf(file, "\\begin{frame}\n");
    fprintf(file, "\\centering\n");
    fprintf(file, "{\\Huge Thank you!}\n");
    fprintf(file, "\\end{frame}\n\n");
}

//Writes the end of the LaTeX document.
 
void presentation_end(FILE *file)
{
    fprintf(file, "\\end{document}\n");
}

//Compiles the generated LaTeX document.
 
int presentation_compile(const char *texFilename)
{
    char command[512];

    snprintf(
        command,
        sizeof(command),
        "pdflatex -interaction=nonstopmode -halt-on-error \"%s\"",
        texFilename
    );

    return system(command);
}

//Opens the generated PDF in presentation mode.
 
int presentation_open_pdf(const char *pdfFilename)
{
    char command[512];

    snprintf(
        command,
        sizeof(command),
        "evince -s \"%s\" &",
        pdfFilename
    );

    return system(command);
}