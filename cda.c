#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "cda.h"

static void growArray(CDA *usrCDA);
static void shrinkArray(CDA *usrCDA);
static int correctIndex(CDA *usrCDA, int oldIndex);

struct cda
{
  void * (*array);
  int capacity;
  int size;
  int startIndex;
  int endIndex;
  int debugVal;
  void (*freeP)(void * ptr); //pointer for use in freeDA function
  void (*displayP)(void * ptr, FILE *fp); //pointer for use in displayDA function
};

extern CDA * newCDA(void)
{
  CDA * arrStruct = malloc(sizeof(CDA));
  assert(arrStruct != 0);
  arrStruct->capacity = 1;
  arrStruct->array = malloc(sizeof(void *) * arrStruct->capacity);
  assert(arrStruct->array != 0);
  arrStruct->size = 0;
  arrStruct->startIndex = 0;
  arrStruct->endIndex = 0;
  arrStruct->debugVal = 0;
  arrStruct->freeP = 0;
  arrStruct->displayP = 0;

  return arrStruct;
}

extern void setCDAdisplay(CDA *usrCDA, void (*displayPtr)(void *ptr, FILE *fp))
{
  usrCDA->displayP = displayPtr;
}

extern void setCDAfree(CDA *usrCDA, void (*freePtr)(void *ptr))
{
  usrCDA->freeP = freePtr;
}

extern void insertCDA(CDA *usrCDA, int index, void *value)
{
  assert(index >= 0 && index <= sizeCDA(usrCDA));

  if (sizeCDA(usrCDA) == usrCDA->capacity)
  {
    growArray(usrCDA);
  }

  if (index == 0)
  {
    if (sizeCDA(usrCDA) != 0)
    {
      usrCDA->startIndex = correctIndex(usrCDA, -1);
    }
    if (sizeCDA(usrCDA) == 0)
    {
      usrCDA->endIndex = 1;
    }
    usrCDA->array[usrCDA->startIndex] = value;
    usrCDA->size += 1;
  }

  else if (index == sizeCDA(usrCDA))
  {
    usrCDA->array[usrCDA->endIndex] = value;
    usrCDA->endIndex = correctIndex(usrCDA, sizeCDA(usrCDA) + 1);
    usrCDA->size += 1;
  }

  else
  {
    int decisionPt = sizeCDA(usrCDA) / 2;
    int trueIndex = correctIndex(usrCDA, index);
    if (index <= decisionPt)
    {
      for (int i = 0; i < index; i++)
      {
        usrCDA->array[correctIndex(usrCDA, i - 1)] = usrCDA->array[correctIndex(usrCDA, i)];
      }
      usrCDA->array[correctIndex(usrCDA, index - 1)] = value;
  	  usrCDA->startIndex = (usrCDA->startIndex + usrCDA->capacity - 1) % usrCDA->capacity;
    }
    else
    {
      for (int i = usrCDA->size - 1; i >= index; i--)
      {
        usrCDA->array[correctIndex(usrCDA, i + 1)] = usrCDA->array[correctIndex(usrCDA, i)];
      }
      usrCDA->array[trueIndex] = value;
      usrCDA->endIndex = (usrCDA->endIndex + 1) % usrCDA->capacity;
    }
    usrCDA->size += 1;
  }
}

extern void *removeCDA(CDA *usrCDA, int index)
{
  assert(index >= 0 && index <= sizeCDA(usrCDA) - 1);
  assert(sizeCDA(usrCDA) > 0);
  void * value = getCDA(usrCDA, index);

  if (index == 0)
  {
    usrCDA->startIndex = correctIndex(usrCDA, 1);
    usrCDA->size -= 1;
  }
  else if (index == sizeCDA(usrCDA) - 1)
  {
    usrCDA->endIndex = (usrCDA->endIndex - 1 + usrCDA->capacity) % usrCDA->capacity;
    usrCDA->size -= 1;
  }
  else
  {
    int decisionPt = sizeCDA(usrCDA) / 2;
    if (index <= decisionPt)
    {
      for (int i = index; i > 0; i--)
      {
        usrCDA->array[correctIndex(usrCDA, i)] = usrCDA->array[correctIndex(usrCDA, i - 1)];
      }
      usrCDA->startIndex = (usrCDA->startIndex + 1) % usrCDA->capacity;
    }

    else
    {
      for (int i = index; i < sizeCDA(usrCDA) - 1; i++)
      {
        usrCDA->array[correctIndex(usrCDA, i)] = usrCDA->array[correctIndex(usrCDA, i + 1)];
      }
      usrCDA->endIndex = (usrCDA->endIndex - 1 + usrCDA->capacity) % usrCDA->capacity;
    }

    usrCDA->size -= 1;
  }

  if ((sizeCDA(usrCDA)/(double)usrCDA->capacity) < .25)
  {
    shrinkArray(usrCDA);
    if ((sizeCDA(usrCDA)/(double)usrCDA->capacity) < .25)
    {
      shrinkArray(usrCDA);
    }
  }

  return value;
}

// takes two arrays and moves all the items in the donor array to the recipient arrays
extern void unionCDA(CDA *recipient, CDA *donor)
{
  for (int i = 0; i < sizeCDA(donor); i++)
  {
    insertCDAback(recipient, getCDA(donor, i));
  }
  donor->size = 0;
  donor->capacity = 1;
  free(donor->array);
  donor->array = malloc(sizeof(void *) * 1);
}

// method returns the value at the given index, from user's perspective
// In the user's view, the first item is at index zero, the second item at index 1, and so on
// routine has to translate between the users view
// and the internal view (where the first item can be anywhere in the underlying array)
extern void *getCDA(CDA *usrCDA, int index)
{
  assert(index >= 0 && index < sizeCDA(usrCDA));
  int trueIndex = correctIndex(usrCDA, index);
  return usrCDA->array[trueIndex];
}

// updates the value at the given index, from user's perspective
// if given index == size of the array, value is inserted at back of array
// if given index == -1 value is inserted at front of array
extern void *setCDA(CDA *usrCDA, int index, void *val)
{
  assert(index >= -1 && index <= sizeCDA(usrCDA));

  if (index == sizeCDA(usrCDA))
  {
    insertCDAback(usrCDA, val);
    return 0;
  }

  else if (index == -1)
  {
    insertCDAfront(usrCDA, val);
    return 0;
  }
  else
  {
    int trueIndex = correctIndex(usrCDA, index);
    void * tempVal = getCDA(usrCDA, trueIndex);
    usrCDA->array[trueIndex] = val;
    return tempVal;
  }

}

// method returns the size of array
extern int sizeCDA(CDA *usrCDA)
{
  return usrCDA->size;
}


extern void displayCDA(CDA *usrCDA, FILE *fp)
{
  if (sizeCDA(usrCDA) == 0)
  {
    if (usrCDA->debugVal > 0)
    {
      fprintf(fp, "((%d))", usrCDA->capacity);
    }
    else
    {
      fprintf(fp, "()");
    }
  }

  else if (usrCDA->displayP == 0)
  {
    if (usrCDA->debugVal > 0)
    {
      fprintf(fp, "(");
      for (int i = 0; i < sizeCDA(usrCDA); i++)
      {
        fprintf(fp, "@%p,", &usrCDA->array[correctIndex(usrCDA, i)]);
      }
      fprintf(fp, "(%d))", (usrCDA->capacity - sizeCDA(usrCDA)));
    }
    else
    {
      fprintf(fp, "(");
      for (int i = 0; i < sizeCDA(usrCDA); i++)
      {
        fprintf(fp, "@%p,", &usrCDA->array[correctIndex(usrCDA, i)]);
      }
      fprintf(fp, ")");
    }
  }
  else {
    if (usrCDA->debugVal > 0)
    {
      fprintf(fp, "(");
      for (int i = 0; i < sizeCDA(usrCDA); i++)
      {
        usrCDA->displayP(getCDA(usrCDA, i), fp);
        if (i != (sizeCDA(usrCDA)))
        {
          fprintf(fp, ",");
        }
      }
      fprintf(fp, "(%d))", (usrCDA->capacity - sizeCDA(usrCDA)));
    }
    else { // display method set and method should not display num. empty indeces
      fprintf(fp, "(");
      for (int i = 0; i < sizeCDA(usrCDA); i++)
      {
        usrCDA->displayP(getCDA(usrCDA, i), fp);
        if (i != (sizeCDA(usrCDA) - 1))
        {
          fprintf(fp, ",");
        }
      }
      fprintf(fp, ")");
    }
  }
}

extern int debugCDA(CDA *usrCDA, int level)
{
  int prevVal = usrCDA->debugVal;
  usrCDA->debugVal = level;

  return prevVal;
}

extern void freeCDA(CDA *usrCDA)
{
  if (usrCDA->freeP != 0)
  {
    for (int i = 0; i < sizeCDA(usrCDA); i++)
    {
      usrCDA->freeP(getCDA(usrCDA, i));
    }
  }
  free(usrCDA->array);
  free(usrCDA);
}

static void growArray(CDA *usrCDA)
{
  int newCap = usrCDA->capacity * 2;
  void * (*temp) = malloc(sizeof(void *) * newCap);
  assert(temp != 0);
  for (int i = 0; i < sizeCDA(usrCDA); i++) { temp[i] = getCDA(usrCDA, i); }
  free(usrCDA->array);
  usrCDA->array = temp;
  usrCDA->startIndex = 0;
  usrCDA->endIndex = sizeCDA(usrCDA);
  usrCDA->capacity = newCap;
}

static void shrinkArray(CDA *usrCDA)
{
  int newCap = 0;
  if(usrCDA->capacity != 1)
  {
      newCap = usrCDA->capacity / 2;
  }
  else
  {
    newCap = usrCDA->capacity;
  }
  void * (*temp) = malloc(sizeof(void*) * newCap);
  assert(temp != 0);
  for (int i = 0; i < sizeCDA(usrCDA); i++) { temp[i] = getCDA(usrCDA, i); }
  free(usrCDA->array);
  usrCDA->array = temp;
  usrCDA->startIndex = 0;
  usrCDA->endIndex = sizeCDA(usrCDA);
  usrCDA->capacity = newCap;
}

static int correctIndex(CDA *usrCDA, int oldIndex)
{
  int index = (oldIndex + usrCDA->capacity + usrCDA->startIndex) % usrCDA->capacity;
  return index;
}
