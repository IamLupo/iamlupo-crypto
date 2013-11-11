#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
	int secret_number;
	int public_number;
	int shared_public_number;
	int secret;
} Person;

void PrintPerson(Person* person);
int GeneratePublicNumber(Person* person, int generator, int modulus);
void CalcSecret(Person* person, int modulus);

int main()
{
	Person* alice = malloc(sizeof(Person));
	Person* bob = malloc(sizeof(Person));

	int generator = 3;
	int modulus = 17;
	
	//Generate secret number they only know
	alice->secret_number = 15;
	bob->secret_number = 13;
	
	//Generate public number and exchange to the other person
	bob->shared_public_number = GeneratePublicNumber(alice, generator, modulus);
	alice->shared_public_number = GeneratePublicNumber(bob, generator, modulus);
	
	//Calculate the Secret
	CalcSecret(alice, modulus);
	CalcSecret(bob, modulus);
	
	//Draw Information
	printf("Alice\n");
	PrintPerson(alice);
	
	printf("Bob\n");
	PrintPerson(bob);
	
	return 0;
}

int GeneratePublicNumber(Person* person, int generator, int modulus)
{
	person->public_number = (int)pow(generator, person->secret_number) % modulus;
	return person->public_number;
}

void CalcSecret(Person* person, int modulus)
{
	person->secret = fmod(pow(person->shared_public_number, person->secret_number), modulus);
}

void PrintPerson(Person* person)
{
	printf(	" Private number = %d\n Public number = %d\n Shared Public number = %d\n Secret = %d\n\n",
			person->secret_number,
			person->public_number,
			person->shared_public_number,
			person->secret);
}