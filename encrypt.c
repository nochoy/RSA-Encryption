
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"

#define OPTIONS "-hvi:o:n:"

// prints help statement
void print_help(void) {
    printf("SYNOPSIS\n   Encrypts data using RSA encryption.\n");
    printf("   Encrypted data is decrypted by the decrypt program.\n\n");
    printf("USAGE\n   ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey -d privkey\n\n");
    printf("OPTIONS\n");
    printf("   -h              Display program help and usage.\n");
    printf("   -v              Display verbose program output.\n");
    printf("   -i infile       Input file of data to encrypt (default: stdin).\n");
    printf("   -o outfile      Output file for encrypted data (default: stdout).\n");
    printf("   -n pbfile       Public key file (default: rsa.pub).\n");
}

// takes in input, output, and public key files
// closes files
void close_files(FILE *infile, FILE *outfile, FILE *pbfile) {
    fclose(infile);
    fclose(outfile);
    fclose(pbfile);
}

// main function to parse command line options and encrypt files
int main(int argc, char **argv) {
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pbfile = NULL;
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
                printf("Failed to open outfile\n");
                return 1;
            }
            break;
        case 'n':
            if ((pbfile = fopen(optarg, "r")) == NULL) {
                printf("Failed to open pbfile\n");
                return 1;
            }
            n_case = true;
            break;
        default: print_help(); return 1; break;
        }
    }

    // if pbfile not specified, default pbfile to rsa.pub
    if (!n_case) { 
        if ((pbfile = fopen("rsa.pub", "r")) == NULL) {
            printf("Failed to open rsa.pub\n");
            return 1;
        }
    }

    // read public key from infile
    mpz_t n, e, s, user;
    mpz_inits(n, e, s, user, NULL);
    char *username = getenv("USER");
    rsa_read_pub(n, e, s, username, pbfile);

    if (v_case) { // if verbose print is selected
        printf("user = %s\n", username);
        gmp_printf("s (%lu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("n (%lu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%lu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    // convert username to mpz_t
    mpz_set_str(user, username, 62);
    // verify signature
    if (!rsa_verify(user, s, e, n)) {
        printf("Error: cannot be verified\n");
        mpz_clears(n, e, s, user, NULL);
        close_files(infile, outfile, pbfile);
        return 1;
    }
    
    // encrypt file
    rsa_encrypt_file(infile, outfile, n, e);
    
    // cleanup time
    close_files(infile, outfile, pbfile);
    mpz_clears(n, e, s, user, NULL);
    return 0;
}
