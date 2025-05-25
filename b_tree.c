#include "b_tree.h"
#include "meta_command.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

uint32_t *leaf_node_num_cells(void *node)
{
  return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

uint32_t *leaf_node_next_leaf(void *node)
{
  return node + LEAF_NODE_NEXT_LEAF_OFFSET;
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
  set_node_root(node, false);
  *leaf_node_num_cells(node) = 0;
  *leaf_node_next_leaf(node) = 0; // 0 represents no sibling
}

void initialize_internal_node(void *node)
{
  set_node_type(node, NODE_INTERNAL);
  set_node_root(node, false);
  *internal_node_num_keys(node) = 0;
}

Cursor *internal_node_find(Table *table, uint32_t page_num, uint32_t key)
{
  void *node = get_page(table->pager, page_num);
  uint32_t num_keys = *internal_node_num_keys(node);

  uint32_t min_index = 0;
  uint32_t max_index = num_keys; // There's one more child than keys

  while (min_index != max_index)
  {
    uint32_t index = min_index + (max_index - min_index) / 2;
    uint32_t key_to_right = *internal_node_key(node, index);
    if (key > key_to_right)
    {
      min_index = index + 1;
    }
    else
    {
      max_index = index;
    }
  }

  uint32_t child_num = *internal_node_child(node, min_index);
  void *child = get_page(table->pager, child_num);
  switch (get_node_type(child))
  {
  case NODE_INTERNAL:
    return internal_node_find(table, child_num, key);
    break;
  case NODE_LEAF:
    return leaf_node_find(table, child_num, key);
  }
}

void leaf_node_insert(Cursor *cursor, uint32_t *key, Row *value)
{
  void *node = get_page(cursor->table->pager, cursor->page_num);
  uint32_t num_cells = *leaf_node_num_cells(node);
  if (num_cells >= LEAF_NODE_MAX_CELLS)
  {
    leaf_node_split_and_insert(cursor, *key, value);
    return;
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
  serialize_row(value, leaf_node_value(node, cursor->cell_num));
}

void leaf_node_split_and_insert(Cursor *cursor, uint32_t key, Row *value)
{
  /*
+  Create a new node and move half the cells over.
+  Insert the new value in one of the two nodes.
+  Update parent or create a new parent.
+  */

  void *old_node = get_page(cursor->table->pager, cursor->page_num);
  uint32_t new_page_num = get_unused_page_num(cursor->table->pager);
  void *new_node = get_page(cursor->table->pager, new_page_num);
  initialize_leaf_node(new_node);
  *leaf_node_next_leaf(new_node) = *leaf_node_next_leaf(old_node);
  *leaf_node_next_leaf(old_node) = new_page_num;

  /*
+  All existing keys plus new key should be divided
+  evenly between old (left) and new (right) nodes.
+  Starting from the right, move each key to correct position.
+  */
  for (int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--)
  {
    void *destination_node;
    if (i >= LEAF_NODE_LEFT_SPLIT_COUNT)
    {
      destination_node = new_node;
    }
    else
    {
      destination_node = old_node;
    }

    uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
    void *destination;
    if (i < LEAF_NODE_LEFT_SPLIT_COUNT)
    {
      // Left split node
      destination_node = old_node;
      index_within_node = i;
    }
    else
    {
      // Right split node
      destination_node = new_node;
      index_within_node = i - LEAF_NODE_LEFT_SPLIT_COUNT;
    }
    destination = leaf_node_cell(destination_node, index_within_node);

    if (i == cursor->cell_num)
    {
      serialize_row(value,
                    leaf_node_value(destination_node, index_within_node));
      *leaf_node_key(destination_node, index_within_node) = key;
    }
    else if (i > cursor->cell_num)
    {
      memcpy(destination, leaf_node_cell(old_node, i - 1), LEAF_NODE_CELL_SIZE);
    }
    else
    {
      memcpy(destination, leaf_node_cell(old_node, i), LEAF_NODE_CELL_SIZE);
    }
  }

  *(leaf_node_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
  *(leaf_node_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

  if (is_node_root(old_node))
  {
    return create_new_root(cursor->table, new_page_num);
  }
  else
  {
    printf("Need to implement updating parent after split.\n");
    exit(EXIT_FAILURE);
  }
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

void create_new_root(Table *table, uint32_t right_child_page_num)
{
  /*
    Handle splitting of root
    Old root copied to new page, becomes left child
    Address of right child passed in.
    Re-initialize root page to contain the new root node.
    New root node points to two children.
  */

  void *root = get_page(table->pager, table->root_page_num);
  void *right_child = get_page(table->pager, right_child_page_num);
  uint32_t left_child_page_num = get_unused_page_num(table->pager);
  void *left_child = get_page(table->pager, left_child_page_num);

  // Left child has data copied from the root
  memcpy(left_child, root, PAGE_SIZE);
  set_node_root(left_child, false);

  // Root node is internal node with one key and two children
  initialize_internal_node(root);
  set_node_root(root, true);

  *internal_node_num_keys(root) = 1;
  *internal_node_child(root, 0) = left_child_page_num;
  uint32_t left_child_max_key = get_node_max_key(left_child);
  *internal_node_key(root, 0) = left_child_max_key;
  *internal_node_right_child(root) = right_child_page_num;
}

uint32_t *internal_node_num_keys(void *node)
{
  return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}

uint32_t *internal_node_right_child(void *node)
{
  return node + INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}

uint32_t *internal_node_cell(void *node, uint32_t cell_num)
{
  return node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE;
}

uint32_t *internal_node_child(void *node, uint32_t child_num)
{
  uint32_t num_keys = *internal_node_num_keys(node);
  if (child_num > num_keys)
  {
    printf("Tried to access child %d > num_keys %d.\n", child_num, num_keys);
    exit(EXIT_FAILURE);
  }
  else if (child_num == num_keys)
  {
    return internal_node_right_child(node);
  }
  else
  {
    return internal_node_cell(node, child_num);
  }
}

uint32_t *internal_node_key(void *node, uint32_t key_num)
{
  return internal_node_cell(node, key_num) + INTERNAL_NODE_CHILD_SIZE;
}

uint32_t get_node_max_key(void *node)
{
  switch (get_node_type(node))
  {
  case NODE_INTERNAL:
    return *internal_node_key(node, *(internal_node_num_keys(node) - 1));
  case NODE_LEAF:
    return *leaf_node_key(node, *(leaf_node_num_cells(node)) - 1);
  }
}

bool is_node_root(void *node)
{
  uint8_t is_root = *(uint8_t *)(node + IS_ROOT_OFFSET);
  return (bool)is_root;
};

void set_node_root(void *node, bool is_root)
{
  *(uint8_t *)(node + IS_ROOT_OFFSET) = (uint8_t)is_root;
}
