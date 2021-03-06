#ifndef _H_SYMTAB_
#define _H_SYMTAB_
#include <stdio.h>

#include "bool.h"
#include "err.h"
#include "list.h"

typedef enum { SymbolSection_Code, SymbolSection_Data } SymbolSection;

typedef struct Symbol Symbol;
struct Symbol {
    char *symbol;
    size_t value;
    bool is_entry;
    bool is_external;
    size_t *dependent_offsets; /* list of all instruction offsets that depend on this symbol */
    size_t dependent_offsets_count;
    SymbolSection section;
    void (*free)(Symbol *self);
};

Symbol *newSymbol(char *name, size_t value, bool is_entry, bool is_external,
                  SymbolSection section);

typedef struct SymbolTable SymbolTable;

/* SymbolTable keeps track of all symbols and their values */
struct SymbolTable {
    ListNode *head;

    /* insert opcode to list, this function takes ownership of this opcode
     * structure */
    ErrorType (*insert)(SymbolTable *self, Symbol *sym);

    /* check if a opcode with a given symbol name exists in list */
    bool (*exists)(SymbolTable *self, char *symbol_name);

    /* get opcode by symbol name */
    Symbol *(*find)(SymbolTable *self, char *symbol_name);

    void (*free)(SymbolTable *self);
};

typedef struct SymbolManager SymbolManager;
/* wrapper around SymbolTable keeping track of external and entry symbols */
struct SymbolManager {
    SymbolTable *symtab;
    bool has_external;
    bool has_entry;

    ErrorType (*insertSymbol)(SymbolManager *self, char *name, size_t value,
                              bool is_extern, SymbolSection section);
    ErrorType (*markSymEntry)(SymbolManager *self, char *name);
    Symbol *(*useSymbol)(SymbolManager *self, char *name,
                         size_t instruction_counter, ErrorType *out_err);
    void (*fixDataSymbolsOffset)(SymbolManager *self, size_t offset);
    void (*writeExtFile)(SymbolManager *self, FILE *file);
    void (*writeEntFile)(SymbolManager *self, FILE *file);
    void (*free)(SymbolManager *self);
};

SymbolTable *newSymbolTable();
SymbolManager *newSymbolManager();

#endif
