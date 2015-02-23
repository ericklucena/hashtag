/* 
* @Author: Erick Lucena Palmeira Silva
* @Date:   2015-01-27 16:32:34
* @Last Modified by:   Erick Lucena Palmeira Silva
* @Last Modified time: 2015-02-23 12:46:50
*/

#ifndef HASHTAG_HASHTAG_H
#define HASHTAG_HASHTAG_H 1

#include <stdbool.h>

#define COMMAND_EXIT "exit"
#define COMMAND_ADD "add"
#define COMMAND_ARG "arg"
#define COMMAND_TIMES "times"
#define COMMAND_BACKGROUND "&"
#define COMMAND_PIPE "|"
#define COMMAND_REDIRECT_INPUT "<"
#define COMMAND_REDIRECT_OUTPUT ">"

int initShell();
int executeCommand (int argc, char **argv);
void add (int argc, char **argv);
void times (int argc, char **argv);
void arg (int argc, char **argv);
bool consumePipe(int argc, char** argv);
bool consumeRedirect(int argc, char** argv);
void execute (int argc, char **argv);
void executeOnBackground (int argc, char **argv);
void executePipeOutput(int argc, char **argv, int argc2, char **argv2);
void executeRedirectInput(int argc, char **argv, char *filename);
void executeRedirectOutput(int argc, char **argv, char *filename);
void onChildExit (int signalNumber);

#endif