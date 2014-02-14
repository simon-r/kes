/*  Author: R. Bourquin
    Copyright: (C) 2014 R. Bourquin
    License: GNU GPL v2 or above
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libkes2.h"


int main(int argc, char* argv[]) {
    int i;
    int maxn, maxp;
    int n, p;
    int validate_ext, validate_weights;
    fmpq_poly_t Hn, En;
    int solvable;
    long nrroots, nrpweights;
    int record;
    fmpz_mat_t table;
    int loglevel;

    if(argc <= 1) {
        printf("Search for generalized Kronrod extensions of Gauss-Hermite rules\n");
        printf("Syntax: kes_enumerate [-ve] [-vw] [-l L] max_n max_p\n");
        printf("        -vne  Validate the polynomial extension (default on)\n");
        printf("        -vw   Validate the weights (default off)\n");
        printf("        -l   Set the log level\n");
        return EXIT_FAILURE;
    }

    maxn = 1;
    maxp = 1;
    validate_ext = 1;
    validate_weights = 0;
    loglevel = 0;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-vne")) {
            validate_ext = 0;
        } else if (!strcmp(argv[i], "-vw")) {
            validate_weights = 1;
        } else if (!strcmp(argv[i], "-l")) {
            loglevel = atoi(argv[i+1]);
            i++;
        } else {
            if(i + 1 < argc) {
                maxn = atoi(argv[i]);
                maxp = atoi(argv[i+1]);
                break;
            } else {
                printf("Missing values for max_n or max_p!\n");
                return EXIT_FAILURE;
            }
        }
    }

    /* Table for results */
    fmpz_mat_init(table, maxn, maxp);

    fmpq_poly_init(Hn);
    fmpq_poly_init(En);

    for(n = 1; n <= maxn; n++) {
	hermite_polynomial_pro(Hn, n);

        for(p = 1/*(n-1)/2+1*/; p <= maxp; p++) {
            printf("Trying to find an order %i Kronrod extension for H%i\n", p, n);
	    record = 0;

            solvable = find_extension(En, Hn, p, loglevel);
            printf("  Solvable extension rule found: %i\n", solvable);

	    if(solvable && validate_weights) {
		record = validate_rule(&nrroots, &nrpweights, En, NCHECKDIGITS, loglevel);
	    } else if(solvable && validate_ext) {
		record = validate_extension_by_poly(&nrroots, En, NCHECKDIGITS, loglevel);
	    } else {
		record = solvable;
	    }

            fmpz_set_ui(fmpz_mat_entry(table , n-1 , p-1), record);
        }
    }

    printf("==============================================\n");
    fmpz_mat_print_pretty(table);
    printf("\n");
    printf("==============================================\n");

    fmpq_poly_clear(Hn);
    fmpq_poly_clear(En);
    fmpz_mat_clear(table);

    return EXIT_SUCCESS;
}
