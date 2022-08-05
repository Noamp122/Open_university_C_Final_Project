#include "finals.h"


/*functions declarations*/
void scan_row(char data[MAX], char mission[MAX][MAX], int * words);
int rebuild_data_without_spaces(char data[MAX], int * i, int * j);
int check_special_word(char mission[MAX][MAX], int * words);
int building_args(char data[MAX], char mission[MAX][MAX], int * i, int * j, int * words);
int check_label_and_functions(char data[MAX], char mission[MAX][MAX], int* i, int* j, int* words);
void clear_path(char data[MAX], int * i);

/* this function removes all the white chars before the first non-white chars */
void clear_path(char data[MAX], int * i) 
{

    while (data[ * i] == ' ' || data[ * i] == '\t') 
	{
        /* cleaning all spaces and tabs before first latter*/

        * i = * i + 1;

    }

}

/* this function receive a string , separate the words and counts them. */
void scan_row(char data[MAX], char mission[MAX][MAX], int* words) 
{

    int i = 0, j = 0;
    *words = 0;
    clear_path(data, & i);

	/*we give this line to a function that goes over it and saves the lable (if there is one) and the function word.
if there is error in the syntax of the beggining of the line the function will return 1, if every thing is fine the function return 0*/
    if (check_label_and_functions(data, mission, &i, &j, words)) 	
	{
        return; /*if there is a syntax error we go out of the method*/
    }

    j = 0;

    memset(data, '\0', i - 1); /*covering all the chars we got so far with '\0' */

	/*moving the rest of the words to the beggining of the line without spaces or tabs*/
/*if the function detected syntax error it returns 1 and we exit this function*/
    if (rebuild_data_without_spaces(data, & i, & j))
	{
        return;
    }

    data[j] = '\0';

    i = 0;
    j = 0;

	/*calling a function that put the arguments of the assemblly function in 
     mission array*/
	/*if there is a syntax error the function returns 1 and we exit this function*/
    if (building_args(data, mission, & i, & j, words)) {

        return;

    }

    if ( *words == 1 && mission[1][0] == '\0') 
	{
        *words = *words - 1;
    }

    *words = *words + 1;

    memset(mission[ *words + 1], '\0', MAX); 

    if (mission[ *words - 1][strlen(mission[ *words - 1]) - 1] == 13) 
	{
        mission[ *words - 1][strlen(mission[ *words - 1]) - 1] = '\0';
    }

    if (mission[ *words - 1][0] == '\0') 
	{
        *words = *words - 1;
    }

}


/*this function checks if the word is a special word in the assembly language, and return 1 if so, 0 otherwise*/

int check_special_word(char mission[MAX][MAX], int * words) {

    if (!strcmp(mission[ * words], ".data") || !strcmp(mission[ * words], ".string") || !strcmp(mission[ * words], ".struct") ||

        !strcmp(mission[ * words], "not") || !strcmp(mission[ * words], "inc") ||

        !strcmp(mission[ * words], "mov") || !strcmp(mission[ * words], "cmp") ||

        !strcmp(mission[ * words], "add") || !strcmp(mission[ * words], "sub") ||

        !strcmp(mission[ * words], "lea") || !strcmp(mission[ * words], "clr") ||

        !strcmp(mission[ * words], "dec") || !strcmp(mission[ * words], "jmp") ||

        !strcmp(mission[ * words], "bne") || !strcmp(mission[ * words], "jsr") ||

        !strcmp(mission[ * words], "hlt") || !strcmp(mission[ * words], "prn") ||

        !strcmp(mission[ * words], "rts") || !strcmp(mission[ * words], "get") ||

        !strcmp(mission[ * words], ".extern") || !strcmp(mission[ * words], ".entry")) 
	{
        return 1;
    }

    return 0;

}

/*this function checks if there is a label in the data and build the first words to mission */
int check_label_and_functions(char data[MAX], char mission[MAX][MAX], int* i, int* j, int* words) 
{

    int temp = 0; /*counter of the allowed words in assembly in this line*/
	int flag = 0; /*a flag that indi*/

    while (data[ * i] != '\n' && data[ * i] != EOF && data[ * i] != '\0') /*while this line not ended*/ 
	{

        if (data[ * i] != ' ' && data[ * i] != '\t') /*while the current char is not a sapace or Tab we continue to build the word to the mission array*/ 
		{
            mission[ * words][ * j] = data[ * i];
            * j += 1;
            * i += 1;
            continue;
        }
 		else /*if the current char is a space or Tab*/
		{
            if (data[ * i - 1] == ':') /*if the previous char is ':' we end the word without it, and move on to the next word*/
			 {
                mission[ * words][ * j] = '\0';
                * j = 0;
                * i += 1;
                * words = * words + 1;
                clear_path(data, i);
                temp++; /*raising the counter of allowed words by 1*/
                continue;
            }

            if (mission[ * words][strlen(mission[ * words]) - 1] == ',') /*if the last char of the word is ',' we raise a flag and end the word without the ','*/
			{
                flag = 1;
                mission[ * words][strlen(mission[ * words]) - 1] = '\0';
            }

            if (check_special_word(mission, words)) /*if the word is one of the special words in the assembly language*/
			{
                temp++; /*raising the counter of allowed words by 1*/
                mission[ * words][ * j] = '\0'; /*ending the word in the array*/
                * j += 1;

                if (flag) /*if we have ',' right after spcial word we give alert about it, and return syntax erorr code (1)*/
				{
                    printf("comma after command\n");
                    return 1;
                }

                * i += 1; /*continue to the next char in the line*/
                clear_path(data, i); /*moving to the next non white space char*/
            }

            if (!temp) /*if the first word (after the lable if there is one) is not an allowed word in the assembly language*/
			{
                printf("the first word isn't allowed");
                return 1; /*returning an error code*/
            }

            if (mission[ * words][ * j - 1] == ',') /*if we have ',' between words we remove it in the array*/
			{
                * j -= 1;
            }

            mission[ * words][ * j] = '\0'; /*ending the word in the array*/

            * j = 0;

            * words = * words + 1; /*moving to the next word in the array*/

            clear_path(data, i); /*moving to the next non white space char*/

            break; /*getting out of the loop*/

        }

    }

    return 0; /*if nothing was wrong we return an non-error code*/

}



/* rebuild the remaining line without spaces and tabs, this will allow to build the arguments more easy */
/*The function returns 1 if there is a syntax error, and 0 otherwise*/
int rebuild_data_without_spaces(char data[MAX], int * i, int * j) 
{

    int in_word = 0;

    while (data[ * i] != '\0' && data[ * i] != '\n' && data[ * i] != EOF) 
	{
        /*removing spaces and tabs in data input*/
        if (data[ * i] != ' ' && data[ * i] != '\t') 
		{
            if (data[ * i] == ',') 
			{
                in_word = 0;
            } 
			else 
			{
                if (in_word == 2) /*if there are to words without ',' in between we alert about it*/
				{
                    printf("Missing comma\n");
                    return 1; /*returning error code*/
                }

                in_word = 1;

            }

            data[ * j] = data[ * i];
            * j += 1;

        } 

		else if (in_word == 1) 
		{
            in_word = 2;
        }

        * i += 1;

    }

    return 0;

}



/* build each argumen to a separate line in mission array*/
/*if there is a syntax error it returns 1, otherwise returns 0 */
int building_args(char data[MAX], char mission[MAX][MAX], int * i, int * j, int * words) 
{

    while (data[ * i] != '\0' && data[ * i] != EOF) 
	{
        /*building args*/

        if (data[ * i] == ',') 
		{
            mission[ * words][ * j] = '\0';
            * words = * words + 1;

            if ( * j == 0 || (data[ * i] == ',' && data[ * i + 1] == '\0')) 
			{
                printf("Multiple consecutive commas\n");
                return 1;
            }

            * j = 0;
            * i += 1;
        } 
		
		else 
		{
            mission[ * words][ * j] = data[ * i];
            * j += 1;
            * i += 1;
        }

    }

    return 0;

}
