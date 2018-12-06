#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gst.h"
#include "rbt.h"
#include "string.h"
#include "interpreter.h"

int main(int argc, char **argv) {
  int green = 0, rbt = 0;

  if (argc == 1) printf("%d arguments!\n",argc-1);

  for (int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "-v") == 0)
    {
      printf("Written by Connor Gill\n");
      exit(0);
    }

    if (strcmp(argv[i], "-g") == 0) { green = i; }
    if (strcmp(argv[i], "-r") == 0) { rbt = i; }

  }

  // FIXME
  if (green) {
    GST * tree = newGST(compareSTRING); // change to GST
    setGSTdisplay(tree, displaySTRING);
    GSTInterpreter(tree, argv, stdout);
    // FIXME: free *tree
  }
  else if (rbt == 1 || (rbt == 0 && green == 0)) {
    RBT * tree = newRBT(compareSTRING);
    setRBTdisplay(tree, displaySTRING);
    RBTInterpreter(tree, argv, stdout);
    // FIXME: free *tree
  }
  else {
    printf("Error: invalid flag. Valid flags are: '-v' | '-g' | '-r'\n");
  }

  return 0;
}
