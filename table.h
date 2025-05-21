#ifndef TABLE_H
#define TABLE_H

#include <stdlib.h>
#include "pager.h"

#define PAGE_SIZE 4096
#define ROWS_PER_PAGE (PAGE_SIZE / ROW_SIZE)
#define TABLE_MAX_ROWS (ROWS_PER_PAGE * TABLE_MAX_PAGES)

typedef struct
{
  uint32_t num_rows;
  Pager *pager;
} Table;

Table *db_open(const char *filename);
void free_table(Table *table);
void db_close(Table *table);

#endif
