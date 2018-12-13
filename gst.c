#include <stdio.h>
#include <stdlib.h>
#include "gst.h"
#include "bst.h"
#include "tnode.h"



typedef struct gstval GSTVAL;

struct gst {
  BST * tree;
  int duplicates;
  int (*compare)(void * one, void * two);
  void (*display)(void * ptr, FILE *fp);
  void (*swap)(TNODE * one, TNODE * two);
  void (*freeMethod)(void * ptr);
};

struct gstval {
  void * val;
  int freq;
  int (*compare)(void * one, void * two);
  void (*display)(void * ptr, FILE *fp);

  void (*freeMethod)(void * ptr);
};

static TNODE * findGSTNode(GST *t, void *key);

static int getGSTduplicates(GST *t);
static int compareGVAL(void * x, void * y);

static void displayGVAL(GSTVAL *v, FILE *fp);
static void freeGVAL(GSTVAL *v);
static void setGSTduplicates(GST *t, int i);
static void setGSTFreq(GSTVAL * v, int f);
static void swapVals(TNODE *x, TNODE *y);

extern GST * newGST(int (*c)(void * one, void * two)) {
  GST * gTree = malloc(sizeof(GST));
  gTree->tree = newBST((void *)compareGVAL);
  gTree->duplicates = 0;
  gTree->display = 0;
  gTree->compare = c;
  gTree->freeMethod = 0;
  gTree->swap = (void *)swapVals;

  return gTree;
}
static GSTVAL * newGSTVAL(GST *t, void *v) {
  GSTVAL * value = malloc(sizeof(GSTVAL));
  value->val = v;
  value->freq = 1;
  value->display = t->display;
  value->compare = t->compare;
  value->freeMethod = t->freeMethod;

  return value;
}

extern void setGSTdisplay(GST *t, void (*d)(void * ptr, FILE *fp)) {
  t->display = d;
  BST * tree = t->tree;
  setBSTdisplay(tree, (void *)displayGVAL);
}

extern void setGSTswapper(GST *t, void (*s)(TNODE * one, TNODE * two)) {
  t->swap = s;
  BST * tree = t->tree;
  setBSTswapper(tree, s);
}

extern void setGSTfree(GST *t, void (*f)(void * ptr)) {
  t->freeMethod = f;
  BST * tree = t->tree;
  setBSTfree(tree, (void *)freeGVAL);
}

extern TNODE *getGSTroot(GST *t) {
  BST * tree = t->tree;
  return getBSTroot(tree);
}

extern void setGSTroot(GST *t, TNODE *replacement) {
  BST * tree = t->tree;
  setTNODEparent(replacement, replacement);
  setBSTroot(tree, replacement);
}

extern void setGSTsize(GST *t, int s) {
  BST * tree = t->tree;
  setBSTsize(tree, s);
}

extern TNODE *insertGST(GST *t, void *value) {
  GSTVAL * newVal = newGSTVAL(t, value);
  TNODE * temp = findGSTNode(t, newVal);
  if (temp) {
    freeGVAL(newVal);
    GSTVAL * ptr = (GSTVAL *)getTNODEvalue(temp);
    setGSTFreq(ptr, ptr->freq + 1);
    setGSTduplicates(t, getGSTduplicates(t) + 1);
    return 0;
  }
  BST * tree = t->tree;
  return insertBST(tree, newVal);
}

extern void * findGST(GST *t, void *key) {
  GSTVAL * v = newGSTVAL(t, key);
  TNODE * temp = findGSTNode(t, v);
  if (temp == 0) {
    free(v);
    return 0;
  }
  void * val = unwrapGST(temp);
  free(v);
  return val;
}

extern TNODE *locateGST(GST *t, void *key) {
  GSTVAL * v = newGSTVAL(t, key);
  BST * tree = t->tree;
  TNODE * ptr = locateBST(tree, v);
  free(v);
  return ptr;
}

extern int deleteGST(GST *t, void *key) {
  int freq = freqGST(t, key);
  BST * tree = t->tree;
  if (freq == 0) {
    return -1;
  }
  else if (freq > 1) {
    GSTVAL * newVal = newGSTVAL(t, key);
    TNODE * node = findGSTNode(t, newVal);
    GSTVAL * ptr = (GSTVAL *)getTNODEvalue(node);
    --freq;
    setGSTFreq(ptr, freq);
    setGSTduplicates(t, getGSTduplicates(t) - 1);
    freeGVAL(newVal); //EGT
    return freq;
  }
  GSTVAL * v = getTNODEvalue(locateGST(t, key));
  int result = deleteBST(tree, v);
  freeGVAL(v); //EGT
  return result;
}

extern TNODE *swapToLeafGST(GST *t, TNODE *node) {
  BST * tree = t->tree;
  return swapToLeafBST(tree, node);
}

extern void pruneLeafGST(GST *t, TNODE *leaf) {
  BST * tree = t->tree;
  pruneLeafBST(tree, leaf);
  free(getTNODEvalue(leaf));
}

extern int sizeGST(GST *t) {
  BST * tree = t->tree;
  return sizeBST(tree);
}

extern void statisticsGST(GST *t, FILE *fp) {
  BST * tree = t->tree;
  fprintf(fp, "Duplicates: %d\n", duplicatesGST(t));
  statisticsBST(tree, fp);
}

extern void displayGST(GST *t, FILE *fp) {
  BST * tree = t->tree;
  displayBST(tree, fp);
}

extern int debugGST(GST *t, int level) {
  BST * tree = t->tree;
  return debugBST(tree, level);
}
extern void freeGST(GST *t) {
  BST * tree = t->tree;
  freeBST(tree);
  free(t);
}

extern void * unwrapGST(TNODE *n) {
  GSTVAL * v = (GSTVAL *)getTNODEvalue(n);
  return v->val;
}

extern int freqGST(GST *g, void *key) {
  GSTVAL * v = newGSTVAL(g, key);
  TNODE * n = findGSTNode(g, v);
  if (n) {
    GSTVAL * ptr = (GSTVAL *)getTNODEvalue(n);
    free(v);
    return ptr->freq;
  }
  free(v);
  return 0;
}

extern int duplicatesGST(GST *g) {
  return getGSTduplicates(g);
}

static void swapVals(TNODE *x, TNODE *y) {
  void *one = getTNODEvalue(x);
  void *two = getTNODEvalue(y);
  setTNODEvalue(x, two);
  setTNODEvalue(y, one);
}

static TNODE * findGSTNode(GST *t, void *key) {
  BST * tree = t->tree;
  TNODE *temp = getBSTroot(tree);
  if (temp == 0 || sizeBST(tree) == 0) {
    return 0;
  }
  while (temp && compareGVAL((GSTVAL *)getTNODEvalue(temp), key) != 0) {
    if (compareGVAL((GSTVAL *)getTNODEvalue(temp), key) > 0) {
      temp = getTNODEleft(temp);
    }
    else {
      temp = getTNODEright(temp);
    }
  }
  return temp;
}

static void displayGVAL(GSTVAL *v, FILE *fp) {
  void *val = v->val;
  v->display(val, fp);
  if (v->freq > 1) {
    fprintf(fp, "<%d>", v->freq);
  }
}

static int compareGVAL(void *x, void *y) {
  GSTVAL *a = (GSTVAL *)x;
  GSTVAL *b = (GSTVAL *)y;
  return a->compare(a->val, b->val);
}

static void freeGVAL(GSTVAL *v) {
  if (v->freeMethod) {
    v->freeMethod(v->val);
  }
  free(v);
}

static int getGSTduplicates(GST * t) {
  return t->duplicates;
}

static void setGSTduplicates(GST * t, int i) {
  t->duplicates = i;
}

static void setGSTFreq(GSTVAL *v, int f) {
  v->freq = f;
}
