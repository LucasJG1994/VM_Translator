/*
	Module: Directory
	Author: LucasJG1994
	Description:
		Enables users of this module to get functionality of their OS' file system directories.


	Code was written based on https://learn.microsoft.com/en-us/windows/win32/fileio/listing-the-files-in-a-directory
	which extracts files within a directory utilizing the Windows API.
*/

#include "dir.h"
#include "c2t.h"

#include <Windows.h>
#include <tchar.h>
#include <fileapi.h>
#include <strsafe.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILES 0xFF

static WIN32_FIND_DATA ffd[0xFF];
static TCHAR szDir[0xFF][MAX_PATH];
static HANDLE hfind[0xFF] = { INVALID_HANDLE_VALUE };
size_t hnd_count = 1;

static LARGE_INTEGER file_size;
size_t length_of_args;
static DWORD dwError = 0;

DIR_HANDLE dir_open_win(const char* path) {
	TCHAR* tmp = char_to_t(path);
	StringCchLength(tmp, MAX_PATH, &length_of_args);

	if (length_of_args >= MAX_PATH) {
		printf("Directory path is way too long...\n");
		return 0;
	}

	StringCchCopy(szDir[hnd_count], MAX_PATH, tmp);
	StringCchCat(szDir[hnd_count], MAX_PATH, TEXT("\\*"));

	hfind[hnd_count] = FindFirstFile(szDir[hnd_count], &ffd[hnd_count]);

	if (hfind[hnd_count] == INVALID_HANDLE_VALUE) {
		printf("Did not find any files...\n");
		return 0;
	}

	return hnd_count++;
}

const char** dir_gfl_win(DIR_HANDLE hdir) {
	if (hdir == 0) {
		printf("Invalid Directory Handle...\n");
		return NULL;
	}

	char** self = (char**)calloc(MAX_FILES, sizeof(char*));
	if (self == NULL) {
		printf("Failed to allocate memory for files...\n");
		return NULL;
	}

	unsigned int index = 0;

	do {
		if(ffd[hdir].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){ /*Do Nothing*/ }
		else {
			file_size.LowPart = ffd[hdir].nFileSizeLow;
			file_size.HighPart = ffd[hdir].nFileSizeHigh;
			
			self[index] = (char*)calloc(0xFF, sizeof(char));
			if (self[index] == NULL) {
				printf("Failed to allocate memory to store file name...\n");
				for (int i = 0; i < MAX_FILES; i++) {
					if (self[i] != NULL) free(self[i]);
				}
				free(self);

				return NULL;
			}

			strcpy(self[index], (char*)ffd[hdir].cFileName);
			index++;
		}
	}while(FindNextFile(hfind[hdir], &ffd[hdir]) != 0);

	dwError = GetLastError();
	return self;
}

void dir_close_win(DIR_HANDLE hdir) {
	if (hdir == 0) {
		printf("Cannot close an Invalid Directory Handle...\n");
		return;
	}

	FindClose(hfind[hdir]);
}