#ifndef PRESENTATION_H
#define PRESENTATION_H

#include <stdio.h>
#include <stddef.h>

#include "SPTableau.h"

//Creates and opens the LaTeX file used for the presentation.
FILE *presentation_create_file(const char *filename);

//Generates a filename based on the current date and time.
void presentation_gen_filename(char *buffer, size_t bufferSize);

// Writes the Beamer preamble.
void presentation_preamble(FILE *file);

//Generates the presentation cover slide.
void presentation_cover(FILE *file);

void presentation_simplex_slide(FILE *file);

void presentation_tableau(FILE *file, const SPTableau *tableau, const char *title);

void presentation_original_problem(FILE *file, const char *problem_name, const LPDefinition *problem);

void presentation_print_base_solution(FILE *file, const SPTableau *tableau, const SPBaseSolution *solution, const char *label);

void presentation_solution(FILE *file, const SPTableau *tableau, enum SPTableauStatus status, const SPMultiSolutions *multiple_solutions);

//Generates the final slide of the presentation.
void presentation_end_slide(FILE *file);

// Writes the end of the LaTeX document.
void presentation_end(FILE *file);

//Compiles the generated LaTeX file using pdflatex.
int presentation_compile(const char *texFilename);

//Opens the generated PDF in presentation mode.
int presentation_open_pdf(const char *pdfFilename);

#endif