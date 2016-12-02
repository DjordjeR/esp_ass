#include <stdio.h> // Standardna biblioteka
#include <stdlib.h> // Trebaće nam za malloc
#include <string.h> // Trebaće nam za stringove

typedef short BOOL;
#define TRUE 1
#define FALSE 0
#define ERROR_FILE_COULD_NOT_BE_READ 3
#define INPUT_COMMAND_LENGHT 255
#define NUMBER_OF_MAX_INPUT_PARAMETERS 5

typedef struct _Person_ // Nesto ovako za svaku osobu posebno
{
  char *name;
  BOOL gender;
  struct _Person_ *parrent; // Ovo nam treba pošto mogu imati roditelja ili nešto tako 
}Person; // Ovo je samo ideja ovo ne mora ovako. 
#include "functions.h"

int main(int argc, char const *argv[])
{

  char *file_name = "black_fam-tree.dot";
  if(!fileExists(file_name)){
    printf("[ERR] Could not read file.\n");
    return ERROR_FILE_COULD_NOT_BE_READ;
  }
  char *file_content_ptr = storeFileIntoMemory(file_name);
  parseDotFile(storeFileIntoMemory(file_name));
  waitForInput();
  return 0;
}


// TODO : istDotFileValid()
// TODO : parseDotFile()
// TODO : parseConsoleInput()
// TODO : createDotFile()
