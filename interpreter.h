#include <stdio.h>
#include "rbt.h"
#include "gst.h"

#ifndef _INTERPRETER_INCLUDED_
#define _INTERPRETER_INCLUDED_

extern void RBTInterpreter(RBT *tree, char **argv, FILE *outfp);
extern void GSTInterpreter(GST *tree, char **argv, FILE *outfp);
extern RBT *readRBTCorpus(RBT *tree, char *filename);
extern void readRBTCommands(RBT *tree, char *filename, FILE *outfp);
extern GST *readGSTCorpus(GST *tree, char * filename);
extern void readGSTCommands(GST *tree, char *filename, FILE *outfp);

#endif 
