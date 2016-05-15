#ifndef REDBLACKSN_H
#define REDBLACKSN_H

typedef struct rbsnNode {
  char *key; /* node key value */
  char isRed; /* 1 for red, 0 for black */
  struct rbsnNode *left;
  struct rbsnNode *right;
  struct rbsnNode *parent;
  int data;
} *rbsnNode;

typedef struct rbsnTree {
  int count; /* nodes currently in tree */
  rbsnNode root;
} *rbsnTree;

rbsnTree rbsn_new();
void rbsn_insert(rbsnTree rbt,char *key,int);
int rbsn_count(rbsnTree rbt);
int rbsn_search(rbsnTree rbt,char *key);
void rbsn_update(rbsnTree rbt,char *key,int value);
void rbsn_traverse(rbsnTree rbt,void(*visit)(char *key,int value,void *data),void *data,int reverse);

int rbsn_isNull(rbsnNode rbn);

#endif
