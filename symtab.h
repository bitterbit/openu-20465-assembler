#ifndef _H_SYMTAB_
#define _H_SYMTAB_
#include <stdio.h>

#include "bool.h"
#include "list.h"
#include "err.h"

typedef enum {
    SymbolSection_Code, 
    SymbolSection_Data
} SymbolSection;

typedef struct Symbol Symbol;
struct Symbol {
    char* symbol;
    size_t value; /* TODO make sure value is not bigger than 24bit? */
    bool is_entry;
    bool is_external;
    SymbolSection section;
    void (*free)(Symbol* self);
};

Symbol* newSymbol(char* name, size_t value, bool is_entry, bool is_external, SymbolSection section);

typedef struct SymbolTable SymbolTable;
struct SymbolTable {
    ListNode* head;

    /* insert opcode to list, this function takes ownership of this opcode structure */
    ErrorType (*insert) (SymbolTable* self, Symbol* sym);

    /* check if a opcode with a given symbol name exists in list */
    bool (*exists)(SymbolTable* self, char* symbol_name);

    /* get opcode by symbol name */
    Symbol* (*find)(SymbolTable* self, char* symbol_name);

    void (*free)(SymbolTable* self);
};

typedef struct SymbolManager SymbolManager;
struct SymbolManager {
    SymbolTable* symtab;
    ListNode *usedSymbolsHead;

    ErrorType (*insertSymbol)(SymbolManager *self, char* name, size_t value, bool is_extern, bool is_entry, SymbolSection section);
    ErrorType (*markSymEntry)(SymbolManager *self, char* name);
    Symbol* (*useSymbol)(SymbolManager *self, char* name);
    void (*fixDataSymbolsOffset)(SymbolManager *self, size_t offset);
    void (*writeExtFile)(SymbolManager *self);
    void (*writeEntFile)(SymbolManager *self);
    void (*free)(SymbolManager *self);
};

SymbolTable* newSymbolTable();
SymbolManager* newSymbolManager();

#endif
