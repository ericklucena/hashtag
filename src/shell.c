/* 
* @Author: Erick Lucena Palmeira Silva
* @Date:   2015-01-28 17:34:06
* @Last Modified by:   Erick Lucena Palmeira Silva
* @Last Modified time: 2015-02-23 12:47:07
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtag.h"
#include "util.h"

#define COMMAND_SIZE 1000

int main()
{

	int argc;
	char **argv;
	char command[COMMAND_SIZE];

	system("clear");
	initShell();

	while (true)
	{
		printf("haSHtag # ");
		fgets(command, COMMAND_SIZE, stdin);
		command[strlen(command)-1] = '\0';
		argv = parseCommand(command, &argc);
		if (argc > 0)
		{
			executeCommand(argc, argv);
		}

		printf("\n");

	}

    return 0;
}