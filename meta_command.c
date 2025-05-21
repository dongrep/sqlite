#include <string.h>
#include "meta_command.h"
#include "input_buffer.h"

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table)
{
  if (strcmp(input_buffer->buffer, ".exit") == 0)
  {
    db_close(table);
    close_input_buffer(input_buffer);
    exit(EXIT_SUCCESS);
  }
  else
  {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}
