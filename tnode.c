#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tnode.h"


struct tnode
{
    TNODE *left;
    TNODE *right;
    TNODE *parent;
    void *val;
    int debug;
    void (*display)(void *ptr, FILE *fp);
    void (*free)(void *ptr);
};

extern TNODE *newTNODE(void *v,TNODE *l,TNODE *r,TNODE *p)
{
    TNODE *n = (TNODE *)malloc(sizeof(TNODE));
    assert(n != 0);
    n->val = v;
    n->left = l;
    n->right = r;
    n->parent = p;
    n->debug = 0;
    n->display = 0;
    n->free = 0;
    return n;
}

extern void   setTNODEdisplay(TNODE *n,void (*d)(void *val,FILE *fp))
{
  n->display = d;
}

extern void   setTNODEfree(TNODE *n,void (*f)(void *val))
{
  n->free = f;
}

extern void  *getTNODEvalue(TNODE *n)
{
  return n->val;
}

extern void   setTNODEvalue(TNODE *n,void *replacement)
{
  n->val = replacement;
}

extern TNODE *getTNODEleft(TNODE *n)
{
  return n->left;
}

extern void   setTNODEleft(TNODE *n,TNODE *replacement)
{
  n->left = replacement;
}

extern TNODE *getTNODEright(TNODE *n)
{
  return n->right;
}

extern void   setTNODEright(TNODE *n,TNODE *replacement)
{
  n->right = replacement;
}

extern TNODE *getTNODEparent(TNODE *n)
{
  return n->parent;
}

extern void   setTNODEparent(TNODE *n,TNODE *replacement)
{
  n->parent = replacement;
}

extern void   displayTNODE(TNODE *n,FILE *fp)
{
  if (n->display == 0 && n->debug == 0)
  {
    fprintf(fp, "&%p\n", &n->val);  //&valAddy
  }
  else if (n->display == 0 && n->debug > 0)
  {
    fprintf(fp, "&%p&%p\n", &n->val, &n); // &valAddy&nodeAddy
  }

  else if (n->display != 0 && n->debug == 0)
  {
    n->display(n->val, fp);
  }
  else
  {
    n->display(n->val, fp);
    fprintf(fp, "&%p\n", &n);
  }

}
extern int    debugTNODE(TNODE *n,int level)
{
  int temp = n->debug;
  n->debug = level;
  return temp;
}
extern void   freeTNODE(TNODE *n)
{
  if (n->free)
  {
    n->free(n->val);
  }
    free(n);
}
