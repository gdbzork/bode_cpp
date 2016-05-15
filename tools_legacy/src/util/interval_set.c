/*! \file interval_set.c
 *  \brief Implements a set of intervals with depth and coverage operations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "interval_set.h"

#define ISRED(x) ((x)->isRed==1)     /*!< returns true if a node is red */
#define SETRED(x) ((x)->isRed = 1)   /*!< turns a node red */
#define SETBLACK(x) ((x)->isRed = 0) /*!< turns a node black */

/*! \brief node in an interval_set tree.
 *
 * Minimum necessary to store red/black nodes 
 */
typedef struct isetNode {
  int leftPoint;           /*!< left end of interval */
  int rightPoint;          /*!< right end of interval */
  char isRed;              /*!< 1 for red, 0 for black */
  struct isetNode *left;   /*!< left subtree */
  struct isetNode *right;  /*!< right subtree */
  struct isetNode *parent; /*!< parent node */
  int depth;               /*!< number of intervals with this start/end */
} *isetNode;

/*! \brief holder for an interval set.
 *
 * This holds a link to the root, and a count of the nodes in the tree.
 */
typedef struct isetTree {
  int count;     /*! nodes currently in tree */
  isetNode root; /*! root of the tree */
} *isetTree;


/* allocate and initialize a new node
 */
static isetNode node_new(int left,int right) {
  isetNode x = (isetNode) malloc(sizeof(struct isetNode));
  x->leftPoint = left;
  x->rightPoint = right;
  x->isRed = 1;
  x->left = NULL;
  x->right = NULL;
  x->parent = NULL;
  x->depth = 1;
  return x;
}

/* compare two nodes, returning -1, 0, 1 as x is less than, equal to, or
 * greater than y.
 * Definition: x < y iff (x.left < y.left)
 *                    OR (x.left == y.left AND x.right < y.right)
 *             x == y iff (x.left == y.left AND x.right == y.right)
 *             x > y otherwise
 */
static int node_cmp(isetNode x,isetNode y) {
  int res = 0;
  if (x->leftPoint < y->leftPoint) {
    res = -1;
  } else if (x->leftPoint > y->leftPoint) {
    res = 1;
  } else {
    assert(x->leftPoint == y->leftPoint);
    if (x->rightPoint < y->rightPoint) {
      res = -1;
    } else if (x->rightPoint > y->rightPoint) {
      res = 1;
    }
  }
  return res;
}

/* return -1,0,1 if node is left of point, covers point, is right of point */
static int node_cmpPoint(isetNode n,int p) {
  int res = 0;
  if (n->rightPoint <= p) {
    res = -1;
  } else if (n->leftPoint > p) {
    res = 1;
  }
  return res;
}

/* insert an interval.  If an equal interval is already present, increment its
 * counter and do not insert the new node.
 * Return 1 if inserted, 0 if incremented existing node.
 */
static int insert(isetTree iset,isetNode node) {
  int c;
  isetNode y = NULL;
  isetNode x = iset->root;
  while (x != NULL) {
    y = x;
    c = node_cmp(node,x);
    if (c < 0) {
      x = x->left;
    } else if (c > 0) {
      x = x->right;
    } else {
      x->depth += 1;
      return 0;
    }
  }
  node->parent = y;
  if (y == NULL) {
    iset->root = node;
  } else if (node_cmp(node,y) < 0) {
    y->left = node;
  } else {
    y->right = node;
  }
  return 1;
}

/* standard red/black tree left rotate.
 */
static void leftRotate(isetTree iset,isetNode node) {
  isetNode y = node->right;
  node->right = y->left;
  if (y->left != NULL) {
    y->left->parent = node;
  }
  y->parent = node->parent;
  if (node->parent == NULL) {
    iset->root = y;
  } else if (node == node->parent->left) {
    node->parent->left = y;
  } else {
    node->parent->right = y;
  }
  y->left = node;
  node->parent = y;
}

/* standard red/black tree right rotate. */
static void rightRotate(isetTree iset,isetNode node) {
  isetNode y = node->left;
  node->left = y->right;
  if (y->right != NULL) {
    y->right->parent = node;
  }
  y->parent = node->parent;
  if (node->parent == NULL) {
    iset->root = y;
  } else if (node == node->parent->right) {
    node->parent->right = y;
  } else {
    node->parent->left = y;
  }
  y->right = node;
  node->parent = y;
}

/* traverse nodes which overlap point, return sum of depths
 */
static int countOverlapPoint(isetNode node,int point) {
  int cmp;
  int rv;
  if (node == NULL) {
    /* we're off the tree */
    rv = 0;
  } else {
    cmp = node_cmpPoint(node,point);
/*    fprintf(stderr,"cmp: node=%d-%d point=%d cmp=%d\n",node->leftPoint,node->rightPoint,point,cmp); */
    if (cmp < 0) {
      /* node is to left of point, better try right subtree */
      rv = countOverlapPoint(node->right,point);
    } else if (cmp == 0) {
      /* we're on the point, but better check to the left and right: */
      rv = countOverlapPoint(node->left,point)
           + node->depth
           + countOverlapPoint(node->right,point);
/*      fprintf(stderr,"over point: current = %d-%d, rv=%d\n",node->leftPoint,node->rightPoint,rv); */
    } else {
      /* we're to right node, must be further left, if it's here at all */
      rv = countOverlapPoint(node->left,point);
    } 
  }
  return rv;
}

/* traverse nodes which overlap interval, return sum of depths
 */
static int countOverlapInterval(isetNode node,int left, int right) {
  int cmpL,cmpR;
  int rv;
  if (node == NULL) {
    /* we're off the tree */
    rv = 0;
  } else {
    cmpL = node_cmpPoint(node,left);
    cmpR = node_cmpPoint(node,right);
    if (cmpL < 0) {
      /* node is to left of point, better try right subtree */
      rv = countOverlapInterval(node->right,left,right);
    } else if (cmpL == 0 || cmpR == 0 || (cmpL > 0 && cmpR < 0)) {
      /* we're in the interval, but better check to the left and right: */
      rv = countOverlapInterval(node->left,left,right)
           + node->depth
           + countOverlapInterval(node->right,left,right);
    } else if (cmpR > 0) {
      /* we're to right node, must be further left, if it's here at all */
      rv = countOverlapInterval(node->left,left,right);
    } else {
      fprintf(stderr,"Missing case: l/r == %d/%d, node= %d/%d\n",
                     left,right,node->leftPoint,node->rightPoint);
      rv = 0;
    }
  }
  return rv;
}

static void node_free(isetNode node) {
  if (node == NULL) {
    return;
  }
  node_free(node->left);
  node_free(node->right);
  free(node);
}

static void tree_dump(isetNode node,FILE *fd,int indent) {
  if (node == NULL) {
    return;
  }
  tree_dump(node->left,fd,indent+2);
  fprintf(fd,"%*s%d-%d %d\n",indent,"",node->leftPoint,node->rightPoint,node->depth);
  tree_dump(node->right,fd,indent+2);
}

/* creates and returns a new interval_set.
 */
interval_set iset_new() {
  isetTree x = (isetTree) malloc(sizeof(struct isetTree));
  x->root = NULL;
  x->count = 0;
  return x;
}

/* performs a post-order traversal of tree nodes, freeing them... then free
 * the root
 */
void iset_free(interval_set iset_hdl) {
  isetTree iset = (isetTree) iset_hdl;
  node_free(iset->root);
  free(iset);
}

/* inserts an interval into the interval set.  If the interval already exists,
 * its copy number is incremented.
 */
void iset_insert(interval_set iset_hdl,int left, int right) {
  int rc;
  isetTree iset = (isetTree) iset_hdl;
  isetNode x = node_new(left,right);
  rc = insert(iset,x);
  if (rc == 0) {
    /* nothing was inserted, so... */
    return;
  }
  while (x!=iset->root && ISRED(x->parent)) {
    if (x->parent->parent->left == x->parent) {
      isetNode y = x->parent->parent->right;
      if (y != NULL && ISRED(y)) {
        SETBLACK(x->parent);
        SETBLACK(y);
        SETRED(x->parent->parent);
        x = x->parent->parent;
      } else {
        if (x == x->parent->right) {
          x = x->parent;
          leftRotate(iset,x);
        }
        SETBLACK(x->parent);
        SETRED(x->parent->parent);
        rightRotate(iset,x->parent->parent);
      }
    } else {
      isetNode y = x->parent->parent->left;
      if (y != NULL && ISRED(y)) {
        SETBLACK(x->parent);
        SETBLACK(y);
        SETRED(x->parent->parent);
        x = x->parent->parent;
      } else {
        if (x == x->parent->left) {
          x = x->parent;
          rightRotate(iset,x);
        }
        SETBLACK(x->parent);
        SETRED(x->parent->parent);
        leftRotate(iset,x->parent->parent);
      }
    }
  }
  SETBLACK(iset->root);
  iset->count++;
}

/* returns depth at a given point
 */
int iset_depth_at_point(interval_set iset_hdl, int point) {
  isetTree iset = (isetTree) iset_hdl;
  return countOverlapPoint(iset->root,point);
}

/* returns coverage over a region
 */
int iset_coverage(interval_set iset_hdl,int left,int right) {
  isetTree iset = (isetTree) iset_hdl;
  return countOverlapInterval(iset->root,left,right);
}

/* returns the number of intervals currently in the set.
 */ 
int iset_count(interval_set iset) {
  return ((isetTree)iset)->count;
}

/* dump the tree to the given file descriptor
 */
void iset_dump(interval_set iset_hdl,FILE *fd) {
  isetTree iset = (isetTree) iset_hdl;
  fprintf(fd,"Tree:\n");
  tree_dump(iset->root,fd,2);
}
