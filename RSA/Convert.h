#include <string.h>

void CharToHex(char* _input_char, unsigned int _length, char* _output_hex);
void HexToChar(char* _input_hex, unsigned int _length, char* _output_char);
void HexFillUp(char* _input_hex, unsigned int* _actual_length, unsigned int _result_length);
void HexToLong(char* _input_hex, unsigned int _length, long* _output_long);