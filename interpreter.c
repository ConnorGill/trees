#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "rbt.h"
#include "gst.h"
#include "scanner.h"
#include "string.h"

RBT *readRBTCorpus(RBT *tree, char *filename);
void readRBTCommands(RBT *tree, char *filename, FILE *outfp);
GST *readGSTCorpus(GST *tree, char * filename);
void readGSTCommands(GST *tree, char *filename, FILE *outfp);
char * cleanString(char * str, int s);
static void skipWhiteSpace(FILE *fp);


void RBTInterpreter(RBT *tree, char **argv, FILE *outfp) {
  tree = readRBTCorpus(tree, argv[2]);
  readRBTCommands(tree, argv[3], outfp);
}

void GSTInterpreter(GST *tree, char **argv, FILE *outfp) {
  tree = readGSTCorpus(tree, argv[2]);
  readGSTCommands(tree, argv[3], outfp);
}

RBT *readRBTCorpus(RBT *tree, char *filename) {
  STRING * finalStr;
  char *str;
  int test;

  int lineTest = 1;
  //char c;
  FILE *fp = fopen(filename, "r");
  if (fp == 0) {
    printf("Error: %d (%s)\n", errno, strerror(errno));
    exit(0);
  }

  skipWhiteSpace(fp);
  test = fgetc(fp);
  if(test == '"') { ungetc(test, fp); str = readString(fp); }
  else { ungetc(test, fp); str = readToken(fp); }

    while (!feof(fp))
    {
      if (strlen(str) > 0) {
        str = cleanString(str, strlen(str));

        if(str[0] == '\0' || (str[0] == ' ' && str[1] == '\0'))
        {
          free(str); //EGT
        }
        else
        {
          //printf("INSERTED[%d]  : %s\n", lineTest, str);
          finalStr = newSTRING(str);
          insertRBT(tree, finalStr);
        }
        lineTest++;
      }
      skipWhiteSpace(fp);
      test = fgetc(fp);
      if(test == '"') { ungetc(test, fp); str = readString(fp); }
      else { ungetc(test, fp); str = readToken(fp); }
    }
  fclose(fp);

  return tree;

}

GST *readGSTCorpus(GST *tree, char * filename) {
  STRING * finalStr;
  char *str;
  int test;

  int lineTest = 1;
  //char c;
  FILE *fp = fopen(filename, "r");
  if (fp == 0) {
    printf("Error: %d (%s)\n", errno, strerror(errno));
    exit(0);
  }

  skipWhiteSpace(fp);
  test = fgetc(fp);
  if(test == '"') { ungetc(test, fp); str = readString(fp); }
  else { ungetc(test, fp); str = readToken(fp); }

    while (!feof(fp))
    {
      if (strlen(str) > 0) {
        str = cleanString(str, strlen(str));

        if(str[0] == '\0' || (str[0] == ' ' && str[1] == '\0'))
        {
          free(str);
        }
        else
        {
          finalStr = newSTRING(str);
          insertGST(tree, finalStr);
        }
        lineTest++;

      }
      skipWhiteSpace(fp);
      test = fgetc(fp);
      if(test == '"') { ungetc(test, fp); str = readString(fp); }
      else { ungetc(test, fp); str = readToken(fp); }
    }
  fclose(fp);

  return tree;
}

void readRBTCommands(RBT *tree, char *filename, FILE *outfp) {
  STRING * finalStr;
  char *str;
  int test;
  int freq;
  FILE *fp = fopen(filename, "r");
  if (fp == 0) {
    fprintf(stderr, "Error: %s could not be opened for reading.\n", filename);
  }
  int c = fgetc(fp); ////////
  while (!feof(fp)) {
    switch (c) {
      case 's':
        displayRBT(tree, outfp);
        break;
      case 'r':
        statisticsRBT(tree, outfp);
        break;
      case 'f':
        skipWhiteSpace(fp);
        test = fgetc(fp);
        if(test == '"') { ungetc(test, fp); str = readString(fp); }
        else { ungetc(test, fp); str = readToken(fp); }

        str = cleanString(str, strlen(str));

        if(str[0] == '\0' || (str[0] == ' ' && str[1] == '\0')) {;}
        else { finalStr = newSTRING(str); freq = freqRBT(tree, finalStr);}
        fprintf(outfp, "Frequency of \"%s\": %d\n", str, freq);
		freeSTRING(finalStr); //EGT
        break;
      case 'd':
        skipWhiteSpace(fp);
        test = fgetc(fp);
        if(test == '"') { ungetc(test, fp); str = readString(fp); }
        else { ungetc(test, fp); str = readToken(fp); }

        str = cleanString(str, strlen(str));

        if(str[0] == '\0' || (str[0] == ' ' && str[1] == '\0')) {free(str);}
        else
        {
          finalStr = newSTRING(str);
          if (freqRBT(tree, finalStr) == 0)
          {
            fprintf(outfp, "Value "); displaySTRING(finalStr, outfp); fprintf(outfp, " not found.\n");
			freeSTRING(finalStr); //EGT
          }
          else{ deleteRBT(tree, finalStr); }
        }
        break;
      case 'i':
        skipWhiteSpace(fp);
        test = fgetc(fp);
        if(test == '"') { ungetc(test, fp); str = readString(fp); }
        else { ungetc(test, fp); str = readToken(fp); }

        str = cleanString(str, strlen(str));

        if(str[0] == '\0' || (str[0] == ' ' && str[1] == '\0')) {;}
        else { finalStr = newSTRING(str); insertRBT(tree, finalStr); }
        break;
    }
    c = fgetc(fp);
  }
  fclose(fp);
}

void readGSTCommands(GST *tree, char *filename, FILE *outfp) {
  STRING * finalStr;
  char *str;
  int test;
  int freq;
  FILE *fp = fopen(filename, "r");
  if (fp == 0) {
    printf("Error: %d (%s)\n", errno, strerror(errno));
    exit(0);
  }
  int switchChar = fgetc(fp);
  while (!feof(fp)) {
    switch (switchChar) {
      case 's':
        displayGST(tree, outfp);
        break;
      case 'r':
        statisticsGST(tree, outfp);
        break;
      case 'f':
        skipWhiteSpace(fp);
        test = fgetc(fp);
        if(test == '"') { ungetc(test, fp); str = readString(fp); }
        else { ungetc(test, fp); str = readToken(fp); }

        str = cleanString(str, strlen(str));

        if(str[0] == '\0' || (str[0] == ' ' && str[1] == '\0')) {;}
        else { finalStr = newSTRING(str); freq = freqGST(tree, finalStr); }
        fprintf(outfp, "Frequency of \"%s\": %d\n", str, freq);
        freeSTRING(finalStr);
        break;
      case 'd':
        skipWhiteSpace(fp);
        test = fgetc(fp);
        if(test == '"') { ungetc(test, fp); str = readString(fp); }
        else { ungetc(test, fp); str = readToken(fp); }

        str = cleanString(str, strlen(str));

        if(str[0] == '\0' || (str[0] == ' ' && str[1] == '\0')) {free(str);}
        else
        {
          finalStr = newSTRING(str);
          if (freqGST(tree, finalStr) == 0)
          {
            fprintf(outfp, "Value "); displaySTRING(finalStr, outfp); fprintf(outfp, " not found.\n");
            freeSTRING(finalStr);
          }
          else{ deleteGST(tree, finalStr); freeSTRING(finalStr);}

        }
        break;
      case 'i':
        skipWhiteSpace(fp);
        test = fgetc(fp);
        if(test == '"') { ungetc(test, fp); str = readString(fp); }
        else { ungetc(test, fp); str = readToken(fp); }

        str = cleanString(str, strlen(str));

        if(str[0] == '\0' || (str[0] == ' ' && str[1] == '\0')) {;}
        else { finalStr = newSTRING(str); insertGST(tree, finalStr); }
        break;
      }
    switchChar = fgetc(fp);
  }
  fclose(fp);
}

char * cleanString(char *str, int s)
{
  char* newStr = malloc(sizeof(char) * (s+1));

  int i = 0;
  int tracker = 0;
  int space = 1;

  for(i = 0; i < s; i++)
  {
    if(isspace(str[i]) && space == 0)
    {
      newStr[tracker] = ' ';
      tracker++;
      space = 1;
    }
    else if(isalpha(str[i]))
    {
      newStr[tracker] = tolower(str[i]);
      tracker++;
      space = 0;
    }
  }

  if(tracker != 0 && newStr[tracker - 1] == ' ')
  {
    newStr[--tracker] = '\0';
  }
  else
  {
    newStr[tracker] = '\0';
  }
  newStr=(char *) realloc(newStr, (tracker+1)*(sizeof(char))); //EGT
  free(str);
  return newStr;

    /*while(i < s)
    {
      if (str[i] == ' ' && i == 0)
      { ; }
      else if(isalpha(str[i]) || (str[i] == ' ' && str[i] !=newStr[tracker -1] )) //&& newStr[i] != '\0'
      {
        newStr[tracker] = tolower(str[i]);
        tracker++;
      }
      i++;
    }
    if(newStr[0] == ' ')
    {
      for(int x = 0; x < s; x++)
      {
        char temp = newStr[x+1];
        newStr[x] = temp;
      }
    }

    if(newStr[tracker -1] == ' ')
    {
      newStr[tracker - 1] = '\0';
      //printf("CLEANED1: %s\n", newStr);
      return newStr;
    }
    newStr[tracker] = '\0';
    //printf("CLEANED2: %s\n", newStr);
    return newStr;*/

}




static void
skipWhiteSpace(FILE *fp)
    {
    int ch;

    /* read chars until a non-whitespace character is encountered */

    while ((ch = fgetc(fp)) != EOF && isspace(ch))
        continue;

    /* a non-space character got us out of the loop, so push it back */

    if (ch != EOF) ungetc(ch,fp);
    }
