#include <stdio.h>

#include "bool.h"
#include "list.h"
#include "err.h"

typedef enum {
    SymbolSection_Code, SymbolSection_Data
} SymbolSection;

typedef struct Opcode Opcode;
struct Opcode {
    char* sybmol;
    size_t value; /* TODO make sure value is not bigger than 24bit? */
    bool is_entry;
    SymbolSection section;
    void (*free)(Opcode* self);
};


/* TODO implement new and free */
Opcode* newOpcode(char* symbol, size_t value, bool is_entry, SymbolSection section);

typedef struct OpcodeList OpcodeList;
struct OpcodeList {
    ListNode* head;

    /* insert opcode to list, this function takes ownership of this opcode structure */
    Error (*insert) (OpcodeList* self, Opcode* opcode);

    /* check if a opcode with a given symbol name exists in list */
    bool (*exists)(OpcodeList* self, char* symbol_name);

    /* get opcode by symbol name */
    Opcode* (*find)(OpcodeList* self, char* symbol_name);

    void (*free)(OpcodeList* self);
};


OpcodeList* newOpcodeList();
