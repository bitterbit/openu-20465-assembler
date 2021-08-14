#ifndef _H_FIRST_PASS_
#define _H_FIRST_PASS_

#include "assembly_line.h"
#include "line_queue.h"
#include "memory.h"
#include "symtab.h"

#include <stdio.h>

bool firstPass(FILE *asm_file, SymbolManager *syms, Memory *memory,
               LineQueue *queue, size_t *instruction_counter, ErrorType *err);

#endif
