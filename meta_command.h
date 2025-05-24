#ifndef META_COMMAND_H
#define META_COMMAND_H

#include "input_buffer.h"
#include "table.h"

typedef enum
{
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND,
} MetaCommandResult;

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table);
void print_tree(Pager *pager, uint32_t page_num, uint32_t indent_level);
void print_constants();

#endif
