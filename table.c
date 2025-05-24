#include "table.h"
#include "row.h"
#include "pager.h"
#include "b_tree.h"
#include <fcntl.h>     // for open(), O_RDONLY, O_RDWR, etc.
#include <sys/types.h> // for types like off_t
#include <errno.h>     // for errno if checking errors
#include <stdio.h>     // for printf() and perror()
#include <stdlib.h>    // for exit()
#include <unistd.h>
#include <stdbool.h>

Table *db_open(const char *filename)
{
  Pager *pager = pager_open(filename);
  uint32_t num_rows = pager->file_length / ROW_SIZE;

  Table *table = (Table *)malloc(sizeof(Table));
  table->pager = pager;
  table->root_page_num = 0;

  if (pager->num_pages == 0)
  {
    // Database is empty. Initialze page 0 as the leaf node.
    void *root_node = get_page(pager, 0);
    initialize_leaf_node(root_node);
    set_node_root(root_node, true);
  }

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

  for (uint32_t i = 0; i < pager->num_pages; i++)
  {
    if (pager->pages[i] == NULL)
    {
      continue;
    }

    pager_flush(pager, i);
    free(pager->pages[i]);
    pager->pages[i] = NULL;
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
