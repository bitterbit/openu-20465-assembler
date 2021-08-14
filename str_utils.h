#ifndef _H_STR_UTILS_
#define _H_STR_UTILS_

#include <string.h>

#include "err.h"
#include "bool.h"

#define MAX_FILENAME_LENGTH 255

char *splitString(char **string, char *delimeters);
void removeAllSpaces(char *str);
void removeLeadingSpaces(char **str);
void removeTrailingSpaces(char **str);
void removeLeadingAndTrailingSpaces(char **str);
int containsSpace(char *str);
int containsChar(char *str, char c);
int checkForEmptyLine(char *command_str);
bool strArrayIncludes(char* str, char* str_arr[], int arr_len);
int findInArray(char *str, char *str_arr[], int arr_len);

ErrorType readline(FILE *file, char *buf);
FILE* openfile(char *path, ErrorType *err);
char* toBasename(char *path);
void removeFileExtension(char *filename);
bool isOnlyLettersAndNumbers(char *str);

#endif
