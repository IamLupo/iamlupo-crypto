#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "Math.h"
#include "Convert.h"

typedef struct {
	mpz_t modulus;
	mpz_t public_key;
	mpz_t private_key;
} Person;

void PersonInit(Person* person);
void PersonGenerateKeys(Person* person, unsigned long int length);

void PersonGenerateFileHeader(mpz_t input_data, long filesize);
bool PersonReadFileHeader(mpz_t input_data, long* filesize);

bool PersonWriteKey(const char* filename, mpz_t data);
bool PersonReadKey(const char* filename, mpz_t data);
int  PersonGetBits(Person* person);
void PersonEncrypt(Person* person, mpz_t input_data, mpz_t output_data);
void PersonDecrypt(Person* person, mpz_t input_data, mpz_t output_data);
void PersonClear(Person* person);
