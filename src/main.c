#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simplex/LPDefinition.h"
#include "simplex/SPTableau.h"
#include "tex/presentation.h"

#define MIN_FIELDS 2
#define MAX_FIELDS 15
#define MAX_SIMPLEX_STEPS 10000

#define UI_FILE_1 "simplex_glade/simplex.glade"
#define UI_FILE_2 "simplex.glade"

typedef struct {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *problem_name_entry;
    GtkWidget *variables_spin;
    GtkWidget *restricciones_spin;
    GtkWidget *objective_combo;
    GtkWidget *objective_fields_box;
    GtkWidget *constraints_fields_box;
    GtkWidget *show_intermediate_check;
    GtkWidget *members_entry;
    GtkWidget *course_entry;
    GtkWidget *semester_entry;
    GtkWidget *output_status_label;
    GtkWidget *var_name_entries[MAX_FIELDS];
    GtkWidget *objective_coeff_entries[MAX_FIELDS];
    GtkWidget *constraint_coeff_entries[MAX_FIELDS][MAX_FIELDS];
    GtkWidget *constraint_rhs_entries[MAX_FIELDS];
    size_t nvars;
    size_t nconstraints;
} App;

static App app;

static GtkWidget *get_widget(const char *id) {
    return GTK_WIDGET(gtk_builder_get_object(app.builder, id));
}

static void set_status(const char *message) {
    if (app.output_status_label)
        gtk_label_set_text(GTK_LABEL(app.output_status_label), message);
}

static void show_error(const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(app.window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
        GTK_BUTTONS_CLOSE, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    set_status(message);
}

static void show_info(const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(app.window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
        GTK_BUTTONS_CLOSE, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static GtkWidget *make_label(const char *text) {
    GtkWidget *label = gtk_label_new(text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    return label;
}

static GtkWidget *make_entry(const char *placeholder, const char *initial) {
    GtkWidget *entry = gtk_entry_new();
    if (placeholder) gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
    if (initial) gtk_entry_set_text(GTK_ENTRY(entry), initial);
    gtk_entry_set_width_chars(GTK_ENTRY(entry), 8);
    return entry;
}

static void clear_box(GtkWidget *box) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(box));
    for (GList *it = children; it; it = it->next)
        gtk_widget_destroy(GTK_WIDGET(it->data));
    g_list_free(children);
}

static void on_generate_fields_clicked(GtkButton *button, gpointer user_data) {
    (void)button; (void)user_data;
    int nv = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app.variables_spin));
    int nc = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app.restricciones_spin));
    if (nv < MIN_FIELDS || nv > MAX_FIELDS || nc < MIN_FIELDS || nc > MAX_FIELDS) {
        show_error("The number of variables and constraints must be between 2 and 15");
        return;
    }

    clear_box(app.objective_fields_box);
    clear_box(app.constraints_fields_box);
    memset(app.var_name_entries, 0, sizeof(app.var_name_entries));
    memset(app.objective_coeff_entries, 0, sizeof(app.objective_coeff_entries));
    memset(app.constraint_coeff_entries, 0, sizeof(app.constraint_coeff_entries));
    memset(app.constraint_rhs_entries, 0, sizeof(app.constraint_rhs_entries));
    app.nvars = (size_t)nv;
    app.nconstraints = (size_t)nc;

    GtkWidget *name_col = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *coef_col = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start(GTK_BOX(app.objective_fields_box), name_col, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(app.objective_fields_box), coef_col, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(name_col), make_label("Variable"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(coef_col), make_label("coefficient"), FALSE, FALSE, 0);
    for (int j = 0; j < nv; ++j) {
        char var_default[32], coef_default[16];
        g_snprintf(var_default, sizeof(var_default), "x%d", j + 1);
        g_snprintf(coef_default, sizeof(coef_default), "%d", j == 0 ? 1 : 0);
        app.var_name_entries[j] = make_entry("Name", var_default);
        app.objective_coeff_entries[j] = make_entry("0", coef_default);
        gtk_box_pack_start(GTK_BOX(name_col), app.var_name_entries[j], FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(coef_col), app.objective_coeff_entries[j], FALSE, FALSE, 0);
    }

    for (int i = 0; i < nc; ++i) {
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        char row_label[32], rhs_default[16];
        g_snprintf(row_label, sizeof(row_label), "Constraint %d:", i + 1);
        g_snprintf(rhs_default, sizeof(rhs_default), "%d", i + 1);
        gtk_box_pack_start(GTK_BOX(app.constraints_fields_box), row, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(row), make_label(row_label), FALSE, FALSE, 0);
        for (int j = 0; j < nv; ++j) {
            char hint[24];
            g_snprintf(hint, sizeof(hint), "coef. x%d", j + 1);
            app.constraint_coeff_entries[i][j] = make_entry(hint, j == 0 ? "1" : "0");
            gtk_box_pack_start(GTK_BOX(row), app.constraint_coeff_entries[i][j], FALSE, FALSE, 0);
        }
        GtkWidget *operator_combo = gtk_combo_box_text_new();

        gtk_combo_box_text_append_text(
            GTK_COMBO_BOX_TEXT(operator_combo), "≤");
        
        gtk_combo_box_text_append_text(
            GTK_COMBO_BOX_TEXT(operator_combo), "≥");
        
        gtk_combo_box_text_append_text(
            GTK_COMBO_BOX_TEXT(operator_combo), "=");
        
        gtk_combo_box_set_active(
            GTK_COMBO_BOX(operator_combo), 0);

        gtk_box_pack_start(
        GTK_BOX(row), operator_combo, FALSE, FALSE, 0);
        app.constraint_rhs_entries[i] = make_entry("Right side ≥ 0", rhs_default);
        gtk_entry_set_width_chars(GTK_ENTRY(app.constraint_rhs_entries[i]), 10);
        gtk_box_pack_start(GTK_BOX(row), app.constraint_rhs_entries[i], FALSE, FALSE, 0);
    }

    gtk_widget_show_all(app.objective_fields_box);
    gtk_widget_show_all(app.constraints_fields_box);
    set_status("Solve and generate PDF.");
}

static void on_clear_clicked(GtkButton *button, gpointer user_data) {
    (void)button; (void)user_data;
    gtk_entry_set_text(GTK_ENTRY(app.problem_name_entry), "");
    gtk_entry_set_text(GTK_ENTRY(app.members_entry), "");
    gtk_entry_set_text(GTK_ENTRY(app.course_entry), "");
    gtk_entry_set_text(GTK_ENTRY(app.semester_entry), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app.variables_spin), 2);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app.restricciones_spin), 2);
    gtk_combo_box_set_active(GTK_COMBO_BOX(app.objective_combo), 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app.show_intermediate_check), FALSE);
    on_generate_fields_clicked(NULL, NULL);
    set_status("Form cleaned.");
}

static gboolean parse_entry_number(GtkWidget *entry, double *value) {
    const char *text = gtk_entry_get_text(GTK_ENTRY(entry));
    char *end = NULL;
    if (!text || !*text) return FALSE;
    double parsed = g_ascii_strtod(text, &end);
    while (end && g_ascii_isspace(*end)) ++end;
    if (!end || end == text || *end != '\0' || !isfinite(parsed)) return FALSE;
    *value = parsed;
    return TRUE;
}

static char *latex_escape(const char *input) {
    GString *out = g_string_new("");
    if (!input) return g_string_free(out, FALSE);
    for (const char *p = input; *p; ++p) {
        switch (*p) {
            case '\\': g_string_append(out, "\\textbackslash{}"); break;
            case '&': g_string_append(out, "\\&"); break;
            case '%': g_string_append(out, "\\%"); break;
            case '$': g_string_append(out, "\\$"); break;
            case '#': g_string_append(out, "\\#"); break;
            case '_': g_string_append(out, "\\_"); break;
            case '{': g_string_append(out, "\\{"); break;
            case '}': g_string_append(out, "\\}"); break;
            case '~': g_string_append(out, "\\textasciitilde{}"); break;
            case '^': g_string_append(out, "\\textasciicircum{}"); break;
            default: g_string_append_c(out, *p); break;
        }
    }
    return g_string_free(out, FALSE);
}

typedef struct { double **cells; size_t rows; size_t cols; } TableauSnapshot;

static void free_snapshot(TableauSnapshot *snap) {
    if (!snap || !snap->cells) return;
    for (size_t r = 0; r < snap->rows; ++r) free(snap->cells[r]);
    free(snap->cells);
    snap->cells = NULL;
}

static gboolean save_snapshot(TableauSnapshot *snap, const SPTableau *spt) {
    if (!snap || !spt || !spt->tableau) return FALSE;
    snap->rows = spt->num_rows;
    snap->cols = spt->num_cols;
    snap->cells = calloc(snap->rows, sizeof(*snap->cells));
    if (!snap->cells) return FALSE;
    for (size_t r = 0; r < snap->rows; ++r) {
        snap->cells[r] = malloc(snap->cols * sizeof(*snap->cells[r]));
        if (!snap->cells[r]) { free_snapshot(snap); return FALSE; }
        memcpy(snap->cells[r], spt->tableau[r], snap->cols * sizeof(*snap->cells[r]));
    }
    return TRUE;
}

static void write_tableau(FILE *f, const SPTableau *tableau_template, const TableauSnapshot *snap, const char *caption) {
    if (!f || !tableau_template || !snap || !snap->cells) return;

    SPTableau snapshot_tableau = *tableau_template;
    snapshot_tableau.tableau = snap->cells;

    presentation_tableau(f, &snapshot_tableau, caption);
}

static const char *status_text(enum SPTableauStatus status) {
    switch (status) {
        case SPT_OPTIMAL_SOLUTION_FOUND: return "Optimal solution found.";
        case SPT_MULTPILE_SOLUTIONS_DETECTED: return "Multiple optimal solutions found.";
        case SPT_DEGENERATED_PROBLEM: return "Degenerated problem detected.";
        case SPT_UNBOUNDED_PROBLEM: return "Unbounded problem.";
        case SPT_DEGENERATED_LOOP: return "Algorithm halted, degenerated loop";
        case SPT_INFEASIBLE_PROBLEM: return "Infeasible problem";
        case SPT_INVALID_STATE: return "Invalid table state.";
        default: return "The algorithm didn't reached a conclusion.";
    }
}

// Generate a report of the solved Simplex problem, save it as PDF and use evince to open it.
// TODO: Matthew Gaviria - Integrate generation of PDF with UI.
static gboolean write_report_and_pdf(const LPDefinition *lp, SPTableau *spt,
                                     const char *problem_name, const char *status,
                                     gboolean include_intermediate, size_t steps,
                                     enum SPTableauStatus final_status,
                                     const TableauSnapshot *initial_snapshot,
                                     const TableauSnapshot *history, size_t history_count) {
    const char *tex_path = "informe_simplex.tex";
    FILE *f = g_fopen(tex_path, "w");
    if (!f) return FALSE;

    char *safe_name = latex_escape(problem_name);
    char *safe_members = latex_escape(gtk_entry_get_text(GTK_ENTRY(app.members_entry)));
    char *safe_course = latex_escape(gtk_entry_get_text(GTK_ENTRY(app.course_entry)));
    char *safe_semester = latex_escape(gtk_entry_get_text(GTK_ENTRY(app.semester_entry)));
    /*fprintf(f, "\\documentclass[11pt]{article}\n"
               "\\usepackage[utf8]{inputenc}\n\\usepackage[T1]{fontenc}\n"
               "\\usepackage[spanish]{babel}\n\\usepackage{geometry}\n"
               "\\usepackage{amsmath}\n\\geometry{margin=2cm}\n"
               "\\usepackage{booktabs}\n"
               "\\title{Informe del método Simplex}\\author{%s}\\date{%s}\n"
               "\\begin{document}\\maketitle\n",safe_members, safe_semester);*/
    
    //Los nombres del grupo responsable del programa.
    presentation_preamble(f);
    presentation_cover(f);
    
    //Explicacion de simplex
    presentation_simplex_slide(f);
    
    //Los nombres de los usuarios del programa
    fprintf(f, "\\section*{Problem}\n"
               "\\textbf{Name:} %s\\\\\n\\textbf{Course:} %s\\\\\n"
               "\\textbf{Members:} %s\\\\\n\\textbf{Semester:} %s\n"
               "\\section*{Mathematical Model}\n", safe_name, safe_course,
               safe_members, safe_semester);
    fprintf(f, "\\[ %s \\quad Z = ", lp->objective_function->type == MAXIMIZE ? "\\max" : "\\min");
    for (size_t j = 0; j < lp->num_decision_vars; ++j) {
        double a = lp->objective_function->coeffs->coefficients[j];
        if (a == 0.0) {
            continue;
        }
        if (a < 0.0) {
            fprintf(f, j == 0 ? "-" : " - ");
        } else if (j > 0) {
            fprintf(f, " + ");
        }
        fprintf(f, "%.4g\\,x_{%zu}", fabs(a), j + 1);
    }
    fprintf(f, " \\]\n\\begin{align*}\n\\text{S.t.}\n");
    for (size_t i = 0; i < lp->constraints->count; ++i) {
        for (size_t j = 0; j < lp->num_decision_vars; ++j) {
            double a = lp->constraints->coeff_sets[i]->coefficients[j];
            if (a == 0.0) {
                continue;
            }
            if (a < 0.0) {
                fprintf(f, j == 0 ? "-" : " - ");
            } else if (j > 0) {
                fprintf(f, " + ");
            }
            fprintf(f, "%.4g\\,x_{%zu}", fabs(a), j + 1);
        }
        fprintf(f, " &\\le %.4g \\\\\n", lp->constraints->rhs_values[i]);
    }
    fprintf(f, "x_j &\\ge 0\\quad\\text{for every variable }j\n\\end{align*}\n");

    write_tableau(f, spt, initial_snapshot, "Initial table");

    if (include_intermediate && history_count > 0) {
        fprintf(f, "\\section*{Middle tables}\nTable states were saved during the iterations\\par\n");
        for (size_t k = 0; k < history_count; ++k) {
            char caption[64];
            g_snprintf(caption, sizeof(caption), "After step %zu", k + 1);
            write_tableau(f, spt, &history[k], caption);
        }
    }
    fprintf(f, "\\section*{Result}\n%s\\par\n", status);

    TableauSnapshot final_snapshot = {0};
    if (save_snapshot(&final_snapshot, spt)) {
        write_tableau(f, spt, &final_snapshot, "Final Table");
        free_snapshot(&final_snapshot);
    }

    presentation_solution(f, spt, final_status, NULL);

    fprintf(f, "\\end{document}\n");
    fclose(f);
    g_free(safe_name); g_free(safe_members); g_free(safe_course); g_free(safe_semester);

    gchar *argv[] = { (gchar *)"pdflatex", (gchar *)"-interaction=nonstopmode",
                      (gchar *)"-halt-on-error", (gchar *)tex_path, NULL };
    gint exit_status = -1;
    GError *error = NULL;
    gboolean spawned = g_spawn_sync(NULL, argv, NULL, G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                                    NULL, NULL, NULL, NULL, &exit_status, &error);
    if (!spawned || !g_spawn_check_wait_status(exit_status, &error)) {
        gchar *msg = g_strdup_printf("Couldn't compile PDF. Download pdflatex and install it. Source: %s%s%s",
                                     tex_path, error ? "\n" : "", error ? error->message : "");
        show_error(msg);
        g_free(msg);
        if (error) g_error_free(error);
        return FALSE;
    }

    const char *auxiliary_files[] = {"informe_simplex.aux", "informe_simplex.log",
        "informe_simplex.out","informe_simplex.toc","informe_simplex.synctex.gz"};

    for (size_t i = 0; i < sizeof(auxiliary_files) / sizeof(auxiliary_files[0]); ++i) {
        g_remove(auxiliary_files[i]);
    }

    gchar *pdf_path = g_strdup("informe_simplex.pdf");
    gchar *open_argv[] = { (gchar *)"evince", (gchar *) "--presentation", pdf_path, NULL };
    if (!g_spawn_async(NULL, open_argv, NULL, G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                       NULL, NULL, NULL, &error)) {
        g_clear_error(&error);
        gchar *fallback[] = { (gchar *)"xdg-open", pdf_path, NULL };
        if (!g_spawn_async(NULL, fallback, NULL, G_SPAWN_SEARCH_PATH,
                           NULL, NULL, NULL, &error)) {
            gchar *msg = g_strdup_printf("PDF created on %s",
                                         pdf_path, error ? error->message : "no way to open PDF.");
            set_status(msg);
            g_free(msg);
            g_clear_error(&error);
        }
    }
    g_free(pdf_path);
    return TRUE;
}

static void free_model(LPDefinition *lp) {
    if (!lp) return;    
    if (lp->objective_function) lp_objectivefunc_free(lp->objective_function);
    if (lp->constraints) lp_constraints_free(lp->constraints);
    if (lp->decision_variables) lp_decisionvars_free(lp->decision_variables);
    lp_definition_free(lp);
}

static void on_solve_clicked(GtkButton *button, gpointer user_data) {
    (void)button; (void)user_data;
    if (app.nvars < MIN_FIELDS || app.nconstraints < MIN_FIELDS) {
        show_error("First press <Update fields> to generate variables and constraints.");
        return;
    }

    LPDecisionVars *vars = lp_decisionvars_create(app.nvars);
    if (!vars) { show_error("Couldn't create variable list."); return; }
    double obj_values[MAX_FIELDS] = {0};
    for (size_t j = 0; j < app.nvars; ++j) {
        const char *name = gtk_entry_get_text(GTK_ENTRY(app.var_name_entries[j]));
        if (!name || !*name) {
            lp_decisionvars_free(vars);
            show_error("Every variable must have a name"); return;
        }
        lp_decisionvars_set_var(vars, j, name);
        if (!parse_entry_number(app.objective_coeff_entries[j], &obj_values[j])) {
            lp_decisionvars_free(vars);
            show_error("Every coefficient from variables must have a valid number."); return;
        }
    }

    LPCoefficients *obj_coeffs = lp_coefficients_create_from_array(app.nvars, obj_values);
    if (!obj_coeffs) { lp_decisionvars_free(vars); show_error("Couldn't create coefficients."); return; }
    enum ObjectiveFunctionType obj_type = gtk_combo_box_get_active(GTK_COMBO_BOX(app.objective_combo)) == 1 ? MINIMIZE : MAXIMIZE;
    LPObjectiveFunc *obj = lp_objectivefunc_create(obj_type, app.nvars, obj_coeffs);
    if (!obj) {
        lp_coefficients_free(obj_coeffs); lp_decisionvars_free(vars);
        show_error("Couldn't create objective function."); return;
    }

    LPConstraints *constraints = lp_constraints_create(app.nconstraints, app.nvars);
    if (!constraints) {
        lp_objectivefunc_free(obj); lp_decisionvars_free(vars);
        show_error("Couldn't create constraints."); return;
    }
    for (size_t i = 0; i < app.nconstraints; ++i) {
        double row_values[MAX_FIELDS] = {0}, rhs = 0;
        for (size_t j = 0; j < app.nvars; ++j) {
            if (!parse_entry_number(app.constraint_coeff_entries[i][j], &row_values[j])) {
                lp_constraints_free(constraints); lp_objectivefunc_free(obj); lp_decisionvars_free(vars);
                show_error("Every coefficient from constraints must have a valid number."); return;
            }
        }
        if (!parse_entry_number(app.constraint_rhs_entries[i], &rhs) || rhs < 0) {
            lp_constraints_free(constraints); lp_objectivefunc_free(obj); lp_decisionvars_free(vars);
            show_error("Each right-hand side must be a real number greater than or equal to zero."); return;
        }
        LPCoefficients *row = lp_coefficients_create_from_array(app.nvars, row_values);
        if (!row) {
            lp_constraints_free(constraints); lp_objectivefunc_free(obj); lp_decisionvars_free(vars);
            show_error("Couldn't create coefficients from constraints."); return;
        }
        lp_constraints_add_constraint(constraints, row, LE, rhs);
        lp_coefficients_free(row);
    }

    LPDefinition *lp = lp_definition_create(vars, obj, constraints);
    if (!lp) {
        lp_constraints_free(constraints); lp_objectivefunc_free(obj); lp_decisionvars_free(vars);
        show_error("LPR not valid."); return;
    }
    SPTableau *spt = sp_tableau_create(lp);
    if (!spt) {
        free_model(lp);
        show_error("Couldn't create Simplex"); return;
    }

    gboolean include_intermediate = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app.show_intermediate_check));
    TableauSnapshot initial_snapshot = {0};
    TableauSnapshot *history = include_intermediate ? calloc(MAX_SIMPLEX_STEPS, sizeof(*history)) : NULL;
    size_t history_count = 0;
    if (!save_snapshot(&initial_snapshot, spt)) {
        free(history); sp_tableau_free(spt); free_model(lp);
        show_error("Couldn't save initial table."); return;
    }
    size_t steps = 0;
    enum SPTableauStatus result = SPT_STANDBY;
    while (steps < MAX_SIMPLEX_STEPS) {
        result = sp_tableau_solve_step(spt);
        ++steps;
        if (include_intermediate && history && history_count < MAX_SIMPLEX_STEPS) {
            if (save_snapshot(&history[history_count], spt)) ++history_count;
        }
        if (result == SPT_OPTIMAL_SOLUTION_FOUND ||
            result == SPT_MULTPILE_SOLUTIONS_DETECTED ||
            result == SPT_UNBOUNDED_PROBLEM || result == SPT_DEGENERATED_LOOP ||
            result == SPT_INFEASIBLE_PROBLEM || result == SPT_INVALID_STATE) break;
    }
    if (steps >= MAX_SIMPLEX_STEPS && result != SPT_OPTIMAL_SOLUTION_FOUND && result != SPT_MULTPILE_SOLUTIONS_DETECTED && result != SPT_UNBOUNDED_PROBLEM && result != SPT_INFEASIBLE_PROBLEM && result != SPT_INVALID_STATE)
        result = SPT_DEGENERATED_LOOP;

    const char *result_message = status_text(result);
    const char *problem_name = gtk_entry_get_text(GTK_ENTRY(app.problem_name_entry));
    if (!problem_name || !*problem_name) problem_name = "Linear Programming Problem";
    gboolean pdf_ok = write_report_and_pdf(lp, spt, problem_name, result_message,
                                           include_intermediate, steps, result,
                                           &initial_snapshot, history, history_count);
    gchar *summary = g_strdup_printf("%s Steps: %zu. %s",
                                     result_message, steps,
                                     pdf_ok ? "PDF generated: informe_simplex.pdf" : "Couldn't generate PDF.");
    set_status(summary);
    show_info(summary);
    g_free(summary);

    free_snapshot(&initial_snapshot);
    for (size_t k = 0; k < history_count; ++k) free_snapshot(&history[k]);
    free(history);
    sp_tableau_free(spt);
    free_model(lp);
}

static void on_main_window_destroy(GtkWidget *widget, gpointer user_data) {
    (void)widget; (void)user_data;
    if (app.builder) g_object_unref(app.builder);
    gtk_main_quit();
}

static gboolean load_builder_file(GtkBuilder *builder, GError **error) {
    if (g_file_test(UI_FILE_1, G_FILE_TEST_EXISTS))
        return gtk_builder_add_from_file(builder, UI_FILE_1, error) != 0;
    if (g_file_test(UI_FILE_2, G_FILE_TEST_EXISTS))
        return gtk_builder_add_from_file(builder, UI_FILE_2, error) != 0;
    g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_NOENT,
                "Couldn't find simplex.glade. Search '%s' and '%s'.", UI_FILE_1, UI_FILE_2);
    return FALSE;
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);
    memset(&app, 0, sizeof(app));
    app.builder = gtk_builder_new();
    GError *error = NULL;
    if (!load_builder_file(app.builder, &error)) {
        g_printerr("Error loading Glade: %s\n", error ? error->message : "Unknown error");
        if (error) g_error_free(error);
        g_object_unref(app.builder);
        return EXIT_FAILURE;
    }

    app.window = get_widget("main_window");
    app.problem_name_entry = get_widget("problem_name_entry");
    app.variables_spin = get_widget("variables_spin");
    app.restricciones_spin = get_widget("restricciones_spin");
    app.objective_combo = get_widget("objective_combo");
    app.objective_fields_box = get_widget("objective_fields_box");
    app.constraints_fields_box = get_widget("constraints_fields_box");
    app.show_intermediate_check = get_widget("show_intermediate_check");
    app.members_entry = get_widget("members_entry");
    app.course_entry = get_widget("course_entry");
    app.semester_entry = get_widget("semester_entry");
    app.output_status_label = get_widget("output_status_label");

    if (!app.window || !app.objective_fields_box || !app.constraints_fields_box) {
        g_printerr("Glade Object Error.\n");
        g_object_unref(app.builder);
        return EXIT_FAILURE;
    }
    g_signal_connect(app.window, "destroy", G_CALLBACK(on_main_window_destroy), NULL);
    g_signal_connect(get_widget("generate_fields_button"), "clicked", G_CALLBACK(on_generate_fields_clicked), NULL);
    g_signal_connect(get_widget("clear_button"), "clicked", G_CALLBACK(on_clear_clicked), NULL);
    g_signal_connect(get_widget("solve_button"), "clicked", G_CALLBACK(on_solve_clicked), NULL);

    gtk_combo_box_set_active(GTK_COMBO_BOX(app.objective_combo), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app.variables_spin), 2);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app.restricciones_spin), 2);
    on_generate_fields_clicked(NULL, NULL);
    gtk_widget_show_all(app.window);
    gtk_main();
    return EXIT_SUCCESS;
}