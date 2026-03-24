# SAT Solver

A propositional satisfiability solver implemented in C.

The project reads propositional formulas in Reverse Polish Notation, converts them to Conjunctive Normal Form with the Tseytin transformation, and solves them using a DPLL-based search algorithm. It is a compact, well-scoped example of parsing, transformation, search, and manual memory management in C. The assignment is built around separate implementation steps for parsing, CNF conversion, and solving.

## Overview

SAT asks whether a propositional formula can be satisfied by some assignment of truth values. This solver follows the standard pipeline used in many practical implementations:

1. parse the input formula
2. convert it to CNF
3. search for a satisfying assignment

That structure keeps the implementation easy to understand and reason about.

## Features

* Parsing propositional formulas from input files
* Support for `!`, `&&`, `||`, `=>`, and `<=>`
* Stack-based parsing of Reverse Polish Notation
* Tseytin transformation to CNF
* DPLL solving with backtracking
* Unit propagation for early pruning
* Command-line workflow
* Automated tests and sanitizer-based validation

## Project Structure

```text
.
├── src/
│   ├── list.c            # Stack implementation
│   ├── parser.c          # Formula parsing
│   ├── propformula.*     # Formula representation
│   ├── tseitin.c         # CNF conversion
│   ├── dpll.c            # SAT solving
│   ├── variables.*       # Variable handling
│   └── cnf.h             # CNF structures
├── tests/                # Public and hidden tests
├── bin/                  # Built executable
└── Makefile
```

The code is split by responsibility so each part can be tested and maintained independently.

## Design

### Parsing

The parser reconstructs formulas from postfix notation using a stack. This keeps token handling simple and avoids deeply nested parsing logic.

### CNF Conversion

The Tseytin transformation introduces fresh variables for subformulas and produces an equisatisfiable CNF without blowing up the formula size.

### Solving

The DPLL solver explores assignments with backtracking and uses unit propagation to reduce the search space.

### Memory Management

The project uses explicit allocation and cleanup throughout the pipeline. That makes correctness and sanitizer safety part of the implementation, not an afterthought.

## Edge Cases

* malformed token sequences
* invalid operator arity
* broken or incomplete input
* unsatisfiable formulas
* formulas that require backtracking
* cleanup after both success and failure paths

## Testing

The repository is built to be tested with both functional and memory-safety checks.

```bash
# Build the project
make

# Run the solver
bin/satsolver [flags] formula.in

# Run all local tests
./tests/run_tests.py

# Run a specific test
./tests/run_tests.py -t "public.stack.emptyclear"
```

## Why This Project Matters

This project shows how to build a small solver in a way that stays readable and maintainable. It demonstrates practical experience with parsing, data structures, algorithmic reasoning, and safe C programming.

## Future Improvements

Possible extensions include:

* better branching heuristics
* clause learning
* richer diagnostics for invalid input
* performance tuning for larger formulas

## License

This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**.

See the LICENSE file for details.
