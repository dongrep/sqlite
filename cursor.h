#ifndef CURSOR_H
#define CURSOR_H

#include <stdint.h>
#include <stdbool.h>
#include "table.h"

typedef struct
{
  Table *table;
  uint32_t page_num;
  uint32_t cell_num;
  bool end_of_table; // Idicates a pos one past the last element
} Cursor;

Cursor *table_start(Table *table);
Cursor *table_find(Table *table, uint32_t key_to_insert);
void *cursor_value(Cursor *cursor);
void cursor_advance(Cursor *cursor);

#endif
