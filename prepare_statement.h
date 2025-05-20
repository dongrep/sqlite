#ifndef PREPARE_STATEMENT_H
#define PREPARE_STATEMENT_H

#include "input_buffer.h"
#include "statement.h"

typedef enum
{
  PREPARE_SUCCESS,
  PREPARE_SYNTAX_ERROR,
  PREPARE_STRING_TO_LONG,
  PREPARE_NEGATIVE_ID,
  PREPARE_UNRECOGNIZED_STATEMENT,
} PrepareResult;

PrepareResult prepare_insert(InputBuffer *input_buffer, Statement *statement);
PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement);

#endif
