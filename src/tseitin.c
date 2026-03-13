#include "tseitin.h"

#include <stdio.h>

#include "err.h"
#include "util.h"

/**
 * Inserts a clause with one literal into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    a literal
 */
void addUnaryClause(VarTable* vt, CNF* cnf, Literal a) {
    Clause* clause = mkTernaryClause(vt, a, 0, 0);
    addClauseToCNF(cnf, clause);
}

/**
 * Inserts a clause with two literals into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    the first literal
 * @param b    the second literal
 */
void addBinaryClause(VarTable* vt, CNF* cnf, Literal a, Literal b) {
    Clause* clause = mkTernaryClause(vt, a, b, 0);
    addClauseToCNF(cnf, clause);
}

/**
 * Inserts a clause with three literals into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    the first literal
 * @param b    the second literal
 * @param c    the third literal
 */
void addTernaryClause(VarTable* vt, CNF* cnf, Literal a, Literal b, Literal c) {
    Clause* clause = mkTernaryClause(vt, a, b, c);
    addClauseToCNF(cnf, clause);
}

/**
 * Adds clauses for a propositional formula to a CNF.
 *
 * For a propositional formula pf, clauses that are added that are equivalent to
 *
 *     x <=> pf
 *
 * where x is usually a fresh variable. This variable is also returned.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param pf   a propositional formula
 * @return     the variable x, as described above
 */
VarIndex addClauses(VarTable* vt, CNF* cnf, const PropFormula* pf) {
    switch (pf->kind) {
        // if formula = variable, return literal
        case VAR: {
            return pf->data.var;
        }
        // if formula = AND, add 3 clauses and return z
        case AND: {
            VarIndex x = addClauses(vt, cnf, pf->data.operands[0]);
            VarIndex y = addClauses(vt, cnf, pf->data.operands[1]);
            VarIndex z = mkFreshVariable(vt);
            addBinaryClause(vt, cnf, -z, x);
            addBinaryClause(vt, cnf, -z, y);
            addTernaryClause(vt, cnf, -x, -y, z);
            return z;
        }
        // if formula = OR, add 3 clauses and return z
        case OR: {
            VarIndex x = addClauses(vt, cnf, pf->data.operands[0]);
            VarIndex y = addClauses(vt, cnf, pf->data.operands[1]);
            VarIndex z = mkFreshVariable(vt);
            addTernaryClause(vt, cnf, -z, x, y);
            addBinaryClause(vt, cnf, -x, z);
            addBinaryClause(vt, cnf, -y, z);
            return z;
        }
        // if formula = IMPLIES, add 3 clauses and return z
        case IMPLIES: {
            VarIndex x = addClauses(vt, cnf, pf->data.operands[0]);
            VarIndex y = addClauses(vt, cnf, pf->data.operands[1]);
            VarIndex z = mkFreshVariable(vt);
            addTernaryClause(vt, cnf, -z, -x, y);
            addBinaryClause(vt, cnf, x, z);
            addBinaryClause(vt, cnf, -y, z);
            return z;
        }
        // if formula = EQUIV, add 4 clauses and return z
        case EQUIV: {
            VarIndex x = addClauses(vt, cnf, pf->data.operands[0]);
            VarIndex y = addClauses(vt, cnf, pf->data.operands[1]);
            VarIndex z = mkFreshVariable(vt);
            addTernaryClause(vt, cnf, -z, -x, y);
            addTernaryClause(vt, cnf, -z, -y, x);
            addTernaryClause(vt, cnf, z, -x, -y);
            addTernaryClause(vt, cnf, z, x, y);
            return z;
        }
        // if formula = NOT, add 2 clauses and return y
        case NOT: {
            VarIndex x = addClauses(vt, cnf, pf->data.single_op);
            VarIndex y = mkFreshVariable(vt);
            addBinaryClause(vt, cnf, -y, -x);
            addBinaryClause(vt, cnf, x, y);
            return y;
        }
        // if no case matched, return error
        default:
            err("Invalid");
            break;
    }
}

CNF* getCNF(VarTable* vt, const PropFormula* f) {
    CNF* res = mkCNF();

    VarIndex x = addClauses(vt, res, f);

    addUnaryClause(vt, res, x);

    return res;
}
