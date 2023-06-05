#include "list.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
struct item {
  uint16_t i;
  struct list_head list;
};

static inline int cmpint(const void *p1, const void *p2) {
  const uint16_t *i1 = (const uint16_t *)p1;
  const uint16_t *i2 = (const uint16_t *)p2;
  return *i1 - *i2;
}

static void list_sort(struct list_head *head) {
  if (list_empty(head) || list_is_singular(head))
    return;

  struct list_head list_less, list_greater;
  // circular doubly linked list
  INIT_LIST_HEAD(&list_less);
  INIT_LIST_HEAD(&list_greater);

  struct item *pivot = list_first_entry(head, struct item, list);
  list_del(&pivot->list);

  struct item *itm = NULL, *is = NULL;
  list_for_each_entry_safe(itm, is, head, list) {
    if (cmpint(&itm->i, &pivot->i) < 0)
      list_move(&itm->list, &list_less);
    else
      list_move(&itm->list, &list_greater);
  }

  list_sort(&list_less);
  list_sort(&list_greater);

  list_add(&pivot->list, head);
  list_splice(&list_less, head);
  list_splice_tail(&list_greater, head);
}

void report_list(struct list_head *head) {
  struct item *itm = NULL, *is = NULL;
  printf("value: ");
  list_for_each_entry_safe(itm, is, head, list) { printf("%d ", itm->i); }
  printf("\n");
}

int get_rand_int(int min, int max) {
  int x = rand() % (max - min + 1) + min;
  return x;
}

int main() {
  srand(time(NULL));
  struct item item_head;
  INIT_LIST_HEAD(&item_head.list);
  struct item items[10];
  for (int i = 0; i < 10; ++i) {
    items[i].i = get_rand_int(0, 100);
    list_add(&items[i].list, &item_head.list);
  }

  report_list(&item_head.list);
  list_sort(&item_head.list);
  report_list(&item_head.list);

  return 0;
}
