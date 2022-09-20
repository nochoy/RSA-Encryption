
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"

#define OPTIONS "-hvi:o:n:"

// prints help statement
void print_help(void) {
    printf("SYNOPSIS\n   Decrypts data using RSA decryption.\n");
    printf("   Encrypted data is encrypted by the encrypt program.\n\n");
    printf("USAGE\n   ./decrypt [-hv] [-i infile] [-o outfile] -n pubkey -d privkey\n\n");
    printf("OPTIONS\n");
    printf("   -h              Display program help and usage.\n");
    printf("   -v              Display verbose program output.\n");
    printf("   -i infile       Input file of data to decrypt (default: stdin).\n");
    printf("   -o outfile      Output file for decrypted data (default: stdout).\n");
    printf("   -n pvfile       Private key file (default: rsa.priv).\n");
}

// takes in input, output, and private key files
// closes files
void close_files(FILE *infile, FILE *outfile, FILE *pvfile) {
    fclose(infile);
    fclose(outfile);
    fclose(pvfile);
}

// main function to parse command line options and decrypt file
int main(int argc, char **argv) {
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pvfile = NULL;
    bool v_case = false;
    bool n_case = false;
    int32_t opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': print_help(); return 1; break;
        case 'v': v_case = true; break;
        case 'i':
            if ((infile = fopen(optarg, "r")) == NULL) {
                printf("Failed to open %s\n", optarg);
                return 1;
            }
            break;
        case 'o':
            if ((outfile = fopen(optarg, "w")) == NULL) {
                printf("Failed to open %s\n", optarg);
                return 1;
            }
            break;
        case 'n':
            if ((pvfile = fopen(optarg, "r")) == NULL) {
                printf("Failed to open %s\n", optarg);
                return 1;
            }
            n_case = true;
            break;
        default: print_help(); return 1; break;
        }
    }

    // if pvfile not specified, default pvfile to rsa.priv
    if (!n_case) { 
        if ((pvfile = fopen("rsa.priv", "r")) == NULL) {
            printf("Failed to open rsa.priv\n");
            return 1;
        }
    }

    // read private key from pvfile
    mpz_t n, d, user;
    mpz_inits(n, d, user, NULL);
    rsa_read_priv(n, d, pvfile);
    if (v_case) { // if verbose print is selected
        gmp_printf("n (%lu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%lu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    // decrypt file
    rsa_decrypt_file(infile, outfile, n, d);
    
    // cleanup time
    close_files(infile, outfile, pvfile);
    mpz_clears(n, d, NULL);
    return 0;
}
