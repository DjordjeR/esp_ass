#include <stdio.h> // Standardna biblioteka
#include <stdlib.h> // Trebaće nam za 
#include <string.h> // Trebaće nam za stringove
#include <ctype.h> // Treba nam za isAlpha

//Substitute for BOOL
typedef short BOOL;
#define TRUE 1
#define FALSE 0
//Success states
#define SUCCESS_PROGRAM_CLOSED 0
//Return values
#define ERROR_FILE_COULD_NOT_BE_READ 3
#define ERROR_TO_MANY_ARGUMENTS 1
#define ERROR_OUT_OF_MEMORY 2
//String lenghts
#define INPUT_COMMAND_LENGHT 256 //NOTE: Treba li vece ? 
#define MAX_NAME_LENGHT 256
//Msc

typedef struct _Person_ 
{
  char name_[MAX_NAME_LENGHT];
  BOOL gender_;
  struct _Person_ *mother_; 
  struct _Person_ *father_; 
}Person;


Person *parseDotFile(char *file_content);

BOOL isValidName(const char *name);

BOOL nameIsUnknown(const char *name);

void parseInput(char *input_command, Person *persons_array);

void waitForInput(Person *persons_array);

BOOL fileExists(const char *file_name);

BOOL fileIsWritable(const char *file_name);

char *storeFileIntoMemory(const char *file_name);

void listPersons(Person *persons);

Person *createPersonInstance(char *name, BOOL gender, Person *mother, Person *father);

Person *findPerson(Person *persons, char *name, BOOL gender);

void showError(short error_code);

/**
 * [main description]
 * @param  argc [description]
 * @param  argv [description]
 * @return      [description]
 */
int main(int argc, char **argv)
{
  if(argc == 1)
  {
    Person *persons_array = (Person*)malloc(sizeof(Person));
    (persons_array)->gender_ = 3;
    waitForInput(persons_array);
  }
  else if(argc == 2)
  {
    char *file_name = argv[1];
    if(!fileExists(file_name))
    {
      showError(ERROR_FILE_COULD_NOT_BE_READ);
      return ERROR_FILE_COULD_NOT_BE_READ;
    }
    waitForInput(parseDotFile(storeFileIntoMemory(file_name))); //NOTE: ovo mozda ljepse napisati
  }
  else
  {
    showError(ERROR_TO_MANY_ARGUMENTS);
    return ERROR_TO_MANY_ARGUMENTS;
  }
  return SUCCESS_PROGRAM_CLOSED;
}

//NOTE: main() je gotovo 
//NOTE: fileIsWritable() je gotov
//NOTE: fileExists() je gotov
//NOTE: findPerson() je gotov
//NOTE: storeFileIntoMemory() je gotov
//NOTE: createPersonInstance() je gotov
//NOTE: nameIsUnknown()
//NOTE: Sta treba raditi sa fajlom ako je nepravilno formatiran ? 
//NOTE: Da li cemo uvijek dobiti fajl sa newline na kraju, mozemo li to koristi za razvajanje ? 
//NOTE: pitaj tutora za sscanf
//NOTE: Pitati za njemacka slova ? 


/**
 * [parseDotFile description]
 * @param file_content [description]
 */
Person *parseDotFile(char *file_content)
{
  int number_of_lines = 0;
  int counter = 0;
  while(*(file_content + counter) != '\0') // Counting number of lines
  {
    if(*(file_content + counter) == '\n')
    {
      number_of_lines++;
    }
    counter++;
  }
  char *lines_separated[number_of_lines];
  int lines_separated_counter = 0;
  int file_content_counter = 0;

  counter = 0;
  while(*(file_content + counter) != '\0') // Extrating values from single string to array of pointers to the specific adresses withing the string
  {
    if(*(file_content + counter) == '\n')
    {
      *(file_content + counter) = '\0'; //Puting null byte insted of newline so the string is later valid for string functions
      lines_separated[lines_separated_counter] = file_content + file_content_counter;
      
      file_content_counter = counter + 1;
      lines_separated_counter++;
    }
    ++counter;
  }
  // Provjeravamo da li je pocetak i da li zagrade odgovaraju
  if(strcmp(lines_separated[0],"digraph FamilyTree") != 0 || strcmp(lines_separated[1],"{") != 0 || file_content[counter-1] != '}')
  {
    free(file_content);
    showError(ERROR_FILE_COULD_NOT_BE_READ);
    exit(ERROR_FILE_COULD_NOT_BE_READ);
  }
  char name[256];

  char gender[4];

  BOOL gender_b;
  int number_of_persons = 0;

  Person *array_of_persons = (Person*)malloc(sizeof(Person)*lines_separated_counter); //This will be reallocated later
  array_of_persons[lines_separated_counter].gender_ = 3;
  for(counter = 2; counter < lines_separated_counter; counter++ )
  {
    sscanf(lines_separated[counter]," \"%[^[][%[^]];",name,gender);
    name[strlen(name)-1] = '\0'; // NOTE: Ovo se rješava zadnjeg praznog mjesta u stringu u imenu, provjeriti da li postoji bolje rješenje.
    gender_b = (gender[0] == 'f') ? TRUE : FALSE;
    if(!(nameIsUnknown(name) == TRUE || isValidName(name) == TRUE))
    {
      free(file_content);
      free(array_of_persons);
      showError(ERROR_FILE_COULD_NOT_BE_READ);
    	exit(ERROR_FILE_COULD_NOT_BE_READ);
    }

    if(findPerson(array_of_persons,name,gender_b) == NULL)
    {
      Person *new_temp_person = createPersonInstance(name,gender_b,NULL,NULL);
      strcpy(array_of_persons[number_of_persons].name_,new_temp_person->name_);
      array_of_persons[number_of_persons].gender_ = new_temp_person->gender_;
      array_of_persons[number_of_persons].mother_ = new_temp_person->mother_;
      array_of_persons[number_of_persons].father_ = new_temp_person->father_;
      free(new_temp_person);
      number_of_persons++;
    }
  }
  array_of_persons[number_of_persons].gender_ = 3; // This is like \0 in string so we know where our array ends
  array_of_persons = (Person*)realloc(array_of_persons,sizeof(Person)*(number_of_persons+1));

  /*
  char name2[256];
  char gender2[4];
  sscanf(lines_separated[3]," \"%[^ [] [%[^]]%*[^->]->%*[^\"]\"%[^[] [%[^]]%*[^;]",name,gender,name2,gender2); //TODO: moraćemo čitati svake navodnike i onaj znak -> i ; na kraju da možemo prebrojati imaju li svi da vidimo da li je fajl važeći
  printf("%s - %s - %s %s\n",name, gender,name2,gender2 );
  printf("%d\n", sscanf(lines_separated[3]," \"%[^ [] [%[^]]%*[^->]->%*[^\"]\"%[^[] [%[^]]%*[^;]",name,gender,name2,gender2));
  
*/
  listPersons(array_of_persons);
  printf("File parsing successful...\n");
  free(file_content);
  return array_of_persons;
}

/**
 * [isValidName description]
 * @param  name [description]
 * @return      [description]
 */
BOOL isValidName(const char *name)
{
  int counter = 0;
  while(*(name+counter) != '\0')
  {
    if(!(isalpha(*(name+counter)) != 0 || *(name+counter) == ' ' || *(name+counter) == '.'))
    {
      return 0;
    }
    counter++;
  }
  return 1;
}
/**
 * nameIsUnknown description]
 * @param  name [description]
 * @return      [description]
 */
BOOL nameIsUnknown(const char *name)
{
	return (name[0] == '?' && isdigit(name[1])) ? TRUE : FALSE; 
}
/**
 * [parseInput description]
 * @param input_command [description]
 * @param input_lenght  [description]
 */
void parseInput(char *input_command, Person *persons_array)
{
  strtok(input_command, "\n"); // Ne treba nam end line na kraju stringa
  if(strcmp(input_command,"quit") == 0)
  {
    free(persons_array);
    printf("Bye.\n");
    exit(SUCCESS_PROGRAM_CLOSED);
  }
  if(strcmp(input_command,"list") == 0)
  {
  	listPersons(persons_array);
  }

}
/**
 * [waitForInput description]
 */
void waitForInput(Person *persons_array)
{
  char input_command[INPUT_COMMAND_LENGHT] ;
  while(TRUE)
  {
    printf("esp> ");
    fgets(input_command,INPUT_COMMAND_LENGHT,stdin);
    if(strlen(input_command) != 1)
    {
        parseInput(input_command,persons_array);
    }
    if (feof(stdin))
    {
      free(persons_array);
      printf("\n");
      exit(SUCCESS_PROGRAM_CLOSED); // TODO: provjeriti üsta ide ovdje, nisam siguran za SUCCESS_PROGRAM_CLOSED
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
void listPersons(Person *persons)
{
  int counter = 0;
  while(persons[counter].gender_  != 3)
  {
    printf("%s ", persons[counter].name_);
    printf("%s\n", (persons[counter].gender_ == 1) ? "[f]" : "[m]");
    ++counter;
  }
}
/**
 * [createPersonInstance description]
 * @param  name   [description]
 * @param  gender [description]
 * @param  mother [description]
 * @param  father [description]
 * @return        [description]
 */
Person *createPersonInstance(char *name, BOOL gender, Person *mother, Person *father)
{
  Person *new_person = (Person*)malloc(sizeof(Person));
  if(new_person == NULL)
  {
    exit(ERROR_OUT_OF_MEMORY);
  }
  strcpy(new_person->name_,name);
  new_person->gender_ = gender;
  new_person->father_ = father;
  new_person->mother_ = mother;

  return new_person;
}
/**
 * [findPerson description]
 * @param  persons           [description]
 * @param  number_of_persons [description]
 * @param  name              [description]
 * @param  gender_           [description]
 * @return                   [description]
 */
Person *findPerson(Person *persons, char *name, BOOL gender)
{
  int counter = 1;
  while((persons+counter)->gender_ != 3)
  {
    if(strcmp((persons+counter)->name_,name) == 0 && (persons+counter)->gender_ == gender)
    {
      return persons+counter;
    }
    counter++;
  }
  return NULL;
}
/**
 * [showError description]
 * @param error_code [description]
 */
void showError(short error_code)
{
  switch(error_code)
  {
    case ERROR_FILE_COULD_NOT_BE_READ:
    printf("[ERR] Could not read file.\n");
    break;
    case ERROR_TO_MANY_ARGUMENTS:
    printf("Usage: ./ass [file-name]\\n\n");
    break;
  }
}