#ifndef TABLE_H
#define TABLE_H

#include <stdlib.h>
#include "pager.h"

#define PAGE_SIZE 4096

typedef struct
{
  Pager *pager;
  uint32_t root_page_num;
} Table;

Table *db_open(const char *filename);
void free_table(Table *table);
void db_close(Table *table);

#endif
