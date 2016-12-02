#include <stdio.h> // Standardna biblioteka
#include <stdlib.h> // Trebaće nam za malloc
#include <string.h> // Trebaće nam za stringove
#include "functions.h"



typedef struct _Person_ // Nesto ovako za svaku osobu posebno
{
  char *name;
  char gender[4];
  struct _Person_ *parrent; // Ovo nam treba pošto mogu imati roditelja ili nešto tako 
}Person; // Ovo je samo ideja ovo ne mora ovako. 


int main(int argc, char const *argv[])
{
  storeFileIntoMemory();
  return 0;
}

// TODO : istDotFileValid()
// TODO : parseDotFile()
// TODO : parseConsoleInput()
// TODO : createDotFile()