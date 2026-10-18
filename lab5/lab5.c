#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
struct header {
  uint64_t size;
  struct header *next;
  int id;
};

void initialize_block(struct header *block, uint64_t size, struct header *next, int id) {
  block->size = size;
  block->next = next;
  block->id = id;
}

int find_first_fit(struct header *free_list_ptr, uint64_t size) {
  struct header *temp = free_list_ptr;
  while (temp != NULL) {
    if (temp->size >= size) {
      return temp->id;
    }
    temp = temp->next;
  }
  return -1;
}

int find_best_fit(struct header *free_list_ptr, uint64_t size) {
  int best_fit_id = -1;
  struct header *temp = free_list_ptr; // create temp for linked list traversal
  bool isFound = false;
  uint64_t currBestSize = UINT64_MAX;
  int currBestId = -1;
  while (temp != NULL) {
    if (size <= temp->size) {
      isFound = true;
      if (temp->size < currBestSize) {
        currBestId = temp->id;
        currBestSize = temp->size;
      }
    }
    temp = temp->next;
  }
  if (isFound) {
    best_fit_id = currBestId;
  }
  return best_fit_id;
}

int find_worst_fit(struct header *free_list_ptr, uint64_t size) {
  int worst_fit_id = -1;
  struct header *temp = free_list_ptr;
  int currWorstId = -1;
  uint64_t maxSize = 0;
  bool isFound = false;
  while (temp != NULL) {
    if (temp->size >= size) {
      isFound = true;
      if (temp->size > maxSize) {
        currWorstId = temp->id;
        maxSize = temp->size;
      }
    }
    temp = temp->next;
  }
  if (isFound) {
    worst_fit_id = currWorstId;
  }
  return worst_fit_id;
}

int main(void) {
  struct header *free_block1 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block2 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block3 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block4 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block5 = (struct header *)malloc(sizeof(struct header));
  initialize_block(free_block1, 6, free_block2, 1);
  initialize_block(free_block2, 12, free_block3, 2);
  initialize_block(free_block3, 24, free_block4, 3);
  initialize_block(free_block4, 8, free_block5, 4);
  initialize_block(free_block5, 4, NULL, 5);

  struct header *free_list_ptr = free_block1;

  int first_fit_id = find_first_fit(free_list_ptr, 7);
  int best_fit_id = find_best_fit(free_list_ptr, 7);
  int worst_fit_id = find_worst_fit(free_list_ptr, 7);

  printf("The ID for First-Fit algorithm:%d\n", first_fit_id);
  printf("The ID for the Best-Fit algorithm:%d\n", best_fit_id);
  printf("The ID for the Worst-Fit algorithm:%d\n", worst_fit_id);
  free(free_block1);
  free(free_block2);
  free(free_block3);
  free(free_block4);
  free(free_block5);
  return 0;
}

// pseudo code : coalescing a newley freed block z
// 1. start with newly freed block Z. browse the free list to find two neighbors
// a. the free block that exactly ends where Z starts
// b. the free block that exactly starts where Z ends
// 2. coalesce based on the result for 1.
// a. free block is only at before Z : Merge the before block and Z into one block
// b. free block is only at after Z: Merge the after block and Z into one block(this merged block
// will take the place the after block's place in the free list)
// c. free blocks are at both before, after Z: Merge the before, Z, after blocks into one block and
// remove the after block from the free list
// d. no free neighbor: just add Z to the free list.
