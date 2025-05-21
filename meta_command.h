#include "input_buffer.h"
#include "table.h"
#ifndef META_COMMAND_H
#define META_COMMAND_H

typedef enum
{
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND,
} MetaCommandResult;

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table);

#endif
