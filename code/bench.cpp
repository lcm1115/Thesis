// File: bench.cpp
// Author: Liam Morris
// Description: Class and function definitions for simple accumulators used for 
//              benchmarking performance.
#include "bench.h"

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <gmp.h>
#include <iostream>

using namespace std::chrono;
using namespace std;

RSAAccumulator::RSAAccumulator(const mpz_t value, const mpz_t modulus) {
    mpz_init_set(_value, value);
    mpz_init_set(_modulus, modulus);
}

void RSAAccumulator::accumulate(const mpz_t value, mpz_t witness) {
    mpz_set(witness, _value);
    mpz_powm(_value, _value, value, _modulus);
}

ECCAccumulator::ECCAccumulator(
        const mpz_t a, const mpz_t point[], const mpz_t modulus) {
    mpz_init_set(_a, a);
    mpz_init_set(_point[0], point[0]);
    mpz_init_set(_point[1], point[1]);
    mpz_init_set(_modulus, modulus);
}

void ECCAccumulator::accumulate(const mpz_t value, mpz_t witness[]) {
    mpz_set(witness[0], _point[0]);
    mpz_set(witness[1], _point[1]);
    pointmul(_point, value, _point);
}

void ECCAccumulator::pointmul(const mpz_t p[], const mpz_t d, mpz_t r[]) {
    long m = mpz_sizeinbase(d, 2);
    mpz_set_ui(r[0], 0);
    mpz_set_ui(r[1], 0);
    while (m >= 0) {
        pointdouble(r, r);
        if (mpz_tstbit(d, m)) {
            pointadd(r, p, r);
        }
        --m;
    }
}

void ECCAccumulator::pointadd(const mpz_t p[], const mpz_t q[], mpz_t r[]) {
    if (mpz_sgn(p[0]) == 0 && mpz_sgn(p[1]) == 0) {
        mpz_set(r[0], q[0]);
        mpz_set(r[1], q[1]);
    } else if (mpz_sgn(q[0]) == 0 && mpz_sgn(q[1]) == 0) {
        mpz_set(r[0], p[0]);
        mpz_set(r[1], p[1]);
    } else {
        // Compute lambda
        mpz_t lambda, temp, x, y;
        mpz_init(x);
        mpz_init(y);
        mpz_init_set(temp, q[1]);
        mpz_sub(temp, temp, p[1]);
        mpz_init_set(lambda, temp);
        mpz_set(temp, q[0]);
        mpz_sub(temp, temp, p[0]);
        mpz_invert(temp, temp, _modulus);
        mpz_mul(lambda, lambda, temp);
        mpz_mod(lambda, lambda, _modulus);

        // Compute Xr
        mpz_set(x, lambda);
        mpz_pow_ui(x, x, 2);
        mpz_sub(x, x, p[0]);
        mpz_sub(x, x, q[0]);
        mpz_mod(x, x, _modulus);

        // Compute Yr
        mpz_set(y, p[0]);
        mpz_sub(y, y, x);
        mpz_mul(y, y, lambda);
        mpz_sub(y, y, p[1]);
        mpz_mod(y, y, _modulus);

        // Set result
        mpz_set(r[0], x);
        mpz_set(r[1], y);
    }
}

void ECCAccumulator::pointdouble(const mpz_t p[], mpz_t r[]) {
    // Initialize variables
    mpz_t temp, lambda, x, y;
    mpz_init(temp);
    mpz_init(lambda);
    mpz_init(x);
    mpz_init(y);

    // Compute lambda
    mpz_pow_ui(lambda, p[0], 2);
    mpz_mul_ui(lambda, lambda, 3);
    mpz_add(lambda, lambda, _a);
    mpz_set(temp, p[1]);
    mpz_mul_ui(temp, temp, 2);
    mpz_invert(temp, temp, _modulus);
    mpz_mul(lambda, lambda, temp);

    // Compute Xr
    mpz_set(x, lambda);
    mpz_pow_ui(x, x, 2);
    mpz_set(temp, p[0]);
    mpz_mul_ui(temp, temp, 2);
    mpz_sub(x, x, temp);
    mpz_mod(x, x, _modulus);

    // Compute Yr
    mpz_set(y, p[0]);
    mpz_sub(y, y, x);
    mpz_mul(y, y, lambda);
    mpz_sub(y, y, p[1]);
    mpz_mod(y, y, _modulus);

    // Set result
    mpz_set(r[0], x);
    mpz_set(r[1], y);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Please enter number of values to accumulate." << endl;
        return 1;
    }
    int num_values = atoi(argv[1]);
    mpz_t val, modulus;
    gmp_randstate_t rand;
    gmp_randinit_default(rand);
    mpz_init(modulus);
    mpz_init_set_ui(val, 3);
    mpz_urandomb(modulus, rand, 3072);
    mpz_nextprime(modulus, modulus);
    RSAAccumulator acc(val, modulus);
    string s;
    mpz_t witness;
    mpz_init(witness);
    mpz_t value;
    mpz_init(value);
    time_point<system_clock> start, end;
    double rsa_seconds, ecc_seconds;

    // Accumulate 'num_values' values using RSA accumulator.
    start = system_clock::now();
    for (int i = 0; i < num_values; ++i) {
        mpz_urandomb(value, rand, 3072);
        acc.accumulate(value, witness);
    }
    end = system_clock::now();
    rsa_seconds = duration_cast<milliseconds>(end - start).count();
    rsa_seconds /= 1000;

    // Initialize ECC accumulator variables.
    mpz_t a;
    mpz_t point[2], eccwitness[2];
    mpz_init(eccwitness[0]);
    mpz_init(eccwitness[1]);
    mpz_init_set_si(a, -3);
    mpz_init_set_str(point[0],
            "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296", 16);
    mpz_init_set_str(point[1],
            "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5", 16);
    mpz_set_str(modulus, "115792089210356248762697446949407573530"
                         "086143415290314195533631308867097853951", 10);
    ECCAccumulator ecc(a, point, modulus);

    // Accumulate 'num_values' values using ECC accumulator.
    start = system_clock::now();
    for (int i = 0; i < num_values; ++i) {
        mpz_urandomb(value, rand, 256);
        ecc.accumulate(value, eccwitness);
    }
    end = system_clock::now();
    ecc_seconds = duration_cast<milliseconds>(end - start).count();
    ecc_seconds /= 1000;

    // Output results.
    cout << "Accumulated " << num_values << " values" << endl;
    cout << "RSA time: " << rsa_seconds << "s" << endl;
    cout << "ECC time: " << ecc_seconds << "s" << endl;
    cout << "Difference Factor: " << (rsa_seconds / ecc_seconds) << endl;
}
