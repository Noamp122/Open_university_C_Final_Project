#ifndef IS_REGISTER_H
#define IS_REGISTER_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "finals.h"


/*A method that gest a string that supposed to represent a number, return 1 it is a valid number and 0 otherwise*/
int check_number(char *number)
{

	int i, num;

	for (i = 0; i < (int)strlen(number); i++)
	{

		if (number[i] == '-' || number[i] == '+')
		{
			continue;
		}

		if (number[i] < '0' || number[i] > '9')
		{
			return 0;
		}
	}

	num = atoi(number);

	if (num > 127 || num < -128) /*if the number is too big to be represent with 8 bits in 2's compliment*/
		return 0;

	return 1;
}


int check_if_number2(char string[])
{
	if (string[0] == '#' && isdigit(string[1]))
	{
		return 1;
	}
	return 0;
}

/* function that gets a string, if it's a register valid name it returns 1 otherwise 0 */
int is_register(char string[MAX])
{
    if (string[0] == 'r' && (string[1] >= '0' && string[1] <= '7') && string[2] == '\0')
    {
        return 1;
    }

    return 0;
}
#endif /* IS_REGISTER_H */