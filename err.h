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
  ERR_INVALID_COMMAND_FORMAT = 10,
  ERR_UNTERMINATED_STRING_ARG = 11,
  ERR_INVALID_CODE_INSTRUCTION = 12,
  ERR_INVALID_REGISTER = 13,
  ERR_INVALID_NUMBER_TOKEN = 14,
  ERR_INVALID_NUMBER_SIZE = 15,
  ERR_INVALID_INSTRUCTION_TYPE = 16,
  ERR_INVALID_DATA_INSTRUCTION = 17

} ErrorType;

void print_error(ErrorType err);

#endif
