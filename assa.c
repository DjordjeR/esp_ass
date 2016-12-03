#include <stdio.h> // Standardna biblioteka
#include <stdlib.h> // Trebaće nam za malloc
#include <string.h> // Trebaće nam za stringove

typedef short BOOL;
#define TRUE 1
#define FALSE 0
#define ERROR_FILE_COULD_NOT_BE_READ 3
#define INPUT_COMMAND_LENGHT 256
#define NUMBER_OF_MAX_INPUT_PARAMETERS 5
#define MAX_NAME_LENGHT 256

typedef struct _Person_ 
{
  char name_[MAX_NAME_LENGHT];
  BOOL gender_;
  struct _Person_ *mother_; 
  struct _Person_ *father_; 
}Person;

void parseInput(char *input_command, size_t input_lenght)
{
  char *input_parameters[NUMBER_OF_MAX_INPUT_PARAMETERS]; //Array of pointers
  char *splitted_string;
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

  splitted_string = strtok(input_command," ");
  int i = 0;
  while (splitted_string != NULL)
  {
    input_parameters[i] = malloc(strlen(splitted_string) + 1); // TODO: osloboditi memoriju poslije
    strcpy(input_parameters[i], splitted_string);
    splitted_string = strtok (NULL, " ");
    printf("%s\n", input_parameters[i]);
    i++;
  }
}

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

char *storeFileIntoMemory(const char *file_name)
{
  char *file_content = NULL; // TODO : Dodati provjeru da li fajl postoji
  FILE *file_stream = fopen(file_name,"r");

  if(file_stream != NULL)
  {
      if(fseek(file_stream,0L,SEEK_END) == 0)
      {
          long buffer_size = ftell(file_stream);
          if(buffer_size == -1)
          {
              printf("Neki error.\n");
          } // TODO : Projvjeriti da li je malloc radio sa if == null
          file_content = (char*)malloc(sizeof(char)*(buffer_size + 1));

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

void parseDotFile(char *file_content)
{

  int number_of_lines = 0;
  int counter = 0;
  while (*(file_content+counter) != '\0')
  {
    if(*(file_content+counter) == ' ')
    {
      *(file_content+counter) = '\0';
      printf("%d\n", strcmp(file_content,"digraph"));
      break;
    }
    counter++;
  }

  free(file_content);
}

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
