#include "parser.h"

#include "cnf.h"
#include "ctype.h"
#include "err.h"
#include "lexer.h"
#include "list.h"
#include "propformula.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "util.h"

/**
 * Assigns symbols to strings.
 *
 * Aborts the program with an error message if an invalid input is detected.
 *
 * @param str  a string to translate
 * @return     the resulting symbol
 */

FormulaKind toKind(const char* str) {
    // checks if string is empty or NULL
    if (*str == '\0' || str == NULL) {
        err("Error: String is Empty");
    }

    // compares string to return appropriate formula kind
    if (strcmp(str, "&&") == 0 || strcmp(str, "AND") == 0) {
        return AND;
    } else if (strcmp(str, "||") == 0 || strcmp(str, "OR") == 0) {
        return OR;
    } else if (strcmp(str, "=>") == 0 || strcmp(str, "IMPLIES") == 0) {
        return IMPLIES;
    } else if (strcmp(str, "<=>") == 0 || strcmp(str, "EQUIV") == 0) {
        return EQUIV;
    } else if (strcmp(str, "!") == 0 || strcmp(str, "NOT") == 0) {
        return NOT;
        // checks if str[0] is alphanumeric
    } else if (isalnum(str[0]) != 0) {
        // checks if each character is alphanumeric
        for (int i = 0; i < ((int)strlen(str)); i++) {
            if (isalnum(str[i]) == 0) {
                err("Error: Variable is incorrect");
            }
        }
        return VAR;
        // if no conditions match, formula kind is incorrect
    } else {
        err("Error: Formula Kind is incorrect");
    }
}

PropFormula* parseFormula(FILE* input, VarTable* vt) {
    // creates stack called "list"
    List list = mkList();

    // declares a pointer new_formula
    PropFormula* new_formula;

    // to store formula node
    void* x;
    // to store formula node
    void* y;

    // reads next token from input file and assigns it to "token"
    char* token = nextToken(input);

    // runs a while loop while token is not equal to NULL
    while (token != NULL) {
        // determines kind of formula represented by current token
        FormulaKind kind = toKind(token);

        // switch based on kind of formula
        switch (kind) {
            // VAR case
            case VAR: {
                // creates a new variable formula and pushes onto stack
                new_formula = mkVarFormula(vt, token);
                push(&list, new_formula);
                break;
            }

            // AND, OR, IMPLIES, EQUIV case
            case AND:
            case OR:
            case IMPLIES:
            case EQUIV: {
                // checks if stack = empty
                if (isEmpty(&list)) {
                    err("Invalid");
                }

                // assigns formula to x
                x = peek(&list);
                pop(&list);

                // checks if stack = empty
                if (isEmpty(&list)) {
                    err("Invalid");
                }

                // assigns formula to y
                y = peek(&list);
                pop(&list);

                // creates a new binary formula and pushes onto stack
                new_formula = mkBinaryFormula(kind, y, x);
                push(&list, new_formula);
                break;
            }

            // NOT case
            case NOT: {
                // checks if stack = empty
                if (isEmpty(&list)) {
                    err("Invalid");
                }

                // assigns formula to x
                x = peek(&list);
                pop(&list);

                // creates a new unary formula and pushes onto stack
                new_formula = mkUnaryFormula(NOT, x);
                push(&list, new_formula);
                break;
            }

            default: {
                err("Invalid");
            }
        }
        // reads next token from input file and assigns it to "token"
        token = nextToken(input);
    }

    // checks if stack is empty
    if (isEmpty(&list)) {
        err("Error: Stack is Empty");
    } else {
        // assigns formula to x
        x = peek(&list);
        pop(&list);

        // if stack = empty, return x
        if (isEmpty(&list)) {
            return x;
        } else {
            err("Error: Incorrect Formula");
        }
    }
}
