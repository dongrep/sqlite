#include "pager.h"
#include "table.h"
#include <fcntl.h>     // for open(), O_RDONLY, O_RDWR, etc.
#include <sys/types.h> // for types like off_t
#include <errno.h>     // for errno if checking errors
#include <stdio.h>     // for printf() and perror()
#include <stdlib.h>    // for exit()
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

Pager *pager_open(const char *filename)
{
  int fd = open(filename,
                O_RDWR |     // Read/Write mode
                    O_CREAT, // Create file if it does not exist
                S_IWUSR |    // User write permission
                    S_IRUSR  // User read permission
  );

  if (fd == -1)
  {
    printf("Unable to open file.\n");
    exit(EXIT_FAILURE);
  }

  off_t file_length = lseek(fd, 0, SEEK_END);

  Pager *pager = malloc(sizeof(Pager));
  pager->file_descriptor = fd;
  pager->file_length = file_length;
  pager->num_pages = (file_length / PAGE_SIZE);

  if (file_length % PAGE_SIZE != 0)
  {
    printf("DB file is not a whole number of pages. Corrupt file.\n");
    exit(EXIT_FAILURE);
  }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
  {
    pager->pages[i] = NULL;
  }

  return pager;
}

void *get_page(Pager *pager, uint32_t page_num)
{
  if (page_num > TABLE_MAX_PAGES)
  {
    printf("Tried to fetch page number %d out of bounds (%d).\n", page_num, TABLE_MAX_PAGES);
    exit(EXIT_FAILURE);
  }

  if (pager->pages[page_num] == NULL)
  {
    // Cache miss, allocate memory and load from file
    void *page = malloc(PAGE_SIZE);
    memset(page, 0, PAGE_SIZE);
    uint32_t num_pages = pager->file_length / PAGE_SIZE;

    // We might have some partial data at the end of a page
    if (pager->file_length % PAGE_SIZE)
    {
      num_pages += 1;
    }

    if (num_pages >= page_num)
    {
      lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
      ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
      if (bytes_read == -1)
      {
        printf("Error reading file %d.\n", errno);
        exit(EXIT_FAILURE);
      }
    }

    if (page_num >= pager->num_pages)
    {
      pager->num_pages = page_num + 1;
    }

    pager->pages[page_num] = page;
  }

  return pager->pages[page_num];
}

void pager_flush(Pager *pager, uint32_t page_num)
{
  if (pager->pages[page_num] == NULL)
  {
    printf("Tried to flush NULL page.\n");
    exit(EXIT_FAILURE);
  }

  off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
  if (offset == -1)
  {
    printf("Error seeking: %d.\n", errno);
    exit(EXIT_FAILURE);
  }

  ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);
  if (bytes_written == -1)
  {
    printf("Error writing: %d.\n", errno);
    exit(EXIT_FAILURE);
  }
}
uint32_t get_unused_page_num(Pager *pager)
{
  return pager->num_pages;
}
