//
// Copyright (c) 2016, Leander Tentrup, Saarland University
//
// Licenced under ISC Licsnse, see ./LICENSE.txt form information
//

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "combine.h"
#include "moore.h"
#include "aiger.h"

typedef enum { MEALY, MOORE } Semantic;

void print_usage(const char* name) {
    printf("%s [OPTIONS] <ltl_monitor.aag> <implementation.aag>\n"
           "ltl_monitor.aag has to be generated from smvtoaig tool\n"
           "OPTIONS:\n"
           "  --moore     Check that implementation.aag is (syntactically) a Moore machine\n"
           "  --help      Print this help\n",
           name);
}

int main(int argc, const char* argv[]) {

    Semantic semantic = MEALY;
    unsigned int num_files = 0;
    const char* monitor_file_name = NULL;
    const char* implementation_file_name = NULL;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--moore") == 0) {
                semantic = MOORE;
            }
            else if (strcmp(argv[i], "--help") == 0) {
                print_usage(argv[0]);
                return 0;
            }
            else {
                fprintf(stderr, "Unknown option: %s\n", argv[i]);
                print_usage(argv[0]);
                return 1;
            }
        }
        else {
            if (num_files == 0) {
                monitor_file_name = argv[i];
            }
            else if (num_files == 1) {
                implementation_file_name = argv[i];
            }
            num_files++;
        }
    }

    if (num_files != 2) {
        fprintf(stderr, "Expect exactly two inputs\n");
        print_usage(argv[0]);
        return 1;
    }

    aiger* monitor = aiger_init();
    if (monitor == NULL) {
        return 1;
    }

    const char* res = aiger_open_and_read_from_file(monitor, monitor_file_name);
    if (res != NULL) {
        fprintf(stderr, "error: cannot read monitor file\n");
        fprintf(stderr, "%s\n", res);
        return 1;
    }

    aiger* implementation = aiger_init();
    if (implementation == NULL) {
        return 1;
    }

    res = aiger_open_and_read_from_file(implementation, implementation_file_name);
    if (res != NULL) {
        fprintf(stderr, "error: cannot read implementation file\n");
        fprintf(stderr, "%s\n", res);
        return 1;
    }

    if (semantic == MOORE && !is_moore(implementation)) {
        fprintf(stderr, "error: could not certify that implementation is a Moore machine\n");
        return 1;
    }

    aiger* combination = combine(monitor, implementation);
    if (combination == NULL) {
        fprintf(stderr, "error: could not combine monitor and implementation\n");
        return 1;
    }

    assert(!aiger_check(combination));

    //aiger_reencode(combination);
    aiger_write_to_file(combination, aiger_ascii_mode, stdout);

    // free memory
    aiger_reset(monitor);
    aiger_reset(implementation);
    aiger_reset(combination);

    return 0;
}
