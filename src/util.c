/* 
* @Author: Erick Lucena Palmeira Silva
* @Date:   2015-01-27 17:36:17
* @Last Modified by:   Erick Lucena Palmeira Silva
* @Last Modified time: 2015-02-23 12:47:13
*/

#include "util.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>


char* stringAlloc (int lenght)
{
	char *string;

	string = malloc (sizeof(char)*lenght+1);
	assert(string);
	string[0]= '\0';

	return string;
}

char** argsAlloc (int argc)
{
	char ** args;

	args = calloc (sizeof(char*), argc);
	assert(args);

	return args;
}

char** argsRealloc (char** args, int argc)
{
	char ** nargs;

	nargs = realloc (args, sizeof(char*)*argc);
	assert(nargs);

	return nargs;
}

void argsFree (int argc, char **args)
{
	int i;

	for (i=0; i<argc; i++)
	{
		free(args[i]);
	}

	free(args);

}

char** argsCopy (int argc, char** argv)
{
	int i;
	char** args;

	args = argsAlloc(argc+1); //One extra space for the NULL terminator of the execvp function

	for (i=0; i<argc; i++)
	{
		args[i] = stringAlloc(strlen(argv[i]));
		strcpy(args[i], argv[i]);
	}

	return args;
}

char** parseCommand (char* command, int *argc)
{
	int i;
	char *arg;
	char **args;
	char **argv;

   	args = argsAlloc(1);
	arg = strToken(command);
	
	for (i=0; arg != NULL;)
	{
		args[i++] = arg;
		args = argsRealloc(args, i+1);
		arg = strToken(NULL);
	}
	
	*argc = i;
   	argv = argsCopy(i, args);

   	return argv;
}

void removeSeparators (char* string)
{
	int i;
	int size;
	bool quotesOpen = false;

	size = strlen(string);

	for (i=0; i<size; i++)
	{
		if (string[i] == '\"')
		{
			if (quotesOpen)
			{
				quotesOpen = false;
			}
			else
			{
				quotesOpen = true;
			}
		}
		else if (string[i] == ' ' && !quotesOpen)
		{
			string[i] = '\0';
		}
	}

	for (i=0; i<size; i++)
	{
		if (string[i] == '\"')
		{
			string[i] = '\0';
		}
	}
}

char* strToken (char* string)
{
	char* token = NULL;
	static int size;
	static int current;
	static char* original;

	if (string != NULL)
	{
		original = string;
		size = strlen(string);
		current = 0;
		removeSeparators(string);
	}

	for (;(original[current] == '\0') && (current<size);current++);

	if (current<size)
	{
		token = original+current;
		current += strlen(original+current)+1;		
	}

	return token;

}