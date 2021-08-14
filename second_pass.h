#ifndef _H_SECOND_PASS_
#define _H_SECOND_PASS_

#include "assembly_line.h"
#include "line_queue.h"
#include "memory.h"
#include "symtab.h"

#include <stdio.h>
#include <string.h>

bool secondPass(SymbolManager *syms, Memory *memory, LineQueue *queue);

#endif
