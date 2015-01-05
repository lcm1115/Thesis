// File: bench.h
// Author: Liam Morris
// Description: Class and function declarations for simple accumulators used for
//              benchmarking performance.
#include <gmp.h>

class RSAAccumulator {
    public:
        // Takes a starting generator value and a modulus for the group.
        RSAAccumulator(const mpz_t value, const mpz_t modulus);

        // Accumulates a single value and stores the witness in a given
        // argument.
        void accumulate(const mpz_t value, mpz_t witness);
    private:
        mpz_t _value, _modulus;
};

class ECCAccumulator {
    public:
        // Takes the value 'a' of a curve, the starting point, and the modulus.
        ECCAccumulator(const mpz_t a,
                       const mpz_t point[],
                       const mpz_t modulus);

        // Accumulates a single value and stores the witness in a given
        // argument.
        void accumulate(const mpz_t value, mpz_t witness[]);

        // Multiplies a point 'p' by a scalar 'd' and stores the result in 'r'.
        // Uses the double-and-add algorithm.
        void pointmul(const mpz_t p[], const mpz_t d, mpz_t r[]);

        // Adds two points 'p' and 'q' and stores the result in 'r'.
        void pointadd(const mpz_t p[], const mpz_t q[], mpz_t r[]);

        // Doubles a point 'p' and stores the result in 'r'.
        void pointdouble(const mpz_t p[], mpz_t r[]);
    private:
        mpz_t _a, _modulus;
        mpz_t _point[2];
};
