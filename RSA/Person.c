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

void PersonGenerateFileHeader(mpz_t input_data, long filesize)
{
	mpz_t bn_filesize;
	
	char header_hex[39];
	char filesize_hex[33];
	
	unsigned int length;
	
	//Init
	mpz_init(bn_filesize);
	
	//Header
	strncpy(header_hex, "454e43", 6);
	
	//Filesize
	mpz_set_ui(bn_filesize, filesize);
	length = strlen(mpz_get_str(NULL, 16, bn_filesize));
	strncpy(filesize_hex, mpz_get_str(NULL, 16, bn_filesize), length);
	HexFillUp(filesize_hex, &length, 32);
	
	strncat(header_hex, filesize_hex, 32);
	mpz_set_str(input_data, header_hex, 16);
}

bool PersonReadFileHeader(mpz_t input_data, long* filesize)
{
	unsigned int length;
	char header_hex[39];
	char header_begin[7];
	char filesize_hex[34];
	
	length = strlen(mpz_get_str(NULL, 16, input_data));
	
	if(length == 38)
	{
		strncpy(header_hex, mpz_get_str(NULL, 16, input_data), length);		//Get the header data
		strncpy(header_begin, header_hex, 6);								//Get begin of the header
		
		if(strcmp(header_begin, "454e43") == 0)
		{
			strncpy(filesize_hex, header_hex + 6, 32);
			filesize_hex[32] = '\0';
			
			HexToLong(filesize_hex, 32, filesize);
			return true;
		}
	}
	return false;
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
