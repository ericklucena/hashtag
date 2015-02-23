/* 
* @Author: Erick Lucena Palmeira Silva
* @Date:   2015-01-27 17:36:38
* @Last Modified by:   Erick Lucena Palmeira Silva
* @Last Modified time: 2015-01-28 17:44:03
*/

#ifndef HASHTAG_UTIL_H
#define HASHTAG_UTIL_H 1

char* stringAlloc (int lenght);
char** argsAlloc (int lenght);
char** argsRealloc (char** args, int argc);
void argsFree (int argc, char **args);
char** argsCopy (int argc, char **argv);
char* strToken (char* string);
char** parseCommand (char* command, int *argc);

#endif
