#include <stdio.h> // Standardna biblioteka
#include <stdlib.h> // Trebaće nam za 
#include <string.h> // Everything has to be done with strings
#include <ctype.h> // Treba nam za isAlpha

//TODO Provjeriti trebamo li staviti ?1 ako neko nema roditelja kada ucitavamo fajl

//Substitute for BOOL
typedef short BOOL;
#define TRUE 1
#define FALSE 0
//Success states
#define SUCCESS_PROGRAM_CLOSED 0
// Success messages
#define MSG_SUCCESS_PROGRAM_CLOSED_WITH_QUIT 1
#define MSG_SUCCESS_PROGRAM_CLOSED_WITH_EOF 2
#define MSG_SUCCESS_DOT_FILE_PARSING 10

//Return values
#define ERROR_FILE_COULD_NOT_BE_READ 3
#define ERROR_TO_MANY_ARGUMENTS_WHILE_LOADING_FILE 1
#define ERROR_OUT_OF_MEMORY 2
#define ERROR_NO_ENTRIES_AVAILABLE 8
#define ERROR_WRONG_LIST_USAGE 7
#define ERROR_WRONG_ADD_USAGE 6
//String lenghts
#define INPUT_COMMAND_LENGHT 530 //NOTE: 256 for one name + 256 second name + 6 for genders + some whitespaces + longest command for longest command = 
#define MAX_NAME_LENGHT 257

//Msc
typedef struct _Person_ 
{
  char name_[MAX_NAME_LENGHT];
  BOOL gender_;
  struct _Person_ *mother_; 
  struct _Person_ *father_; 
}Person;


Person *parseDotFile(char *file_content);

BOOL nameIsUnknown(const char *name);

BOOL fileExists(const char *file_name);

BOOL fileIsWritable(const char *file_name);

BOOL parseSingleFileLine(char *line_to_parse,char *name,BOOL *gender_b, char *parrent_name, BOOL *parrent_gender_b);

Person createPersonInstance(char *name, BOOL gender, Person *mother, Person *father);

Person *findPerson(Person *persons, char *name, BOOL gender);

void showError(short error_code);

void showSuccessMessage(short msg_code);

char *storeFileIntoMemory(const char *file_name);

BOOL listPersons(Person *persons);

void parseInput(char *input_command, Person *persons_array);

void waitForInput(Person *persons_array);

BOOL parseAddInput(char *input_command);

BOOL copyPerson(Person *first_person, Person *second_person);

void parseDrawAllInput(char *input_command);

void parseDrawInput(char *input_command);

void parseRelationshipInput(char *input_command);

int numberOfPersons(Person *persons);
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
    showError(ERROR_TO_MANY_ARGUMENTS_WHILE_LOADING_FILE);
    return ERROR_TO_MANY_ARGUMENTS_WHILE_LOADING_FILE;
  }
  return SUCCESS_PROGRAM_CLOSED;
}

//NOTE: Sta treba raditi sa fajlom ako je nepravilno formatiran ? Ispisati error could not read file ili tako nesto
//NOTE: Da li cemo uvijek dobiti fajl sa newline na kraju, mozemo li to koristi za razvajanje ? Hocemo
//NOTE: pitaj tutora za sscanf, umjesto sscanf napisana funkcija koja sve to provjerava
//NOTE: Pitati za njemacka slova ? Nece doci njemacka slova


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
  char name[MAX_NAME_LENGHT];
  char parrent_name[MAX_NAME_LENGHT];
  BOOL gender;
  BOOL parrent_gender;
  int number_of_persons = 0;

  Person *array_of_persons = (Person*)malloc(sizeof(Person)*lines_separated_counter); //This will be reallocated later
  array_of_persons[lines_separated_counter].gender_ = 3;
  for(counter = 2; counter < lines_separated_counter; counter++ )
  {
    if(parseSingleFileLine(lines_separated[counter], name, &gender, parrent_name, &parrent_gender) == FALSE)
    {
      showError(ERROR_FILE_COULD_NOT_BE_READ);
      exit(ERROR_FILE_COULD_NOT_BE_READ);
    }
    name[strlen(name)-1] = '\0'; // NOTE: Ovo se rješava zadnjeg praznog mjesta u stringu u imenu, provjeriti da li postoji bolje rješenje.
    if(findPerson(array_of_persons,name,gender) == NULL)
    {
      strcpy(array_of_persons[number_of_persons].name_,name);
      array_of_persons[number_of_persons].gender_ = gender;
      array_of_persons[number_of_persons].mother_ = NULL;
      array_of_persons[number_of_persons].father_ = NULL;
      if(parrent_name[0] != '\0')
      {
        if(findPerson(array_of_persons,parrent_name,parrent_gender) == NULL)
        {
          strcpy(array_of_persons[number_of_persons+1].name_,parrent_name);
          array_of_persons[number_of_persons+1].gender_ = parrent_gender;
          array_of_persons[number_of_persons+1].mother_ = NULL;
          array_of_persons[number_of_persons+1].father_ = NULL;
          if(parrent_gender == TRUE)
          {
            array_of_persons[number_of_persons].mother_ = &array_of_persons[number_of_persons+1];
          }
          else if(parrent_gender == FALSE)
          {
            array_of_persons[number_of_persons].father_ = &array_of_persons[number_of_persons+1];
          }
          number_of_persons++;
        }
        else
        {
          Person *parrent = findPerson(array_of_persons,parrent_name,parrent_gender);
          if(parrent_gender == TRUE)
          {
            array_of_persons[number_of_persons].mother_ = parrent;
          }
          else if(parrent_gender == FALSE)
          {
            array_of_persons[number_of_persons].father_ = parrent;
          }
        }
      }
      number_of_persons++;
      parrent_name[0] = '\0';
    }
    else
    {
      if(parrent_name[0] != '\0')
      {
        Person *new_temp_person = findPerson(array_of_persons,name,gender);
        if(parrent_gender == TRUE && new_temp_person->mother_ == NULL)
        {
          Person *temp_parrent_mother = findPerson(array_of_persons,parrent_name,parrent_gender);
          if(temp_parrent_mother == NULL)
          {
            strcpy(array_of_persons[number_of_persons].name_,parrent_name);
            array_of_persons[number_of_persons].gender_ = parrent_gender;
            array_of_persons[number_of_persons].mother_ = NULL;
            array_of_persons[number_of_persons].father_ = NULL;
            new_temp_person->mother_ = &array_of_persons[number_of_persons];
            number_of_persons++;
          }
          else
          {
            new_temp_person->mother_ = temp_parrent_mother;
          }
        }
        else if(parrent_gender == FALSE && new_temp_person->father_ == NULL)
        {
          Person *temp_parrent_father = findPerson(array_of_persons,parrent_name,parrent_gender);
          if(temp_parrent_father == NULL)
          {
            strcpy(array_of_persons[number_of_persons].name_,parrent_name);
            array_of_persons[number_of_persons].gender_ = parrent_gender;
            array_of_persons[number_of_persons].mother_ = NULL;
            array_of_persons[number_of_persons].father_ = NULL;
            new_temp_person->father_ = &array_of_persons[number_of_persons];
            number_of_persons++;
          }
          else
          {
            new_temp_person->mother_ = temp_parrent_father;
          }
        }
      }
    }
  }
  array_of_persons[number_of_persons].gender_ = 3; // This is like \0 in string so we know where our array ends
  array_of_persons = (Person*)realloc(array_of_persons,sizeof(Person)*(number_of_persons+1));
  if(!listPersons(array_of_persons))
  {
    showError(ERROR_NO_ENTRIES_AVAILABLE);
  }
  showSuccessMessage(MSG_SUCCESS_DOT_FILE_PARSING);
  free(file_content);
  return array_of_persons;
}

/**
 * [parseSingleFileLine description]
 * @param  line_to_parse    [description]
 * @param  name             [description]
 * @param  gender_b         [description]
 * @param  parrent_name     [description]
 * @param  parrent_gender_b [description]
 * @return                  [description]
 */
BOOL parseSingleFileLine(char *line_to_parse, char *name,BOOL *gender_b, char *parrent_name, BOOL *parrent_gender_b)
{
  if(*(line_to_parse) != ' ' || *(line_to_parse+1) != ' ' || *(line_to_parse+2) != '"')
  {
    return FALSE;
  }
  int counter = 3;
  int null_counter = 0;
  while(*(line_to_parse+counter) != '[')
  {
    *(name+null_counter) = *(line_to_parse+counter);
    counter++;
    null_counter++;
    if(counter >= MAX_NAME_LENGHT || *(line_to_parse+counter) == ']' || *(line_to_parse+counter) == '>' || *(line_to_parse+counter) == ';' || *(line_to_parse+counter) == '"')
    {
      return FALSE;
    }
  }
  *(name+(null_counter)) = '\0';
  counter+=2; // We are skipping [
  if(*(line_to_parse+counter) != ']')
  {
    return FALSE;
  }
  if(*(line_to_parse+(counter-1)) != 'f' && *(line_to_parse+(counter-1)) != 'm')
  {
    return FALSE;
  }
  *gender_b = (*(line_to_parse+(counter-1)) == 'f') ? TRUE : FALSE;
  ++counter;
  if(*(line_to_parse+counter) == ' ' || *(line_to_parse+(counter+1)) == ';')
  {
    *parrent_name = '\0';
    return TRUE;
  }
  counter+=2;
  if(*(line_to_parse+(counter)) != '-' || *(line_to_parse+(counter+1)) != '>' || *(line_to_parse+(counter+2)) != ' ' || *(line_to_parse+(counter+3)) != '"')
  {
    return FALSE;
  }
  counter+=4;
  null_counter = 0;
  while(*(line_to_parse+counter) != '[')
  {
    *(parrent_name+null_counter) = *(line_to_parse+counter);
    counter++;
    null_counter++;
    if(counter >= MAX_NAME_LENGHT || *(line_to_parse+counter) == ']' || *(line_to_parse+counter) == '>' || *(line_to_parse+counter) == ';' || *(line_to_parse+counter) == '"')
    {
      return FALSE;
    }
  }
  *(parrent_name+(null_counter-1)) = '\0';
  counter+=2; // We are skipping [
  if(*(line_to_parse+counter) != ']' || *(line_to_parse+(counter+1)) != '"' || *(line_to_parse+(counter+2)) != ';')
  {
    return FALSE;
  }
  if(*(line_to_parse+(counter-1)) != 'f' && *(line_to_parse+(counter-1)) != 'm')
  {
    return FALSE;
  }
  *parrent_gender_b = (*(line_to_parse+(counter-1)) == 'f') ? TRUE : FALSE;
 return TRUE;
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
  if(strcmp(input_command,"quit\n") == 0)
  {
    free(persons_array);
    showSuccessMessage(MSG_SUCCESS_PROGRAM_CLOSED_WITH_QUIT);
    exit(SUCCESS_PROGRAM_CLOSED);
  }
  if(strcmp(input_command,"list\n") == 0)
  {
    if(!listPersons(persons_array))
    {
      showError(ERROR_NO_ENTRIES_AVAILABLE);
    }
  }
  else
  {
    int counter = 0;
    while(*(input_command+counter) != '\n')
    {
      if(*(input_command+counter) == ' ')
      {
        break;
      }
    counter++;  
    }
    *(input_command+counter) = '\0';
    counter+=1;
    char *command = input_command;
    if(strcmp(command,"list") == 0)
    {
      showError(ERROR_WRONG_LIST_USAGE);
    }
    if(strcmp(command,"add") == 0)
    {
      if(!parseAddInput(input_command))
      {
        showError(ERROR_WRONG_ADD_USAGE);
      }
    }
    if(strcmp(command,"draw") == 0)
    {
      parseDrawInput(input_command);
    }
    if(strcmp(command,"draw-all") == 0)
    {
      parseDrawAllInput(input_command);
    }
    if(strcmp(command,"relationship") == 0)
    {
      parseRelationshipInput(input_command);
    }
  }
}

/**
 * [parseAddInput description]
 * @param input_command [description]
 */
BOOL parseAddInput(char *input_command)
{
  int counter = 4;
  while(*(input_command + counter) != '[')
  {
    counter++;
    if(*(input_command + counter) == '\n')
    {
      return FALSE;
    }
  }
  if(*(input_command + (counter-1)) != ' ')
  {
    return FALSE;
  }
  *(input_command + (counter-1)) = '\0';
  char *first_person_name = input_command + 4;
  if(*(input_command + counter) != '[' || *(input_command + (counter+2)) != ']' || *(input_command + (counter+3)) != ' ')
  {
    return FALSE;
  }
  counter++;
  if(*(input_command + counter) != 'f' && *(input_command + counter) != 'm')
  {
    return FALSE;
  }
  BOOL first_person_gender = (*(input_command + counter) == 'f') ? TRUE : FALSE;
  counter+=3;
  char *relationship = input_command + (counter);
  while(*(input_command + counter) != ' ')
  {
    if(*(input_command + counter) == '\n' || *(input_command + counter) == '[' || *(input_command + counter) == ']')
    {
      return FALSE;
    }
    counter++;
  }
  *(input_command + counter) = '\0';
  if (*(input_command + (counter + 1)) == ' ')
  {
    return FALSE;
  }
  while(*(input_command + counter) != '[')
  {
    counter++;
    if(*(input_command + counter) == '\n')
    {
      return FALSE;
    }
  }
  if(*(input_command + (counter-1)) != ' ')
  {
    return FALSE;
  }
  *(input_command + (counter-1)) = '\0';
  char *second_person_name = input_command + 4;
  if(*(input_command + counter) != '[' || *(input_command + (counter+2)) != ']' || *(input_command + (counter+3)) != '\n')
  {
    return FALSE;
  }
  counter++;
  if(*(input_command + counter) != 'f' && *(input_command + counter) != 'm')
  {
    return FALSE;
  }
  BOOL second_person_gender = (*(input_command + counter) == 'f') ? TRUE : FALSE;
  return TRUE;
  //memset(input_command,0,INPUT_COMMAND_LENGHT);
}
/**
 * [parseDrawInput description]
 * @param input_command [description]
 */
void parseDrawInput(char *input_command)
{
  printf("Parse draw input ...\n");
}
/**
 * [parseDrawAllInput description]
 * @param input_command [description]
 */
void parseDrawAllInput(char *input_command)
{
  printf("Parse draw all input ...\n");
}
/**
 * [parseRelationshipInput description]
 * @param input_command [description]
 */
void parseRelationshipInput(char *input_command)
{
  printf("Parse elationship input ...\n");
}

/**
 * [waitForInput description]
 * @param persons_array [description]
 */
void waitForInput(Person *persons_array)
{
  char input_command[INPUT_COMMAND_LENGHT] ;
  while(TRUE)
  {
    printf("esp>");
    fgets(input_command,INPUT_COMMAND_LENGHT,stdin);
    if(strlen(input_command) != 1 && !feof(stdin))
    {
        parseInput(input_command,persons_array);
    }
    if (feof(stdin))
    {
      free(persons_array);
      showSuccessMessage(MSG_SUCCESS_PROGRAM_CLOSED_WITH_EOF);
      exit(SUCCESS_PROGRAM_CLOSED);
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
BOOL fileIsWritable(const char *file_name) // TODO: Izgleda da je nepotrebno
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
            showError(ERROR_OUT_OF_MEMORY);
            exit(ERROR_OUT_OF_MEMORY);
          }
          file_content = (char*)malloc(sizeof(char)*(buffer_size + 1));
          if(file_content == NULL)
          {
            showError(ERROR_OUT_OF_MEMORY);
            exit(ERROR_OUT_OF_MEMORY);
          }
          if(fseek(file_stream,0L,SEEK_SET) != 0)
          {
            free(file_content);
            showError(ERROR_FILE_COULD_NOT_BE_READ);
            exit(ERROR_FILE_COULD_NOT_BE_READ);
          }
          size_t new_lenght = fread(file_content,sizeof(char),buffer_size,file_stream);
          if(ferror(file_stream) != 0)
          {
            free(file_content);
            showError(ERROR_FILE_COULD_NOT_BE_READ);
            exit(ERROR_FILE_COULD_NOT_BE_READ);
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

BOOL sortPersons(Person *persons)
{
  Person person_placeholder;
  int number_of_persons = numberOfPersons(persons);
  int counter;
  int switch_counter;

  if(number_of_persons <= 1)
  {
    return FALSE;
  }
  for(counter = 0; counter < number_of_persons; counter++)
  {
    for(switch_counter = 0; switch_counter < number_of_persons - 1; switch_counter ++)
    {
      if (strcmp((persons + switch_counter)->name_,(persons + (switch_counter + 1))->name_) > 0)
      {
        copyPerson(&person_placeholder,(persons + switch_counter));
        copyPerson((persons + switch_counter),(persons + (switch_counter + 1)));
        copyPerson((persons + (switch_counter + 1)),&person_placeholder);
      }
    }
  }
  return TRUE;
}

/**
 * [listPersons description]
 * @param persons           [description]
 * @param number_of_entries [description]
 */
BOOL listPersons(Person *persons) // TODO: Provjeriti da li ovdje moramo sortirati po abecedi osobe
{
  int counter = 0;
  sortPersons(persons);
  while((persons+counter)->gender_  != 3)
  {
    printf("%s ", (persons+counter)->name_);
    printf("%s\n", ((persons+counter)->gender_ == 1) ? "[f]" : "[m]");
    ++counter;
  }
  if(counter <= 1)
  {
    return FALSE;
  }
  return TRUE;
}
/**
 * [copyPerson description]
 * @param  first_person  [description]
 * @param  second_person [description]
 * @return               [description]
 */
BOOL copyPerson(Person *first_person, Person *second_person)
{
  strcpy(first_person->name_,second_person->name_);
  first_person->gender_ = second_person->gender_;
  first_person->father_ = second_person->father_;
  first_person->mother_ = second_person->mother_;

  return TRUE;
}

/**
 * [counterPersons description]
 * @param  persons [description]
 * @return         [description]
 */
int numberOfPersons(Person *persons)
{
  int number_of_persons = 0;
  while((persons + number_of_persons)->gender_ != 3)
  {
    number_of_persons++;
  }
  return number_of_persons;
}
/**
 * [createPersonInstance description]
 * @param  name   [description]
 * @param  gender [description]
 * @param  mother [description]
 * @param  father [description]
 * @return        [description]
 */
Person createPersonInstance(char *name, BOOL gender, Person *mother, Person *father)
{
  Person new_person;  
  strcpy(new_person.name_,name);
  new_person.gender_ = gender;
  new_person.father_ = father;
  new_person.mother_ = mother;
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
  int counter = 0;
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
    case ERROR_TO_MANY_ARGUMENTS_WHILE_LOADING_FILE:
    printf("Usage: ./ass [file-name]\n");
    break;
    case ERROR_NO_ENTRIES_AVAILABLE:
    printf("[ERR] No entries available.\n");
    break;
    case ERROR_OUT_OF_MEMORY:
    printf("[ERR] Out of memory.\n");
    break;
    case ERROR_WRONG_LIST_USAGE:
    printf("[ERR] Wrong usage - list.\n");
    break;
    case ERROR_WRONG_ADD_USAGE:
    printf("[ERR] Wrong usage - add <namePerson1> [m/f] <relation> <namePerson2> [m/f].\n");
    break;
  }
}
/**
 * [showSuccessMessage description]
 * @param msg_code [description]
 */
void showSuccessMessage(short msg_code)
{
  switch(msg_code)
  {
    case MSG_SUCCESS_PROGRAM_CLOSED_WITH_EOF:
    printf("\n");
    break;
    case MSG_SUCCESS_PROGRAM_CLOSED_WITH_QUIT:
    printf("Bye.\n");
    break;
    case MSG_SUCCESS_DOT_FILE_PARSING:
    printf("File parsing successful...\n");
    break;
  }
}