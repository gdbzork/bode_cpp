#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

queue q_create(int spare) {
  int i;
  struct queue *node;
  struct elem *item;
  node = (struct queue *) malloc(sizeof(struct queue));
  node->count = 0;
  node->head = NULL;
  node->tail = NULL;
  node->spare = NULL;
  for (i=0;i<spare;i++) {
    item = (struct elem *) malloc(sizeof(struct elem));
    item->next = node->spare;
    node->spare = item;
  }
  return node;
}

int q_size(queue q) {
  return q->count;
}

int q_add(queue q,void *value) {
  struct elem *item;
  if (q->spare != NULL) {
    item = q->spare;
    q->spare = item->next;
  } else {
    item = (struct elem *) malloc(sizeof(struct elem));
    if (item == NULL) {
      return 1;
    }
  }
  item->item = (void *) value;
  item->next = NULL;
  if (q->tail != NULL) {
    q->tail->next = item;
  } else {
    q->head = item;
  }
  q->tail = item;
  q->count++;
  return 0;
}

void *q_get(queue q) {
  if (q->head != NULL) {
    return q->head->item;
  } else {
    return NULL;
  }
}

void q_remove(queue q) {
  struct elem *item;
  if (q->head != NULL) {
    item = q->head;
    q->head = item->next;
    item->next = q->spare;
    q->spare = item;
    if (q->head == NULL) {
      q->tail = NULL;
    }
    q->count--;
  }
}
