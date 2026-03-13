#include "dpll.h"

#include "cnf.h"
#include "err.h"
#include "list.h"
#include "util.h"
#include "variables.h"

typedef enum Reason { CHOSEN, IMPLIED } Reason;

/**
 * Struct to represent an entry in the assignment stack. Should only be created
 * and freed by pushAssignment and popAssignment.
 */
typedef struct Assignment {
    VarIndex var;
    Reason reason;
} Assignment;

/**
 * Adds a new assignment to the assignment stack.
 *
 * @param stack  an assignment stack
 * @param var    the variable to assign
 * @param r      the reason for the assignment
 */
void pushAssignment(List* stack, VarIndex var, Reason r) {
    Assignment* a = (Assignment*)malloc(sizeof(Assignment));
    a->var = var;
    a->reason = r;
    push(stack, a);
}

/**
 * Removes the head element of an assignment stack and frees it.
 *
 * @param stack  an assignment stack
 */
void popAssignment(List* stack) {
    Assignment* a = (Assignment*)peek(stack);
    free(a);
    pop(stack);
}

/**
 * Führt eine Iteration des DPLL Algorithmus aus.
 *
 * @param vt       die zugrunde liegende Variablentabelle
 * @param stack    der Zuweisungsstack
 * @param cnf      die zu prüfende Formel
 * @return         1 falls der Algorithmus mit SAT terminieren sollte,
 *                 0 falls der Algorithmus weiterlaufen sollte,
 *                 -1 falls der Algorithmus mit UNSAT terminieren sollte
 */
/**
 * Performs one iteration of the DPLL algorithm.
 *
 * @param vt       the underlying variable table
 * @param stack    an assignment stack
 * @param cnf      the formula to check
 * @return         1 if the algorithm should terminate with SAT,
 *                 0 if the algorithm should continue,
 *                -1 if the algorithm should terminate with UNSAT
 */

int backtracking(List* list, VarTable* vt) {
    // while loop till list is empty
    while (!isEmpty(list)) {
        // assigns top element of stack to x
        Assignment* x = peek(list);

        // uses switch to check value of "reason" field of x
        switch (x->reason) {
            // case IMPLIED
            case IMPLIED: {
                // removes top element from list
                popAssignment(list);
                break;
            }

            // case CHOSEN
            case CHOSEN: {
                // makes variable value to TRUE
                updateVariableValue(vt, x->var, FALSE);
                // switches CHOSEN to IMPLIED
                x->reason = IMPLIED;
                return 0;
            }

            // case neither CHOSEN nor IMPLIED
            default: {
                err("Error: Reason is Invalid");
                break;
            }
        }
    }
    return 1;
}

int iterate(VarTable* vt, List* stack, CNF* cnf) {
    // uses switch that evaluates CNF and provides three cases
    switch (evalCNF(cnf)) {
        // case TRUE
        case TRUE: {
            // returns 1 if formula = satisfiable
            return 1;
            break;
        }

        // case FALSE
        case FALSE: {
            //  if stack = empty, formula = unsatisfiable
            if (!isEmpty(stack)) {
                backtracking(stack, vt);
            } else {
                return -1;
            }
            break;
        }

        // case UNDEFINED
        case UNDEFINED: {
            // declares list iterator y to iterate over list of clauses
            ListIterator y = mkIterator(&cnf->clauses);

            int unit_literal_count = 0;

            // runs while loop till iterator is valid
            while (isValid(&y)) {
                // assigns current clause from iterator to current_clause
                Clause* current_clause = (Clause*)getCurr(&y);

                // assigns uni_literal
                Literal unit_literal = getUnitLiteral(vt, current_clause);

                // If unit literal != 0, updates variable's value
                if (unit_literal != 0) {
                    TruthValue unit_literal_value;
                    if (unit_literal < 0) {
                        //-ve unit literal
                        unit_literal_value = FALSE;
                    } else {
                        //+ve unit literal
                        unit_literal_value = TRUE;
                    }

                    updateVariableValue(vt, abs(unit_literal),
                                        unit_literal_value);

                    // pushes assignment onto stack with unit literal and reason
                    // IMPLIED
                    pushAssignment(stack, (unit_literal), IMPLIED);

                    // incremented to indicate presence of unit literal
                    unit_literal_count++;
                }
                // iterator goes to next item in list
                next(&y);
            }

            // if unit_literal_count <= 0, no unit clause was found
            if (unit_literal_count <= 0) {
                VarIndex undefined_var = getNextUndefinedVariable(vt);

                // if undefined_var != 0, updates variable's value to TRUE
                if (undefined_var != 0) {
                    updateVariableValue(vt, undefined_var, TRUE);

                    pushAssignment(stack, undefined_var, CHOSEN);

                    // calls "iterate" func recursively
                    int recursive_iterate = iterate(vt, stack, cnf);

                    // If recursive call returns 1, formula = satisfiable
                    if (recursive_iterate == 1) {
                        return 1;
                    }
                }
            }
            break;
        }

        // case No condtions match
        default: {
            err("Error: No conditions match");
            break;
        }
    }

    // returns 0 if no previous conditions meet
    return 0;
}

char isSatisfiable(VarTable* vt, CNF* cnf) {
    List stack = mkList();

    int res;
    do {
        res = iterate(vt, &stack, cnf);
    } while (res == 0);

    while (!isEmpty(&stack)) {
        popAssignment(&stack);
    }

    return (res < 0) ? 0 : 1;
}
