//
// Copyright (c) 2019, Philipp Meyer, Technical University of Munich
//
// Licenced under ISC Licsnse, see ./LICENSE.txt form information
//

#include "moore.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    unsigned* stack;
    bool* on_stack;
    size_t top;
    size_t size;
} dfs_stack;

static bool init_stack(dfs_stack* stack, size_t size) {
    assert(size > 0);
    stack->stack = (unsigned*) malloc(size * sizeof(unsigned));
    if (stack->stack == NULL) {
        return false;
    }
    stack->on_stack = (bool*) calloc(size, sizeof(bool));
    if (stack->on_stack == NULL) {
        return false;
    }
    stack->top = 0;
    stack->size = size;
    return true;
}

static void reset_stack(dfs_stack* stack) {
    free(stack->stack);
    free(stack->on_stack);
}

static bool stack_empty(const dfs_stack* stack) {
    return stack->top == 0;
}

static bool stack_full(const dfs_stack* stack) {
    return stack->top == stack->size;
}

static unsigned stack_pop(dfs_stack* stack) {
    assert(!stack_empty(stack));
    stack->top--;
    return stack->stack[stack->top];
}

static void stack_push(dfs_stack* stack, unsigned x) {
    assert(!stack_full(stack));
    if (!stack->on_stack[x]) {
        stack->on_stack[x] = true;
        stack->stack[stack->top] = x;
        stack->top++;
    }
}

/**
 * Check if an input signal is connected through a path to an output signal
 * by a backwards depth-first search. If there is no such path, then the
 * implementation is a Moore machine. Otherwise, it might or might not be a
 * Moore machine (this is only a sufficient syntactic criterion).
 */
bool is_moore(const aiger* implementation) {

    // create stack
    const size_t size = 1 + implementation->maxvar;
    dfs_stack stack;
    if (!init_stack(&stack, size)) {
        fprintf(stderr, "error: could not allocate memory for stack\n");
        return false;
    }

    bool moore = true;

    // look at variable of each output
    for (size_t i = 0; moore && i < implementation->num_outputs; i++) {
        aiger_symbol* output = &implementation->outputs[i];
        stack_push(&stack, aiger_lit2var(output->lit));

        // traverse variables of aig through and gates up to inputs, latches or constants
        while (!stack_empty(&stack)) {
            const unsigned lit = aiger_var2lit(stack_pop(&stack));
            const unsigned tag = aiger_lit2tag(implementation, lit);

            if (tag == 1) { // input
                //aiger_symbol* input = aiger_is_input(implementation, lit);
                //assert(input != NULL);
                //printf("found path from input %s to output %s\n", input->name, output->name);
                moore = false;
                break;
            }
            else if (tag == 3) { // and
                aiger_and* and = aiger_is_and(implementation, lit);
                assert(and != NULL);
                stack_push(&stack, aiger_lit2var(and->rhs0));
                stack_push(&stack, aiger_lit2var(and->rhs1));
            }
            // latch or constant otherwise
        }
    }

    // free memory
    reset_stack(&stack);

    return moore;
}
