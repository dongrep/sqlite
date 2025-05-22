#include "cursor.h"
#include "row.h"
#include "b_tree.h"
#include <stdbool.h>

/**
 * Creates a cursor pointing to the start of the table.
 *
 * @param table A pointer to the Table structure representing the table.
 * @return A pointer to the Cursor structure pointing to the start of the table.
 */
Cursor *table_start(Table *table)
{
  // Allocate memory for a new Cursor structure
  Cursor *cursor = malloc(sizeof(Cursor));

  // Initialize the cursor to point to the table's root page
  cursor->table = table;
  cursor->page_num = table->root_page_num;
  cursor->cell_num = 0;

  // Retrieve the root node of the table
  void *root_node = get_page(table->pager, table->root_page_num);

  // Get the number of cells in the root node
  uint32_t num_cells = *leaf_node_num_cells(root_node);

  // If the root node has no cells, mark the cursor as at the end of the table
  cursor->end_of_table = (num_cells == 0);

  return cursor;
}

/**
 * Creates a cursor pointing to the end of the table.
 *
 * @param table A pointer to the Table structure representing the table.
 * @return A pointer to the Cursor structure pointing to the end of the table.
 */
Cursor *table_end(Table *table)
{
  // Allocate memory for a new Cursor structure
  Cursor *cursor = malloc(sizeof(Cursor));

  // Initialize the cursor to point to the table's root page
  cursor->table = table;
  cursor->page_num = table->root_page_num;

  // Retrieve the root node of the table
  void *root_node = get_page(table->pager, table->root_page_num);

  // Get the number of cells in the root node
  uint32_t num_cells = *leaf_node_num_cells(root_node);

  // Set the cursor's cell number to the number of cells in the root node
  cursor->cell_num = num_cells;

  // Mark the cursor as at the end of the table
  cursor->end_of_table = true;

  return cursor;
}

/**
 * Retrieves a pointer to the memory location of the current row in the table.
 *
 * @param cursor A pointer to the Cursor structure pointing to the current row.
 * @return A void pointer to the memory location of the current row.
 */
void *cursor_value(Cursor *cursor)
{
  uint32_t page_num = cursor->page_num;
  void *page = get_page(cursor->table->pager, page_num);
  return leaf_node_value(page, cursor->cell_num);
}

void cursor_advance(Cursor *cursor)
{
  uint32_t page_num = cursor->page_num;
  void *page = get_page(cursor->table->pager, page_num);

  cursor->cell_num += 1;
  if (cursor->cell_num >= (*leaf_node_num_cells(page)))
  {
    cursor->end_of_table = true;
  }
}
