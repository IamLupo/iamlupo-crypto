#ifndef H_CFILE
#define H_CFILE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned char BYTE;

typedef struct {
	FILE* file;
	char filename[250];
	char mode[4];
	long filesize;
	long pdata;
} CFile;

void CFile_Init(CFile* file, const char* filename, const char* mode);
bool CFile_Open(CFile* file);
bool CFile_Close(CFile* file);
long CFile_GetSize(CFile* file);
bool CFile_IsOpen(CFile* file);
bool CFile_SetDataPosition_Begin(CFile* file);
bool CFile_SetDataPosition(CFile* file, long _pointer);
bool CFile_GetData(CFile* file, BYTE* _output_data, int* _output_length, unsigned int _input_length);
bool CFile_WriteData(CFile* file, BYTE* _input_data, unsigned int _input_length);

#endif