
#include <stdio.h>
#include "randstate.h"

// rand state
gmp_randstate_t state;

// takes in seed (default: 256)
// initializes randstate with Mersenns Twister algorithm using seed as the random seed
void randstate_init(uint64_t seed) {
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
}

// clears and frees all memory used by randstate
void randstate_clear(void) {
    gmp_randclear(state);
}
