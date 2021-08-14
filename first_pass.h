#ifndef _H_FIRST_PASS_
#define _H_FIRST_PASS_

#include "assembly_line.h"
#include "symtab.h"
#include "memory.h"
#include "line_queue.h"

#include <stdio.h>

ErrorType firstPass(FILE* asm_file, SymbolManager *syms, Memory *memory, LineQueue *queue, size_t *instruction_counter);

#endif
