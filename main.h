#ifndef MAIN_H
#define MAIN_H

#include "handler.h"
#include "finals.h"
#include "symbol.h"

/*functions declartions*/
int file_assembler(FILE *fptr,char* file_name);
int stage_two(FILE *fptr, struct symbol *symbols ,int num_symbols, char mc[MACHINE_CODES][MACHINE_CODE_ROW]);


/*The function that gets the assembly text after macro-spreading and translate it to machine language*/
int file_assembler(FILE *fptr,char* file_name)
{
	int ICF = START_OF_ICF, DCF = 0, symbols_index = 0; /*Level 1*/
	int size = MAX_SYMBOLS;
	char machine_codes[MACHINE_CODES][MACHINE_CODE_ROW]; /* the array that will hold the instructions decoded in binary */
	char data_codes[MACHINE_CODES][MACHINE_CODE_ROW];	 /* the array that will hold the data decoded in binary */
	int keeper[MAX_keeper];
	struct symbol *symbols; /*list of symbols*/
	char mission[MAX][MAX]; /*array that we hold the words of the currnet line, each word in a diffrent row*/
	char data[MAX];			/*holds the current line.*/
	int cur /*cur-points to the currnet word in mission array*/,
		words, /*words-the number of words in the line*/
		 i, j,
		L /*L-how much bainery lines needed to decode this sentense*/,
		temp, temp_len, label, number_of_arguments /*the number of arguments in the instraction*/,
		 flag = 0, 
		 counter = 0 /*counter -count the number of line in the input file*/;
	
	
	memset(keeper, 0, MAX_keeper * sizeof(int)); /*initializing the keeper array with zeros*/

	symbols = (struct symbol *)malloc(size * sizeof(struct symbol)); /*creating space in the memory for all the symbols structs*/

	machine_code_reset(machine_codes); /*initializing the machine code array to start with only zeros*/
	machine_code_reset(data_codes);	   /*initializing the data code array to start with only zeros*/

	while (fgets(data, MAX, fptr) != NULL) /*Level 2, going through every line of the input file*/
	{
		/*if the memory space for new symbols is about to run out, we increase the symbols memory size*/
		if (symbols_index < size && symbols_index > (size - 5))
		{
			symbols = symbols_increase(symbols, &size);
		}

		for (i = 0; i < MAX; i++) /*initializing all the chars in mission to '\0'*/
		{
			memset(mission[i], '\0', MAX);
		}

		counter++; /*the line number in the original input file*/

		if (data[0] == '\0' || data[0] == '\n' || data[0] == 13) /*if this is an empty line we continue to the next line*/
		{
			continue;
		}

		i = 0;
		cur = 0;
		words = 0;
		L = 0;
		temp_len = 0;
		label = 0;

		clear_path(data, &i); /* bringing i to the first non-white space character */

		if (data[i] == ';') /* if the first char is ; it's the file name so we can skip this line */
		{
			continue;
		}
		/*
		 *	going over the current line and enter the words into
		 * the mission array, each word in a seperate row
		 */
		scan_row(data, mission, &words);

		/*debugging*/
		printf("\n \n");
		j = 0;
		while (mission[j][0] != '\0')
		{
			printf("%s	", mission[j]);
			j++;
		}

		temp_len = strlen(mission[cur]);

		/*Level 3*/
		if (temp_len > 0 && mission[cur][temp_len - 1] == ':') /*if the first word of this line is a label*/
		{
			label = 1; /*Level 4, activating the label flag*/
			cur++;

			if (cur == words)
			{
				/*what to do? end of line to soon*/
			}
		}

		/*Level 5*/
		if (!strcmp(mission[cur], ".data") || !strcmp(mission[cur], ".string") || !strcmp(mission[cur], ".struct")) /*if the current word is a data instruction*/
		{
			if (label) /*Level 6, if the first word was a label*/
			{
				temp = edit_or_create_symbol(symbols, &symbols_index, mission[0], DCF, 0, 1, 0, 0, 0); /*creating the label as a data symbol*/
				/*if there was a problem with the symbol the function returns 0, and we raise a flag*/
				if (!temp)
				{
					flag = 1;
				}
			}

			L = words - (cur + 1); /*calculating how many words there are after the .string/.data/.struct*/

			data_handler(mission, &words, mission[cur], &DCF, L, data_codes); /*Level 7, entering the data to the data table and updating the DC*/

			continue;
		}

		/*Level 8*/
		if (!strcmp(mission[cur], ".extern") || !strcmp(mission[cur], ".entry"))
		{
			/*Level 9*/

			if (!strcmp(mission[cur], ".extern"))
			{

				cur = cur + 1;

				if (cur == words)
				{

					/*what to do? end of file to soon*/
				}

				while (mission[cur][0] != '\0')
				{
					temp = edit_or_create_symbol(symbols, &symbols_index, mission[cur], 0, 0, 0, 0, 1, 0); /*saving the symbols as externals*/
					cur++;

					if (!temp)
					{
						/*if exist without ext  error*/
						flag = 1;
					}
				}

				continue; /*Level 10*/
			}

			continue;
		}

		/*Level 11, this is an instruction row*/
		if (label) /*if there is a lebel we enter it as code label*/
		{

			temp = edit_or_create_symbol(symbols, &symbols_index, mission[0], ICF, 1, 0, 0, 0, 0);

			if (!temp)
			{
				flag = 1;
			}
		}

		/*Level 12-13*/
		if (words > 0)
		{
			/*calculating if we need  3 or 2 or 1 code lines to decode this action*/
			/*if the action word is invalid the function returns -1 */
			number_of_arguments = job_secure(mission[cur]);

			if (number_of_arguments == -1)
			{
				flag = 1;
				continue;
			}

			if (words - label - 1 != number_of_arguments)
			{
				flag = 1;
				printf("the number of arguments: %d, are not the right. the action: %s needs: %d arguments", (words - label - 1), mission[cur], number_of_arguments);

				continue;
			}

			L = handle_machine_code_stage_one(mission, cur, &ICF, machine_codes); /*decode the first word of the instuction and returning L (the number of machine code line this command will take)*/

			ICF = ICF + L; /*Level 14*/

			continue;
		}
	}

	/*Level 16*/
	if (flag)
	{
		printf("\n there was a syntax error, moving to the next input file. \n");
		return flag;
	}

	/*Level 17, going over the symbols table and adding the IC to the data symbols*/
	for (j = 0; j < symbols_index; j++)
	{
		if (symbols[j]._isdata == 1)
		{
			symbols[j]._value = symbols[j]._value + ICF;
		}
	}

	
#if 1
	/* debugging */
/*	printf("\n \n \n Instruction code: \n");
	for(j = START_OF_ICF;j < ICF;j++)
	{
		printf("%d	(%.10s) \n", j,machine_codes[j]);
	}

	printf("Data code: \n");
	for(j = 0;j < DCF;j++)
	{
		printf("%d	(%.10s) \n",ICF + j,data_codes[j]);
	}

	printf("\nSymbols: \n");
	for(j = 0;j < symbols_index;j++)
	{
		printf("%d %s \n",symbols[j]._value,symbols[j]._name);
	}*/
#endif /* debugging */

	stage_two(fptr, symbols, symbols_index, machine_codes);

	build_files(symbols,symbols_index,file_name,machine_codes,ICF);/*making the output files lvl 11*/
	
	return 0;
}

int stage_two(FILE *fptr, struct symbol *symbols, int num_symbols, char mc[MACHINE_CODES][MACHINE_CODE_ROW])
{
	char data[MAX];
	char mission[MAX][MAX];
	int L, cur, words, i, temp_len, temp, flag = 0;
	int ICF = START_OF_ICF; /* level 1*/

	fseek(fptr, 0, SEEK_SET); /* begging to scan the file from the first line.*/

	while (fgets(data, MAX, fptr) != NULL) /*Level 2, going through every line of the input file*/
	{

		if (data[0] == '\0' || data[0] == '\n') /* if its an empty line'l than ignore*/
		{
			continue;
		}

		for (i = 0; i < MAX; i++) /*initializing all the chars in mission to '\0'*/
		{
			memset(mission[i], 0, MAX);
		}

		i = 0;
		cur = 0; /* variable for the current word in the sentence */
		words = 0; /* how many words there is in the current sentence */
		temp = 0;
		temp_len = 0;
		

		clear_path(data, &i);

		if (data[i] == ';') /* if it a comment the assembler will ignore it and continue to the next sentence.*/
		{
			continue;
		}

		scan_row(data, mission, &words); /* scan the data inside the current row */

		temp_len = strlen(mission[cur]);

		if (temp_len > 0 && mission[cur][temp_len - 1] == ':') /*level 2 ,checks if this is a label*/
		{

			cur = cur + 1;

			if (cur == words)
			{
				printf("to edit: mission too short\n");
			}
		}
		/* level 4,checks if its an data, string, extern or struct if not than continue to the next line. */
		if (!strcmp(mission[cur], ".data") || !strcmp(mission[cur], ".string") || !strcmp(mission[cur], ".extern") || !strcmp(mission[cur], ".struct"))
		{
			continue;
		}

		/*level 5*/
		if (!strcmp(mission[cur], ".entry"))
		{
			cur = cur + 1;
			temp = edit_or_create_symbol(symbols,&num_symbols, mission[cur], ICF, 0, 0, 0, 0, 1); /*levle 6 saving the symbols as entry*/

			if(!temp)
			{
				printf("ERROR");
			}
			/*check if there any other words. (add a checking method)*/
			if (cur + 1 < words)
			{
				printf("ERROR, entry instruction with more than one label");
				flag = 1;
			}

			continue;
		}

		/*level 7*/
		L = handle_machine_code_stage_two(&flag, mission, mc, words, cur, ICF, symbols, num_symbols);

		ICF = ICF + L; /*level 8*/

		continue; /*Level 9*/

		if (flag)
		{
			return flag;
		}
	}
	

	return 1;
}
#endif /* MAIN_H */
