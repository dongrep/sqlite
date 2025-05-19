#include "table.h"
#include "row.h"

Table *new_table()
{
  Table *table = (Table *)malloc(sizeof(Table));
  table->num_rows = 0;
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
  {
    table->pages[i] = NULL;
  }
  return table;
}

void free_table(Table *table)
{
  for (uint32_t i = 0; table->pages[i]; i++)
  {
    free(table->pages[i]);
  }
  free(table);
}

/**
 * Retrieves a pointer to the memory location of a specific row in the table.
 *
 * @param table A pointer to the Table structure containing the rows.
 * @param row_num The index of the row to retrieve (0-based).
 * @return A void pointer to the memory location of the specified row.
 */
void *row_slot(Table *table, uint32_t row_num)
{
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  void *page = table->pages[page_num];
  if (page == NULL)
  {
    // Only assign memory when we try accessing the page
    page = table->pages[page_num] = malloc(PAGE_SIZE);
  }
  uint32_t row_offset = row_num % ROWS_PER_PAGE;
  uint32_t bytes_offset = row_offset * ROW_SIZE;
  return page + bytes_offset;
}
