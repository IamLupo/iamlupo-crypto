/*
	wiki: http://en.wikipedia.org/wiki/RSA_%28algorithm%29
*/
#include <stdlib.h>
#include <iostream>

#include "include/BigIntegerLibrary.h"

using namespace std;

typedef struct {
	BigUnsigned modulus;
	BigUnsigned public_key;
	BigUnsigned private_key;
} Person;

void GenerateKeys(Person* person);
BigUnsigned Encrypt(Person* person, const BigUnsigned &number);
BigUnsigned Decrypt(Person* person, const BigUnsigned &encrypted_number);
void PrintPerson(Person* person);

int main()
{
	Person alice, bob;
	
	//Generate Public and Private keys
	GenerateKeys(&alice);
	
	//Alice gives the public key and modulus to bob
	bob.modulus = alice.modulus;
	bob.public_key = alice.public_key;
	
	//bob encrypts a number
	cout << "Bob's number = " << 65 << endl;
	BigUnsigned encrypted_number = Encrypt(&bob, 65);
	cout << "Bob's encrypted_number = " << encrypted_number << endl;
	
	//Only Alice can now decrypt the message
	BigUnsigned decrypted_number = Decrypt(&alice, encrypted_number);
	cout << "Alice's decrypted number = " << decrypted_number << endl;
	
	return 0;
}

void GenerateKeys(Person* person)
{
	BigUnsigned p = 61;		//Random prime number
	BigUnsigned q = 53;		//Random prime number
	
	//wiki: http://en.wikipedia.org/wiki/Euler%27s_totient_function
	BigUnsigned product = totient(p, q);
	
	person->modulus = p * q;
	person->public_key = 17;				//the public key must be a prime number that can not been divisor of "product"
	//wiki http://en.wikipedia.org/wiki/Modular_multiplicative_inverse
	person->private_key = modinv(person->public_key, product);
}

BigUnsigned Encrypt(Person* person, const BigUnsigned &number)
{
	return pow(number, person->public_key) % person->modulus;
}

BigUnsigned Decrypt(Person* person, const BigUnsigned &encrypted_number)
{
	return pow(encrypted_number, person->private_key) % person->modulus;
}

void PrintPerson(Person* person)
{
	cout << " modules = "		<< person->modulus		<< endl;
	cout << " Public key = "	<< person->public_key	<< endl;
	cout << " Private key = "	<< person->private_key	<< endl;
}
