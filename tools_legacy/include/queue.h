#ifndef QUEUE_H
#define QUEUE_H

struct elem {
  void *item;
  struct elem *next;
};

typedef struct queue {
  struct elem *head;
  struct elem *tail;
  struct elem *spare;
  int count;
} *queue;

queue q_create(int spare);
int q_add(queue q,void *value);
int q_size(queue q);
void *q_get(queue q);
void q_remove(queue q);

#endif
