#include "err.h"
#include "assembly_line.h"
#include "bool.h"

void print_error(ErrorType err) {
    /* print a string reflecting the error */
    char *str;
    bool found = true;

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

    case ERR_INVALID_LABEL:
        str = "Invalid label";
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

    case ERR_INVALID_CODE_INSTRUCTION:
        str = "Invalid code instruction";
        break;

    case ERR_INVALID_REGISTER:
        str = "Invalid register";
        break;

    case ERR_INVALID_NUMBER_TOKEN:
        str = "Invalid number token";
        break;

    case ERR_INVALID_NUMBER_SIZE:
        str = "Invalid number size";
        break;

    case ERR_EOF:
        str = "Encountered end of file";
        break;

    case ERR_UNKNOWN_LABEL_REFERENCED:
        str = "An unknown label was referenced";
        break;

    case ERR_INVALID_EXTERNAL_LABEL_REFERENCE:
        str = "An external label was referenced in and invalid manner";
        break;

    case ERR_LABEL_TOO_FAR:
        str = "Invalid branch, label too far";
        break;

    default:
        str = "Unknown error value";
        found = false;
        break;
    }

    printf("Error: %s\n", str);
    if (found == false) {
        printf("Error number %d\n", err);
    }
}
