#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Convert.h"
#include "CFile.h"
#include "Person.h"

#include "gmp.h"

enum MAIN_EVENT
{
	event_info,
	event_situation,
	event_generate,
	event_encrypt,
	event_decrypt
} event;

int main(int argc, char *argv[])
{
	//Init
	event = event_info;
	
	//Command Input
	if(argc > 1)
	{
		if(strcmp(argv[1], "-s") == 0)		{	event = event_situation;	}
		else if(strcmp(argv[1], "-g") == 0)	{	event = event_generate;		}
		else if(strcmp(argv[1], "-e") == 0)	{	event = event_encrypt;		}
		else if(strcmp(argv[1], "-d") == 0)	{	event = event_decrypt;		}
	}
	
	if(event == event_info)
	{
		printf("Generate new keys:\n %s -g\n %s -g <number>\n", argv[0], argv[0]);
		printf("Encrypt message:\n %s -e \n %s -e <message>\n", argv[0], argv[0]);
		printf("Encrypt message:\n %s -d <file>\n", argv[0]);
		
		return 0;
	}
	else if(event == event_generate)
	{
		Person person;
		unsigned long int input_number;
		
		if(argc > 2)
			input_number = atoi(argv[2]);
		else
			input_number = 1024;
		
		//Init
		PersonInit(&person);
		
		//Person generates the keys
		PersonGenerateKeys(&person, input_number);
		
		printf("modulus = %s\n",		mpz_get_str(NULL, 16, person.modulus));
		printf("public_key = %s\n",		mpz_get_str(NULL, 16, person.public_key));
		printf("private_key = %s\n",	mpz_get_str(NULL, 16, person.private_key));
		printf("\n");
		
		if(	PersonWriteKey("compiled/keys/modulus",		person.modulus) &&
			PersonWriteKey("compiled/keys/public_key",	person.public_key) &&
			PersonWriteKey("compiled/keys/private_key",	person.private_key))
		{
			printf("Write to File's finished!\n");
		}
		
		//Clean
		PersonClear(&person);
	}
	else if(event == event_situation)
	{
		char message[25], message_hex[50];
		mpz_t input_data, output_data;
		
		Person alice, bob;
		
		//Init
		PersonInit(&alice);
		PersonInit(&bob);
		
		mpz_init(input_data);
		mpz_init(output_data);
		
		if(	PersonReadKey("compiled/keys/modulus",		alice.modulus) &&
			PersonReadKey("compiled/keys/public_key",	alice.public_key) &&
			PersonReadKey("compiled/keys/private_key",	alice.private_key))
		{
			printf("Read from File's finished!\n");
		}
	
		//Alice gives the public key and the modulo to bob
		mpz_init_set(bob.modulus, alice.modulus);
		mpz_init_set(bob.public_key, alice.public_key);
		
		//Bob needs a message to send
		if(argc > 2)
			strlcpy(message, argv[2], strlen(argv[2]) + 1);
		else
			strlcpy(message, "Secret Message!!!", 17);
		
		//Convert from Message to Hex
		CharToHex(message, strlen(message), message_hex);
		mpz_set_str(input_data, message_hex, 16);
		
		printf("message = %s\n", message);
		//printf("message hex = %s\n", mpz_get_str(NULL, 16, input_data));
		printf("\n");
		
		PersonEncrypt(&bob, input_data, output_data);
		printf("data encrypt = %s\n", mpz_get_str(NULL, 10, output_data));
		printf("\n");
		
		strcpy(message, ""); // Make Empty
		strcpy(message_hex, ""); // Make Empty
		
		//The output_data is our encrypted message and only alice can read this message
		PersonDecrypt(&alice, output_data, output_data);
		printf("data decrypted = %s\n", mpz_get_str(NULL, 16, output_data));
		strcpy(message_hex, mpz_get_str(NULL, 16, output_data));
		printf("message hex = %s\n", message_hex);
		
		//Convert from Hex to Message
		HexToChar(message_hex, strlen(message_hex), message);
		printf("message = %s\n", message);
		
		//Clean
		PersonClear(&alice);
		PersonClear(&bob);
	}
	else if(event == event_encrypt)
	{
		Person person;
		
		CFile input_file, output_file;
		mpz_t input_data, output_data;
		
		//Init
		PersonInit(&person);
		mpz_init(input_data);
		mpz_init(output_data);
		
		CFile_Init(&input_file, "compiled/secrite_woman.png", "r");
		CFile_Init(&output_file, "compiled/secrite_woman.enc.png", "w");
		
		//Read Files
		if(	!CFile_Open(&input_file) || 
			!CFile_Open(&output_file) ||
			!PersonReadKey("compiled/keys/modulus",		person.modulus) ||
			!PersonReadKey("compiled/keys/public_key",	person.public_key))
		{
			printf("Error read/write files\n");
			return 0;
		}
		
		printf("bits = %i\n", PersonGetBits(&person));
		int bits_key = PersonGetBits(&person);
		
		int bytes_count = bits_key / 8;
		int hex_count = bits_key / 4;
		
		BYTE data[bytes_count];
		char data_hex[hex_count];

		int data_length;
		while(CFile_GetData(&input_file, data, &data_length, bytes_count - 1))
		{
			//Convert
			CharToHex((char *)data, data_length, data_hex);
			mpz_set_str(input_data, data_hex, 16);
			
			//Encrypt
			PersonEncrypt(&person, input_data, output_data);
			
			//Convert
			unsigned int encrypt_data_length = strlen(mpz_get_str(NULL, 16, output_data));
			strlcpy(data_hex, mpz_get_str(NULL, 16, output_data), hex_count + 1);
			//Add 0x0 on the end if it doesn't have "hex_count" length
			HexFillUp(data_hex, &encrypt_data_length, hex_count);
			HexToChar(data_hex, hex_count, (char *)data);
			
			//Write to File
			CFile_WriteData(&output_file, data, bytes_count);
		}
		
		CFile_Close(&input_file);
		CFile_Close(&output_file);
	}
	else if(event == event_decrypt)
	{
		Person person;
		
		CFile input_file, output_file;
		mpz_t input_data, output_data;
		
		//Init
		PersonInit(&person);
		
		mpz_init(input_data);
		mpz_init(output_data);
		
		CFile_Init(&input_file, "compiled/secrite_woman.enc.png", "r");
		CFile_Init(&output_file, "compiled/secrite_woman.dec.png", "w");
		
		//Read Files
		if(	!CFile_Open(&input_file) || 
			!CFile_Open(&output_file) ||
			!PersonReadKey("compiled/keys/modulus",		person.modulus) ||
			!PersonReadKey("compiled/keys/private_key",	person.private_key))
		{
			printf("Error read/write files\n");
			return 0;
		}
		
		int bits_key = PersonGetBits(&person);
		
		int hex_count = bits_key / 4;
		int bytes_count = bits_key / 8;
		
		BYTE data[bytes_count];
		char data_hex[hex_count];

		int data_length;
		unsigned int decrypt_data_length;
		while(CFile_GetData(&input_file, data, &data_length, bytes_count))
		{
			mpz_init(input_data);
			mpz_init(output_data);
		
			//Convert to input_data
			CharToHex((char *)data, data_length, data_hex);
			mpz_set_str(input_data, data_hex, 16);
			
			//Encrypt input_data
			PersonDecrypt(&person, input_data, output_data);
			
			//Convert to data
			decrypt_data_length = strlen(mpz_get_str(NULL, 16, output_data));
			strlcpy(data_hex, mpz_get_str(NULL, 16, output_data), decrypt_data_length + 1);
			//Add 0x0 on the end if it doesn't have "hex_count - 2" length
			HexFillUp(data_hex, &decrypt_data_length, (hex_count - 2));
			HexToChar(data_hex, decrypt_data_length, (char *)data);
			
			//Write to File
			CFile_WriteData(&output_file, data, decrypt_data_length / 2);
		}
		
		CFile_Close(&input_file);
		CFile_Close(&output_file);
	}
	
	return 0;
}
