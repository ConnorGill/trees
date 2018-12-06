#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "queue.h"
#include "cda.h"

struct cda
{
  int size;
  int capacity;
  void * (*array);
  int startIndex;
  int debugVal;
  void (*freeP)(void * ptr);
  void (*displayP)(void * ptr, FILE *fp);
};

struct queue
{
  CDA * qArr;
  int debugVal;
  void (*freeP)(void * ptr);
  void (*displayP)(void * ptr, FILE *fp);
};

extern QUEUE *newQUEUE(void)
{
  QUEUE * qStruct = malloc(sizeof(QUEUE));
  assert(qStruct != 0);
  qStruct->qArr = newCDA();
  assert(qStruct->qArr != 0);
  qStruct->debugVal = 0;
  qStruct->freeP = 0;
  qStruct->displayP = 0;

  return qStruct;
}
extern void  setQUEUEdisplay(QUEUE *usrQ, void (*displayPtr)(void * ptr,FILE *fp))
{
  usrQ->displayP = displayPtr;
}
extern void  setQUEUEfree(QUEUE *usrQ, void (*freePtr)(void *ptr))
{
  usrQ->freeP = freePtr;
}

/*The enqueue method runs in constant or amortized constant time. The value to
be enqueued is stored in the underlying data structure. */
extern void  enqueue(QUEUE *usrQ, void *val)
{
  insertCDAback(usrQ->qArr, val);
}

/*The dequeue method runs in constant or amortized constant time. The value to
be dequeued is removed in the underlying data structure. */
extern void *dequeue(QUEUE *usrQ)
{
  assert(sizeQUEUE(usrQ) > 0);
  void * deqVal = removeCDAfront(usrQ->qArr);
  return deqVal;
}

/*The peek method returns the value ready to come off the queue, but leaves the
queue unchanged. It runs in constant time. */
extern void *peekQUEUE(QUEUE *usrQ)
{
  assert(sizeQUEUE(usrQ) > 0);
  void * peekVal = getCDA(usrQ->qArr, 0);
  return peekVal;
}

/*This display method prints the usrQ stored in the queue. If the integers
5, 6, 2, 9, and 1 are enqueued in the order given, the method would generate
this output: <5,6,2,9,1> with no preceding or following whitespace. An empty
queue displays as <>. */
extern void  displayQUEUE(QUEUE *usrQ,FILE *fp)
{
  if (usrQ->debugVal == 0)
  {
    if (usrQ->displayP == 0)
    {
      fprintf(fp, "<");
      for (int i = 0; i < sizeQUEUE(usrQ); i++)
      {
        fprintf(fp, "@%p,", &usrQ->qArr->array[(i + usrQ->qArr->capacity) % usrQ->qArr->capacity]);
        if (i != sizeQUEUE(usrQ) - 1)
        {
          fprintf(fp, ",");
        }
      }
      fprintf(fp, ">");
    }
    else
    {
      fprintf(fp, "<");
      for (int i = 0; i < sizeQUEUE(usrQ); i++)
      {
        usrQ->displayP(getCDA(usrQ->qArr, i), fp);
        if (i != sizeQUEUE(usrQ) - 1)
        {
          fprintf(fp, ",");
        }
      }
      fprintf(fp, ">");
    }
  }
  else if (usrQ->debugVal == 1)
  {
    debugCDA(usrQ->qArr, 0);
    setCDAdisplay(usrQ->qArr, usrQ->displayP);
    displayCDA(usrQ->qArr, fp);
  }
  else
  {
    debugCDA(usrQ->qArr, 1);
    setCDAdisplay(usrQ->qArr, usrQ->displayP);
    displayCDA(usrQ->qArr, fp);
  }
}

extern int   debugQUEUE(QUEUE *usrQ,int newBug)
{
  int prevBug = usrQ->debugVal;
  usrQ->debugVal = newBug;

  return prevBug;
}
extern void  freeQUEUE(QUEUE *usrQ)
{
  setCDAfree(usrQ->qArr, usrQ->freeP);
  freeCDA(usrQ->qArr);
  free(usrQ);
}

/*The size method returns the number of usrQ stored in the queue.
It runs in amortized constant time. */
extern int   sizeQUEUE(QUEUE *usrQ)
{
  return sizeCDA(usrQ->qArr);
}
