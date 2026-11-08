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
    fprintf(file, "\\documentclass[11pt]{article}\n");
    fprintf(file, "\\usepackage[utf8]{inputenc}\n");
    fprintf(file, "\\usepackage[T1]{fontenc}\n");
    fprintf(file, "\\usepackage[margin=2.5cm]{geometry}\n");
    fprintf(file, "\\usepackage{amsmath}\n");
    fprintf(file, "\\usepackage{booktabs}\n");
    fprintf(file, "\\usepackage{array}\n");
    fprintf(file, "\\usepackage{graphicx}\n");
    fprintf(file, "\\usepackage[table]{xcolor}\n");
    fprintf(file, "\\usepackage[expansion=false]{microtype}\n\n");

    fprintf(file, "\\title{Project 01 - Simplex}\n");
    fprintf(file, "\\author{Matthew Gaviria Brenes - 2023179864 \\\\\n Esteban Fonseca Montoya - 201028937 \\\\\n Josué Meza Solano - 2024240640}\n");
    fprintf(file, "\\date{\\today}\n\n");

    fprintf(file, "\\begin{document}\n\n");
}

// Generates the cover slide.
 
void presentation_cover(FILE *file)
{
    fprintf(file, "\\maketitle\n");
    fprintf(file, "\\thispagestyle{empty}\n");
}

void presentation_simplex_slide(FILE *file)
{
    if (file == NULL) {
        return;
    }

    fprintf(file, "\\section{History of the Simplex Method}\n");
    fprintf(file, "\\begingroup\\small\n");
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
    fprintf(file, "\\endgroup\n");
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

    fprintf(file, "\\section{");
    presentation_tex_text(file, title != NULL ? title : "Simplex Tableau");
    fprintf(file, "}\n");
    fprintf(file, "\\begin{center}\n");
    fprintf(file, "\\small\n");
    fprintf(file, "\\setlength{\\tabcolsep}{8pt}\n");

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
            if (tableau->lp_def->decision_variables != NULL &&
                tableau->lp_def->decision_variables->names != NULL &&
                tableau->lp_def->decision_variables->names [col -1] != NULL) {
                    fputs("\\text{\\texttt{", file);

                    if (strlen(tableau->lp_def->decision_variables->names[col - 1]) > 4) {
                        char truncated_name[5] = {0};
                        strncpy(truncated_name, tableau->lp_def->decision_variables->names[col - 1], 4);
                        presentation_tex_text(file, truncated_name);
                    } else {
                        presentation_tex_text(file, tableau->lp_def->decision_variables->names[col - 1]);
                    }
                    fputs("}}", file);
                } else {
                    fprintf(file, "$x_{%zu}$", col);
                }
                 
        } else if (col <= tableau->num_decision_vars +
                          tableau->num_slack_vars) {
            fprintf(file, "$S_{%zu}$", col - tableau->num_decision_vars);
        } else if (col <= tableau->num_decision_vars +
                          tableau->num_slack_vars +
                          tableau->num_excess_vars) {
            fprintf(file, "$E_{%zu}$",
                    col - tableau->num_decision_vars -
                    tableau->num_slack_vars);
        } else {
            fprintf(file, "$A_{%zu}$",
                    col - tableau->num_decision_vars -
                    tableau->num_slack_vars -
                    tableau->num_excess_vars);
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
    fprintf(file, "\\end{tabular}\n");
    fprintf(file, "\\end{center}\n\n");
}

void presentation_intermediate_tableau(FILE *file, const SPTableau *tableau,
    const char *title, size_t pivot_column, size_t pivot_row, const double *min_ratios){
    if (file == NULL || tableau == NULL || tableau->tableau == NULL ||
        tableau->num_rows == 0 || tableau->num_cols < 2) {
        return;
    }

    const size_t rhs_column = tableau->num_cols - 1;
    const int valid_pivot_column = pivot_column > 0 && pivot_column < rhs_column;
    const int valid_pivot_row = pivot_row > 0 && pivot_row < tableau->num_rows;
    const int valid_pivot = valid_pivot_column && valid_pivot_row;

    fprintf(file, "\\section{");
    presentation_tex_text(file, title != NULL ? title : "Simplex Pivot Step");
    fprintf(file, "}\n");
    fprintf(file, "\\begin{center}\n");
    fprintf(file, "\\small\n");
    fprintf(file, "\\setlength{\\tabcolsep}{6pt}\n");

    // label tableau rows and columns, adds column for ratios
    // highlights the pivot column and pivot row if valid.
    fprintf(file, "\\begin{tabular}{c");
    for (size_t col = 0; col < tableau->num_cols + 1; ++col) {
        fprintf(file, "c");
    }
    fprintf(file, "}\n");

    fprintf(file, "\\toprule\n");
    fprintf(file, "Row & $Z$");

    for (size_t col = 1; col < rhs_column; ++col) {
        fprintf(file, " & ");

        if (valid_pivot_column && col == pivot_column) {
            fprintf(file, "\\cellcolor{blue!15}");
        }

        if (col <= tableau->num_decision_vars) {
            if (tableau->lp_def->decision_variables != NULL &&
                tableau->lp_def->decision_variables->names != NULL &&
                tableau->lp_def->decision_variables->names [col -1] != NULL) {
                    fputs("\\text{\\texttt{", file);

                    if (strlen(tableau->lp_def->decision_variables->names[col - 1]) > 4) {
                        char truncated_name[5] = {0};
                        strncpy(truncated_name, tableau->lp_def->decision_variables->names[col - 1], 4);
                        presentation_tex_text(file, truncated_name);
                    } else {
                        presentation_tex_text(file, tableau->lp_def->decision_variables->names[col - 1]);
                    }
                    fputs("}}", file);
                } else {
                    fprintf(file, "$x_{%zu}$", col);
                }

        } else if (col <= tableau->num_decision_vars + tableau->num_slack_vars) {
            fprintf(file, "$S_{%zu}$", col - tableau->num_decision_vars);
        
        } else if (col <= tableau->num_decision_vars + tableau->num_slack_vars + tableau->num_excess_vars) {
            fprintf(file, "$E_{%zu}$", col - tableau->num_decision_vars - tableau->num_slack_vars);
        
        } else {
            fprintf(file, "$A_{%zu}$", col - tableau->num_decision_vars - tableau->num_slack_vars - tableau->num_excess_vars);
        
        }
    }

    fprintf(file, " & RHS & Ratio \\\\\n");
    fprintf(file, "\\midrule\n");

    for (size_t row = 0; row < tableau->num_rows; ++row) {
        if (row == 0) {
            fprintf(file, "$Z$");
        } else {
            fprintf(file, "$R_{%zu}$", row);
        }

        for (size_t col = 0; col < tableau->num_cols; ++col) {
            fprintf(file, " & ");

            if (valid_pivot && row == pivot_row && col == pivot_column) {
                fprintf(file, "\\cellcolor{green!10}" "\\boxed{%.4g}", tableau->tableau[row][col]);
            
            } else {
                if (valid_pivot_column && col == pivot_column) {
                    fprintf(file, "\\cellcolor{blue!15}");
                }
                fprintf(file, "%.4g", tableau->tableau[row][col]);
            
            }
        }

        if (row == 0 || min_ratios == NULL || min_ratios[row] < 0.0 ||
            !valid_pivot_column || tableau->tableau[row][pivot_column] <= 0.0) {
            fprintf(file, " & --");
        } else {
            fprintf(file, " & $\\frac{%.4g}{%.4g}=%.4g$", tableau->tableau[row][rhs_column],
                tableau->tableau[row][pivot_column], min_ratios[row]);
        }

        fprintf(file, " \\\\\n");
    }

    fprintf(file, "\\bottomrule\n");
    fprintf(file, "\\end{tabular}\n");
    fprintf(file, "\\end{center}\n\n");

    if (valid_pivot && min_ratios != NULL) {
        double selected_ratio = min_ratios[pivot_row];
        size_t tied_rows = 0;
        for (size_t row = 1; row < tableau->num_rows; ++row) {
            if (min_ratios[row] >= 0.0 &&
                min_ratios[row] == selected_ratio) {
                    ++tied_rows;
                }
            }
            if (selected_ratio == 0.0) {
                fprintf(file,
                    "\\par\\textbf{Degeneracy:} the minimum ratio is zero "
                    "in row $R_{%zu}$. The pivot changes the basis, but "
                    "does not improve the objective value in this iteration.\\par\n", pivot_row);
            }
            if (tied_rows > 1) {
                fprintf(file,
                    "\\par\\textbf{Tie:} %zu rows share the minimum ratio. "
                    "The row with the smallest positive coefficient in the pivot column is selected; "
                    "if the tie persists, the first row found is kept.\\par\n", tied_rows);
            }
        
    }
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

    fputs("\\section{Original Problem}\n", file);
    fputs("\\subsection{Problem Name:} ", file);

    presentation_tex_text(
        file,
        problem_name != NULL ? problem_name : "Linear Programming Problem"
    );

    fputs("\n\\subsection{Mathematical Model} ", file);
    fputs("\n\\[\n\\begin{array}{rcl}\n", file);
    fprintf(file, "%s\\quad Z & = & ", optimization);

    presentation_print_expression(
        file,
        problem,
        problem->objective_function->coeffs->coefficients
    );

    fputs(" \\\\\n\\multicolumn{3}{l}{\\text{s.t.}} \\\\\n", file);

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
        if (i + 1 < problem->num_decision_vars) {
            fputs(", ", file);
        }
    }
    fputs(" & \\geq & 0 \\\\\n", file);
    fputs("\\end{array}\n\\]\n\\clearpage\n\n", file);
}

void presentation_print_base_solution(FILE *file, const SPTableau *tableau,
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

    fprintf(file, "\\section{Problem Solution}\n");
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

                SPBaseSolution *solution_m1 = sp_multi_solutions_calculate_midpoint(multiple_solutions, 0.5);
                SPBaseSolution *solution_m2 = sp_multi_solutions_calculate_midpoint(multiple_solutions, 0.25);
                SPBaseSolution *solution_m3 = sp_multi_solutions_calculate_midpoint(multiple_solutions, 0.75);

                presentation_print_base_solution(
                    file, tableau, multiple_solutions->bfs_a,
                    "First optimal solution (BFS A)");
                presentation_print_base_solution(
                    file, tableau, multiple_solutions->bfs_b,
                    "Second optimal solution (BFS B)");
                fprintf(file, "\\item The following additional solutions between the found optimal solutions (BFS A, BFS B) were calculated using the following equation:\n");
                fprintf(file, "\\[\\text{Middlepoint Solution} = \\alpha \\cdot \\text{BFS A} + (1 - \\alpha) \\cdot \\text{BFS B}\\]\n");

                presentation_print_base_solution(
                    file, tableau, solution_m1,
                    "Calculated solution ($\\alpha = 0.5$)");
                presentation_print_base_solution(
                    file, tableau, solution_m2,
                    "Calculated solution ($\\alpha = 0.25$)");
                presentation_print_base_solution(
                    file, tableau, solution_m3,
                    "Calculated solution ($\\alpha = 0.75$)");

                sp_base_solution_free(solution_m1);
                sp_base_solution_free(solution_m2);
                sp_base_solution_free(solution_m3);
                
            } else {
                fprintf(file,
                    "\\item Solver detected multiplicity, but the two "
                    "solutions were not received for display.\n");
            }
            break;

        case SPT_UNBOUNDED_PROBLEM:
            fprintf(file,
                "\\item Unbounded Problem: A constraint is missing or not properly defined, leading to an unbounded solution. "
                "Please review the problem formulation and add (or update) any necessary constraints.\n");
            break;

        case SPT_INFEASIBLE_PROBLEM:        // for V2
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
                "\\item The algorithm reached the degeneracy limit and stopped executing to avoid a potential infinite loop. "
                "An optimal solution cannot be confirmed.\n");
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
    fprintf(file, "\\clearpage\n\n");
}

//Writes the end of the LaTeX document.
 
void presentation_end(FILE *file)
{
    fprintf(file, "\\end{document}\n");
}

//Compiles the generated LaTeX document.
 
static int presentation_path_with_suffix(char *destination,size_t destination_size,
    const char *tex_filename,const char *suffix)
{
    if (destination == NULL || tex_filename == NULL || suffix == NULL) {
        return -1;
    }

    const char *dot = strrchr(tex_filename, '.');
    const char *slash = strrchr(tex_filename, '/');

    size_t base_length = strlen(tex_filename);
    if (dot != NULL && (slash == NULL || dot > slash)) {
        base_length = (size_t)(dot - tex_filename);
    }

    int written = snprintf(destination, destination_size, "%.*s%s", 
        (int)base_length, tex_filename, suffix);

    if (written < 0 || (size_t)written >= destination_size) {
        return -1;
    }

    return 0;
}

int presentation_compile(const char *tex_filename)
{
    if (tex_filename == NULL) {
        return -1;
    }

    char pdf_filename[1024];
    if (presentation_path_with_suffix(pdf_filename, sizeof(pdf_filename), tex_filename, ".pdf") != 0) {
        fprintf(stderr, "Error: TEX filename is too long.\n");
        return -1;
    }

    char command[2048];
    int written = snprintf(command, sizeof(command),
        "pdflatex -interaction=nonstopmode -halt-on-error \"%s\"  > /dev/null 2>&1", tex_filename);

    if (written < 0 || (size_t)written >= sizeof(command)) {
        fprintf(stderr, "Error: Compile command is too long.\n");
        return -1;
    }

    int status = system(command);
    if (status != 0) {
        fprintf(stderr, "Error: Failed to generate PDF from TEX file.\n");
        return status;
    }

    const char *auxiliary_suffixes[] = {".aux", ".log", ".out", ".toc", 
        ".nav", ".snm", ".vrb", ".synctex.gz"};

    for (size_t i = 0; i < sizeof(auxiliary_suffixes) / sizeof(auxiliary_suffixes[0]);++i) {
        char auxiliary_filename[1024];

        if (presentation_path_with_suffix(auxiliary_filename,sizeof(auxiliary_filename),
                tex_filename, auxiliary_suffixes[i]) == 0) {
            remove(auxiliary_filename);
        }
    }

    return presentation_open_pdf(pdf_filename);
}

//Opens the generated PDF in presentation mode.
 
int presentation_open_pdf(const char *pdf_filename)
{
    if (pdf_filename == NULL) {
        return -1;
    }

    char command[2048];
    int written = snprintf(command, sizeof(command),
        "if command -v evince >/dev/null 2>&1; then evince --presentation \"%s\" >/dev/null 2>&1 & "
        "elif command -v okular >/dev/null 2>&1; then okular --presentation \"%s\" >/dev/null 2>&1 & "
        "else echo 'Error: neither Evince nor Okular is installed.' >&2; "
        "exit 1; fi",pdf_filename, pdf_filename);

    if (written < 0 || (size_t)written >= sizeof(command)) {
        fprintf(stderr, "Error: PDF filename is too long.\n");
        return -1;
    }

    return system(command);
}