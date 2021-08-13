#include "output.h" 
#include <string.h>

void saveSymtabOutput(FILE *file, SymbolTable *symtab, bool is_entry, bool is_extern) {
    ListNode *node = NULL;
    ListIterator *iterator = newListIterator(symtab->head);

    while ((node = iterator->next(iterator)) != NULL) {
        Symbol *sym = node->data;

        if (sym == NULL) {
            continue;
        }

        if (sym->is_entry & is_entry || sym->is_external & is_extern) {
            fprintf(file, "%s %04lu\n", sym->symbol, sym->value);
        }
    }

    iterator->free(iterator);
}

ErrorType saveOutout(char* name, Memory *memory, SymbolTable *symtab) {
    char filename[MAX_FILENAME_LENGTH];
    FILE *outfile;

    if (strlen(name) + 4 > MAX_FILENAME_LENGTH) {
        return ERR_FILENAME_TOO_LONG;
    }

    memset(filename, 0, MAX_FILENAME_LENGTH);
    strcpy(filename, name);
    strcat(filename, ".ob");
    outfile = fopen(filename, "w");
    if (outfile == NULL) {
        return ERR_CREATING_OUTPUT_FILE;
    }
    memory->toFile(memory, outfile);
    fclose(outfile);


    memset(filename, 0, MAX_FILENAME_LENGTH);
    strcpy(filename, name);
    strcat(filename, ".ext");
    outfile = fopen(filename, "w");
    if (outfile == NULL) {
        return ERR_CREATING_OUTPUT_FILE;
    }
    saveSymtabOutput(outfile, symtab, false, true); /* entry: true, extern: false */
    fclose(outfile);

    memset(filename, 0, MAX_FILENAME_LENGTH);
    strcpy(filename, name);
    strcat(filename, ".ent");
    outfile = fopen(filename, "w");
    if (outfile == NULL) {
        return ERR_CREATING_OUTPUT_FILE;
    }
    saveSymtabOutput(outfile, symtab, true, false); /* entry: true, extern: false */
    fclose(outfile);

    return SUCCESS;
}
