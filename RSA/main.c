#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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
	event_decrypt,
	event_test
} event;

void getPath(char* current_path, char* command_path, char *argv[]);

int main(int argc, char *argv[])
{
	//Init
	event = event_info;
	
	char current_path[1024];
	char command_path[1024];
	
	getPath(current_path, command_path, argv);
	
	//Command Input
	if(argc > 1)
	{
		if(strcmp(argv[1], "-s") == 0)		{	event = event_situation;	}
		else if(strcmp(argv[1], "-g") == 0)	{	event = event_generate;		}
		else if(strcmp(argv[1], "-e") == 0)	{	event = event_encrypt;		}
		else if(strcmp(argv[1], "-d") == 0)	{	event = event_decrypt;		}
		else if(strcmp(argv[1], "-t") == 0)	{	event = event_test;		}
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
			strncpy(message, argv[2], strlen(argv[2]));
		else
			strncpy(message, "Secret Message!!!", 17);
		
		//Convert from Message to Hex
		CharToHex(message, strlen(message), message_hex);
		mpz_set_str(input_data, message_hex, 16);
		
		printf("message = %s\n", message);
		//printf("message hex = %s\n", mpz_get_str(NULL, 16, input_data));
		printf("\n");
		
		PersonEncrypt(&bob, input_data, output_data);
		printf("data encrypt = %s\n", mpz_get_str(NULL, 10, output_data));
		printf("\n");
		
		strncpy(message, "", 1); // Make Empty
		strncpy(message_hex, "", 1); // Make Empty
		
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

		int bits_key = PersonGetBits(&person);
		int bytes_count = bits_key / 8;
		int hex_count = bits_key / 4;
		
		BYTE data[bytes_count * 2];
		char data_hex[hex_count * 2];

		int data_length;
		unsigned int data_hex_length;
		
		//Write Header
		PersonGenerateFileHeader(input_data, input_file.filesize);
		PersonEncrypt(&person, input_data, output_data);
		
		//Convert
		data_hex_length = strlen(mpz_get_str(NULL, 16, output_data));
		strncpy(data_hex, mpz_get_str(NULL, 16, output_data), hex_count);
		HexFillUp(data_hex, &data_hex_length, hex_count);
		HexToChar(data_hex, hex_count, (char *)data);
		
		//Write in file
		CFile_WriteData(&output_file, data, bytes_count);
		
		//Get Data
		while(CFile_GetData(&input_file, data, &data_length, bytes_count - 1))
		{
			//Convert
			CharToHex((char *)data, data_length, data_hex);
			mpz_set_str(input_data, data_hex, 16);
			
			//Encrypt
			PersonEncrypt(&person, input_data, output_data);
			
			//Convert
			data_hex_length = strlen(mpz_get_str(NULL, 16, output_data));
			strncpy(data_hex, mpz_get_str(NULL, 16, output_data), hex_count);
			HexFillUp(data_hex, &data_hex_length, hex_count);
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
		unsigned int data_hex_length;
		long filesize;
		
		//Get Header
		if(CFile_GetData(&input_file, data, &data_length, bytes_count))
		{
			//Convert to input_data
			CharToHex((char *)data, data_length, data_hex);
			mpz_set_str(input_data, data_hex, 16);
			
			//Encrypt input_data
			PersonDecrypt(&person, input_data, output_data);
			
			PersonReadFileHeader(output_data, &filesize);
		}
		
		int fillup_length = (hex_count - 2);
		
		//Get Data
		while(CFile_GetData(&input_file, data, &data_length, bytes_count))
		{
			if(filesize * 2 < fillup_length)
				fillup_length = (filesize * 2);
			
			//Convert to input_data
			CharToHex((char *)data, data_length, data_hex);
			mpz_set_str(input_data, data_hex, 16);
			
			//Encrypt input_data
			PersonDecrypt(&person, input_data, output_data);
			
			//Convert to data
			data_hex_length = strlen(mpz_get_str(NULL, 16, output_data));
			strncpy(data_hex, mpz_get_str(NULL, 16, output_data), data_hex_length);
			HexFillUp(data_hex, &data_hex_length, fillup_length);
			HexToChar(data_hex, data_hex_length, (char *)data);
			
			//Write to File
			CFile_WriteData(&output_file, data, data_hex_length / 2);
			
			filesize -= (data_hex_length / 2);
		}
		
		CFile_Close(&input_file);
		CFile_Close(&output_file);
	}
	else if(event == event_test)
	{
		struct stat s;

		strncat(command_path, "/secrite_woman.png", 18);
		printf("%s\n", command_path);
		if (!stat(command_path, &s))
			printf("'%s' is a directory.\n", command_path);
	}
	
	return 0;
}

void getPath(char* current_path, char* command_path, char *argv[])
{
	getcwd(current_path, sizeof(current_path));
	printf("%s\n", current_path);
	char* pch;
	int length = 0;
	pch = strchr(argv[0], '/');
	while(pch != NULL)
	{
		length = pch-argv[0];
		pch = strchr(pch + 1, '/');
	}
	strncpy(command_path, current_path, strlen(current_path));
	strncat(command_path, "/", 1);
	strncat(command_path, argv[0], length);
}

