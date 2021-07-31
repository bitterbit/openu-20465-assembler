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

  case ERR_DUPLICATE_SYMBOL:
    str = "The same symbol was decleared twice";
    break;

  case ERR_INVALID_ENTRY:
    str = "Invalid entry";
    break;

  case ERR_ENTRY_SYM_NOT_FOUND:
    str = "No entry specified, must specify an .entry";
    break;

  case ERR_INVALID_COMMAND_NAME:
    str = "Invalid command name";
    break;

  case ERR_INVALID_COMMAND_FORMAT:
    str = "Invalid command format";
    break;

  case ERR_UNTERMINATED_STRING_ARG:
    str = "Unterminated string argument";
    break;

  default:
    str = "Unknown error value";
    break;
  }

  printf("Error: %s\n", str);
}
