#include <stdio.h>
#include <string.h>
#include "meta_command.h"
#include "input_buffer.h"
#include "row.h"
#include "b_tree.h"

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table)
{
  if (strcmp(input_buffer->buffer, ".exit") == 0)
  {
    db_close(table);
    close_input_buffer(input_buffer);
    exit(EXIT_SUCCESS);
  }
  else if (strcmp(input_buffer->buffer, ".btree") == 0)
  {
    printf("Tree:\n");
    print_tree(table->pager, table->root_page_num, 0);
    return META_COMMAND_SUCCESS;
  }
  else if (strcmp(input_buffer->buffer, ".constants") == 0)
  {
    printf("Constants:\n");
    print_constants();
    return META_COMMAND_SUCCESS;
  }
  else
  {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

void print_constants()
{
  printf("ROW_SIZE: %lu\n", ROW_SIZE);
  printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
  printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
  printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
  printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
  printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

void indent(uint32_t level)
{
  for (uint32_t i = 0; i < level; i++)
  {
    printf("  ");
  }
}

void print_tree(Pager *pager, uint32_t page_num, uint32_t indent_level)
{
  void *node = get_page(pager, page_num);
  uint32_t num_keys, child;

  switch (get_node_type(node))
  {
  case NODE_INTERNAL:
    num_keys = *internal_node_num_keys(node);
    indent(indent_level);
    printf("- internal (size %d)\n", num_keys);
    for (uint32_t i = 0; i < num_keys; i++)
    {
      child = *internal_node_child(node, i);
      print_tree(pager, child, indent_level + 1);

      indent(indent_level + 1);
      printf("- key %d\n", *internal_node_key(node, i));
    }
    child = *internal_node_right_child(node);
    print_tree(pager, child, indent_level + 1);
    break;
  case (NODE_LEAF):
    num_keys = *leaf_node_num_cells(node);
    indent(indent_level);
    printf("- leaf (size %d)\n", num_keys);
    for (uint32_t i = 0; i < num_keys; i++)
    {
      indent(indent_level + 1);
      printf("- %d\n", *leaf_node_key(node, i));
    }
    break;

  default:
    break;
  }
}
