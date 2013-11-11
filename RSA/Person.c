#include "Person.h"

void PersonInit(Person* person)
{
	mpz_init(person->modulus);
	mpz_init(person->public_key);
	mpz_init(person->private_key);
}

void PersonGenerateKeys(Person* person, unsigned long int length)
{
	mpz_t p, q, n, product, e, d;
	gmp_randstate_t rs;
	
	//Init
	mpz_init(p);
	mpz_init(q);
	mpz_init(n);
	mpz_init(product);
	mpz_init(e);
	mpz_init(d);
	
	srand(time(NULL));
	gmp_randinit_default(rs); 
	gmp_randseed_ui(rs, rand());
	
	//Generate
	do {
		do {
			generate_random_prime(p, rs, length / 2);
			generate_random_prime(q, rs, length / 2);
		} while(mpz_cmp(p, q) == 0);
		
		totient(p, q, product);
		
		generate_random_prime(e, rs, length);
	} while(mpz_cmp(product, e) < 0 && mpz_cmp_ui(e, 1) > 0);
	
	mpz_mul(n, p, q);
	mpz_invert(d, e, product);
	
	//Copy
	mpz_init_set(person->modulus, n);
	mpz_init_set(person->public_key, e);
	mpz_init_set(person->private_key, d);
	
	//Clear
	mpz_clear(p);
	mpz_clear(q);
	mpz_clear(n);
	mpz_clear(product);
	mpz_clear(e);
	mpz_clear(d);
	gmp_randclear(rs);
}

bool PersonWriteKey(const char* filename, mpz_t data)
{
	FILE* stream;
	stream = fopen(filename, "w");
	if(stream != NULL)
	{
		mpz_out_str(stream, 16, data);
		fclose(stream);
		return true;
	}
	return false;
}

bool PersonReadKey(const char* filename, mpz_t data)
{
	FILE* stream;
	stream = fopen(filename, "r");
	if(stream != NULL)
	{
		mpz_inp_str(data, stream, 16);
		fclose(stream);
		return true;
	}
	return false;
}

int PersonGetBits(Person* person)
{
	return strlen(mpz_get_str(NULL, 2, person->modulus));
}

void PersonEncrypt(Person* person, mpz_t input_data, mpz_t output_data)
{
	//output_data = (input_data ^ person->public_key) % person->modulus;
	mpz_powm(output_data, input_data, person->public_key, person->modulus);
}

void PersonDecrypt(Person* person, mpz_t input_data, mpz_t output_data)
{
	//output_data = (input_data ^ person->private_key) % person->modulus;
	mpz_powm(output_data, input_data, person->private_key, person->modulus);
}

void PersonClear(Person* person)
{
	mpz_clear(person->modulus);
	mpz_clear(person->public_key);
	mpz_clear(person->private_key);
}
