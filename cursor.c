#include "cursor.h"
#include "row.h"
#include "b_tree.h"
#include <stdbool.h>
#include <stdio.h>

Cursor *table_start(Table *table)
{
  Cursor *cursor = table_find(table, 0);
  void *node = get_page(table->pager, cursor->page_num);

  // Get the number of cells in the root node
  uint32_t num_cells = *leaf_node_num_cells(node);
  // If the root node has no cells, mark the cursor as at the end of the table
  cursor->end_of_table = (num_cells == 0);

  return cursor;
}

/*
+Return the position of the given key.
+If the key is not present, return the position
+where it should be inserted
+*/
Cursor *table_find(Table *table, uint32_t key_to_insert)
{
  uint32_t root_page_num = table->root_page_num;
  void *root_node = get_page(table->pager, root_page_num);

  if (get_node_type(root_node) == NODE_LEAF)
  {
    return leaf_node_find(table, root_page_num, key_to_insert);
  }
  else
  {
    return internal_node_find(table, root_page_num, key_to_insert);
  }
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
  void *node = get_page(cursor->table->pager, page_num);

  cursor->cell_num += 1;
  if (cursor->cell_num >= (*leaf_node_num_cells(node)))
  {
    // Advance to the next leaf
    uint16_t next_page_num = *leaf_node_next_leaf(node);
    if (next_page_num == 0)
    {
      cursor->end_of_table = true;
    }
    else
    {
      cursor->page_num = next_page_num;
      cursor->cell_num = 0;
    }
  }
}
