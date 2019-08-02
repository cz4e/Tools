#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <dirent.h>

extern long ErrorCode;
#define OPEN_DIR_SUCCESS(error)		(error == 0)
extern char *ObjectFileName;
extern DIR *CurrentDir;

struct Option {
	char *DirName;
	char *FileName;
};

struct Option Options;

static const struct option long_opts[] = 
{
	{"file",required_argument,NULL,'f'},
	{"dir",required_argument,NULL,'d'},
	{NULL,0,NULL,0}
};

static void
getOptions(int argc,char *argv[]) 
{
	char ch;
	while((ch = getopt_long(argc,argv,
					"df",long_opts,NULL)) != -1) {
		switch(ch) {
			case 'd':
				Options.DirName = optarg;
				break;
			case 'f':
				Options.FileName = optarg;
				break;
			default:
				printf("Unkonw Option: %c\n",ch);
				exit(-1);
		}
	}
}

int
main(int argc,char *argv[])
{	
	long UID = 0;
	if(argc == 1) {
		printf("gf:	gf [Options]\n");
		printf("	--dir=dirname		set start dir,default root dir\n");
		printf("	--file=filename		set object file\n");
		exit(-1);
	}

	UID = getuid();

	if(UID != 0) {
		printf("	If you use this program with non-root prority,there are some directories\n");
		printf("can not be opened!\n");
		sleep(2);
	}
	getOptions(argc,argv);
	
	if(Options.DirName == NULL) {
		Options.DirName = (char *)malloc(sizeof(char) * 2);
		Options.DirName[0] = '/';
		Options.DirName[1] = '\0';
	}
	ErrorCode = OpenDir(Options.DirName);

	if(ErrorCode == -1) {
		exit(-1);
	}
	if(OPEN_DIR_SUCCESS(ErrorCode)) {
		ObjectFileName = Options.FileName;
		printf("File Name		Path\n");
		DisplayDirTree();
		closedir(CurrentDir);
	}	
	return (0);
} 
