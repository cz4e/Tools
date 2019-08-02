#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/common.h"

DIR	*CurrentDir;
struct dirent *Dirent;

typedef long	error_t;

error_t ErrorCode;

size_t	ParentDirLength;
char *ParentDir;

char *ObjectFileName;
char *CurrentDirName;

error_t
OpenDir(const char *DirName)
{
	CurrentDir = opendir(DirName);

	if(OPEN_DIR_ERROR(CurrentDir)) {
#ifdef	GF_DEBUG	
		printf("Can not open directory: %s\n",DirName);
#endif
		return (-1);
	}
	
	CurrentDirName = (char *)malloc(sizeof(char) * (strlen(DirName) + 1));
	ParentDirLength = strlen(DirName);
	strcpy(CurrentDirName,DirName);
	return (0);
}

#define MAX_FILE_TYPES 			9

static const char *FileTypes[MAX_FILE_TYPES] = {
		"Unknow Type",
		"FIFO",
		"CHR",
		"DIR",
		"BLK",
		"REG",
		"LNK",
		"SOCK",
		"WHT"
};

static const char *
FileType(__uint8_t FileType)
{
	int Index = 0;
	if(FileType == DT_UNKNOWN) {
		;
	} else if(FileType == DT_FIFO) {
		Index = 1;
	} else if(FileType == DT_CHR) {
		Index = 2;
	} else if(FileType == DT_DIR) {
		Index = 3;
	} else if(FileType == DT_BLK) {
		Index = 4;
	} else if(FileType == DT_REG) {
		Index = 5;
	} else if(FileType == DT_LNK) {
		Index = 6;
	} else if(FileType == DT_SOCK) {
		Index = 7;
	} else {
		Index = 8;
	}
	return (FileTypes[Index]);
}

#define DisplayFileInfo(Dirent)		do{\
						printf("Name:	%s\n",Dirent->d_name);\
						printf("Type:	%s\n",FileType(Dirent->d_type));\
						printf("\n");\
					}while(0);
#define IS_DIR(d_type)			(d_type == DT_DIR)
#define IS_FILE(d_type)			(d_type == DT_REG || d_type == DT_LNK)


error_t
DisplayDirTree(void)
{
	Dirent = readdir(CurrentDir);
	while(Dirent != NULL) {
		if(strcmp(Dirent->d_name,".") == 0 || strcmp(Dirent->d_name,"..") == 0) {
			Dirent = readdir(CurrentDir);
			continue;
		} 

		if(IS_DIR(Dirent->d_type)) {
			DIR *OldDirHandle = CurrentDir;
			char *SubDir = (char *)malloc(sizeof(char) * (strlen(Dirent->d_name) + ParentDirLength + 1));
			int 	Index;
			char *OldDir;
			int	OldParentDirLength;

			for( Index = 0;Index < ParentDirLength;Index++){
				SubDir[Index] = CurrentDirName[Index];
			}
			if(SubDir[Index - 1] != '/') {
				SubDir[Index++] = '/';
			}
			for(int Sub = 0;Sub < strlen(Dirent->d_name);Sub++)
				SubDir[Index++] = Dirent->d_name[Sub];
			SubDir[Index] = '\0';
			OldDir = CurrentDirName;
			OldParentDirLength = ParentDirLength;
			ErrorCode = OpenDir(SubDir);
			if(ErrorCode == -1) {
				exit(-1);
			} else { 
				DisplayDirTree();
				free(CurrentDirName);
				closedir(CurrentDir);
			}
			free(SubDir);
			ParentDirLength = OldParentDirLength;
			CurrentDirName = OldDir;
			CurrentDir = OldDirHandle;
		} else if(IS_FILE(Dirent->d_type)){
			if(strcmp(Dirent->d_name,ObjectFileName) == 0) {
				printf("%s:		%s\n",Dirent->d_name,CurrentDirName);
			}
		}

		Dirent = readdir(CurrentDir);
	}
	return (0);
}
