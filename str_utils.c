#include <stdlib.h>
#include <stdio.h>

#include "str_utils.h"
#include "assembly_line.h"
#include "err.h"
#include "ctype.h"
#include "bool.h"

char *seperate_string_by_token(char **string, char *delimeters) {
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

void remove_all_spaces(char *str) {
  /* Remove all spaces from a string, to simplify parsing */
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

void remove_leading_spaces(char **str){
  if (str == NULL || *str == NULL) {
    return;
  }

  /* Remove leading spaces */
  while (**str != '\0' && isspace(**str)) {
    (*str)++;
  }
}

void remove_trailing_spaces(char **str){
  /* Remove trailing spaces */

  if (str == NULL || *str == NULL) {
    return;
  }

  char *string_end = (*str + strlen(*str) - 1);
  while (string_end > (*str) && isspace(*string_end)) {
    string_end--;
  }

  /* Found last char which isn't space - move one forward and cut the string */
  string_end++;
  *string_end = '\0';
}

void remove_leading_and_trailing_spaces(char **str) {
  remove_leading_spaces(str);
  remove_trailing_spaces(str);
}

bool contains_space(char *str) {
  /* return true if str contains space, false otherwise */
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

int check_for_empty_line(char *line_str) {
  /* Return 0 if line is empty, or containing only space chars, positive integer
   * otherwise. */
  char cmd_copy[BUFFER_SIZE];
  int len;
  strcpy(cmd_copy, line_str);

  remove_all_spaces(cmd_copy);
  len = strlen(cmd_copy);

  return len;
}


ErrorType readline(FILE *file, char *buf) {
  /* Read a line from stdin, and check for the end of input */
  char *string_end;
  size_t len;

  buf = fgets(buf, BUFFER_SIZE, file);

  /* TODO: Is this needed? */
  if (buf == NULL) {
    return INVALID_FILE_END;
  }

  /* Remove trailing \n */
  len = strlen(buf);
  string_end = (buf) + len - 1;
  /* TODO: why do i need to len read is more than 0? */
  if (len > 0 && *string_end == '\n') {
    *string_end = '\0';
  }

  /* Check for too long lines, if buf doesn't end with a newline, and the maximum bytes were read*/
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

FILE* openfile(char *path, ErrorType *err){
    FILE *file = fopen(path, "r");

    if (file == NULL){
        *err = FILE_OPEN_ERROR;
    }
    return file;
}


bool str_in_str_array(char* str, char* str_arr[], int arr_len){
    int i;
    for(i = 0; i < arr_len; i++) {
      if(strncmp(str_arr[i], str, strlen(str)) == 0) {
        return true;
      }
    }
    return false;
}
