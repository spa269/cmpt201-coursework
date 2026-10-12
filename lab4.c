#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BLOCK_SIZE 128
#define BUF_SIZE 128
void handle_error(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

struct header {
  uint64_t size;       // 8 byte
  struct header *next; // 8 byte
};

void print_out(char *format, void *data, size_t data_size) {
  char buf[128];
  ssize_t len = snprintf(buf, BUF_SIZE, format,
                         data_size == sizeof(uint64_t) ? *(uint64_t *)data : *(void **)data);
  if (len < 0) {
    handle_error("snprintf");
  }
  write(STDOUT_FILENO, buf, len);
}

int main(void) {
  // make heap 256
  void *heap_start = sbrk(256);
  if (heap_start == (void *)-1) {
    handle_error("sbrk error");
  }

  // divide one 256 byte block to two 128 byte block
  //  heap_Start is void --> so have to change it to struct header!
  //  need a block in heap --> amek it as a ptr
  struct header *first_block = (struct header *)heap_start;
  struct header *second_block = (struct header *)((char *)heap_start + 128);

  // initialize header
  first_block->size = 128;
  first_block->next = NULL;
  second_block->size = 128;
  second_block->next = first_block;

  // initialize first, second block
  size_t data_size = BLOCK_SIZE - sizeof(struct header); // have to keep the space for size,
                                                         // next(16byte)
  unsigned char *first_data = (unsigned char *)first_block + sizeof(struct header);
  unsigned char *second_data = (unsigned char *)second_block + sizeof(struct header);
  memset(first_data, 0, data_size);
  memset(second_data, 1, data_size);

  // print out the result
  print_out("first block:     %p\n", &first_block, sizeof(first_block));
  print_out("second block:    %p\n", &second_block, sizeof(second_block));
  print_out("first block size:     %lu\n", &first_block->size, sizeof(first_block->size));
  print_out("first block next:     %p\n", &first_block->next, sizeof(first_block->next));
  print_out("second block size:     %lu\n", &second_block->size, sizeof(second_block->size));
  print_out("second block next:      %p\n", &second_block->next, sizeof(second_block->next));

  // print each data byte
  for (size_t i = 0; i < data_size; i++) {
    uint64_t value = first_data[i];
    print_out("%lu\n", &value, sizeof(value));
  }

  for (size_t i = 0; i < data_size; i++) {
    uint64_t value = second_data[i];
    print_out("%lu\n", &value, sizeof(value));
  }

  return 0;
}
