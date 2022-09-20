
#include <stdio.h>
#include <stdlib.h>

#include "numtheory.h"
#include "randstate.h"

// takes in large integers a, b
// computes greatest common divisor of a and b and stores it in g
// return value through g
void gcd(mpz_t g, mpz_t a, mpz_t b) {
    mpz_t temp, mod, a_val, b_val;
    mpz_inits(temp, mod, a_val, b_val, NULL);
    mpz_set(a_val, a);
    mpz_set(b_val, b);
    while (mpz_sgn(b_val) != 0) {   // while b != 0
        mpz_set(temp, b_val);
        mpz_mod(mod, a_val, b_val); // mod = a % b
        mpz_set(b_val, mod);
        mpz_set(a_val, temp);
    }
    mpz_set(g, a_val);
    mpz_clears(temp, mod, a_val, b_val, NULL);
}

// takes in large integers a, n
// computes inverse a of modulo n
// return value through o
void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {
    mpz_t r, r1, t, t1, q, math, r1_temp, t1_temp;
    mpz_inits(r, r1, t, t1, q, math, r1_temp, t1_temp, NULL);
    mpz_set(r, n);              // r = n
    mpz_set(r1, a);             // r' = a
    mpz_set_ui(t, 0);           // t = 0
    mpz_set_ui(t1, 1);          // t' = 1
    while (mpz_sgn(r1) != 0) {  // while r1 != 0
        mpz_tdiv_q(q, r, r1);   // q = r / r'
        // r' = r - q * r'
        mpz_set(r1_temp, r1);
        mpz_mul(math, q, r1);
        mpz_sub(r1, r, math);
        mpz_set(r, r1_temp);    // r = r'
        // t' = t - q * t'
        mpz_set(t1_temp, t1);
        mpz_mul(math, q, t1);
        mpz_sub(t1, t, math);
        mpz_set(t, t1_temp);    // t = t'
    }
    if (mpz_cmp_ui(r, 1) > 0) {         // if r > 1
        mpz_set_ui(o, 0);
    } else if (mpz_sgn(t) == -1) {      // if t < 0
        mpz_add(t, t, n);       // t = t + n
        mpz_set(o, t);
    } else {
        mpz_set(o, t);          // o = t
    }
    mpz_clears(r, r1, t, t1, q, math, r1_temp, t1_temp, NULL);
}

// takes in large integers a, d, n
// computes base a to the exponent d power modulus n, storing value in o
// return value through o
void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
    mpz_t v, p, d_val;
    mpz_init_set_ui(v, 1);  // v = 1
    mpz_init_set(p, a);     // p = a
    mpz_init_set(d_val, d);
    while (mpz_sgn(d_val) == 1) {       // while d > 0
        if (mpz_odd_p(d_val) != 0) {    // if d is odd
            // v = v*v % n
            mpz_mul(v, v, p);
            mpz_mod(v, v, n);
        }
        // p = (p * p) % n
        mpz_mul(p, p, p);
        mpz_mod(p, p, n);
        mpz_tdiv_q_ui(d_val, d_val, 2); // d /= 2
    }
    mpz_set(o, v);
    mpz_clears(v, p, d_val, NULL);
}

// takes in large integer n, number of iterations iters
// conducts Miller-Rabin primality test to determine if n is prime after iters number of iterations
// returns boolean if prime
bool is_prime(mpz_t n, uint64_t iters) {
    // cases 0 - 3
    if (mpz_cmp_ui(n, 2) < 0) { // n < 2
        return false;
    } else if (mpz_cmp_ui(n, 3) == 0 || mpz_cmp_ui(n, 2) == 0) { // n == 3 or n == 2
        return true;
    }
    // even numbers
    if (mpz_even_p(n) != 0) { // if n is even
        return false;
    }
    // declare variables
    mpz_t s, r, value, n1, a, y, j, s1, two;
    mpz_inits(s, r, value, n1, a, y, j, s1, two, NULL);
    mpz_set_ui(value, 1);       // value = 1
    mpz_sub_ui(r, n, 1);        // r = n - 1
    mpz_sub_ui(n1, n, 1);       // n1 = n - 1
    mpz_set_ui(two, 2);         // two = 2
    while (true) {      // write n - 1 = 2^s * r such that r is odd
        mpz_set_ui(value, 1);
        for (uint64_t i = 0; mpz_cmp_ui(s, i) > 0; i += 1) {
            mpz_mul_ui(value, value, 2);
        }
        mpz_mul(value, value, r); // value = value * r
        if (mpz_cmp(value, n1) == 0 && mpz_odd_p(r) != 0) { // if value == n - 1 and r is odd
            break;
        }
        mpz_add_ui(s, s, 1);    // s += 1
        mpz_tdiv_q_ui(r, r, 2); // r /= 2
    }
    mpz_sub_ui(s1, s, 1);       // s1 = s - 1
    // loop through iters for greater confidence
    for (uint64_t i = 0; i < iters; i += 1) {
        mpz_urandomm(a, state, n1);
        while (mpz_cmp_ui(a, 0) == 0 || mpz_cmp_ui(a, 1) == 0) { // while a == 0 or 1
            mpz_urandomm(a, state, n1);
        }
        pow_mod(y, a, r, n);
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n1) != 0) { // if y != 1 or n - 1
            mpz_set_ui(j, 1);   // j = 1
            // while j <= s - 1 and y != n - 1
            while (mpz_cmp(j, s1) <= 0 && mpz_cmp(y, n1) != 0) {
                pow_mod(y, y, two, n);
                if (mpz_cmp_ui(y, 1) == 0) { // if y == 1
                    mpz_clears(s, r, value, n1, a, y, j, s1, two, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);    // j += 1
            }
            if (mpz_cmp(y, n1) != 0) {  // y != n - 1
                mpz_clears(s, r, value, n1, a, y, j, s1, two, NULL);
                return false;
            }
        }
    }
    mpz_clears(s, r, value, n1, a, y, j, s1, two, NULL);
    return true;
}

// takes in number of bits, number of iterations
// generate prime number of bits bits long and tests with is_prime
// return value through p
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_urandomb(p, state, bits + 1);
    // while p is not prime or p_bits < bits
    while (!is_prime(p, iters) || mpz_sizeinbase(p, 2) < bits) {
        mpz_urandomb(p, state, bits);
    }
}
