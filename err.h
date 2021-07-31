#ifndef _H_ERR_
#define _H_ERR_

#include <stdio.h>

typedef enum {
  SUCCESS = 0,
  INVALID_LABEL_NAME = 1,
  LINE_TOO_LONG = 2,
  INVALID_FILE_END = 3,
  FILE_OPEN_ERROR = 4,
  ERR_DUPLICATE_SYMBOL = 5,
  ERR_INVALID_ENTRY = 6,
  ERR_ENTRY_SYM_NOT_FOUND = 7,
  ERR_INVALID_LABEL = 8,
  ERR_INVALID_COMMAND_NAME = 9,
  ERR_INVALID_COMMAND_FORMAT = 10

} ErrorType;

void print_error(ErrorType err);

#endif
