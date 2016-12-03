#include <stdio.h> // Standardna biblioteka
#include <stdlib.h> // Trebaće nam za 
#include <string.h> // Trebaće nam za stringove

typedef short BOOL;
#define TRUE 1
#define FALSE 0

//Return values
#define ERROR_FILE_COULD_NOT_BE_READ 3
#define ERROR_TO_MANY_ARGUMENTS 1
#define ERROR_OUT_OF_MEMORY 2
//String lenghts
#define INPUT_COMMAND_LENGHT 256
#define MAX_NAME_LENGHT 256
//Msc
#define INITIAL_NUMBER_OF_PERSONS 5 //NOTE: ovo je samo za testiranje

typedef struct _Person_ 
{
  char name_[MAX_NAME_LENGHT];
  BOOL gender_;
  struct _Person_ *mother_; 
  struct _Person_ *father_; 
}Person;
/**
 * [parseInput description]
 * @param input_command [description]
 * @param input_lenght  [description]
 */
void parseInput(char *input_command, size_t input_lenght)
{
  strtok(input_command, "\n"); // Ne treba nam end line na kraju stringa
  if(strcmp(input_command,"quit") == 0)
  {
      printf("Bye.\n");
      exit(0);
  }
  if(strcmp(input_command,"list") == 0)
  {
      printf("Skoro pa kul\n"); //TODO : napraviti funkciju
  }
}
/**
 * [waitForInput description]
 */
void waitForInput()
{
  char input_command[INPUT_COMMAND_LENGHT] ;
  while(TRUE)
  {
    printf("esp>");
    fgets(input_command,INPUT_COMMAND_LENGHT,stdin);
    if(strlen(input_command) != 1)
    {
        parseInput(input_command,strlen(input_command));
    }
    if (feof(stdin))
    {
        exit(0);
    } 
  }
}
/**
 * [fileExists description]
 * @param  file_name [description]
 * @return           [description]
 */
BOOL fileExists(const char *file_name) // Provjeravamo da li fajl postoji 
{
  FILE *file_stream;
  if((file_stream = fopen(file_name,"r")))
  {
      fclose(file_stream);
      return TRUE;
  }
  return FALSE;
}
/**
 * [fileIsWritable description]
 * @param  file_name [description]
 * @return           [description]
 */
BOOL fileIsWritable(const char *file_name) // Provjeravamo da li je moguće pisati u fajl
{
  FILE *file_stream;
  if((file_stream = fopen(file_name,"rw")))
  {
      fclose(file_stream);
      return TRUE;
  }
  return FALSE; 
}
/**
 * [storeFileIntoMemory description]
 * @param  file_name [description]
 * @return           [description]
 */
char *storeFileIntoMemory(const char *file_name)
{
  char *file_content = NULL;
  FILE *file_stream = fopen(file_name,"r");

  if(file_stream != NULL)
  {
      if(fseek(file_stream,0L,SEEK_END) == 0)
      {
          long buffer_size = ftell(file_stream);
          if(buffer_size == -1)
          {
              printf("Neki error.\n");
          }
          file_content = (char*)malloc(sizeof(char)*(buffer_size + 1));
          if(file_content == NULL)
          {
            exit(ERROR_OUT_OF_MEMORY);
          }
          if(fseek(file_stream,0L,SEEK_SET) != 0)
          {
              printf("Neki error\n");
          }
          size_t new_lenght = fread(file_content,sizeof(char),buffer_size,file_stream);
          if(ferror(file_stream) != 0)
          {
              fputs("Error reading this file",stderr);
          }
          else
          {
              file_content[new_lenght] = '\0';
          }
      }
      fclose(file_stream);
  }
  return file_content;
}
/**
 * [listPersons description]
 * @param persons           [description]
 * @param number_of_entries [description]
 */
void listPersons(Person *persons, int number_of_entries)
{
  int counter;
  for(counter = 0; counter < number_of_entries; counter++)
  {
    printf("%s ", persons[counter].name_);
    printf("%s\n", (persons[counter].gender_ == 1) ? "[f]" : "[m]");
  }
}
/**
 * [parseDotFile description]
 * @param file_content [description]
 */
void parseDotFile(char *file_content) //TODO: najvazniej
{
  free(file_content);
}
/**
 * [findPerson description]
 * @param  persons           [description]
 * @param  number_of_persons [description]
 * @param  name              [description]
 * @param  gender_           [description]
 * @return                   [description]
 */
Person *findPerson(Person *persons, int number_of_persons, char *name, BOOL gender_)
{
  int counter;
  for(counter = 0; counter < number_of_persons; counter++)
  {
    if(strcmp((persons+counter)->name_,name) == 0 && (persons+counter)->gender_ == gender_)
    {
      return persons+counter;
    }
  }
  return NULL;
}
/**
 * [main description]
 * @param  argc [description]
 * @param  argv [description]
 * @return      [description]
 */
int main(int argc, char **argv)
{
  //Only for testing
  Person *list_of_persons = (Person*)malloc(sizeof(Person)*INITIAL_NUMBER_OF_PERSONS);
  strcpy(list_of_persons[0].name_,"Marija");
  list_of_persons[0].gender_ = 1;
  strcpy(list_of_persons[1].name_,"Aragan");
  list_of_persons[1].gender_ = 0;

  if(argc == 1)
  {
    listPersons(list_of_persons,2);
    free(list_of_persons);
    waitForInput();
  }
  else if(argc == 2)
  {
    char *file_name = argv[1];
    if(!fileExists(file_name))
    {
      printf("[ERR] Could not read file.\n");
      return ERROR_FILE_COULD_NOT_BE_READ;
    }
    char *file_content_ptr = storeFileIntoMemory(file_name);
    parseDotFile(file_content_ptr);
    //parseDotFile
    //printPersons
    waitForInput();
  }
  else
  {
    return ERROR_TO_MANY_ARGUMENTS;
  }
  return 0;
}