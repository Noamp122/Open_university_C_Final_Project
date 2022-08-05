#include "main.h"

int main(int argc,char** argv)
{
	int i;
	char files[MAX][MAX];
	FILE* fp = NULL;

	/*going over the files, for each file creating an output file after macros replacment*/
	macro_handler(argc, argv, files); 

	/*going over the files after the macro-spreding, for each file creating an machine language output file*/
	for(i = 0;i < argc-1;i++) 
	{
		fp = fopen(files[i], "r");
		file_assembler(fp,files[i]); 
		fclose(fp);
	}	

	return 0;
}
