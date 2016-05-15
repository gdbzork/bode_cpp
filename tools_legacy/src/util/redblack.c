#include <stdio.h>
#include <stdlib.h>
#include "redblack.h"

#define ISRED(x) ((x)->isRed==1)
#define SETRED(x) ((x)->isRed = 1)
#define SETBLACK(x) ((x)->isRed = 0)

redBlackTreeP rb_new() {
  redBlackTreeP x = (redBlackTreeP) malloc(sizeof(struct redBlackTree));
  x->root = NULL;
  x->count = 0;
  return x;
}

static redBlackNodeP node_new(int k,void *data) {
  redBlackNodeP x = (redBlackNodeP) malloc(sizeof(struct redBlackNode));
  x->key = k;
  x->isRed = 1;
  x->left = NULL;
  x->right = NULL;
  x->parent = NULL;
  x->data = data;
  return x;
}

static void insert(redBlackTreeP rbt,redBlackNodeP rbn) {
  redBlackNodeP y = NULL;
  redBlackNodeP x = rbt->root;
  while (x != NULL) {
    y = x;
    if (rbn->key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }
  rbn->parent = y;
  if (y == NULL) {
    rbt->root = rbn;
  } else if (rbn->key < y->key) {
    y->left = rbn;
  } else {
    y->right = rbn;
  }
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

void rb_insert(redBlackTreeP rbt,int key,void *data) {
  redBlackNodeP x = node_new(key,data);
  insert(rbt,x);
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

void *rb_search(redBlackTreeP rbt,int key) {
  redBlackNodeP rbn;
  void *result = NULL;
  rbn = search(rbt->root,key);
  if (rbn != NULL) {
    result = rbn->data;
  }
  return result;
}

static void traverse(redBlackNodeP rbn,void (*visit)(int key,void *value,void *data),void *data) {
  if (rbn == NULL) {
    return;
  } else {
    traverse(rbn->left,visit,data);
    visit(rbn->key,rbn->data,data);
    traverse(rbn->right,visit,data);
  }
}

static void traverseRL(redBlackNodeP rbn,void (*visit)(int key,void *value,void *data),void *data) {
  if (rbn == NULL) {
    return;
  } else {
    traverseRL(rbn->right,visit,data);
    visit(rbn->key,rbn->data,data);
    traverseRL(rbn->left,visit,data);
  }
}

void rb_traverse(redBlackTreeP r,void(*visit)(int key,void *value,void *data),void *data,int reverse) {
  if (!reverse) {
    traverse(r->root,visit,data);
  } else {
    traverseRL(r->root,visit,data);
  }
}

int rb_count(redBlackTreeP rbt) {
  return rbt->count;
}

int rb_isNull(redBlackNodeP rbn) {
  return rbn == NULL;
}
