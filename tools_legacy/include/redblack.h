#ifndef REDBLACK_H
#define REDBLACK_H

typedef struct redBlackNode {
  int key; /* node key value */
  char isRed; /* 1 for red, 0 for black */
  struct redBlackNode *left;
  struct redBlackNode *right;
  struct redBlackNode *parent;
  void *data;
} *redBlackNodeP;

typedef struct redBlackTree {
  int count; /* nodes currently in tree */
  redBlackNodeP root;
} *redBlackTreeP;

redBlackTreeP rb_new();
void rb_insert(redBlackTreeP rbt,int key,void *data);
int rb_count(redBlackTreeP rbt);
void *rb_search(redBlackTreeP rbt,int key);
void rb_traverse(redBlackTreeP rbt,void(*visit)(int key,void *value,void *data),void *data,int reverse);

int rb_isNull(redBlackNodeP rbn);

#endif
