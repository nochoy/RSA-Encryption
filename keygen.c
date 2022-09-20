
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"

#define OPTIONS "hvb:i:n:d:s:"

// prints help statement
void print_help(void) {
    printf("SYNOPSIS\n   Generates an RSA public/private ket pair.\n\n");
    printf("USAGE\n   ./keygen [-hv] [-b bits] -n pbfile -d pvfile\n\n");
    printf("OPTIONS\n");
    printf("   -h              Display program help and usage.\n");
    printf("   -v              Display verbose program output.\n");
    printf("   -b bits         Minimum bits needed for public key n.\n");
    printf("   -c confidence   Miller-Rabin iterations for testing primes (default: 50).\n");
    printf("   -n pbfile       Public key file (default: rsa.pub).\n");
    printf("   -d pvfile       Private key file (default: rsa.priv).\n");
    printf("   -s seed         Random seed for testing.\n");
}

// main function to parse command line options and create public and private keys
int main(int argc, char **argv) {
    FILE *pbfile = NULL;
    FILE *pvfile = NULL;
    bool v_case = false;
    bool n_case = false;
    bool d_case = false;
    uint64_t pubkey_bits = 256; // min bits for public key n defaulted to 256
    uint64_t MR_iters = 50;     // Miller-Rabin iterations defaulted to 50
    uint64_t seed = time(NULL); // seed defaulted to time(NULL);
    int64_t opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': print_help(); return 1; break;
        case 'v': v_case = true; break;
        case 'b': pubkey_bits = strtoul(optarg, NULL, 10); break;
        case 'c': MR_iters = strtoul(optarg, NULL, 10); break;
        case 'n':
            if ((pbfile = fopen(optarg, "w+")) == NULL) {
                printf("Failed to open %s\n", optarg);
                return 1;
            }
            n_case = true;
            break;
        case 'd':
            if ((pvfile = fopen(optarg, "w+")) == NULL) {
                printf("Failed to open %s\n", optarg);
                return 1;
            }
            d_case = true;
            break;
        case 's': seed = strtoul(optarg, NULL, 10); break;
        default: print_help(); return 1; break;
        }
    }

    // if pbfile not specified, default to rsa.pub
    if (!n_case) { 
        if ((pbfile = fopen("rsa.pub", "w+")) == NULL) {
            printf("Failed to open rsa.pub\n");
            return 1;
        }
    }
    // if pvfile not specified, default pvfile to rsa.priv
    if (!d_case) { 
        if ((pvfile = fopen("rsa.priv", "w+")) == NULL) {
            printf("Failed to open rsa.priv\n");
            return 1;
        }
    }

    // set pvfile permissions
    int pvfile_fd = fileno(pvfile);
    fchmod(pvfile_fd, 0600);

    // initialize the random state
    randstate_init(seed);

    // create public and private keys
    mpz_t p, q, n, e, d, user, s;
    mpz_inits(p, q, n, e, d, user, s, NULL);
    rsa_make_pub(p, q, n, e, pubkey_bits + 1, MR_iters);
    rsa_make_priv(d, e, p, q);

    // get current user name and convert to mpz_t
    char *username = getenv("USER");
    mpz_set_str(user, username, 62);

    // create signature
    rsa_sign(s, user, d, n);

    // write public key to pbfile and private key to pvfile
    rsa_write_pub(n, e, s, username, pbfile);
    rsa_write_priv(n, d, pvfile);

    if (v_case) { // if verbose print is selected
        printf("user = %s\n", username);
        gmp_printf("s (%lu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("p (%lu bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%lu bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%lu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%lu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("d (%lu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    // cleanup time
    fclose(pbfile);
    fclose(pvfile);
    randstate_clear();
    mpz_clears(p, q, n, e, d, user, s, NULL);
    return 0;
}
