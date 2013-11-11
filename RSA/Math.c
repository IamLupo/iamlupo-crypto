#include "Math.h"

void generate_random_prime(mpz_t number, gmp_randstate_t random, unsigned long int length)
{
	mpz_urandomb(number, random, length);
	mpz_setbit(number, length - 1);
	mpz_setbit(number, length - 2);
	mpz_nextprime(number, number);
}

//c = (a - 1)(b - 1)
void totient(mpz_t a, mpz_t b, mpz_t c)
{
	mpz_t x, y;
	
	mpz_init_set_ui(x, 1);
	mpz_init_set_ui(y, 1);
	
	mpz_sub(x, a, x);
	mpz_sub(y, b, y);
	mpz_mul(c, x, y);
	
	mpz_clear(x);
	mpz_clear(y);
}