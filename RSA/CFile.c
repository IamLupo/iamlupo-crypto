#include "CFile.h"

void CFile_Init(CFile* file, const char* _filename, const char* _mode)
{
	file->file = NULL;
	strncpy(file->filename, _filename, 200);
	strncpy(file->mode, _mode, 4);
	file->filesize = 0;
	file->pdata = 0;
}

bool CFile_Open(CFile* file)
{
	if((file->file = fopen(file->filename, file->mode)) != NULL)
	{
		CFile_GetSize(file);
		return true;
	}
	return false;
}

bool CFile_Close(CFile* file)
{
	if(file->file != NULL)
	{
		fclose(file->file);
		return true;
	}
	return false;
}

long CFile_GetSize(CFile* file)
{
    long lCurPos;
	
    lCurPos = ftell(file->file);
    fseek(file->file, 0, 2);
    file->filesize = ftell(file->file);
    fseek(file->file, lCurPos, 0);
	
    return file->filesize;
}

bool CFile_IsOpen(CFile* file)
{
	if(file->file != NULL)
		return true;
	return false;
}

bool CFile_SetDataPosition_Begin(CFile* file)
{
	return CFile_SetDataPosition(file, 0);
}

bool CFile_SetDataPosition(CFile* file, long _pointer)
{
	if(_pointer < file->filesize)
	{
		fseek(file->file, _pointer, SEEK_SET);
		file->pdata = _pointer;
		return true;
	}
	return false;
}

bool CFile_GetData(CFile* file, BYTE* _output_data, int* _output_length, unsigned int _input_length)
{
	if(file->pdata < file->filesize)
	{
		if((long)_input_length > (long)file->filesize)
		{
			fread(_output_data, 1, file->filesize, file->file);
			file->pdata = file->filesize;
			*_output_length = (int)file->filesize;
		}
		else if((long)(file->pdata + _input_length) <= (long)file->filesize)
		{
			fread(_output_data, 1, _input_length, file->file);
			file->pdata += _input_length;
			*_output_length = _input_length;
		}
		else
		{
			long size = (file->filesize - file->pdata);
			fread(_output_data, 1, (unsigned int)size, file->file);
			file->pdata = file->filesize;
			*_output_length = size;
		}
		return true;
	}
	return false;
}

bool CFile_WriteData(CFile* file, BYTE* _input_data, unsigned int _input_length)
{
	fwrite (_input_data , 1 , _input_length , file->file );
	
	return true;
}