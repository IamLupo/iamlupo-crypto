#include "Convert.h"

void CharToHex(char* _input_char, unsigned int _length, char* _output_hex)
{
	char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	int count = _length;
	int size = (count * 2);
	char char_return[size + 1];
	int i;
	
	for(i = 0; i < count; i++)
	{
		int value_id = (i);
		int char_id = (i * 2);
		char_return[char_id + 0] = hex[((_input_char[value_id] >> 4) & 0xF)];
		char_return[char_id + 1] = hex[((_input_char[value_id]) & 0x0F)];
	}
	char_return[size] = '\0';
	strlcpy(_output_hex, char_return, size + 1);
}

void HexToChar(char* _input_hex, unsigned int _length, char* _output_char)
{
	int i;
	for(i = 0; i < _length / 2; i++)
	{
		int hex1, hex2;
		
		hex1 = (int)_input_hex[i * 2];
		hex2 = (int)_input_hex[(i * 2) + 1];
		
		if(hex1 > 90)
			hex1 -= 87;
		else if(hex1 > 64)
			hex1 -= 55;
		else
			hex1 -= 48;
		
		if(hex2 > 90)
			hex2 -= 87;
		else if(hex2 > 64)
			hex2 -= 55;
		else
			hex2 -= 48;
		
		_output_char[i] = (hex1 * 16) + hex2;
	}
}

void HexFillUp(char* _input_hex, unsigned int* _actual_length, unsigned int _result_length)
{
	while(*_actual_length < _result_length)
	{
		int i;
		char temp_a, temp_b;
		temp_b = _input_hex[0];
		
		for(i = 1; i <= *_actual_length; i++)
		{
			temp_a = _input_hex[i];
			_input_hex[i] = temp_b;
			temp_b = temp_a;
		}
		
		_input_hex[0] = 0x30;
		*_actual_length += 1;
	}
}