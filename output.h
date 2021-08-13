#ifndef _H_OUTPUT_
#define _H_OUTPUT_

#include "err.h"
#include "symtab.h"
#include "memory.h"
#include "str_utils.h"

void saveSymtabOutput(FILE *file, SymbolTable *symtab, bool is_entry, bool is_extern);
ErrorType saveOutout(char* name, Memory *memory, SymbolTable *symtab);

#endif
