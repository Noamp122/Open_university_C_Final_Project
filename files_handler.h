#include <stdio.h>

#include <stdlib.h>

#include <stdint.h>

#include <string.h>

#include <math.h>

#include "finals.h"

#include "symbol.h"

/*Functions declarions*/
extern char * strtok_r(char * , const char * , char ** );
void remove_spaces(char* str);
void macro_handler(int argc, char* argv[],char files[MAX][MAX]);
void save_macro_body(char* macro_name, char* line, char* line2, FILE* fpin);
void check_macro(int macros_counter, char * macros, char * checked_word, char * line2, FILE * fpout);
char* convert_into_base32(int symbol_valuel,char* buffer);
void build_entry(struct symbol *symbols,int num_symbols,char* file_name);
void build_obj(char machine_code[MACHINE_CODES][MACHINE_CODE_ROW],int IC,char* file_name);
void build_external(struct symbol *symbols,int num_symbols,char* file_name);
void build_files(struct symbol *symbols,int num_symbols,char* file_name,char machine_code[MACHINE_CODES][MACHINE_CODE_ROW],int IC);


char base32_array[32]={'!','@','#','$','%','^','&','*','<','>','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v'};

/*Function thats conver from binary into decimal*/
int convert_bin_to_dec(char machine_code[MACHINE_CODES][MACHINE_CODE_ROW],int index)
{
	int rem=0,dec=0,j=0,i;

	for(i=9;i>=0;i--)
	{
		rem=machine_code[index][i] -'0';
		dec += rem * pow(2, j);
		j++;
	}

	 return dec;

}

/*Function thats converts into base 32*/
char* convert_into_base32(int symbol_value,char* buffer)
{
	int i=0;
	int temp = symbol_value / 32;
	int rem = symbol_value % 32;

	buffer[i] = base32_array[temp];
	buffer[i+1] = base32_array[rem];
	buffer[2]= 0;
	buffer[3]=0;

	return buffer; 
}



/*Method that gets a string and remove all the spaces in it*/
void remove_spaces(char* str)
{
	int i;
	int count = 0;

	for (i = 0; str[i];i++) /*going thourgh the entire string*/
	{
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n') /*if the current character is not space, Tab or Enter*/
		{
			str[count] = str[i];
			count++;
		}
	}

	str[count] = '\0'; /*puting the end of string*/

}

void macro_handler(int argc, char* argv[],char files[MAX][MAX])
{
	int i, f = 0, macros_counter = 0, j;
	
	char macros[MAX][MAX]; /*array that will hold the macros*/
	char line[MAX]; 
	char line2[MAX];
	
	char* line1 = &line[0]; /*line1 points to the beginning of line*/
	char* file_name;

	char* token_command;

	for(i = 1;i<argc;i++) /*going through all the files*/
	{
		FILE* fpin;
		FILE* fpout;

		if((fpin = fopen(argv[i],"r")) == NULL) /*if we can't open the file*/
		{
			printf("\nCan not open %s \n", argv[i]);
			continue;
		}
		
		/*Creating an output file with the same name but with .am ending*/
		token_command = strtok_r(argv[i],".", &argv[i]); /*getting the file num without the .as*/
		file_name = strcat(token_command, ".am");
		strcpy(files[f], file_name); /*saving the new output file name to the files array*/
		f++;
		fpout = fopen(file_name, "w"); /*opening the new file*/

		while(fgets(line, MAX, fpin) != NULL) /*going through each line in the input file*/
		{
			strcpy(line2, line);
			line1 = &line[0]; /*line1 pointing to the new line string*/
			/*getting the first word of the line*/
			token_command = strtok_r(NULL, " ",&line1);
			remove_spaces(token_command);
			
			if(!strcmp(token_command, "macro")) /*if the first word is macro*/
			{
				token_command = strtok_r(NULL, " ", &line1); /*getting the name of the macro*/
				remove_spaces(token_command);

				strcpy(macros[macros_counter], token_command); /*saving the macro name in the macros array*/
				macros_counter++;
				save_macro_body(token_command, line, line2, fpin);
				
				continue; /*moving on to the next file*/

			}

			check_macro(macros_counter, macros[0], token_command, line2, fpout); /*checking if the current line is a macro symbole we already saved*/

		}

		/*deleting all the temporary macros files*/
		for(j = 0;j < macros_counter; j++)
		{
			remove(macros[j]);
			strcpy(macros[j],"");
		}

		macros_counter = 0;
		fclose(fpin); /*closing the input file*/
		fclose(fpout); /*closing the output file*/

	}

}

/*Method that saves the macro body to the macro file*/
void save_macro_body(char* macro_name, char* line, char* line2, FILE* fpin)
{
	FILE* fp_temp;

	fp_temp = fopen(macro_name, "w"); /*opening the macro file*/

	while(fgets(line, MAX, fpin) != NULL) /*reading every next line*/
	{
		strcpy(line2, line);

		if(line[0] == 'e' && line[1] == 'n' && line[2] == 'd' && line[3] == 'm' && line[4] == 'a' && line[5] == 'c' && line[6] == 'r' && line[7] == 'o') /*if we get to the end of the macro we finish*/
		{break;}

		fputs(line2, fp_temp); /*writing the new line in to the macro file*/  		
	}

	fclose(fp_temp);

}

/*Method that gets a word and check if it is a macro name, if so it replace the macro name with is actual body*/
void check_macro(int macros_counter, char * macros, char * checked_word, char * line2, FILE * fpout)
{
	int j;

	for(j = 0;j < macros_counter;j++)
	{
		
		/*if it's a macro name, we open the macro file and replacing it's content with the macro name*/		
		if (!(strcmp(checked_word, &macros[j]))) 
		{
			FILE* fp_temp = fopen(checked_word, "r");

			while(fgets(line2, MAX, fp_temp) != NULL)
			{
				fputs(line2, fpout);
			}
			
			fclose(fp_temp);
		
			break;

		}
	
	}
	
	/*if the word is not a macro name we put it back in*/
	if((j == macros_counter))
	{
		fputs(line2, fpout);
	}
	
}
/*Function that build the entry file*/

void build_obj(char machine_code[MACHINE_CODES][MACHINE_CODE_ROW],int IC,char* file_name)
{
	FILE* fptr;
	int temp=0;
	int i=START_OF_ICF;
	char* no_dot;
	char* final_name;
	char* value_in_base32;

	value_in_base32=(char *)malloc(sizeof(char) * (3));

	no_dot = strtok(file_name,".");

	final_name=strcat(no_dot,".ob");

	fptr = fopen(final_name, "w+");

	fprintf(fptr,"%c  %c\n",'m' ,'f');		

	for(;i<=IC; i++)
	{
		
		convert_into_base32(i,value_in_base32); /*Converting the address into base 32*/		

		fprintf(fptr,"%s",value_in_base32 ); 

		temp = convert_bin_to_dec(machine_code,i);

		convert_into_base32(temp,value_in_base32);

		fprintf(fptr,"  %s \n",value_in_base32 );

		
	}
fclose(fptr);
}


void build_entry(struct symbol *symbols,int num_symbols,char* file_name)
{
	FILE* fptr;
	int i=0;
	char* value_in_base32;
	char* no_dot;
	char* final_name;
	int entry_flag=1;

	value_in_base32=(char *)malloc(sizeof(char) * (3));

	no_dot = strtok(file_name,".");

	final_name=strcat(no_dot,".ent");

	for(i=0; i<num_symbols;i++)/*Checking if there are entry symbols*/
	{
		if(symbols[i]._isEnt == 1)
		entry_flag = 0;
	}

	if(entry_flag==0)
	{
	fptr = fopen(final_name, "w+"); /*Make a entry file only if there is an enrty label*/
	
	for(i=0;i<num_symbols; i++)
	{
		if(symbols[i]._isEnt == 1)
		{		
			convert_into_base32(symbols[i]._value,value_in_base32); /*Converting the value into base 32*/
			
			fprintf(fptr," %s     %s\n",symbols[i]._name,value_in_base32); /*Printing into the file the symbol name and its value*/
		
		}
		
	}
	fclose(fptr);
	}
}
/*Function that build the external file*/
void build_external(struct symbol *symbols,int num_symbols,char* file_name) 
{

    FILE * fptr;
    int i=0;
	char* value_in_base32;
	char* no_dot;
	char* final_name;
	int extern_flag=1;


	
	value_in_base32=(char *)malloc(sizeof(char) * (3));

	no_dot = strtok(file_name,".");

	final_name=strcat(no_dot,".ext");

	for(i=0;i<num_symbols; i++)
	{
		if(symbols[i]._isExt == 1)
		extern_flag = 0;
	}

	if(extern_flag== 0)
	{
		fptr = fopen(final_name, "w+");/*Making a file only if there is an external label*/
	

    for (i = 0; i <num_symbols; i++)
	 {
        if (symbols[i]._isExt)
		 {  

			convert_into_base32(symbols[i]._value,value_in_base32); /*Converting the value into base 32*/

			fprintf(fptr," %s     %s\n",symbols[i]._name,value_in_base32); /*Printing into the file the symbol name and its value*/
			
            }
		
        }
		fclose(fptr);
		}
    }

	/*building the files*/
	void build_files(struct symbol *symbols,int num_symbols,char* file_name,char machine_code[MACHINE_CODES][MACHINE_CODE_ROW],int IC)
	{
		build_entry(symbols,num_symbols,file_name);

		build_external(symbols,num_symbols,file_name);

		build_obj(machine_code,IC,file_name);

	}
