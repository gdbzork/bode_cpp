#include <stdio.h>
#include <stdlib.h>
#include "histogram.h"

#define UNUSED(x) (void)(x)
#define ISRED(x) ((x)->isRed==1)
#define SETRED(x) ((x)->isRed = 1)
#define SETBLACK(x) ((x)->isRed = 0)

typedef struct redBlackNode {
  int key; /* node key value */
  char isRed; /* 1 for red, 0 for black */
  struct redBlackNode *left;
  struct redBlackNode *right;
  struct redBlackNode *parent;
  int count;
} *redBlackNodeP;

typedef struct redBlackTree {
  int count; /* nodes currently in tree */
  redBlackNodeP root;
} *redBlackTreeP;

histogram hist_new() {
  redBlackTreeP x = (redBlackTreeP) malloc(sizeof(struct redBlackTree));
  x->root = NULL;
  x->count = 0;
  return (histogram) x;
}

static redBlackNodeP node_new(int k) {
  redBlackNodeP x = (redBlackNodeP) malloc(sizeof(struct redBlackNode));
  x->key = k;
  x->isRed = 1;
  x->left = NULL;
  x->right = NULL;
  x->parent = NULL;
  x->count = 0;
  return x;
}

static redBlackNodeP insert(redBlackTreeP rbt,int k) {
  redBlackNodeP y = NULL;
  redBlackNodeP x = rbt->root;
  redBlackNodeP rbn = NULL;

  while (x != NULL && x->key != k) {
    y = x;
    if (k < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }
  if (x == NULL) {
    rbn = node_new(k);
    rbn->count++;
    rbn->parent = y;
    if (y == NULL) {
      rbt->root = rbn;
    } else if (rbn->key < y->key) {
      y->left = rbn;
    } else {
      y->right = rbn;
    }
  } else {
    x->count++;
  }
  return rbn;
}

static void leftRotate(redBlackTreeP rbt,redBlackNodeP rbn) {
  redBlackNodeP y = rbn->right;
  rbn->right = y->left;
  if (y->left != NULL) {
    y->left->parent = rbn;
  }
  y->parent = rbn->parent;
  if (rbn->parent == NULL) {
    rbt->root = y;
  } else if (rbn == rbn->parent->left) {
    rbn->parent->left = y;
  } else {
    rbn->parent->right = y;
  }
  y->left = rbn;
  rbn->parent = y;
}

static void rightRotate(redBlackTreeP rbt,redBlackNodeP rbn) {
  redBlackNodeP y = rbn->left;
  rbn->left = y->right;
  if (y->right != NULL) {
    y->right->parent = rbn;
  }
  y->parent = rbn->parent;
  if (rbn->parent == NULL) {
    rbt->root = y;
  } else if (rbn == rbn->parent->right) {
    rbn->parent->right = y;
  } else {
    rbn->parent->left = y;
  }
  y->right = rbn;
  rbn->parent = y;
}

void hist_increment(histogram hist,int key) {
  redBlackTreeP rbt = (redBlackTreeP) hist;
  redBlackNodeP x;
  x = insert(rbt,key);
  if (x != NULL) {
    while (x!=rbt->root && ISRED(x->parent)) {
      if (x->parent->parent->left == x->parent) {
        redBlackNodeP y = x->parent->parent->right;
        if (y != NULL && ISRED(y)) {
          SETBLACK(x->parent);
          SETBLACK(y);
          SETRED(x->parent->parent);
          x = x->parent->parent;
        } else {
          if (x == x->parent->right) {
            x = x->parent;
            leftRotate(rbt,x);
          }
          SETBLACK(x->parent);
          SETRED(x->parent->parent);
          rightRotate(rbt,x->parent->parent);
        }
      } else {
        redBlackNodeP y = x->parent->parent->left;
        if (y != NULL && ISRED(y)) {
          SETBLACK(x->parent);
          SETBLACK(y);
          SETRED(x->parent->parent);
          x = x->parent->parent;
        } else {
          if (x == x->parent->left) {
            x = x->parent;
            rightRotate(rbt,x);
          }
          SETBLACK(x->parent);
          SETRED(x->parent->parent);
          leftRotate(rbt,x->parent->parent);
        }
      }
    }
    SETBLACK(rbt->root);
    rbt->count++;
  }
}

static redBlackNodeP search(redBlackNodeP rbn,int key) {
  if (rbn == NULL || rbn->key == key) {
    return rbn;
  }
  if (key < rbn->key) {
    return search(rbn->left,key);
  } else {
    return search(rbn->right,key);
  }
}

int hist_contains(histogram hist,int key) {
  redBlackTreeP rbt = (redBlackTreeP) hist;
  redBlackNodeP x = search(rbt->root,key);
  return (x != NULL);
}

int hist_get(histogram hist,int key) {
  redBlackTreeP rbt = (redBlackTreeP) hist;
  redBlackNodeP x = search(rbt->root,key);
  int rv;
  if (x == NULL) {
    rv = 0;
  } else {
    rv = x->count;
  }
  return rv;
}

static void traverseInOrder(redBlackNodeP rbn,void (*visit)(int i,int c,void *data),void *data) {
  if (rbn == NULL) {
    return;
  } else {
    traverseInOrder(rbn->left,visit,data);
    visit(rbn->key,rbn->count,data);
    traverseInOrder(rbn->right,visit,data);
  }
}

void dumpNode(int i,int c,void *data) {
  printf("%d\t%d\n",i,c);
  UNUSED(data);
}

void hist_write(histogram hist) {
  redBlackTreeP rbt = (redBlackTreeP) hist;
  redBlackNodeP rbn = rbt->root;

  traverseInOrder(rbn,dumpNode,NULL);
}

void hist_traverse(histogram hist,void(*visit)(int i,int c,void *data),void *data) {
  redBlackTreeP rbt = (redBlackTreeP) hist;
  traverseInOrder(rbt->root,visit,data);
}
