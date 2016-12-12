//-----------------------------------------------------------------------------
// assa.c
//
// Program take releationships in family tree and put it in memory, by wish
// program can print list or new documet with whole or part tree.
//
// Group: 3 study assistant Lorenz Kofler
//
// Authors: Djordje Rajic  TODO:<Matriculum Number> Stefan Rajinovic 1431905
//
//-----------------------------------------------------------------------------
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
#define MSG_SUCCESS_CREATING_DOT_FILE 11
//ERRORS
#define ERROR_TO_MANY_ARGUMENTS_WHILE_LOADING_FILE 1
#define ERROR_OUT_OF_MEMORY 2
#define ERROR_FILE_COULD_NOT_BE_READ 3
#define ERROR_FILE_COULD_NOT_BE_WRITTEN 4
#define ERROR_WRONG_DRAW_ALL_USAGE 5
#define ERROR_WRONG_ADD_USAGE 6
#define ERROR_WRONG_LIST_USAGE 7
#define ERROR_NO_ENTRIES_AVAILABLE 8
#define ERROR_BOTH_PEOPLE_ARE_THE_SAME 9
#define ERROR_SEX_DOES_NOT_MATCH 10
#define ERROR_RELATION_NOT_POSSIBLE 11
//String lenghts
#define INPUT_COMMAND_LENGHT 530 //NOTE: 256 for one name + 256 second name + 
// 6 for genders + some whitespaces + longest command = around 530
#define MAX_NAME_LENGHT 257 // +1 for \0
//Msc
#define INIT_PERSONS_ARRAY_SIZE 100

typedef struct _Person_
{
  char name_[MAX_NAME_LENGHT];
  BOOL gender_;
  struct _Person_ *mother_;
  struct _Person_ *father_;
}Person;


// forward declaration
Person *parseDotFile(char *file_content);

// forward declaration
BOOL nameIsUnknown(const char *name);

// forward declaration
BOOL fileExists(const char *file_name);

// forward declaration
BOOL fileIsWritable(const char *file_name);

// forward declaration
BOOL parseSingleFileLine(char *line_to_parse, char *name, BOOL *gender_b, char
*parrent_name, BOOL *parrent_gender_b);

// forward declaration
Person createPersonInstance(char *name, BOOL gender, Person *mother, Person
*father);

// forward declaration
Person *findPerson(Person *persons, char const *name, BOOL gender);

// forward declaration
void showError(short error_code);

// forward declaration
void showSuccessMessage(short msg_code);

// forward declaration
char *storeFileIntoMemory(const char *file_name);

// forward declaration
BOOL listPersons(Person *persons);

// forward declaration
void parseInput(char *input_command, Person *persons_array);

// forward declaration
void waitForInput(Person *persons_array);

// forward declaration
BOOL parseAddInput(char *input_command, Person *array_of_persons);

// forward declaration
BOOL copyPerson(Person *first_person, Person *second_person);

// forward declaration
void parseDrawInput(char *input_command);

// forward declaration
void parseRelationshipInput(char *input_command);

// forward declaration
char *parseDrawAllInput(char *input_command, Person *persons);

// forward declaration
BOOL sortPersons(Person *persons);

// forward declaration
int numberOfPersons(Person *persons);

// forward declaration
BOOL writePersonToFile(char *file_name, Person *persons_to_write);

// forward declaration
void addRelationship(char const *first_person_name, BOOL first_person_gender,
char const *second_person_name, BOOL second_person_gender, char const
*relationship, Person *array_of_persons);

// forward declaration
Person *addUnknownPerson(Person *array_of_persons, BOOL gender);

//------------------------------------------------------------------------------
///
/// The main program.
/// Checking the number of parameters, giving the necessary errors, passing the
/// data to right function for further processing.
///
/// @param argc check that must be exactly 2 argumments
/// @param argv is used to describe file_name
///
/// @return success program closed
//
int main(int argc, char **argv)
{
  if(argc == 1)
  {
    Person *persons_array = (Person*)malloc(sizeof(Person)*INIT_PERSONS_ARRAY_SIZE);
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

//------------------------------------------------------------------------------
///
/// We are parsing the file contents that have been loaded into memory, into
/// string
///
/// @param file_content string, with dot file data to be parsed
///
/// @return array_of_persons, array of persons and their data, dynamicly
/// allocated
//
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
      *(file_content + counter) = '\0'; //Puting null byte insted of newline so
      // the string is later valid for use with function from string.h
      lines_separated[lines_separated_counter] = file_content + file_content_counter;

      file_content_counter = counter + 1;
      lines_separated_counter++;
    }
    ++counter;
  }
  // Checking if file format is valid, otherwise give error acordingly
  if(strcmp(lines_separated[0],"digraph FamilyTree") != 0 || strcmp
    (lines_separated[1],"{") != 0 || file_content[counter-1] != '}')
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

  Person *array_of_persons = (Person*)malloc(sizeof(Person)*
  (lines_separated_counter*10)); //Big chunk of memory for persons, it'll be
  // reallocated later
  array_of_persons[(lines_separated_counter*10)-1].gender_ = 3;
  for(counter = 2; counter < lines_separated_counter; counter++ )
  {
    if(parseSingleFileLine(lines_separated[counter], name, &gender, parrent_name, &parrent_gender) == FALSE)
    {
      showError(ERROR_FILE_COULD_NOT_BE_READ);
      exit(ERROR_FILE_COULD_NOT_BE_READ);
    }
    name[strlen(name)-1] = '\0'; // NOTE: Getting rid of last empty space
    if(findPerson(array_of_persons,name,gender) == NULL)
    {
      strcpy(array_of_persons[number_of_persons].name_,name);
      array_of_persons[number_of_persons].gender_ = gender;
      if(parrent_name[0] != '\0')
      {
        if(findPerson(array_of_persons,parrent_name,parrent_gender) == NULL)
        {
          strcpy(array_of_persons[number_of_persons+1].name_,parrent_name);
          array_of_persons[number_of_persons+1].gender_ = parrent_gender;
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
            new_temp_person->father_ = &array_of_persons[number_of_persons];
            number_of_persons++;
          }
          else
          {
            new_temp_person->father_ = temp_parrent_father;
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

//------------------------------------------------------------------------------
///
/// Parsing a signle line from dot file, storing data into pointers
///
/// @param line_to_parse which line line is check
/// @param name check correctes of name 
/// @param gender_b check gender of first person
/// @param parrant_name check name of second person
/// @param parrant_gender_b check gender of second person
///
/// @return TRUE or FALSE, FALSE if an error occured, meaning line is not valid
/// TRUE if everything went okay and the line from file is valid
//
BOOL parseSingleFileLine(char *line_to_parse, char *name, BOOL *gender_b, char *parrent_name, BOOL *parrent_gender_b)
{
  if(*(line_to_parse) != ' ' || *(line_to_parse + 1) != ' ' || *(line_to_parse + 2) != '"')
  {
    return FALSE;
  }
  int counter = 3;
  int null_counter = 0;
  while(*(line_to_parse + counter) != '[')
  {
    *(name+null_counter) = *(line_to_parse + counter);
    counter++;
    null_counter++;
    if(counter >= MAX_NAME_LENGHT || *(line_to_parse + counter) == ']' || *(line_to_parse + counter) == '>' || *(line_to_parse + counter) == ';' || *(line_to_parse + counter) == '"')
    {
      return FALSE;
    }
  }
  *(name + (null_counter)) = '\0';
  counter+=2; // We are skipping [
  if(*(line_to_parse + counter) != ']')
  {
    return FALSE;
  }
  if(*(line_to_parse + (counter - 1)) != 'f' && *(line_to_parse + (counter - 1)) != 'm')
  {
    return FALSE;
  }
  *gender_b = (*(line_to_parse + (counter - 1)) == 'f') ? TRUE : FALSE;
  ++counter;
  if(*(line_to_parse + counter) == ' ' || *(line_to_parse + (counter + 1)) == ';')
  {
    *parrent_name = '\0';
    return TRUE;
  }
  counter += 2;
  if(*(line_to_parse + (counter)) != '-' || *(line_to_parse + (counter + 1)) != '>' || *(line_to_parse + (counter + 2)) != ' ' || *(line_to_parse + (counter + 3)) != '"')
  {
    return FALSE;
  }
  counter += 4;
  null_counter = 0;
  while(*(line_to_parse + counter) != '[')
  {
    *(parrent_name + null_counter) = *(line_to_parse + counter);
    counter++;
    null_counter++;
    if(counter >= MAX_NAME_LENGHT || *(line_to_parse + counter) == ']' || *(line_to_parse + counter) == '>' || *(line_to_parse + counter) == ';' || *(line_to_parse + counter) == '"')
    {
      return FALSE;
    }
  }
  *(parrent_name + (null_counter - 1)) = '\0';
  counter += 2; // We are skipping [
  if(*(line_to_parse + counter) != ']' || *(line_to_parse + (counter + 1)) != '"' || *(line_to_parse + (counter + 2)) != ';')
  {
    return FALSE;
  }
  if(*(line_to_parse + (counter - 1)) != 'f' && *(line_to_parse + (counter - 1)) != 'm')
  {
    return FALSE;
  }
  *parrent_gender_b = (*(line_to_parse + (counter - 1)) == 'f') ? TRUE : FALSE;

 return TRUE;
}

//------------------------------------------------------------------------------
///
/// Check if name is Unknown
///
/// @param name name of person for checking
///
/// @return TRUE or FALSE
//
BOOL nameIsUnknown(const char *name)
{
  return (name[0] == '?' && isdigit(name[1])) ? TRUE : FALSE;
}


//------------------------------------------------------------------------------
///
/// Parse the input command 
///	check command input and parse it
///
/// @param input_command, name of command 
/// @param pesons_array is array of persons
///
/// @return 
//
void parseInput(char *input_command, Person *persons_array)
{
  if(strcmp(input_command, "quit\n") == 0)
  {
    free(persons_array);
    showSuccessMessage(MSG_SUCCESS_PROGRAM_CLOSED_WITH_QUIT);
    exit(SUCCESS_PROGRAM_CLOSED);
  }
  if(strcmp(input_command, "list\n") == 0)
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
    *(input_command + counter) = '\0';
    counter += 1;
    char *command = input_command;
    if(strcmp(command, "list") == 0)
    {
      showError(ERROR_WRONG_LIST_USAGE);
    }
    if(strcmp(command, "add") == 0)
    {
      if(!parseAddInput(input_command, persons_array))
      {
        showError(ERROR_WRONG_ADD_USAGE);
      }
    }
    if(strcmp(command, "draw") == 0)
    {
      parseDrawInput(input_command);
    }
    if(strcmp(command, "draw-all") == 0)
    {
      char *file_name = parseDrawAllInput(input_command, persons_array);
      if(file_name == NULL)
      {
        showError(ERROR_WRONG_DRAW_ALL_USAGE);
      }
      else
      {
        if(numberOfPersons(persons_array) <= 1)
        {
          showError(ERROR_NO_ENTRIES_AVAILABLE);
        }
        else
        {
          if(!writePersonToFile(file_name, persons_array))
          {
            showError(ERROR_FILE_COULD_NOT_BE_WRITTEN);
          }
        }
      }
    }
    if(strcmp(command, "relationship") == 0)
    {
      parseRelationshipInput(input_command);
    }
  }
}

//------------------------------------------------------------------------------
///
/// Parse add input
///	check if correct format of input relationship is, and parse it
///
/// @param input_command, name of command 
/// @param pesons_array is array of persons
///
/// @return TRUE/FALSE
//
BOOL parseAddInput(char *input_command, Person *array_of_persons)
{
  int counter = 4;
  while(*(input_command + counter) != '[' && *(input_command + counter) != '\0')
  {
    counter++;
    if(*(input_command + counter) == '\n')
    {
      return FALSE;
    }
  }
  if(*(input_command + (counter - 1)) != ' ')
  {
    return FALSE;
  }
  *(input_command + (counter - 1)) = '\0';
  char *first_person_name = input_command + 4;
  if(*(input_command + counter) != '[' || *(input_command + (counter + 2)) != ']' || *(input_command + (counter + 3)) != ' ')
  {
    return FALSE;
  }
  counter++;
  if(*(input_command + counter) != 'f' && *(input_command + counter) != 'm')
  {
    return FALSE;
  }
  BOOL first_person_gender = (*(input_command + counter) == 'f') ? TRUE : FALSE;
  counter += 3;
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
  char *second_person_name = input_command + (counter + 1);
  while(*(input_command + counter) != '[')
  {
    counter++;
    if(*(input_command + counter) == '\n')
    {
      return FALSE;
    }
  }
  if(*(input_command + (counter - 1)) != ' ')
  {
    return FALSE;
  }
  *(input_command + (counter - 1)) = '\0';

  if(*(input_command + counter) != '[' || *(input_command + (counter + 2)) != ']' || *(input_command + (counter + 3)) != '\n')
  {
    return FALSE;
  }
  counter++;
  if(*(input_command + counter) != 'f' && *(input_command + counter) != 'm')
  {
    return FALSE;
  }
  BOOL second_person_gender = (*(input_command + counter) == 'f') ? TRUE : FALSE;

  if(strcmp(relationship, "mother") != 0 && strcmp(relationship, "father") != 0 && strcmp(relationship, "mgm") != 0 && strcmp(relationship, "fgm") != 0 && strcmp(relationship, "mgf") != 0 && strcmp(relationship, "fgf") != 0)
  {
    return FALSE;
  }

  addRelationship(first_person_name, first_person_gender, second_person_name, second_person_gender, relationship,array_of_persons);

  return TRUE;
  //memset(input_command,0,INPUT_COMMAND_LENGHT);
}

//------------------------------------------------------------------------------
///
/// Add Relation ship 
///	Store the person in struct, person perent, 
/// and person grandmother/grandfather
///
/// @param first_person_name 
/// @param first_person_gender
/// @param second_person_name
/// @param second_person_gender
/// @param relationship
/// @param array_of_persons
///
/// @return 
//
void addRelationship(char const *first_person_name, BOOL first_person_gender, char const *second_person_name, BOOL second_person_gender, char const *relationship, Person *array_of_persons)
{
  if(strcmp(first_person_name, second_person_name) == 0 && first_person_gender == second_person_gender)
  {
    showError(ERROR_BOTH_PEOPLE_ARE_THE_SAME);
  }
  else
  {
    if(strcmp(relationship, "mother") == 0)
    {
      if(first_person_gender != 1)
      {
        showError(ERROR_SEX_DOES_NOT_MATCH);
      }
      else
      {
        if(findPerson(array_of_persons, second_person_name, second_person_gender) != NULL)
        {
          Person *child = findPerson(array_of_persons, second_person_name, second_person_gender);
          if(child->mother_ == NULL || nameIsUnknown(child->mother_->name_))
          {
            if(child->mother_ != NULL && nameIsUnknown(child->mother_->name_))
            {
              Person *mother = findPerson(array_of_persons, child->mother_->name_, 1);
              strcpy(mother->name_, first_person_name);
            }
            else
            {
              if(findPerson(array_of_persons, first_person_name, first_person_gender) != NULL)
              {
                child->mother_ = findPerson(array_of_persons, first_person_name, first_person_gender);
              }
              else
              {
                int number_of_persons = numberOfPersons(array_of_persons);
                array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 2));
                strcpy(array_of_persons[number_of_persons].name_, first_person_name);
                array_of_persons[number_of_persons].gender_ = first_person_gender;
                array_of_persons[number_of_persons].mother_ = NULL;
                array_of_persons[number_of_persons].father_ = NULL;
                array_of_persons[number_of_persons + 1].gender_ = 3;

                child->mother_ = &array_of_persons[number_of_persons];
              }
            }
          }
          else
          {
            showError(ERROR_RELATION_NOT_POSSIBLE);
          }
        }
        else
        {
          int number_of_persons = numberOfPersons(array_of_persons);
          if (number_of_persons == 0)
          {
            strcpy(array_of_persons[0].name_, second_person_name);
            array_of_persons[0].gender_ = second_person_gender;
            array_of_persons[0].father_ = NULL;
            strcpy(array_of_persons[1].name_, first_person_name);
            array_of_persons[1].gender_ = 1;
            array_of_persons[1].mother_ = NULL;
            array_of_persons[1].father_ = NULL;
            array_of_persons[2].gender_ = 3;
            array_of_persons[0].mother_ = (array_of_persons + 1);
          }
          else
          {
            if(findPerson(array_of_persons, first_person_name, first_person_gender))
            {
              array_of_persons = realloc(array_of_persons,sizeof(Person)*(number_of_persons + 2));
              strcpy(array_of_persons[number_of_persons].name_, second_person_name);
              array_of_persons[number_of_persons].gender_ = first_person_gender;
              array_of_persons[number_of_persons].father_ = NULL;
              array_of_persons[number_of_persons].mother_ = findPerson(array_of_persons, first_person_name, first_person_gender);
              array_of_persons[number_of_persons + 1].gender_ = 3;
            }
            else
            {
              array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 3));
              strcpy(array_of_persons[number_of_persons].name_, first_person_name);
              array_of_persons[number_of_persons].gender_ = 1;
              array_of_persons[number_of_persons].mother_ = NULL;
              array_of_persons[number_of_persons].father_ = NULL;
              strcpy(array_of_persons[number_of_persons + 1].name_, second_person_name);
              array_of_persons[number_of_persons + 1].gender_ = second_person_gender;
              array_of_persons[number_of_persons + 1].mother_ = &array_of_persons[number_of_persons];
              array_of_persons[number_of_persons + 1].father_ = NULL;
              array_of_persons[number_of_persons + 2].gender_ = 3;
            }
          }
        }
      }
    }
    if(strcmp(relationship, "father") == 0)
    {
      if(first_person_gender != 0)
      {
        showError(ERROR_SEX_DOES_NOT_MATCH);
      }
      else
      {
        if(findPerson(array_of_persons, second_person_name, second_person_gender) != NULL)
        {
          Person *child = findPerson(array_of_persons, second_person_name, second_person_gender);
          if(child->father_ == NULL || nameIsUnknown(child->father_->name_))
          {
            if(child->father_ != NULL && nameIsUnknown(child->father_->name_))
            {
              Person *father = findPerson(array_of_persons, child->father_->name_, 0);
              strcpy(father->name_, first_person_name);
            }
            else
            {
              if(findPerson(array_of_persons, first_person_name, first_person_gender) != NULL)
              {
                child->father_ = findPerson(array_of_persons, first_person_name, first_person_gender);
              }
              else
              {
                int number_of_persons = numberOfPersons(array_of_persons);
                array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 2));
                strcpy(array_of_persons[number_of_persons].name_, first_person_name);
                array_of_persons[number_of_persons].gender_ = first_person_gender;
                array_of_persons[number_of_persons].mother_ = NULL;
                array_of_persons[number_of_persons].father_ = NULL;
                array_of_persons[number_of_persons + 1].gender_ = 3;

                child->father_ = &array_of_persons[number_of_persons];
              }
            }
          }
          else
          {
            showError(ERROR_RELATION_NOT_POSSIBLE);
          }
        }
        else
        {
          int number_of_persons = numberOfPersons(array_of_persons);
          if (number_of_persons == 0)
          {
            strcpy(array_of_persons[0].name_, second_person_name);
            array_of_persons[0].gender_ = second_person_gender;
            array_of_persons[0].mother_ = NULL;
            strcpy(array_of_persons[1].name_,first_person_name);
            array_of_persons[1].gender_ = 0;
            array_of_persons[1].mother_ = NULL;
            array_of_persons[1].father_ = NULL;
            array_of_persons[2].gender_ = 3;
            array_of_persons[0].father_ = (array_of_persons + 1);
          }
          else
          {
            if(findPerson(array_of_persons, first_person_name, first_person_gender))
            {
              array_of_persons = realloc(array_of_persons,sizeof(Person)*(number_of_persons + 2));
              strcpy(array_of_persons[number_of_persons].name_, second_person_name);
              array_of_persons[number_of_persons].gender_ = first_person_gender;
              array_of_persons[number_of_persons].mother_ = NULL;
              array_of_persons[number_of_persons].father_ = findPerson(array_of_persons, first_person_name, first_person_gender);
              array_of_persons[number_of_persons + 1].gender_ = 3;
            }
            else
            {
              array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 3));
              strcpy(array_of_persons[number_of_persons].name_, first_person_name);
              array_of_persons[number_of_persons].gender_ = 0;
              array_of_persons[number_of_persons].mother_ = NULL;
              array_of_persons[number_of_persons].father_ = NULL;
              strcpy(array_of_persons[number_of_persons+1].name_, second_person_name);
              array_of_persons[number_of_persons + 1].gender_ = second_person_gender;
              array_of_persons[number_of_persons + 1].father_ = &array_of_persons[number_of_persons];
              array_of_persons[number_of_persons + 1].mother_ = NULL;
              array_of_persons[number_of_persons + 2].gender_ = 3;
            }
          }
        } 
      }
    }
    if(strcmp(relationship, "mgm") == 0)
    {
      if(first_person_gender != 1)
      {
        showError(ERROR_SEX_DOES_NOT_MATCH);
      }
      else
      {
        if(findPerson(array_of_persons, second_person_name, second_person_gender) != NULL)
        {
          Person *child = findPerson(array_of_persons, second_person_name, second_person_gender);
          if(child->mother_ != NULL)
          {
            if(child->mother_->mother_ == NULL)
            {
              int number_of_persons = numberOfPersons(array_of_persons);
              array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 2));
              strcpy(array_of_persons[number_of_persons].name_, first_person_name);
              array_of_persons[number_of_persons].gender_ = first_person_gender;
              array_of_persons[number_of_persons].mother_ = NULL;
              array_of_persons[number_of_persons].father_ = NULL;
              array_of_persons[number_of_persons + 1].gender_ = 3;
              child->mother_->mother_ = &array_of_persons[number_of_persons];
            }
            else if(child->mother_->mother_ != NULL && nameIsUnknown(child->mother_->mother_->name_))
            {
              strcpy(child->mother_->mother_->name_, first_person_name);
            }
            else
            {
              showError(ERROR_RELATION_NOT_POSSIBLE);
            }
          }
          else if(child->mother_ == NULL)
          {
            child->mother_ = addUnknownPerson(array_of_persons, first_person_gender);
            int number_of_persons = numberOfPersons(array_of_persons);
            array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 2));
            strcpy(array_of_persons[number_of_persons].name_, first_person_name);
            array_of_persons[number_of_persons].gender_ = first_person_gender;
            array_of_persons[number_of_persons].father_ = NULL;
            array_of_persons[number_of_persons + 1].gender_ = 3;
            child->mother_->mother_ = &array_of_persons[number_of_persons];
          }
        }
        else
        {
          Person *mother = addUnknownPerson(array_of_persons, 1);
          int number_of_persons = numberOfPersons(array_of_persons);
          array_of_persons = realloc(array_of_persons, sizeof(Person) * (number_of_persons + 3));
          strcpy(array_of_persons[number_of_persons].name_, second_person_name);
          array_of_persons[number_of_persons].gender_ = second_person_gender;
          array_of_persons[number_of_persons].father_ = NULL;
          strcpy(array_of_persons[number_of_persons + 1].name_, first_person_name);
          array_of_persons[number_of_persons + 1].gender_ = first_person_gender;
          array_of_persons[number_of_persons + 1].father_ = NULL; 
          array_of_persons[number_of_persons + 1].mother_ = NULL;
          array_of_persons[number_of_persons + 2].gender_ = 3;
          array_of_persons[number_of_persons].mother_ = mother;
          mother->mother_ = &array_of_persons[number_of_persons + 1];
        }
      }
    }
    if(strcmp(relationship, "fgm") == 0)
    {
      if(first_person_gender != 1)
      {
        showError(ERROR_SEX_DOES_NOT_MATCH);
      }
      else
      {
        if(findPerson(array_of_persons, second_person_name, second_person_gender) != NULL)
        {
          Person *child = findPerson(array_of_persons, second_person_name, second_person_gender);
          if(child->father_ != NULL)
          {
            if(child->father_->mother_ == NULL)
            {
              int number_of_persons = numberOfPersons(array_of_persons);
              array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 2));
              strcpy(array_of_persons[number_of_persons].name_, first_person_name);
              array_of_persons[number_of_persons].gender_ = first_person_gender;
              array_of_persons[number_of_persons].mother_ = NULL;
              array_of_persons[number_of_persons].father_ = NULL;
              array_of_persons[number_of_persons + 1].gender_ = 3;
              child->father_->mother_ = &array_of_persons[number_of_persons];
            }
            else if(child->father_->mother_ != NULL && nameIsUnknown(child->father_->mother_->name_))
            {
              strcpy(child->father_->mother_->name_, first_person_name);
            }
            else
            {
              showError(ERROR_RELATION_NOT_POSSIBLE);
            }
          }
          else if(child->father_ == NULL)
          {
            child->father_ = addUnknownPerson(array_of_persons, first_person_gender);
            int number_of_persons = numberOfPersons(array_of_persons);
            array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 2));
            strcpy(array_of_persons[number_of_persons].name_, first_person_name);
            array_of_persons[number_of_persons].gender_ = first_person_gender;
            array_of_persons[number_of_persons].mother_ = NULL;
            array_of_persons[number_of_persons].father_ = NULL;
            array_of_persons[number_of_persons + 1].gender_ = 3;
            child->father_->mother_ = &array_of_persons[number_of_persons];
          }
        }
        else
        {
          Person *father = addUnknownPerson(array_of_persons, 0);
          int number_of_persons = numberOfPersons(array_of_persons);
          array_of_persons = realloc(array_of_persons, sizeof(Person) * (number_of_persons + 3));
          strcpy(array_of_persons[number_of_persons].name_, second_person_name);
          array_of_persons[number_of_persons].gender_ = second_person_gender;
          array_of_persons[number_of_persons].mother_ = NULL;
          strcpy(array_of_persons[number_of_persons + 1].name_, first_person_name);
          array_of_persons[number_of_persons + 1].gender_ = 1;
          array_of_persons[number_of_persons + 1].father_ = NULL; 
          array_of_persons[number_of_persons + 1].mother_ = NULL;
          array_of_persons[number_of_persons + 2].gender_ = 3;
          array_of_persons[number_of_persons].father_ = father;
          father->mother_ = &array_of_persons[number_of_persons + 1];
        }
      }
    }
    if(strcmp(relationship, "mgf") == 0)
    {
      if(first_person_gender != 0)
      {
        showError(ERROR_SEX_DOES_NOT_MATCH);
      }
      else
      {
        if(findPerson(array_of_persons, second_person_name, second_person_gender) != NULL)
        {
          Person *child = findPerson(array_of_persons, second_person_name, second_person_gender);
          if(child->mother_ != NULL)
          {
            if(child->mother_->father_ == NULL)
            {
              int number_of_persons = numberOfPersons(array_of_persons);
              array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 2));
              strcpy(array_of_persons[number_of_persons].name_, first_person_name);
              array_of_persons[number_of_persons].gender_ = first_person_gender;
              array_of_persons[number_of_persons].mother_ = NULL;
              array_of_persons[number_of_persons].father_ = NULL;
              array_of_persons[number_of_persons + 1].gender_ = 3;
              child->mother_->father_ = &array_of_persons[number_of_persons];
            }
            else if(child->mother_->father_ != NULL && nameIsUnknown(child->mother_->father_->name_))
            {
              strcpy(child->mother_->father_->name_, first_person_name);
            }
            else
            {
              showError(ERROR_RELATION_NOT_POSSIBLE);
            }
          }
          else if(child->mother_ == NULL)
          {
            child->mother_ = addUnknownPerson(array_of_persons, first_person_gender);
            int number_of_persons = numberOfPersons(array_of_persons);
            array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 2));
            strcpy(array_of_persons[number_of_persons].name_,first_person_name);
            array_of_persons[number_of_persons].gender_ = first_person_gender;
            array_of_persons[number_of_persons].mother_ = NULL;
            array_of_persons[number_of_persons + 1].gender_ = 3;
            child->mother_->father_ = &array_of_persons[number_of_persons];
          }
        }
        else
        {
          Person *mother = addUnknownPerson(array_of_persons, 1);
          int number_of_persons = numberOfPersons(array_of_persons);
          array_of_persons = realloc(array_of_persons, sizeof(Person) * (number_of_persons + 3));
          strcpy(array_of_persons[number_of_persons].name_, second_person_name);
          array_of_persons[number_of_persons].gender_ = second_person_gender;
          array_of_persons[number_of_persons].mother_ = NULL;
          strcpy(array_of_persons[number_of_persons + 1].name_, first_person_name);
          array_of_persons[number_of_persons + 1].gender_ = first_person_gender;
          array_of_persons[number_of_persons + 1].father_ = NULL; 
          array_of_persons[number_of_persons + 1].mother_ = NULL;
          array_of_persons[number_of_persons + 2].gender_ = 3;
          array_of_persons[number_of_persons].mother_ = mother;
          mother->father_ = &array_of_persons[number_of_persons + 1];
        }
      }
    }
    if(strcmp(relationship, "fgf") == 0)
    {
      if(first_person_gender != 0)
      {
        showError(ERROR_SEX_DOES_NOT_MATCH);
      }
      else
      {
        if(findPerson(array_of_persons, second_person_name, second_person_gender) != NULL)
        {
          Person *child = findPerson(array_of_persons, second_person_name, second_person_gender);
          if(child->father_ != NULL)
          {
            if(child->father_->father_ == NULL)
            {
              int number_of_persons = numberOfPersons(array_of_persons);
              array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 2));
              strcpy(array_of_persons[number_of_persons].name_,first_person_name);
              array_of_persons[number_of_persons].gender_ = first_person_gender;
              array_of_persons[number_of_persons].mother_ = NULL;
              array_of_persons[number_of_persons].father_ = NULL;
              array_of_persons[number_of_persons + 1].gender_ = 3;
              child->father_->father_ = &array_of_persons[number_of_persons];
            }
            else if(child->father_->father_ != NULL && nameIsUnknown(child->father_->father_->name_))
            {
              strcpy(child->father_->father_->name_, first_person_name);
            }
            else
            {
              showError(ERROR_RELATION_NOT_POSSIBLE);
            }
          }
          else if(child->father_ == NULL)
          {
            child->father_ = addUnknownPerson(array_of_persons, first_person_gender);
            int number_of_persons = numberOfPersons(array_of_persons);
            array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 2));
            strcpy(array_of_persons[number_of_persons].name_, first_person_name);
            array_of_persons[number_of_persons].gender_ = first_person_gender;
            array_of_persons[number_of_persons].mother_ = NULL;
            array_of_persons[number_of_persons + 1].gender_ = 3;
            child->father_->father_ = &array_of_persons[number_of_persons];
          }
        }
        else
        {
          Person *father = addUnknownPerson(array_of_persons, 0);
          int number_of_persons = numberOfPersons(array_of_persons);
          array_of_persons = realloc(array_of_persons, sizeof(Person) * (number_of_persons + 3));
          strcpy(array_of_persons[number_of_persons].name_, second_person_name);
          array_of_persons[number_of_persons].gender_ = second_person_gender;
          array_of_persons[number_of_persons].mother_ = NULL;
          strcpy(array_of_persons[number_of_persons + 1].name_, first_person_name);
          array_of_persons[number_of_persons + 1].gender_ = 1;
          array_of_persons[number_of_persons + 1].father_ = NULL; 
          array_of_persons[number_of_persons + 1].mother_ = NULL;
          array_of_persons[number_of_persons + 2].gender_ = 3;
          array_of_persons[number_of_persons].father_ = father;
          father->father_ = &array_of_persons[number_of_persons + 1];
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
///
/// Add Relation  unknown person 
///	Add person which we dont't know "?1 [m/f]"
///
/// @param array_of_person
/// @param gender
///
/// @return array_of_person[number_of_person]
//
Person *addUnknownPerson(Person *array_of_persons, BOOL gender)
{
  static unsigned long unknown_person_count = 1;
  int number_of_persons = numberOfPersons(array_of_persons);
  char buffer[MAX_NAME_LENGHT - 1];
  char unknown[MAX_NAME_LENGHT] = "?";
  snprintf(buffer, 10, "%lu", unknown_person_count);
  strcat(unknown, buffer);
  array_of_persons = realloc(array_of_persons, sizeof(Person)*(number_of_persons + 2));
  strcpy(array_of_persons[number_of_persons].name_, unknown);
  array_of_persons[number_of_persons].gender_ = gender;
  array_of_persons[number_of_persons].mother_ = NULL;
  array_of_persons[number_of_persons].father_ = NULL;
  array_of_persons[number_of_persons + 1].gender_ = 3;
  unknown_person_count++;
  return &array_of_persons[number_of_persons];
}

//------------------------------------------------------------------------------
///
/// TODO:
///
/// @param input_command
///
/// @return 
//
void parseDrawInput(char *input_command)
{
  printf("Parse draw input ...\n");
}

//------------------------------------------------------------------------------
///
/// Parse Draw ALl input
/// make file with end ".dot" and store persons in file, to be ready to draw
///
/// @param input_command
/// @param persons 
///
/// @return file_name with end .dot
//
char *parseDrawAllInput(char *input_command, Person *persons)
{
  int counter = 0;
  char *file_name = input_command + 9;
  if(strlen(file_name) <= 1)
  {
    return NULL;
  }
  if(*(file_name + counter) == ' ')
  {
    return NULL;
  }
  while(*(file_name + counter) != '\n' && *(file_name + counter) != '\0')
  {
    if(counter > INPUT_COMMAND_LENGHT)
    {
      return NULL;
    }
    counter++;
  }
  *(file_name + counter) = '\0';
  return strcat(file_name,".dot");
}

//------------------------------------------------------------------------------
///
/// TODO:
///
/// @param input_command
///
/// @return 
//
void parseRelationshipInput(char *input_command)
{
  printf("Parse elationship input ...\n");
}

//------------------------------------------------------------------------------
///
/// Wait for input
/// put on screen "esp>" while program is running, and close program if EOF 
/// make free on close
///
/// @param person_array
///
/// @return 
//
void waitForInput(Person *persons_array)
{
  char input_command[INPUT_COMMAND_LENGHT] ;
  while(TRUE)
  {
    printf("esp>");
    fgets(input_command,INPUT_COMMAND_LENGHT, stdin);
    if(strlen(input_command) != 1 && !feof(stdin))
    {
        parseInput(input_command, persons_array);
    }
    if (feof(stdin))
    {
      free(persons_array);
      showSuccessMessage(MSG_SUCCESS_PROGRAM_CLOSED_WITH_EOF);
      exit(SUCCESS_PROGRAM_CLOSED);
    }
  }
}

//------------------------------------------------------------------------------
///
/// File exists
/// check if file exist
///
/// @param file_name
///
/// @return TRUE/FALSE
//
BOOL fileExists(const char *file_name) 
{
  FILE *file_stream;
  if((file_stream = fopen(file_name, "r")))
  {
      fclose(file_stream);
      return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
///
/// Is file Writable
/// check if file writable
///
/// @param file_name
///
/// @return TRUE/FALSE
//
BOOL fileIsWritable(const char *file_name)
{
  FILE *file_stream;
  if((file_stream = fopen(file_name, "w")))
  {
      fclose(file_stream);
      return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
///
/// Write person to file
/// Is function in which we put/write persons in file
///
/// @param file_name
/// @param persons_to_write
///
/// @return TRUE/FALSE
//
BOOL writePersonToFile(char *file_name, Person *persons_to_write)
{
  if(fileExists(file_name) && !fileIsWritable(file_name))
  {
    return FALSE;
  }
  if(fileExists(file_name) && fileIsWritable(file_name))
  {
    FILE *file_stream;
    int counter = 0;
    file_stream = fopen(file_name, "w");
    fprintf(file_stream, "digraph FamilyTree\n");
    fprintf(file_stream, "{\n");
    //sortPersons(persons_to_write);
    while((persons_to_write + counter)->gender_ != 3)
    {
      if((persons_to_write + counter)->mother_ != NULL)
      {
        fprintf(file_stream, "  \"%s [%c]\" -> \"%s [%c]\";\n", (persons_to_write + counter)->name_, ((persons_to_write + counter)->gender_ == TRUE) ? 'f' : 'm',
          ((persons_to_write + counter)->mother_)->name_, 'f');
      }
      if((persons_to_write + counter)->father_ != NULL)
      {
        fprintf(file_stream, "  \"%s [%c]\" -> \"%s [%c]\";\n", (persons_to_write + counter)->name_,
          (((persons_to_write + counter)->gender_) == TRUE) ? 'f' : 'm', ((persons_to_write + counter)->father_)->name_, 'm');
      }
      if((persons_to_write + counter)->father_ == NULL && (persons_to_write + counter)->mother_ == NULL)
      {
        fprintf(file_stream, "  \"%s [%c]\";\n", (persons_to_write + counter)->name_, ((persons_to_write + counter)->gender_ == TRUE) ? 'f' : 'm');
      }
      counter++;
    }
    fprintf(file_stream, "}");
    showSuccessMessage(MSG_SUCCESS_CREATING_DOT_FILE);
    fclose(file_stream);
    return TRUE;
  }
  else if(!fileExists(file_name))
  {
    FILE *file_stream;
    int counter = 0;
    file_stream = fopen(file_name, "wb");
    fprintf(file_stream, "digraph FamilyTree\n");
    fprintf(file_stream, "{\n");
    //sortPersons(persons_to_write);
    while((persons_to_write + counter)->gender_ != 3)
    {
      if((persons_to_write + counter)->mother_ != NULL)
      {
        fprintf(file_stream, "  \"%s [%c]\" -> \"%s [%c]\";\n", (persons_to_write + counter)->name_, ((persons_to_write + counter)->gender_ == TRUE) ? 'f' : 'm',
          ((persons_to_write + counter)->mother_)->name_, 'f');
      }
      if((persons_to_write + counter)->father_ != NULL)
      {
        fprintf(file_stream, "  \"%s [%c]\" -> \"%s [%c]\";\n", (persons_to_write + counter)->name_, 
          (((persons_to_write + counter)->gender_) == TRUE) ? 'f' : 'm', ((persons_to_write + counter)->father_)->name_, 'm');
      }
      if((persons_to_write + counter)->father_ == NULL && (persons_to_write + counter)->mother_ == NULL)
      {
        fprintf(file_stream, "  \"%s [%c]\";\n",(persons_to_write + counter)->name_, ((persons_to_write + counter)->gender_ == TRUE) ? 'f' : 'm');
      }
      counter++;
    }
    fprintf(file_stream, "}");
    showSuccessMessage(MSG_SUCCESS_CREATING_DOT_FILE);
    fclose(file_stream);
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
///
/// Store file into memory
/// TODO:
///
/// @param file_name
///
/// @return file_content
//
char *storeFileIntoMemory(const char *file_name)
{
  char *file_content = NULL;
  FILE *file_stream = fopen(file_name, "r");
  int size;
  if(file_stream != NULL)
  {
      fseek (file_stream, 0, SEEK_END);
      size = ftell(file_stream);
      if(size == 0)
      {
        showError(ERROR_FILE_COULD_NOT_BE_READ);
        exit(ERROR_FILE_COULD_NOT_BE_READ);
      }
      if(fseek(file_stream, 0L, SEEK_END) == 0)
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
          if(fseek(file_stream, 0L, SEEK_SET) != 0)
          {
            free(file_content);
            showError(ERROR_FILE_COULD_NOT_BE_READ);
            exit(ERROR_FILE_COULD_NOT_BE_READ);
          }
          size_t new_lenght = fread(file_content, sizeof(char), buffer_size, file_stream);
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

//------------------------------------------------------------------------------
///
/// Sort Persons
///
/// @param persons
///
/// @return TRUE/FALSE
//
BOOL sortPersons(Person *persons)
{
  Person person_placeholder;
  int number_of_persons = numberOfPersons(persons);
  int counter;
  int parrent_counter;
  int switch_counter;

  if(number_of_persons <= 1)
  {
    return FALSE;
  }
  for(counter = 0; counter < number_of_persons; counter++)
  {
    for(switch_counter = 0; switch_counter < number_of_persons - 1; switch_counter ++)
    {
      if (strcmp((persons + switch_counter)->name_, (persons + (switch_counter + 1))->name_) > 0)
      {
        person_placeholder = persons[switch_counter];
        parrent_counter = 0;
        while((persons + parrent_counter)->gender_ != 3)
        {
          if((persons + parrent_counter)->mother_ == &persons[switch_counter])
          {
            (persons + parrent_counter)->mother_ = &person_placeholder;
          }
          if((persons + parrent_counter)->father_ == &persons[switch_counter])
          {
            (persons + parrent_counter)->father_ = &person_placeholder;
          }
          parrent_counter++;
        }
        persons[switch_counter] = persons[switch_counter + 1];
        parrent_counter = 0;
        while((persons + parrent_counter)->gender_ != 3)
        {
          if((persons + parrent_counter)->mother_ == &persons[switch_counter + 1])
          {
            (persons + parrent_counter)->mother_ = &persons[switch_counter];
          }
          if((persons + parrent_counter)->father_ == &persons[switch_counter + 1])
          {
            (persons + parrent_counter)->father_ = &persons[switch_counter];
          }
          parrent_counter++;
        }
        persons[switch_counter + 1] = person_placeholder;
        parrent_counter = 0;
        while((persons + parrent_counter)->gender_ != 3)
        {
          if((persons + parrent_counter)->mother_ == &person_placeholder)
          {
            (persons + parrent_counter)->mother_ = &persons[switch_counter + 1];
          }
          if((persons + parrent_counter)->father_ == &person_placeholder)
          {
            (persons + parrent_counter)->father_ = &persons[switch_counter + 1];
          }
          parrent_counter++;
        }
      }/*
      else if(strcmp((persons + switch_counter)->name_,(persons + (switch_counter + 1))->name_) == 0 && (persons + switch_counter)->gender_ < ((persons + (switch_counter+1))->gender_))
      {
        person_placeholder = persons[switch_counter];
        parrent_counter = 0;
        while((persons + parrent_counter)->gender_ != 3)
        {
          if((persons + parrent_counter)->mother_ == &persons[switch_counter])
          {
            (persons + parrent_counter)->mother_ = &person_placeholder;
          }
          if((persons + parrent_counter)->father_ == &persons[switch_counter])
          {
            (persons + parrent_counter)->father_ = &person_placeholder;
          }
          parrent_counter++;
        }
        persons[switch_counter] = persons[switch_counter + 1];
        parrent_counter = 0;
        while((persons + parrent_counter)->gender_ != 3)
        {
          if((persons + parrent_counter)->mother_ == &persons[switch_counter + 1])
          {
            (persons + parrent_counter)->mother_ = &persons[switch_counter];
          }
          if((persons + parrent_counter)->father_ == &persons[switch_counter + 1])
          {
            (persons + parrent_counter)->father_ = &persons[switch_counter];
          }
          parrent_counter++;
        }
        persons[switch_counter + 1] = person_placeholder;
        parrent_counter = 0;
        while((persons + parrent_counter)->gender_ != 3)
        {
          if((persons + parrent_counter)->mother_ == &person_placeholder)
          {
            (persons + parrent_counter)->mother_ = &persons[switch_counter + 1];
          }
          if((persons + parrent_counter)->father_ == &person_placeholder)
          {
            (persons + parrent_counter)->father_ = &persons[switch_counter + 1];
          }
          parrent_counter++;
        }
      }*/
      //TODO: check length of name
    }
  }
  return TRUE;
}
//------------------------------------------------------------------------------
///
/// List of Persons
/// with command list get we all person stored in memory
///
/// @param persons
///
/// @return TRUE/FALSE
//
BOOL listPersons(Person *persons)
{
  int counter = 0;
  //sortPersons(persons);
  while((persons+counter)->gender_  != 3)
  {
    printf("%s %s\n", (persons + counter)->name_, ((persons + counter)->gender_
    == 1) ? "[f]" : "[m]");
    counter++;
  }
  if(counter < 1)
  {
    return FALSE;
  }
  return TRUE;
}

//------------------------------------------------------------------------------
///
/// Copy person
/// switch frist and second person
///
/// @param first_person
/// @param second_person
///
/// @return TRUE
//
BOOL copyPerson(Person *first_person, Person *second_person)
{
  strcpy(first_person->name_, second_person->name_);
  first_person->gender_ = second_person->gender_;
  first_person->father_ = second_person->father_;
  first_person->mother_ = second_person->mother_;

  return TRUE;
}

//------------------------------------------------------------------------------
///
/// Number of persons
/// count the person in array
///
/// @param persons
///
/// @return number_of_persons;
//
int numberOfPersons(Person *persons)
{
  int number_of_persons = 0;
  while((persons + number_of_persons)->gender_ != 3)
  {
    number_of_persons++;
  }
  return number_of_persons;
}

//------------------------------------------------------------------------------
///
/// Create Person Instance
///
/// @param name
/// @param gender
/// @param mother
/// @param father
///
/// @return new_person;
//
Person createPersonInstance(char *name, BOOL gender, Person *mother, Person *father)
{
  Person new_person;
  strcpy(new_person.name_,name);
  new_person.gender_ = gender;
  new_person.father_ = father;
  new_person.mother_ = mother;
  return new_person;
}

//------------------------------------------------------------------------------
///
/// Find person
///	search for person
///
/// @param persons
/// @param name
/// @param gender
///
/// @return finded person or NULL
//
Person *findPerson(Person *persons, char const  *name, BOOL gender)
{
  int counter = 0;
  while((persons + counter)->gender_ != 3 && (persons + counter) != NULL)
  {
    if(strcmp((persons + counter)->name_, name) == 0 && (persons + counter)->gender_ == gender)
    {
      return persons + counter;
    }
    counter++;
  }
  return NULL;
}

//------------------------------------------------------------------------------
///
/// Show error
/// print error messege
///
/// @param error_code
///
/// @return 
//
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
    case ERROR_WRONG_DRAW_ALL_USAGE:
    printf("[ERR] Wrong usage - draw-all <file-name>.\n");
    break;
    case ERROR_FILE_COULD_NOT_BE_WRITTEN:
    printf("[ERR] Could not write file.\n");
    break;
    case ERROR_BOTH_PEOPLE_ARE_THE_SAME:
    printf("[ERR] Both people are the same.\n");
    break;
    case ERROR_SEX_DOES_NOT_MATCH:
    printf("[ERR] Sex does not match with relation.\n");
    break;
    case ERROR_RELATION_NOT_POSSIBLE:
    printf("[ERR] Relation not possible.\n");
    break;
  }
}

//------------------------------------------------------------------------------
///
/// Show Success Message
/// print success Message
///
/// @param msg_code
///
/// @return 
//
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
    case MSG_SUCCESS_CREATING_DOT_FILE:
    printf("Creating DOT-file was successful.\n");
    break;
  }
}