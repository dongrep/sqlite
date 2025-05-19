#ifndef INPUT_BUFFER_H
#define INPUT_BUFFER_H

#include <stdlib.h>
#include <sys/_types/_ssize_t.h>

typedef struct
{
  char *buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

InputBuffer *new_input_buffer();
void print_prompt();
void read_input(InputBuffer *input_buffer);
void close_input_buffer(InputBuffer *input_buffer);

#endif
