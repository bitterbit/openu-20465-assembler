#ifndef _H_ERR_
#define _H_ERR_

typedef int Error;

#define STOP -1
#define OK 0
#define ERR_DUPLICATE_SYMBOL 1
#define ERR_INVALID_ENTRY 2
#define ERR_ENTRY_SYM_NOT_FOUND 3



typedef enum {
  SUCCESS = 0,
  INVALID_LABEL_NAME = 1,
  LINE_TOO_LONG = 2,
  INVALID_FILE_END = 3,
  FILE_OPEN_ERROR = 4,

} ErrorType;

void print_parsing_error(ErrorType err);

#endif