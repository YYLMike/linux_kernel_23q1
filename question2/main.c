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
#define MAX_DEPTH 512
static void list_sort_nr(struct list_head *head) {
  if (list_empty(head) || list_is_singular(head))
    return;

  struct list_head stack[MAX_DEPTH];
  for (int i = 0; i < MAX_DEPTH; i++)
    INIT_LIST_HEAD(&stack[i]);
  int top = -1;
  list_splice_init(head, &stack[++top]);

  struct list_head partition;
  INIT_LIST_HEAD(&partition);

  while (top >= 0) {
    INIT_LIST_HEAD(&partition);
    list_splice_init(&stack[top--], &partition);
    if (!list_empty(&partition) && !list_is_singular(&partition)) {
      struct list_head list_less, list_greater;
      INIT_LIST_HEAD(&list_less);
      INIT_LIST_HEAD(&list_greater);
      struct item *pivot = list_first_entry(&partition, struct item, list);
      list_del(&pivot->list);
      INIT_LIST_HEAD(&pivot->list);

      struct item *itm = NULL, *is = NULL;
      list_for_each_entry_safe(itm, is, &partition, list) {// GGGG
        list_del(&itm->list);
        if (cmpint(&itm->i, &pivot->i) < 0)
          list_move(&itm->list, &list_less);
        else
          list_move(&itm->list, &list_greater);
      }

      list_add_tail(&pivot->list, &list_less); // HHHH
      if (!list_empty(&list_greater))
        list_splice_tail(&list_greater, &stack[++top]); // IIII
      if (!list_empty(&list_less))
        list_splice_tail(&list_less, &stack[++top]); // JJJJ
    } else {
      top++;
      list_splice_tail(&partition, &stack[top]);
      while (top >= 0 && list_is_singular(&stack[top])) {
        struct item *tmp = list_first_entry(&stack[top], struct item, list);
        list_del(&tmp->list);
        INIT_LIST_HEAD(&stack[top--]); // KKKK
        list_add_tail(&tmp->list, head);
      }
    }
  }
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
    // items[i].i = i;
    list_add(&items[i].list, &item_head.list);
  }

  report_list(&item_head.list);
  list_sort_nr(&item_head.list);
  report_list(&item_head.list);

  return 0;
}
