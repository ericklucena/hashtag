/* 
* @Author: Erick Lucena Palmeira Silva
* @Date:   2015-01-27 16:32:23
* @Last Modified by:   Erick Lucena Palmeira Silva
* @Last Modified time: 2015-02-23 12:47:02
*/

#define _XOPEN_SOURCE 500

//#define DEBUG
//#define TTY_INPUT

#include "hashtag.h"
#include "util.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <termios.h>


FILE *input;
FILE *output;

int initShell ()
{
	input = fopen("/dev/tty", "r");
	output = fopen("/dev/tty", "w");

	#ifdef TTY_INPUT
		struct termios initial_settings, new_settings;
		tcgetattr(fileno(input),&initial_settings);
		new_settings = initial_settings;
		new_settings.c_lflag &= ~ICANON; 
		new_settings.c_cc[VMIN] = 1;
		new_settings.c_cc[VTIME] = 0;

		if(tcsetattr(fileno(input), TCSANOW, &new_settings)) {
			fprintf(stderr,"could not set attributes for tty\n");
			return EXIT_FAILURE;
		}
	#endif

	return 0;
}

int executeCommand (int argc, char **argv)
{
	bool redirected = false;
	bool piped = false;

	#ifdef DEBUG
	int i;

	for (i=0; i<argc; i++)
	{
		fprintf(output, "##: %s\n", argv[i]);
	}
	fprintf(output, "\n\n");

	#endif

	if (argc > 0 && !strcmp(argv[argc-1], COMMAND_BACKGROUND))
	{
		executeOnBackground(argc-1, argsCopy(argc-1, argv));
	}
	else
	{
		piped = consumePipe(argc, argv);

		if (!piped)
		{
			redirected = consumeRedirect(argc, argv);
		}

		if (!redirected && !piped)
		{
			if (!strcmp(argv[0], COMMAND_EXIT))
			{
				exit(0);
			}
			else if (!strcmp(argv[0], COMMAND_ADD))
			{
				add(argc-1, argv+1);
			}
			else if (!strcmp(argv[0], COMMAND_TIMES))
			{
				times(argc-1, argv+1);
			}
			else if (!strcmp(argv[0], COMMAND_ARG))
			{
				arg(argc-1, argv+1);
			}
			else
			{
				execute(argc, argv);
			}	
		}	
	}
	return 0;
}

/*
 * Search for a redirection token, consumes it and calls for the fuction that executes the redirection
 */
bool consumeRedirect (int argc, char** argv)
{
	int i;
	bool redirected;

	for (i=argc-1; i > 0; i--){
		if(!strcmp(argv[i], COMMAND_REDIRECT_INPUT) || !strcmp(argv[i], COMMAND_REDIRECT_OUTPUT))
		{
			if (!strcmp(argv[i], COMMAND_REDIRECT_INPUT))
			{
				executeRedirectInput(i, argsCopy(i, argv), argv[i+1]);
				redirected = true;
				break;
			}
			else if	(!strcmp(argv[i], COMMAND_REDIRECT_OUTPUT))
			{
				executeRedirectOutput(i, argsCopy(i, argv), argv[i+1]);
				redirected = true;
				break;
			}
		}
	}

	return redirected;
}

/*
 * Search for a pipe token, consumes it and calls for the fuction that executes the redirection
 */
bool consumePipe (int argc, char** argv)
{
	int i;
	bool piped;

	for (i=argc-1; i > 0; i--){
		if(!strcmp(argv[i], COMMAND_PIPE))
		{
			executePipeOutput(i, argsCopy(i, argv), argc-i-1, argsCopy(argc-i-1, argv+i+1));
			piped = true;
			break;
		}
	}

	return piped;
}

void add (int argc, char **argv)
{
	int sum = 0;
	int toSum;
	int i;

	for (i=0; i<argc; i++)
	{
		sscanf(argv[i], "%i", &toSum);
		sum += toSum;
		if (i == (argc-1))
		{
			printf("%s = ", argv[i]);
		}
		else
		{
			printf("%s + ", argv[i]);
		}
	}
	if(argc>0)
	{
		printf("%d\n", sum);
	}
	else
	{
		printf("Nothing to sum\n");
	}
}

void times (int argc, char **argv)
{
	int result = 1;
	int toMultiply;
	int i;

	for (i=0; i<argc; i++)
	{
		sscanf(argv[i], "%i", &toMultiply);
		result *= toMultiply;
		if (i == (argc-1))
		{
			printf("%s = ", argv[i]);
		}
		else
		{
			printf("%s * ", argv[i]);
		}
	}

	if (argc > 0)
	{
		printf("%d\n", result);
	}
	else
	{
		printf("Nothing to multiply\n");
	}
}

void arg (int argc, char **argv)
{
	int i;

	printf("argc = %d, args = ", argc);

	for (i=0; i<argc; i++)
	{
		printf("%s", argv[i]);
		if (i != (argc-1))
		{
			printf(", ");
		}else{
			printf("\n");
		}
	}
}

/*
 * Execute the program received
 */
void execute (int argc, char **argv)
{
	int pid;
	pid = fork();

	sigset(SIGCHLD, SIG_DFL);

	if(pid == 0) //Child process
	{ 
		#ifdef DEBUG
			int i;
			fprintf(output, "RUNNING: \n");
			for (i=0; i<argc; i++)
			{
				fprintf(output, "%s ", argv[i]);
			}
			fprintf(output, "\n");
		#endif

		execvp(argv[0], argv);

		#ifdef DEBUG
			fprintf(output, "ENDED: \n");
			for (i=0; i<argc; i++)
			{
				fprintf(output, "%s ", argv[i]);
			}
			fprintf(output, "\n");
		#endif

		exit(0);
	}
	else
	{
		wait(&pid);
	}
}

/*
 * Run the program received as arguments in a background process
 */
void executeOnBackground(int argc, char **argv)
{
	int pid;

	#ifdef DEBUG
	int i;

	for (i=0; i<argc; i++)
	{
		fprintf(output, "b#%s\n", argv[i]);
	}

	#endif

	sigset(SIGCHLD, &onChildExit);

	pid = fork();

	if(pid == 0) //Child process
	{ 
		executeCommand(argc, argv);
		exit(0);
	}
	else
	{
		fprintf(output, "\n%d started\n", pid);
	}

}

/*
 * Creates a pipe between the programs received as parameters
 */
void executePipeOutput(int argc, char **argv, int argc2, char **argv2)
{
	int pid;
	int status;
	int pipefd[2];


	#ifdef DEBUG
	int i;

	for (i=0; i<argc; i++)
	{
		fprintf(output, "|1#%s\n", argv[i]);
	}
	fprintf(output, "\n");

	for (i=0; i<argc2; i++)
	{
		fprintf(output, "|2#%s\n", argv2[i]);
	}

	#endif

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	sigset(SIGCHLD, SIG_DFL);
	pid = fork();

	if(pid == 0) //Child process	
	{
		close(STDOUT_FILENO);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);
		executeCommand(argc, argv);
		fclose(stdout);
		exit(0);

	}
	else
	{	
		sigset(SIGCHLD, SIG_DFL);
		pid = fork();

		if(pid == 0) //Child process
		{

			close(pipefd[1]);
			close(STDIN_FILENO);
			dup2(pipefd[0],STDIN_FILENO);
			executeCommand(argc2, argv2);
			close(pipefd[0]);
			exit(0);
		}
		else{
			pid = wait(&status);

			#ifdef DEBUG
			fprintf(output, "Piped: %d\n", pid);
			#endif

			//pid = wait(&status);

			#ifdef DEBUG
			fprintf(output, "Piped: %d\n", pid);
			#endif
		}
	}
}

/*
 * Redirects the input for the selected file
 */
void executeRedirectInput(int argc, char **argv, char *filename)
{
	int pid;
	int status;

	sigset(SIGCHLD, SIG_DFL);

	#ifdef DEBUG
		int i;

		for (i=0; i<argc; i++)
		{
			fprintf(output, "<#: %s\n", argv[i]);
		}
		fprintf(output, "\n\n");
	#endif

	pid = fork();

	if(pid == 0) //Child process
	{
		freopen(filename, "r", stdin);
		executeCommand(argc, argv);
		fclose(stdin);
		exit(0);
	}
	else
	{
		wait(&status);
	}
}

/*
 * Redirects the output for the selected file
 */
void executeRedirectOutput(int argc, char **argv, char *filename)
{
	int pid;
	int status;

	#ifdef DEBUG
		int i;

		for (i=0; i<argc; i++)
		{
			fprintf(output, ">#: %s\n", argv[i]);
		}
		fprintf(output, "\n\n");
	#endif

	pid = fork();

	if(pid == 0) //Child process
	{ 
		freopen(filename, "w", stdout);
		executeCommand(argc, argv);
		fclose(stdout);
		exit(0);
	}
	else
	{
		wait(&status);
	}
}

/*
 * Handler function for the SIGCHLD signal.
 */
void onChildExit (int signalNumber)
{
	int pid;
	int status;

	pid = wait(&status);

	fprintf(output, "\n%d done\n", pid);	
}