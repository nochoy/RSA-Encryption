
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"

// takes in number of bits nbits, number of iterations iters
// create public key with large primes p, q, their product n, public exponent e
// return values through p, q, n, e
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    mpz_t totient, math, p1, q1, rand, gcd_num;
    mpz_inits(totient, math, p1, q1, rand, gcd_num, NULL);
    // calculate the number of bits for p and q
    uint64_t pp = random() % (((3 * nbits) / 4) - (nbits / 4)) + nbits / 4;
    uint64_t qq = nbits - pp;
    // calculate primes p and q
    make_prime(p, pp + 1, iters);
    make_prime(q, qq + 1, iters);
    // calculate totient
    mpz_sub_ui(p1, p, 1);   // p1 = p - 1
    mpz_sub_ui(q1, q, 1);   // q1 = q - 1
    mpz_mul(math, p1, q1);  // totient = p1 * q1
    mpz_set(totient, math);
    // calculate n = p * q
    mpz_mul(n, p, q);
    // calculate public exponent e
    while (true) {
        mpz_urandomb(rand, state, nbits);
        gcd(gcd_num, rand, totient);
        if (mpz_cmp_ui(gcd_num, 1) == 0) { // gcd_num == 1 (rand and totient are coprimes)
            break;
        }
    }
    mpz_set(e, rand);
    mpz_clears(totient, math, p1, q1, rand, gcd_num, NULL);
}

// takes in large integers n, e, s, string username, public key file
// write public key (n, e), signature s, and username to pbfile
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    gmp_fprintf(pbfile, "%s\n", username);
}

// takes in large integers n, e, s, string username, public key file
// read public key (n, e), signature s, and username from pbfile
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx", n);
    gmp_fscanf(pbfile, "%Zx", e);
    gmp_fscanf(pbfile, "%Zx", s);
    fscanf(pbfile, "%s", username);
}

// takes in large primes p, q and public exponent e
// create private key with large primes p, q and public exponent e
// return value through d
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t math, p1, q1;
    mpz_inits(math, p1, q1, NULL);
    // calculate totient
    mpz_sub_ui(p1, p, 1);
    mpz_sub_ui(q1, q, 1);
    mpz_mul(math, p1, q1); // totient = (p - 1) * (q - 1)
    // calculate d
    mod_inverse(d, e, math);
    mpz_clears(math, p1, q1, NULL);
}

// takes in large integers n, d, and private key file
// write private key (d), modulus n to pvfile
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

// takes in large integers n, d, and private key file
// read private key from pvfile
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx", n);
    gmp_fscanf(pvfile, "%Zx", d);
}

// takes in message m, public exponent e, modulus n
// performs RSA encryption to encrypt message m to compute ciphertext c
// return value through c
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
}

// takes in input, output files, public key (n, e)
// performs RSA encryption using the public key (n, e) to encrypt infile and write to outfile
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    mpz_t k_mpz, math, message, cipher;
    mpz_inits(k_mpz, math, message, cipher, NULL);
    size_t j = 1;
    // calculate block size k
    uint64_t k_log = mpz_sizeinbase(n, 2) - 1;
    mpz_set_ui(math, k_log); // math = log (base 2) n
    mpz_sub_ui(math, math, 1); // math = math - 1
    mpz_tdiv_q_ui(k_mpz, math, 8); // k = (log (base 2) n - 1) / 8
    uint64_t k = mpz_get_ui(k_mpz);
    // read from infile while there are still bytes to read
    while (j > 0) {
        // allocate memory for array
        uint8_t *arr = (uint8_t *) calloc(k, sizeof(uint8_t));
        arr[0] = 0xFF;
        // read bytes from infile
        j = fread(arr + 1, sizeof(uint8_t), k - 1, infile);
        // convert bytes to mpz_t
        mpz_import(message, j + 1, 1, 1, 1, 0, arr);
        // encrypt message
        rsa_encrypt(cipher, message, e, n);
        // write cipher to outfile
        gmp_fprintf(outfile, "%Zx\n", cipher);
        // free arr
        free(arr);
        arr = NULL;
    }
    mpz_clears(k_mpz, math, message, cipher, NULL);
}

// takes in ciphertext c, private key (d), modulus n
// perfroms RSA decryption to decrypt ciphertext c to compute message m
// return value through m
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
}

// takes in input, output files, private key (d), modulus n
// performs RSA decryption using private key (d) to decrypt infile to outfile
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    mpz_t k, math, cipher, message;
    mpz_inits(k, math, cipher, message, NULL);
    uint64_t total = 0;
    size_t j = 0;
    // calculate block size k
    uint64_t k_log = mpz_sizeinbase(n, 2) - 1;
    mpz_set_ui(math, k_log);    // math = log (base 2) n
    mpz_sub_ui(math, math, 1);  // math = math - 1
    mpz_tdiv_q_ui(k, math, 8);  // k = (log (base 2) n - 1) / 8
    uint64_t k_num = mpz_get_ui(k);
    // read from infile while there are still bytes to read
    while (true) {
        // allocate memory for array
        uint8_t *arr = (uint8_t *) calloc(k_num, sizeof(uint8_t));
        // if EOF is reached --> break
        if (feof(infile)) {
            free(arr);
            arr = NULL;
            break;
        }
        // read from infile
        gmp_fscanf(infile, "%Zx\n", cipher);
        // decrypt cipher
        rsa_decrypt(message, cipher, d, n);
        // convert mpz_t to bytes
        mpz_export(arr, &j, 1, 1, 1, 0, message);
        // write to outfile
        total = fwrite(arr + 1, sizeof(uint8_t), j - 1, outfile);
        // free arr
        free(arr);
        arr = NULL;
    }
    mpz_clears(k, math, cipher, message, NULL);
}

// takes in message m, private key (d), modulus n
// performs RSA signing, producing signature s
// return value through s
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
}

// takes in message m, signature s, public key (n, e)
// performs RSA verfication, returning boolean if signature s is verified
// returns boolean if s is verified
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);
    pow_mod(t, s, e, n);
    if (mpz_cmp(t, m) != 0) { // if t != m
        mpz_clear(t);
        return false;
    }
    mpz_clear(t);
    return true;
}
