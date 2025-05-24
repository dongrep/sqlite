#ifndef STATEMENT_H
#define STATEMENT_H

#include "row.h"
#include "table.h"

typedef enum
{
  EXECUTE_SUCCESS,
  EXECUTE_TABLE_FULL,
  EXECUTE_DUPLICATE_KEY
} ExecuteResult;

typedef enum
{
  STATEMENT_INSERT,
  STATEMENT_SELECT,
} StatementType;

typedef struct
{
  StatementType type;
  Row row_to_insert;
} Statement;

ExecuteResult execute_insert(Statement *statement, Table *table);
ExecuteResult execute_select(Statement *statement, Table *table);
ExecuteResult execute_statement(Statement *statement, Table *Table);

#endif
