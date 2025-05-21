#include "table.h"
#include "row.h"
#include "pager.h"
#include <fcntl.h>     // for open(), O_RDONLY, O_RDWR, etc.
#include <sys/types.h> // for types like off_t
#include <errno.h>     // for errno if checking errors
#include <stdio.h>     // for printf() and perror()
#include <stdlib.h>    // for exit()
#include <unistd.h>

Table *db_open(const char *filename)
{
  Pager *pager = open_pager(filename);
  uint32_t num_rows = pager->file_length / ROW_SIZE;

  Table *table = (Table *)malloc(sizeof(Table));
  table->pager = pager;
  table->num_rows = num_rows;

  return table;
}

void free_table(Table *table)
{
  for (uint32_t i = 0; table->pager->pages[i]; i++)
  {
    free(table->pager->pages[i]);
  }
  free(table);
}

void db_close(Table *table)
{
  Pager *pager = table->pager;
  uint32_t num_full_pages = table->num_rows / ROW_SIZE;

  for (uint32_t i = 0; i < num_full_pages; i++)
  {
    if (pager->pages[i] == NULL)
    {
      continue;
    }

    pager_flush(pager, i, PAGE_SIZE);
    free(pager->pages[i]);
    pager->pages[i] = NULL;
  }

  // There maybe a partial page remaining at the end of the file
  // This won't be needed when we switch to B-tree
  uint32_t num_of_remaining_rows = table->num_rows % ROW_SIZE;
  if (num_of_remaining_rows > 0)
  {
    uint32_t page_num = num_full_pages;
    if (pager->pages[page_num] != NULL)
    {
      pager_flush(pager, page_num, num_of_remaining_rows * ROW_SIZE);
      free(pager->pages[page_num]);
      pager->pages[page_num] = NULL;
    }
  }

  int result = close(pager->file_descriptor);
  if (result == -1)
  {
    printf("Error closing db file.\n");
    exit(EXIT_FAILURE);
  }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
  {
    void *page = pager->pages[i];
    if (page)
    {
      free(page);
      pager->pages[i] = NULL;
    }
  }

  free(pager);
  free(table);
}
