#include "err.h"


/* TODO: maybe add parameter of the offending line to be printed as well */
void print_error(ErrorType err) {
  /* print a string reflecting the error */
  char *str;
  switch (err) {
  case INVALID_LABEL_NAME:
    str = "Invalid label name encountered";
    break;

  case LINE_TOO_LONG:
    str = "Line is longer than allowed length";
    break;
    
  case INVALID_FILE_END:
    str = "Encounterd an invalid file end";
    break;

  case FILE_OPEN_ERROR:
    str = "Could not open file";
    break;

  default:
    str = "Unknown error value";
    break;
  }

  printf("%s\n", str);
}
