
#ifndef HANDLER_H
#define HANDLER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "is_register.h"
#include "in_out_put.h"
#include "finals.h"
#include "files_handler.h"
#include "symbol.h"

/* functions declaritions */
void machine_code_reset(char mc[MACHINE_CODES][MACHINE_CODE_ROW]);
int check_string(char *string, int len);
int check_number(char *number);
char *dec_to_bin(int value, char *buffer, int base, int size);
int get_operand_op_code(char command[MAX]);
void copy_string_to_array(char *string, char *copy_to, int start, int end);
void fill_rows_with(char mc[MACHINE_CODES][MACHINE_CODE_ROW], char character, int start_row, int num_rows);

/* Declaration of local variables  */

/*the opcode decoded in reverse because when we print the binary to the output file we start from the last digit (9)*/
char *op_code_array[16] = {

	"0000",

	"0001",

	"0010",

	"0011",

	"0100",

	"0101",

	"0110",

	"0111",

	"1000",

	"1001",

	"1010",

	"1011",

	"1100",

	"1101",

	"1110",

	"1111"

};

char *instruction_name[] =
	{"mov", /*0*/

	 "cmp", /*1*/

	 "add", /*2*/

	 "sub", /*3*/

	 "not", /*4*/

	 "clr", /*5*/

	 "lea", /*6*/

	 "inc", /*7*/

	 "dec", /*8*/

	 "jmp", /*9*/

	 "bne", /*10*/

	 "get", /*11*/

	 "prn", /*12*/

	 "jsr", /*13*/

	 "rts", /*14*/

	 "hlt" /*15*/};

/*a method that gets the binary instructions array, and initilaized all the chars to 0*/
void machine_code_reset(char mc[MACHINE_CODES][MACHINE_CODE_ROW])
{

	int i = START_OF_ICF;

	for (; i < MACHINE_CODES; i++)
	{
		memset(mc[i], '0', MACHINE_CODE_ROW);
	}
}

/*method that doubles the memory allocated to hold symbols structs*/
struct symbol *symbols_increase(struct symbol *symbols, int *size)
{

	struct symbol *struct_ptr;

	*size = *size * 2;

	struct_ptr = (struct symbol *)realloc(symbols, sizeof(struct symbol) * (*size));

	if (struct_ptr == NULL)
	{
		printf("Error! memory not allocated.");
		exit(0);
	}

	return struct_ptr;
}

/*a function that save the data as binary*/
void data_handler(char mission[MAX][MAX], int *words, char type[], int *DC, int L, char mc[MACHINE_CODES][MACHINE_CODE_ROW])
{

	int j = 0, len = 0;

	char *string = mission[*words - 1]; /*getting the actuall data string or numbers*/

	if (!strcmp(type, ".string"))
	{

		if (L > 1)
		{
			printf("there is too many arguments for the command .string");

			return;
		}

		len = strlen(string);

		if (!check_string(string, len))
		{
			printf("\nnot a valid string \n");
			return;
		}

		/*if the string is valid*/

		/*getting rid of the " in the end and beggining*/
		string[len - 1] = '\0';
		string = &string[1];

		while (string[j] != '\0') /*going through every character of the string untlin the end*/
		{

			dec_to_bin(string[j], &mc[*DC][0], 2, 10); /*saving the character as binary in the memory*/

			*DC = *DC + 1;

			j++;
		}

		/*after the last char we have a line with only zeros*/
		memset(mc[*DC], '0', MACHINE_CODE_ROW);

		*DC = *DC + 1;
	}
	else if (!strcmp(type, ".data"))
	{

		j = *words - L;

		while (mission[j][0] != '\0') /*going through every data argument*/
		{

			if (!check_number(mission[j])) /*if the number is not valid*/
			{

				printf("the number: %s isn't allowed.\n", mission[j]);

				j++; /*moving to the next argument*/

				continue;
			}

			dec_to_bin(atoi(mission[j]), &mc[*DC][0], 2, 10); /*decoding the number to the array*/

			*DC = *DC + 1;

			j++; /*moving to the next argument*/
		}
	}
	else if (!strcmp(type, ".struct"))
	{

		/*decoding the data argument*/
		j = *words - L;

		if (!check_number(mission[j])) /*if the number is not valid*/
		{
			printf("the number: %s isn't allowed.\n", mission[j]);
			return;
		}
		else
		{

			dec_to_bin(atoi(mission[j]), &mc[*DC][0], 2, 10); /*decoding the number to the array*/

			*DC = *DC + 1;

			j++; /*moving to the next argument*/
		}

		/*decoding the string argument*/

		string = mission[j];
		len = strlen(string);
		if (!check_string(string, len))
		{
			printf("\nnot a valid string \n");
			return;
		}

		/*if the string is valid*/

		/*getting rid of the " in the end and beggining*/
		string[len - 1] = '\0';
		string = &string[1];

		j = 0;

		while (string[j] != '\0') /*going through every character of the string untlin the end*/
		{

			dec_to_bin(string[j], &mc[*DC][0], 2, 10); /*saving the character as binary in the memory*/

			*DC = *DC + 1;

			j++;
		}

		/*after the last char we have a line with only zeros*/
		memset(mc[*DC], '0', MACHINE_CODE_ROW);
		*DC = *DC + 1;
	}
}

/*a function that gets a string, if the string starts and ends with " it returns 1, otherwise returns 0*/
int check_string(char *string, int len)
{

	if (string[0] == 34 && string[len - 1] == 34)
	{
		return 1;
	}

	return 0;
}

char *dec_to_bin(int value, char *buffer, int base, int size)
{

	char *temp;
	char *onescomplement;

	int r, n, i = 0, carry = 1;

	onescomplement = (char *)malloc(sizeof(char) * (size + 1));

	temp = (char *)malloc(sizeof(char) * size);

	memset(temp, '0', size);

	if (base < 2 || base > 32)
	{

		return buffer;
	}

	n = abs(value);

	while (n)
	{

		r = n % base;

		temp[i++] = 48 + r;

		n = n / base;
	}

	if (i == 0)
	{

		temp[i++] = '0';
	}

	if (value < 0 && base == 10)
	{

		temp[i++] = '-';
	}

	if (value < 0)
	{

		/*        char onescomplement[size+1];*/

		for (i = 0; i < size; i++)
		{

			if (temp[i] == '0')

				onescomplement[i] = '1';

			else if (temp[i] == '1')

				onescomplement[i] = '0';
		}

		onescomplement[size] = '\0';

		for (i = 0; i < size; i++)

		{

			if (onescomplement[i] == '1' && carry == 1)

			{

				buffer[size - 1 - i] = '0';
			}
			else if (onescomplement[i] == '0' && carry == 1)

			{

				buffer[size - 1 - i] = '1';

				carry = 0;
			}
			else

			{

				buffer[size - 1 - i] = onescomplement[i];
			}
		}
	}
	else
	{

		for (i = 0; i < size; i++)
		{

			buffer[size - 1 - i] = temp[i];
		}
	}

	free(temp);

	return buffer;
}


/*a method that gets a string that represent an assemblly action word, and returns how many arguments she suppose to have*/
/*if the action code is invalid the method alert about it, and returns -1 */
int job_secure(char job[MAX])
{

	if (!strcmp(job, "mov") || !strcmp(job, "cmp") || !strcmp(job, "add") || !strcmp(job, "sub") || !strcmp(job, "lea"))
	{
		return 2;
	}

	else if (!strcmp(job, "clr") || !strcmp(job, "not") || !strcmp(job, "inc") || !strcmp(job, "dec") || !strcmp(job, "jmp") || !strcmp(job, "bne") || !strcmp(job, "jsr") || !strcmp(job, "get") || !strcmp(job, "prn"))
	{
		return 1;
	}

	else if (!strcmp(job, "rts") || !strcmp(job, "hlt"))
	{
		return 0;
	}

	printf("\nthere isn't any action word by the name: %s\n", job);

	return -1;
}

/*decoding the first code line of the first stage*/
int handle_machine_code_stage_one(char mission[MAX][MAX], int current_line, int *IC, char mc[MACHINE_CODES][MACHINE_CODE_ROW])
{
	int command_index;
	int L = 1; /*the number of machine code lines we will need to decode this command*/
	char *opcode_binary;

	/*getting the command binary code*/
	command_index = get_operand_op_code(mission[current_line]);
	opcode_binary = op_code_array[command_index];

	copy_string_to_array(opcode_binary, mc[*IC], 0, 3); /*decoding the opcode to the machine code line*/

	/*in the first decoded line the A,R,E is always 00 */
	mc[*IC][8] = '0';
	mc[*IC][9] = '0';

	if (command_index == 14 || command_index == 15) /*the command is 'hlt' or 'rts' so there is no operands and the A,R,E is 00 */
	{
		mc[*IC][4] = '0';
		mc[*IC][5] = '0';
		mc[*IC][6] = '0';
		mc[*IC][7] = '0';

		return L;
	}

	current_line++; /*moving to the first argument*/

	if (command_index == 4 || command_index == 5 || command_index == 7 || command_index == 9 || command_index == 10 || command_index == 11 || command_index == 11 || command_index == 12 || command_index == 13) /*if the function have only destanation operand*/
	{

		L++;

		/*because there is no origin operand, is address is 00 */
		mc[*IC][4] = '0';
		mc[*IC][5] = '0';

		if (command_index == 12 && mission[current_line][0] == '#') /*if its a 'prn' command and itsinstance address*/
		{
			mc[*IC][6] = '0';
			mc[*IC][7] = '0';
			fill_rows_with(mc, '?', *IC + 1, L - 1); /*filling the other machine code lines with '??????????' until the second stage*/
			return L;
		}

		if (is_register(mission[current_line])) /*if it is a register name (11) */
		{
			mc[*IC][6] = '1';
			mc[*IC][7] = '1';
			fill_rows_with(mc, '?', *IC + 1, L - 1); /*filling the other machine code lines with '??????????' until the second stage*/
			return L;
		}

		if (strstr(mission[current_line], ".") != NULL) /*if the argument contains '.' we conclude it pointing to a struct (10) */
		{
			L++; /*a struct fild takes 2 machine code line, one for the address of the struct and the other for the filed number inside the struct*/

			mc[*IC][6] = '1';
			mc[*IC][7] = '0';
			fill_rows_with(mc, '?', *IC + 1, L - 1); /*filling the other machine code lines with '??????????' until the second stage*/
			return L;
		}
		else /*we assume it a direct address (01) */
		{
			mc[*IC][6] = '0';
			mc[*IC][7] = '1';
			fill_rows_with(mc, '?', *IC + 1, L - 1); /*filling the other machine code lines with '??????????' until the second stage*/
			return L;
		}
	}

	if (command_index == 0 || command_index == 1 || command_index == 2 || command_index == 3 || command_index == 6)
	{
		L = L + 2;

		if (mission[current_line][0] == '#' && command_index != 6) /*the command is not 'lea' and its first operand has instance address*/
		{
			mc[*IC][4] = '0';
			mc[*IC][5] = '0';
		}
		else if (command_index != 6 && is_register(mission[current_line]))
		{
			mc[*IC][4] = '1';
			mc[*IC][5] = '1';
		}
		else if (strstr(mission[current_line], ".") != NULL)
		{
			L++; /*a struct fild takes 2 machine code line, one for the address of the struct and the other for the filed number inside the struct*/

			mc[*IC][4] = '1';
			mc[*IC][5] = '0';
		}
		else /*we assume it a direct address (01) */
		{
			mc[*IC][4] = '0';
			mc[*IC][5] = '1';
		}

		current_line++; /*moving to the second operand*/

		if (command_index == 1 && mission[current_line][0] == '#')
		{
			mc[*IC][6] = '0';
			mc[*IC][7] = '0';
			fill_rows_with(mc, '?', *IC + 1, L - 1); /*filling the other machine code lines with '??????????' until the second stage*/
			return L;
		}
		else if (is_register(mission[current_line]))
		{
			mc[*IC][6] = '1';
			mc[*IC][7] = '1';

			if (is_register(mission[current_line]) && is_register(mission[current_line - 1])) /*if the 2 operands are registers we only need one extra machine code line, not 2*/
			{
				L--;
			}
			fill_rows_with(mc, '?', *IC + 1, L - 1); /*filling the other machine code lines with '??????????' until the second stage*/
			return L;
		}
		else if (strstr(mission[current_line], ".") != NULL) /*we assuming the second operand is a struct*/
		{
			L++;

			mc[*IC][6] = '1';
			mc[*IC][7] = '0';
			fill_rows_with(mc, '?', *IC + 1, L - 1); /*filling the other machine code lines with '??????????' until the second stage*/
			return L;
		}
		else /*we assuming the second operand  has direct address*/
		{
			mc[*IC][6] = '0';
			mc[*IC][7] = '1';
			fill_rows_with(mc, '?', *IC + 1, L - 1); /*filling the other machine code lines with '??????????' until the second stage*/
			return L;
		}
	}

	return -1;
}

/*function that gets an action word and return its opcode acording to the instructions words array*/
int get_operand_op_code(char command[MAX])
{

	int i = 0;

	remove_spaces(command);

	for (; i < 16; i++)
	{

		if (!strcmp(command, instruction_name[i]))

			return i;
	}

	return 16;
}

/*function that returns the value of the symbol if it appears in the symbol list, if not return -1  */
int get_symbol_value(struct symbol *symbols, int symbol_index, char symbol_name[])
{
	int i = 0;

	for (i = 0; i < symbol_index; i++)
	{
		if (!strcmp(symbols[i]._name, symbol_name))
			return symbols[i]._value;
	}
	return -1;
}
/*function that check if a symbol is from a type struct if not returns -1. */
int is_struct(char *symbol_name)
{
	char *p;

	p = strstr(symbol_name, ".");

	if (p)
	{
		return 1;
	}
	return 0;
}
/*function that separations between the  labels name and thier field */
int label_separations(char *label)
{
	int i = 0;

	while (label[i] != '.')
	{
		i = i + 1;
	}
	i = i + 1; /*ignoring the "."*/

	return atoi(&label[i]);
}

/*function that returns the number of register*/
int register_number(char reg[])
{
	int i = 48;
	int counter = 0;

	while (reg[1] != i)
	{
		counter = counter + 1;
		i = i + 1;
	}
	return counter;
}

int handle_machine_code_stage_two(
			int *flag, 
			char mission[MAX][MAX], 
			char machine_code[MACHINE_CODES][MACHINE_CODE_ROW], 
			int mission_words __attribute__((unused)),
			int current_word_in_mission,
			int IC, 
			struct symbol *symbols,
			int num_symbols)

{
	enum {
		AM_INVALID = -1,
		AM_IMMEDIATE = 0,
		AM_DIRECT = 1,
		AM_STRUCT = 2,
		AM_REGISTER = 3 
	};
	/* Get the opcode */
	int number_of_operands = -1;
	int addressing_modes[2] = {AM_INVALID, AM_INVALID};
	int opnd = 0;
	int L = 1;
	int immediate_value;
	int label_index;
	int field_number;
	int reg1;

	number_of_operands = job_secure(mission[current_word_in_mission]);
	
	/* Read the address modes of all operands */
	if (number_of_operands > 0)
	{
		/*Getting the source operand*/
		addressing_modes[0] = (machine_code[IC][4] - '0') * 2 + (machine_code[IC][5] - '0');

		L += 1;

		/*If number of operands = 1, than thats mean there is no source operand only a destenation operand*/
		if(number_of_operands == 1)
		{
			addressing_modes[0] =  (machine_code[IC][6] - '0') * 2 + (machine_code[IC][7] - '0');
		}
	}
	
	if (number_of_operands > 1)
	{
		L += 1;

		/*Getting the destenation operand*/
		addressing_modes[1] =  (machine_code[IC][6] - '0') * 2 + (machine_code[IC][7] - '0');
		
	}

	if (addressing_modes[0] == AM_REGISTER && addressing_modes[1] == AM_REGISTER)
	{
		L -= 1; /* Both operands share the additional word */

		current_word_in_mission+=1;
		IC+=1;
		/*Getting the register number, op_code_array holds the binary form of 0-7*/
		addressing_modes[0]=mission[current_word_in_mission][1]-'0';
		addressing_modes[1]=mission[current_word_in_mission+1][1]-'0';

		strcpy(&machine_code[IC][0],op_code_array[addressing_modes[0]]);
		strcpy(&machine_code[IC][4],op_code_array[addressing_modes[1]]);


		machine_code[IC][8] = '0';
		machine_code[IC][9] = '0';

		return L;
	}
	else /* we can handle each addressing mode alone */
	{
		for (opnd = 0; opnd < number_of_operands; ++opnd)
		{
			IC += 1;
			current_word_in_mission += 1;
			switch (addressing_modes[opnd])
			{
			case AM_IMMEDIATE: 
			{
				immediate_value = atoi(&mission[current_word_in_mission][1]);

				dec_to_bin(immediate_value, &machine_code[IC][0], 2, 8);

				machine_code[IC][8] = '0';
				machine_code[IC][9] = '0';

				
				
			} break;
			case AM_DIRECT: 
			{
				/* find value of mission[curren] in symbols */
				label_index = check_if_symbol(&mission[current_word_in_mission][0], symbols, num_symbols);

				if (label_index < 0)
				{
					printf("Huston, we have a problem. Could not find %s\n", &mission[current_word_in_mission][0]);
					*flag = 1;
					exit(0);
				}


				/* write its value into the next machine code */
				dec_to_bin(symbols[label_index]._value, &machine_code[IC][0], 2, 8);

				/* set the A, R, E bits according to the symbol table entry */
				if (symbols[label_index]._isExt)
				{
					machine_code[IC][8] = '0';
					machine_code[IC][9] = '1';
				}
				else
				{
					machine_code[IC][8] = '1';
					machine_code[IC][9] = '0';
				}
			} break;
			case AM_STRUCT: 
			{
				L+=1;

				label_index = check_if_symbol(&mission[current_word_in_mission][0], symbols, num_symbols);
				/*Checking if the label exists*/
				if (label_index < 0)
				{
					printf("Huston, we have a problem. Could not find %s\n", &mission[current_word_in_mission][0]);
					*flag = 1;
					exit(0);
				}
				/* write its value into the next machine code */
				dec_to_bin(symbols[label_index]._value, &machine_code[IC][0], 2, 8);

				/* set the A, R, E bits according to the symbol table entry */
				if (symbols[label_index]._isExt)
				{
					machine_code[IC][8] = '0';
					machine_code[IC][9] = '1';
				}
				else
				{
					machine_code[IC][8] = '1';
					machine_code[IC][9] = '0';
				}


				field_number = label_separations(&mission[current_word_in_mission][0]);
				
				if(field_number < 1 || field_number > 2)
				{
					printf("Huston, we have a problem. The field nubmer in the symbol is invaild");
					*flag = 1;
					exit(0);
				}

				IC+=1;
				/* write the label field number into the next machine code */
				dec_to_bin(field_number, &machine_code[IC][0], 2, 8);
				machine_code[IC][8] = '0';
					machine_code[IC][9] = '0';
				

			} break;
			case AM_REGISTER: 
			{
				/*Getting the register number, op_code_array holds the binary form of 0-7*/
				reg1 = register_number(&mission[current_word_in_mission][0]);
				
				strcpy(&machine_code[IC][0],op_code_array[reg1]);

				memset(&machine_code[IC][3],'0',7);

			} break;
			default:
			{
				printf(": %d is unhandled\n", addressing_modes[opnd]);
				exit(0);
			} break;
			}
		}
	}
	/* For each valid address mode replace its symbols in the symbol table */
	return L;
}
	


void copy_string_to_array(char *string, char *copy_to, int start, int end)
{
	int i;

	for (i = 0; i <= (end - start); i++)
	{
		copy_to[start + i] = string[i];
	}
}

void fill_rows_with(char mc[MACHINE_CODES][MACHINE_CODE_ROW], char character, int start_row, int num_rows)
{
	int i;

	for (i = 0; i < num_rows; i++)
	{
		memset(mc[start_row + i], character, MACHINE_CODE_ROW);
	}
}
#endif /* HANDLER_H */