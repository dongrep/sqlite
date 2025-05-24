#include "b_tree.h"
#include <stdio.h>
#include <string.h>

uint32_t *leaf_node_num_cells(void *node)
{
  return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

void *leaf_node_cell(void *node, uint32_t cell_num)
{
  return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t *leaf_node_key(void *node, uint32_t cell_num)
{
  return leaf_node_cell(node, cell_num);
}

void *leaf_node_value(void *node, uint32_t cell_num)
{
  return leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

void initialize_leaf_node(void *node)
{
  set_node_type(node, NODE_LEAF);
  *leaf_node_num_cells(node) = 0;
}

void leaf_node_insert(Cursor *cursor, uint32_t *key, Row *value)
{
  void *node = get_page(cursor->table->pager, cursor->page_num);
  uint32_t num_cells = *leaf_node_num_cells(node);
  if (num_cells >= LEAF_NODE_MAXIMUM_CELLS)
  {
    // Node full
    printf("Need to implement splitting a leaf node.\n");
    exit(EXIT_FAILURE);
  }

  if (cursor->cell_num < num_cells)
  {
    for (uint32_t i = num_cells; i > cursor->cell_num; i--)
    {
      memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1), LEAF_NODE_CELL_SIZE);
    }
  }

  *(leaf_node_num_cells(node)) += 1;
  *(uint32_t *)leaf_node_key(node, cursor->cell_num) = *key;
  serialze_row(value, leaf_node_value(node, cursor->cell_num));
}

Cursor *leaf_node_find(Table *table, uint32_t page_num, uint32_t key_to_insert)
{
  void *node = get_page(table->pager, page_num);
  uint32_t num_cells = *leaf_node_num_cells(node);

  Cursor *cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = page_num;

  // Binary search
  uint32_t min_index = 0;
  uint32_t one_past_max_index = num_cells;

  while (min_index != one_past_max_index)
  {
    uint32_t mid_index = min_index + (one_past_max_index - min_index) / 2;
    uint32_t key_at_mid = *(leaf_node_key(node, mid_index));
    if (key_to_insert == key_at_mid)
    {
      cursor->cell_num = mid_index;
      return cursor;
    }
    else if (key_to_insert > key_at_mid)
    {
      min_index = mid_index + 1;
    }
    else
    {
      one_past_max_index = mid_index;
    }
  }

  cursor->cell_num = min_index;
  return cursor;
}

NodeType get_node_type(void *node)
{
  uint8_t value = *((uint8_t *)node + NODE_TYPE_OFFSET);
  return (NodeType)value;
}

void set_node_type(void *node, NodeType nodeType)
{
  uint8_t value = nodeType;
  *((uint8_t *)node + NODE_TYPE_OFFSET) = value;
}
