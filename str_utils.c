#include <stdio.h>
#include <stdlib.h>

#include "assembly_line.h"
#include "bool.h"
#include "ctype.h"
#include "err.h"
#include "str_utils.h"

/* Split string by a list of delimter characters */
char *splitString(char **string, char *delimeters) {
    char *string_start = *string;
    char *string_end;

    /* Empty string */
    if (string_start == NULL)
        return NULL;

    /* find the delimeter, and end the string at that point */
    string_end = string_start + strcspn(string_start, delimeters);

    if (*string_end) {
        /* add null to seperate the strings */
        *string_end = '\0';
        /* original string points after the delimiter/null */
        *string = string_end + 1;
    } else {
        /* Couldn't find the token */
        *string = NULL;
    }
    return string_start;
}

/* Remove all spaces from a string, to simplify parsing */
void removeAllSpaces(char *str) {
    char *index = str;
    if (str == NULL) {
        return;
    }

    do {
        while (isspace(*index)) {
            index++;
        }
    } while ((*str++ = *index++) != 0);
}

/* Remove the leading spaces of a string */
void removeLeadingSpaces(char **str) {
    if (str == NULL || *str == NULL) {
        return;
    }

    while (**str != '\0' && isspace(**str)) {
        (*str)++;
    }
}

/* Remove the trailing spaces of a string */
void removeTrailingSpaces(char **str) {
    char *string_end;
    if (str == NULL || *str == NULL) {
        return;
    }

    string_end = (*str + strlen(*str) - 1);
    while (string_end > (*str) && isspace(*string_end)) {
        string_end--;
    }

    /* Found last char which isn't space - move one forward and cut the string
     */
    string_end++;
    *string_end = '\0';
}

void removeLeadingAndTrailingSpaces(char **str) {
    removeLeadingSpaces(str);
    removeTrailingSpaces(str);
}

/* return true if str contains space, false otherwise */
bool containsSpace(char *str) {
    bool ret = false;
    if (str != NULL) {
        while (*str != '\0') {
            if (isspace(*str)) {
                ret = true;
                break;
            }
            str++;
        }
    }

    return ret;
}

/* Return true if a string contains a char */
bool containsChar(char *str, char c) {
    if (str == NULL) {
        return false;
    }

    if (strchr(str, c) == NULL) {
        return false;
    }

    return true;
}

/* Return 0 if line is empty or contains only space chars,
    * non zero int otherwise */
int checkForEmptyLine(char *line_str) {
    char cmd_copy[BUFFER_SIZE];
    int len;

    if (line_str == NULL) {
        return 0;
    }

    strcpy(cmd_copy, line_str);

    removeAllSpaces(cmd_copy);
    len = strlen(cmd_copy);

    return len;
}

/* Read a line from stdin, and check for the end of input */
ErrorType readline(FILE *file, char *buf) {
    char *string_end;
    size_t len;

    buf = fgets(buf, BUFFER_SIZE, file);

    if (buf == NULL) {
        return ERR_EOF;
    }

    len = strlen(buf);
    string_end = (buf) + len - 1;
    if (len > 0 && *string_end == '\n') {
        *string_end = '\0';
    }

    /* Check for too long lines, if buf doesn't end with a newline, and the
       maximum bytes were read*/
    else if (len == (BUFFER_SIZE - 1)) {
        /* skip to next line */
        while (fgets(buf, BUFFER_SIZE, file) != NULL) {
            if (strlen(buf) != (BUFFER_SIZE - 1)) {
                break;
            }
        }
        return LINE_TOO_LONG;
    }
    return SUCCESS;
}

/* Open a file safely, caller should close the file when finished */
FILE *openfile(char *path, ErrorType *err) {
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        *err = FILE_OPEN_ERROR;
    }
    return file;
}

/* Check if an array of string includes a string */
bool strArrayIncludes(char *str, char *str_arr[], int arr_len) {
    if (findInArray(str, str_arr, arr_len) == -1) {
        return false;
    }

    return true;
}

/* returns index of string in array if found, or -1 if not in array */
int findInArray(char *str, char *str_arr[], int arr_len) {
    int i;
    for (i = 0; i < arr_len; i++) {
        if (strcmp(str, str_arr[i]) == 0)
            return i;
    }
    return -1;
}

/* get the base name from a path */
char *toBasename(char *path) {
    char *last_slash = NULL;

    size_t len = strlen(path);
    if (path[len - 1] == '/') {
        path[len - 1] = '\0';
    }

    last_slash = strrchr(path, '/');
    if (last_slash == NULL) {
        return path;
    }

    return last_slash + 1;
}

/* Remove the file extension from a file */
void removeFileExtension(char *filename) {
    char *ext = NULL;
    ext = strrchr(filename, '.');
    if (ext == NULL) {
        return;
    }

    *ext = '\0';
}

bool isNumber(char c) {
    if (c >= '0' && c <= '9') {
        return true;
    }

    return false;
}

bool isLetter(char c) {
    if (c >= 'a' && c <= 'z') {
        return true;
    }

    if (c >= 'A' && c <= 'Z') {
        return true;
    }

    return false;
}

bool isOnlyNumbers(char *str) {
    size_t len = strlen(str);
    int i;

    for (i = 0; i < len; i++) {
        char c = str[i];

        if (isNumber(c) == true) {
            continue;
        }

        return false;
    }

    return true;
}

/* Check if a string contains only numbers and letters */
bool isOnlyLettersAndNumbers(char *str) {
    size_t len = strlen(str);
    int i;

    for (i = 0; i < len; i++) {
        char c = str[i];

        if (isLetter(c) == true || isNumber(c) == true) {
            continue;
        }

        return false;
    }

    return true;
}
