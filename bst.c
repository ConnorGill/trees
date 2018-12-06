#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "queue.h"
#include "bst.h"
#include "tnode.h"

static int isLeftChild(TNODE *n);
static int isRoot(TNODE *n);
static int isRightChild(TNODE *n);
static int isLeaf(TNODE *n);
static int getMinHeight(TNODE *n);
static int getMaxHeight(TNODE *n);
static int min(int x, int y);
static int max(int x, int y);

static void displayNode(BST *t, TNODE *n, FILE *fp);
static void levelOrder(BST *t, FILE *fp);
static void inOrder(BST *t, TNODE *n, FILE *fp);
static void preOrder(BST *t, TNODE *n, FILE *fp);
static void postOrder(BST *t, TNODE *n, FILE *fp);
static void freeAllNodes(TNODE *n);

static TNODE * swapVals(TNODE *x, TNODE *y);
static TNODE *getPrev(TNODE *n);
static TNODE *getNext(TNODE *n);
static TNODE *findBSTNode(BST *tree, void *key);

struct bst
{
  TNODE * root;
  int size;
  int debugVal;
  int (*compare)(void * one, void * two);
  void (*display)(void * ptr, FILE *fp);
  void (*swap)(TNODE * one, TNODE * two);
  void (*free)(void * ptr);
};


extern BST * newBST(int (*c)(void * one, void * two))
{
  BST * tree = malloc(sizeof(BST));
  assert(tree != NULL);
  tree->root = 0;
  tree->size = 0;
  tree->debugVal = 0;
  tree->compare = c;
  tree->display = 0;
  tree->swap = (void *)swapVals;
  tree->free = 0;

  return tree;
}


extern void setBSTdisplay(BST * t, void (*d)(void * ptr, FILE * fp))
{
  t->display = d;
}

extern void setBSTswapper(BST *t, void (*s)(TNODE * one, TNODE * two))
{
  t->swap = s;
}

extern void setBSTfree(BST *t, void (*f)(void * ptr))
{
  t->free = f;
}

extern TNODE * getBSTroot(BST *t)
{
  return t->root;
}

extern void setBSTroot(BST *t, TNODE *replacement)
{
  t->root = replacement;
  setTNODEparent(replacement, replacement);
}

extern void setBSTsize(BST *t, int s)
{
  t->size = s;
}

extern TNODE *insertBST(BST *t, void * value)
{
  TNODE * newNode = newTNODE(value, 0, 0, 0);
  assert(newNode != NULL);
  setTNODEfree(newNode, (void *)t->free);
  setTNODEdisplay(newNode, (void *)t->display);
  TNODE * temp = getBSTroot(t);


  if (temp == 0)
  {
    setBSTroot(t, newNode);
    setBSTsize(t, sizeBST(t) + 1);
    return newNode;
  }
  while (temp != 0)
  {
    setTNODEparent(newNode, temp);
    if (t->compare(getTNODEvalue(getTNODEparent(newNode)), value) >= 1)
    {
      temp = getTNODEleft(temp);
    }
    else
    {
      temp = getTNODEright(temp);
    }
  }
  if (t->compare(getTNODEvalue(getTNODEparent(newNode)), value) >= 1)
  {
    setTNODEleft(getTNODEparent(newNode), newNode);
  }
  else
  {
    setTNODEright(getTNODEparent(newNode), newNode);
  }

  setBSTsize(t, sizeBST(t) + 1);
  return newNode;
}

extern void * findBST(BST *t, void *key)
{
  TNODE * temp = findBSTNode(t, key);
  if (temp != 0)
  {
    return getTNODEvalue(temp);
  }
  else
  {
    return 0;
  }
}

extern TNODE * locateBST(BST *t, void *key)
{
  TNODE * temp = findBSTNode(t, key);
  if (temp != 0)
  {
    return temp;
  }
  else
  {
    return 0;
  }
}

extern int deleteBST(BST *t, void *key)
{
  TNODE * temp = findBSTNode(t, key);
  if (temp)
  {
    temp = swapToLeafBST(t, temp);
    pruneLeafBST(t, temp);
    free(temp);

    setBSTsize(t, sizeBST(t) - 1);
    return 0;
  }
  else
  {
    printf("Error: node with key not found\n");
    return -1;
  }
}

extern TNODE *swapToLeafBST(BST *t, TNODE *node)
{
  if (node == 0)
  {
    return node;
  }
  if (isLeaf(node))
  {
    return node;
  }
  TNODE *temp = node;
  if (getTNODEleft(temp))
  {
    TNODE* tempPrev = getPrev(temp);
    t->swap(temp, tempPrev);
    temp = tempPrev;
  }
  else
  {
    TNODE * tempNext = getNext(temp);
    t->swap(temp, tempNext);
    temp = tempNext;
  }
  return swapToLeafBST(t, temp);
}

extern void pruneLeafBST(BST *t, TNODE *leaf) {
  if (t == 0) {
    return;
  }
  else if (isRoot(leaf)) {
    return;
  }
  TNODE * parent = getTNODEparent(leaf);
  if (getTNODEleft(parent) == leaf) {
    setTNODEleft(parent, 0);
    setTNODEparent(leaf, 0);
  }
  else {
    setTNODEright(parent, 0);
    setTNODEparent(leaf, 0);
  }
}

static int isRightChild(TNODE *n)
{
  if (getTNODEright(getTNODEparent(n)) == n) {
    return 1;
  }
  return 0;
}

static int isLeftChild(TNODE *n)
{
  if (getTNODEleft(getTNODEparent(n)) == n) {
    return 1;
  }
  return 0;
}

static int isLeaf(TNODE *n)
{
  if (getTNODEleft(n) == 0 && getTNODEright(n) == 0) {
    return 1;
  }
  return 0;
}

static int isRoot(TNODE *n)
{
  if (getTNODEparent(n) == 0) {
    return 1;
  }
  return 0;
}

extern int sizeBST(BST * t)
{
  return t->size;
}

extern void statisticsBST(BST *t, FILE *fp)
{
  fprintf(fp, "Nodes: %d\n", sizeBST(t));
  int minDepth = 0;
  int maxDepth = 0;
  if (sizeBST(t) == 0) {
    minDepth = -1;
    maxDepth = -1;
  }
  else
  {
    minDepth = getMinHeight(getBSTroot(t));
    maxDepth = getMaxHeight(getBSTroot(t));
  }
  fprintf(fp, "Minimum depth: %d\n", minDepth);
  fprintf(fp, "Maximum depth: %d\n", maxDepth);
}

extern void displayBST(BST *t, FILE *fp)
{

  if (t->debugVal == 0)
  {
    levelOrder(t, fp);
  }

  else if (t->debugVal == 1)
  {
    TNODE * temp = getBSTroot(t);
    inOrder(t, temp, fp);
  }

  else if (t->debugVal == 2)
  {
    TNODE * temp = getBSTroot(t);
    preOrder(t, temp, fp);
  }

  else
  {
    TNODE * temp = getBSTroot(t);
    postOrder(t, temp, fp);
  }
}

extern int debugBST(BST *t, int level)
{
  int prevVal = t->debugVal;
  t->debugVal = level;

  return prevVal;
}

static TNODE * findBSTNode(BST *tree, void *key)
{
  TNODE *temp = getBSTroot(tree);
  if (temp == 0 || sizeBST(tree) == 0) {
    return 0;
  }
  while (temp && tree->compare(getTNODEvalue(temp), key) != 0)
  {
    if (tree->compare(getTNODEvalue(temp), key) > 0)
    {
      temp = getTNODEleft(temp);
    }
    else
    {
      temp = getTNODEright(temp);
    }
  }
  return temp;
}

static TNODE * swapVals(TNODE *x, TNODE *y)
{
  void *temp = getTNODEvalue(x);
  setTNODEvalue(x, getTNODEvalue(y));
  setTNODEvalue(y, temp);
  return y;
}

static TNODE *getPrev(TNODE *n)
{
  TNODE *temp = getTNODEleft(n);
  while (getTNODEright(temp))
  {
    temp = getTNODEright(temp);
  }
  return temp;
}

static TNODE *getNext(TNODE *n)
{
  TNODE *temp = getTNODEright(n);
  while (getTNODEleft(temp)) temp = getTNODEleft(temp);
  return temp;
}

static int getMinHeight(TNODE *n)
{

  if (n == 0)
  {
    return 0;
  }


  if (getTNODEleft(n) == 0 || getTNODEright(n) == 0)
  {
    return 0;
  }


  if (!getTNODEleft(n))
  {
    return getMinHeight(getTNODEright(n)) + 1;
  }


  if (!getTNODEright(n))
  {
    return getMinHeight(getTNODEleft(n)) + 1;
  }

  return min(getMinHeight(getTNODEleft(n)), getMinHeight(getTNODEright(n))) + 1;
}

static int getMaxHeight(TNODE *n)
{

  if (n == 0)
  {
    return 0;
  }


  if (getTNODEleft(n) == 0 && getTNODEright(n) == 0)
  {
    return 0;
  }


  if (!getTNODEleft(n)) {
    return getMaxHeight(getTNODEright(n)) + 1;
  }


  if (!getTNODEright(n)) {
    return getMaxHeight(getTNODEleft(n)) + 1;
  }

  return max(getMaxHeight(getTNODEleft(n)), getMaxHeight(getTNODEright(n))) + 1;
}


static void displayNode(BST *t, TNODE *n, FILE *fp)
{
  if (n == 0) {
    return;
  }
  if (isLeaf(n)) {
    fprintf(fp, "=");
  }
  t->display(getTNODEvalue(n), fp);

  fprintf(fp, "(");
  t->display(getTNODEvalue(getTNODEparent(n)), fp);
  fprintf(fp, ")");
  if (getBSTroot(t) == n) {
    fprintf(fp, "X");
  }
  if (isLeftChild(n)) {
    fprintf(fp, "L");
  }
  else if (isRightChild(n)) {
    fprintf(fp, "R");
  }
}


static void levelOrder(BST *t, FILE *fp)
{
  int level = 0;
  if (t == 0 || sizeBST(t) == 0) {
    fprintf(fp, "0:\n");
    return;
  }
  else {
    QUEUE *q = newQUEUE();
    setQUEUEdisplay(q, t->display);

    fprintf(fp, "%d:", level);
    if (getBSTroot(t) != 0) {
      enqueue(q, getBSTroot(t));
      enqueue(q, NULL);
    }
    while (sizeQUEUE(q) != 0) {

      TNODE *temp = (TNODE *)dequeue(q);
      if (temp == 0) {
        if (sizeQUEUE(q)) {
          fprintf(fp, "\n");
          fprintf(fp, "%d:", ++level);
          enqueue(q, NULL);
        }
      }
      else {
        fprintf(fp, " ");
        displayNode(t, temp, fp);

        if (getTNODEleft(temp)) { enqueue(q, getTNODEleft(temp)); }
        if (getTNODEright(temp)) { enqueue(q, getTNODEright(temp)); }
      }
    }
    fprintf(fp, "\n");
    freeQUEUE(q);
  }
}

static void inOrder(BST * t, TNODE * n, FILE * fp)
{
  if (t == 0 || sizeBST(t) == 0) {
    fprintf(fp, "[]");
    return;
  }
  if (sizeBST(t) == 1) {
    fprintf(fp, "[");
    t->display(getTNODEvalue(getBSTroot(t)), fp);
    fprintf(fp, "]");
    return;
  }
  if (n == 0) {
    return;
  }
  fprintf(fp, "[");
  inOrder(t, getTNODEleft(n), fp);

  if (getTNODEright(n) && getTNODEleft(n))
  {
    fprintf(fp, " ");
  }

  if (!getTNODEright(n) && getTNODEleft(n))
  {
    fprintf(fp, " ");
  }

  t->display(getTNODEvalue(n), fp);

  if (getTNODEright(n) && getTNODEleft(n))
  {
    fprintf(fp, " ");
  }

  if (getTNODEright(n) && !getTNODEleft(n))
  {
    fprintf(fp, " ");
  }

  inOrder(t, getTNODEright(n), fp);
  fprintf(fp, "]");
}

static void preOrder(BST *t, TNODE *n, FILE *fp)
{
  if (t == 0 || sizeBST(t) == 0) {
    fprintf(fp, "[]\n");
    return;
  }
  if (sizeBST(t) == 1)
  {
    fprintf(fp, "[");
    t->display(getTNODEvalue(getBSTroot(t)), fp);
    fprintf(fp, "]");
    return;
  }
  if (n == 0) {
    return;
  }

  if (n == getBSTroot(t)) { fprintf(fp, "["); }
  else { fprintf(fp, " ["); }

  t->display(getTNODEvalue(n), fp);

  preOrder(t, getTNODEleft(n), fp);
  preOrder(t, getTNODEright(n), fp);

  fprintf(fp, "]");
}

static void postOrder(BST *t, TNODE *n, FILE *fp)
{
  if (t == 0 || sizeBST(t) == 0)
  {
    fprintf(fp, "[]\n");
    return;
  }
  if (sizeBST(t) == 1)
  {
    fprintf(fp, "[");
    t->display(getTNODEvalue(getBSTroot(t)), fp);
    fprintf(fp, "]");
    return;
  }
  if (n == 0)
  {
    return;
  }
  fprintf(fp, "[");

  postOrder(t, getTNODEleft(n), fp);
  postOrder(t, getTNODEright(n), fp);

  t->display(getTNODEvalue(n), fp);

  fprintf(fp, "]");
  if (n != getBSTroot(t))
  {
    fprintf(fp, " ");
  }
}

static int min(int x, int y)
{
  if (x < y)
  {
    return x;
  }
  else
  {
    return y;
  }
}

static int max(int x, int y)
{
  if (x > y) {
    return x;
  }
  else { return y; }
}

extern void freeBST(BST *t)
{
  TNODE * temp = getBSTroot(t);
  if (temp == 0 || sizeBST(t) == 0)
  {
    free(t);
    return;
  }
  freeAllNodes(temp);
  free(t);
}

static void freeAllNodes(TNODE *n)
{
  if (n == 0)
  {
    return;
  }
  TNODE *ptr = n;
  freeAllNodes(getTNODEleft(ptr));
  freeAllNodes(getTNODEright(ptr));
  freeTNODE(ptr);
}
