//-----------------------------------------------------------------------------
// assa.c
//
// The program takes in the members of family either by the command line or
// from the already made documents. All necessary checks are being made in an
// attempt to ensure file and data validity. After all family members are
// successfully loaded or manually written, program gives a possibility
// to store acquired data in a .dot file format for further use. Dot file is
// properly formatted, so it can be used to create an image of family a tree.
// 
// Group: 3 study assistant Lorenz Kofler
//
// Authors: Djordje Rajic 1431564
// Stefan Rajinovic 1431905
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
#define MSG_RELATION_PEOPLE_ARE_RELATED 12
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
#define ERROR_WRONG_RELATIONSHIP_USAGE 12
#define ERROR_RELATIONSHIP_PERSON_DOES_NOT_EXISTS 13
#define ERROR_RELATION_NOT_RELATED 14
// Relation defines
#define RELATION_SISTER 1
#define RELATION_BROTHER 2
#define RELATION_MOTHER 3
#define RELATION_FATHER 4
#define RELATION_GRANDMOTHER_FIRST_PERSON 5
#define RELATION_GRANDMOTHER_SECOND_PERSON 6
#define RELATION_GRANDFATHER_FIRST_PERSON 7
#define RELATION_GRANDFATHER_SECOND_PERSON 8 
//String lenghts
#define INPUT_COMMAND_LENGHT 530 //NOTE: 256 for one name + 256 second name + 
// 6 for genders + some whitespaces + longest command = around 530
#define MAX_NAME_LENGHT 257 // +1 for \0
//Msc
#define INIT_PERSONS_ARRAY_SIZE 100

// Structure for storing every familly member
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
void parseInput(char *input_command, Person **persons_array);

// forward declaration
void waitForInput(Person **persons_array);

// forward declaration
BOOL parseAddInput(char *input_command, Person **array_of_persons);

// forward declaration
BOOL copyPerson(Person *first_person, Person *second_person);

// forward declaration
void parseDrawInput(char *input_command);

// forward declaration
BOOL parseRelationshipInput(char *input_command, Person *array_of_persons);

// forward declaration
char *parseDrawAllInput(char *input_command, Person *persons);

// forward declaration
Person *sortPersons(Person *persons);
// forward declaration
int numberOfPersons(Person *persons);

// forward declaration
BOOL writePersonToFile(char *file_name, Person *persons_to_write);

// forward declaration
void addRelationship(char const *first_person_name, BOOL first_person_gender,
char const *second_person_name, BOOL second_person_gender, char const
*relationship, Person **array_of_persons);

// forward declaration
void addMother(char const *first_person_name, BOOL first_person_gender, char
 const *second_person_name, BOOL second_person_gender, char const *relationship, 
 Person **array_of_persons);

// forward declaration
void addFather(char const *first_person_name, BOOL first_person_gender, char
 const *second_person_name, BOOL second_person_gender, char const *relationship,
 Person **array_of_persons);

// forward declaration
void addMgm(char const *first_person_name, BOOL first_person_gender, char const
 *second_person_name, BOOL second_person_gender, char const *relationship, 
 Person **array_of_persons);

// forward declaration
void addFgm(char const *first_person_name, BOOL first_person_gender, char const
 *second_person_name, BOOL second_person_gender, char const *relationship,
 Person **array_of_persons);

// forward declaration
void addMgf(char const *first_person_name, BOOL first_person_gender, char const
 *second_person_name, BOOL second_person_gender, char const *relationship,
 Person **array_of_persons);

// forward declaration
void addFgf(char const *first_person_name, BOOL first_person_gender, char const
 *second_person_name, BOOL second_person_gender, char const *relationship,
 Person **array_of_persons);

// forward declaration
Person *addUnknownPerson(Person *array_of_persons, BOOL gender);
// forward declaration
BOOL namesArePartiallyEqual(char const *first_name, char const *second_name);
int checkRelation(Person *first_person, Person *second_person);
void showRelationship(Person *persons_array, char const *first_person_name, BOOL
 first_person_gender, char const *second_person_name, BOOL
 second_person_gender);
//------------------------------------------------------------------------------
///
/// The main program.
/// Checking the number of parameters, giving the necessary errors, passing the
/// data to right functions for further processing.
///
/// @param argc used for checking the number of parameters
/// @param argv all command line parameters are stred here
///
/// @return success program closed
//
int main(int argc, char **argv)
{
  if(argc == 1)
  {
    Person *persons_array = malloc(INIT_PERSONS_ARRAY_SIZE*sizeof(Person));
    if(persons_array == NULL)
    {
    	showError(ERROR_OUT_OF_MEMORY);
    	exit(ERROR_OUT_OF_MEMORY);
    }
    (persons_array)->gender_ = 3;
    Person **persons_array_ptr = &persons_array;
    waitForInput(persons_array_ptr);
  }
  else if(argc == 2)
  {

    char *file_name = argv[1];
    if(!fileExists(file_name))
    {
      showError(ERROR_FILE_COULD_NOT_BE_READ);
      return ERROR_FILE_COULD_NOT_BE_READ;
    }
    char *file_contetnt = storeFileIntoMemory(file_name);
    Person *persons_array = parseDotFile(file_contetnt);
    Person **persons_array_ptr = &persons_array;
    waitForInput(persons_array_ptr);
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
/// @param file_content string, with dot file data to be parsed. Essentially a
///  very long string whit all data from .dot file.
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
  while(*(file_content + counter) != '\0') // Extrating values from single 
    //string to array of pointers to the specific adresses withing the string
  {
    if(*(file_content + counter) == '\n')
    {
      *(file_content + counter) = '\0'; //Puting null byte insted of newline so
      // the string is later valid for use with function from string.h
      lines_separated[lines_separated_counter] = file_content + 
      file_content_counter;

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

  Person *array_of_persons = calloc(
  (lines_separated_counter + INIT_PERSONS_ARRAY_SIZE),sizeof(Person)); // Bit
  // more then we need, but is's okay to have more.
  if(array_of_persons == NULL) 
  {
    free(file_content);
  	showError(ERROR_OUT_OF_MEMORY);
  	exit(ERROR_OUT_OF_MEMORY);
  }
  array_of_persons[(lines_separated_counter +
   INIT_PERSONS_ARRAY_SIZE)-1].gender_ = 3;
  for(counter = 2; counter < lines_separated_counter; counter++ )
  {
    if(parseSingleFileLine(lines_separated[counter], name, &gender, 
    parrent_name, &parrent_gender) == FALSE)
    {
      free(file_content);
      free(array_of_persons);
      showError(ERROR_FILE_COULD_NOT_BE_READ);
      exit(ERROR_FILE_COULD_NOT_BE_READ);
    }
    name[strlen(name)-1] = '\0'; // NOTE: Getting rid of last empty space
    if(findPerson(array_of_persons, name, gender) == NULL)
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
            array_of_persons[number_of_persons].mother_ =  &array_of_persons
            [number_of_persons+1];
          }
          else if(parrent_gender == FALSE)
          {
            array_of_persons[number_of_persons].father_ = &array_of_persons
            [number_of_persons+1];
          }
          number_of_persons++;
        }
        else
        {
          Person *parrent = findPerson(array_of_persons, parrent_name, 
          parrent_gender);
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
          Person *temp_parrent_mother = findPerson(array_of_persons,
           parrent_name, parrent_gender);
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
          Person *temp_parrent_father = findPerson(array_of_persons,  
          parrent_name, parrent_gender);
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
  array_of_persons[number_of_persons].gender_ = 3; // This is like \0 in string
  if(!listPersons(array_of_persons))
  {
    showError(ERROR_NO_ENTRIES_AVAILABLE);
  }
  showSuccessMessage(MSG_SUCCESS_DOT_FILE_PARSING); // Printing out all personst
  // that have been loaded, and giving succes message
  free(file_content);

  return array_of_persons;
}

//------------------------------------------------------------------------------
///
/// Parsing each line in a dot file, checking for mistakes, or misformated .dot
/// file
///
/// @param line_to_parse line in a form of a string
/// @param name referece so we can change it and use it in a another function
/// @param gender_b referece so we can change it and use it in a another
/// function
/// @param parrant_name referece so we can change it and use it in a another 
/// function
/// @param parrant_gender_b referece so we can change it and use it in a another
/// function
///
/// @return TRUE or FALSE, FALSE if an error occured, meaning line is not valid
/// TRUE if everything went okay and the line from file is valid
//
BOOL parseSingleFileLine(char *line_to_parse, char *name, BOOL *gender_b, char 
    *parrent_name, BOOL *parrent_gender_b)
{
  if(*(line_to_parse) != ' ' || *(line_to_parse + 1) != ' ' ||  *(line_to_parse
   + 2) != '"')
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
    if(counter >= MAX_NAME_LENGHT || *(line_to_parse + counter) == ']' ||  *
      (line_to_parse + counter) == '>' || *(line_to_parse + counter) == ';' || 
      *(line_to_parse + counter) == '"')
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
  if(*(line_to_parse + (counter - 1)) != 'f' && *(line_to_parse + (counter - 1))
    != 'm')
  {
    return FALSE;
  }
  *gender_b = (*(line_to_parse + (counter - 1)) == 'f') ? TRUE : FALSE;
  ++counter;
  if(*(line_to_parse + counter) == ' ' || *(line_to_parse + (counter + 1)) == 
   ';')
  {
    *parrent_name = '\0';
    return TRUE;
  }
  counter += 2;
  if(*(line_to_parse + (counter)) != '-' || *(line_to_parse + (counter + 1)) != 
    '>' || *(line_to_parse + (counter + 2)) != ' ' || *(line_to_parse +  
      (counter + 3)) != '"')
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
    if(counter >= MAX_NAME_LENGHT || *(line_to_parse + counter) == ']' || *
      (line_to_parse + counter) == '>' || *(line_to_parse + counter) == ';' ||
      *(line_to_parse + counter) == '"')
    {
      return FALSE;
    }
  }
  *(parrent_name + (null_counter - 1)) = '\0';
  counter += 2; // We are skipping [
  if(*(line_to_parse + counter) != ']' || *(line_to_parse + (counter + 1)) !=  
   '"' || *(line_to_parse + (counter + 2)) != ';')
  {
    return FALSE;
  }
  if(*(line_to_parse + (counter - 1)) != 'f' && *(line_to_parse + (counter -
   1)) != 'm')
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
/// @param name, name of person for checking
///
/// @return TRUE or FALSE
//
BOOL nameIsUnknown(const char *name)
{
  int name_lenght = strlen(name);
  int name_counter;
  if(name_lenght < 2 )
  {
    return FALSE;
  }
  for(name_counter = 1; name_counter < name_lenght; name_counter++)
  {
    if(isdigit(name[name_counter]) == 0)
    {
      return FALSE;
    }
  }
  return TRUE;
}


//------------------------------------------------------------------------------
///
/// Parse the input command 
/// check command input and call the coresponding function
///
/// @param input_command, name of command 
/// @param pesons_array is array of persons
///
/// @return 
//
void parseInput(char *input_command, Person **persons_array)
{
  if(strcmp(input_command, "quit\n") == 0)
  {
    free(*persons_array);
    showSuccessMessage(MSG_SUCCESS_PROGRAM_CLOSED_WITH_QUIT);
    exit(SUCCESS_PROGRAM_CLOSED);
  }
  if(strcmp(input_command, "list\n") == 0)
  {
    if(!listPersons(*persons_array))
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
    counter++;
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
      char *file_name = parseDrawAllInput(input_command, *persons_array);
      if(file_name == NULL)
      {
        showError(ERROR_WRONG_DRAW_ALL_USAGE);
      }
      else
      {
        if(numberOfPersons(*persons_array) <= 1)
        {
          showError(ERROR_NO_ENTRIES_AVAILABLE);
        }
        else
        {
          if(!writePersonToFile(file_name, *persons_array))
          {
            showError(ERROR_FILE_COULD_NOT_BE_WRITTEN);
          }
        }
      }
    }
    if(strcmp(command, "relationship") == 0)
    {
      if(!parseRelationshipInput(input_command, *persons_array))
      {
        showError(ERROR_WRONG_RELATIONSHIP_USAGE);
      }
    }
  }
}

//------------------------------------------------------------------------------
///
/// Parse add input
/// check if the format is right, and extract values from input line
///
/// @param input_command, string for parsing
/// @param pesons_array our array of persons
///
/// @return TRUE/FALSE
//
BOOL parseAddInput(char *input_command, Person **array_of_persons)
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
  if(*(input_command + counter) != '[' || *(input_command + (counter + 2)) !=
   ']' || *(input_command + (counter + 3)) != ' ')
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
    if(*(input_command + counter) == '\n' || *(input_command + counter) == '['
     || *(input_command + counter) == ']')
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

  if(*(input_command + counter) != '[' || *(input_command + (counter + 2)) !=
   ']' || *(input_command + (counter + 3)) != '\n')
  {
    return FALSE;
  }
  counter++;
  if(*(input_command + counter) != 'f' && *(input_command + counter) != 'm')
  {
    return FALSE;
  }
  BOOL second_person_gender = (*(input_command + counter) == 'f') ? TRUE : FALSE;

  if(strcmp(relationship, "mother") != 0 && strcmp(relationship, "father") != 0 
   && strcmp(relationship, "mgm") != 0 && strcmp(relationship, "fgm") != 0 && 
   strcmp(relationship, "mgf") != 0 && strcmp(relationship, "fgf") != 0)
  {
    return FALSE;
  }

  addRelationship(first_person_name, first_person_gender, second_person_name,
   second_person_gender, relationship, array_of_persons);

  return TRUE;
}

//------------------------------------------------------------------------------
///
/// Parse relationship input
/// check if the format is right, and extract values from input line 
///
/// @param input_command, string for parsing
/// @param pesons_array our array of persons
///
/// @return TRUE/FALSE
//
BOOL parseRelationshipInput(char *input_command, Person *array_of_persons)
{
  int counter = 13;
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
  char *first_person_name = input_command + 13;
  if(*(input_command + counter) != '[' || *(input_command + (counter + 2)) !=
   ']' || *(input_command + (counter + 3)) != ' ')
  {
    return FALSE;
  }
  counter++;
  if(*(input_command + counter) != 'f' && *(input_command + counter) != 'm')
  {
    return FALSE;
  }
  BOOL first_person_gender = (*(input_command + counter) == 'f') ? TRUE : FALSE;
  counter += 2;
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

  if(*(input_command + counter) != '[' || *(input_command + (counter + 2)) !=
   ']' || *(input_command + (counter + 3)) != '\n')
  {
    return FALSE;
  }
  counter++;
  if(*(input_command + counter) != 'f' && *(input_command + counter) != 'm')
  {
    return FALSE;
  }
  BOOL second_person_gender = (*(input_command + counter) == 'f') ? TRUE : FALSE;
  
  showRelationship(array_of_persons, first_person_name, first_person_gender,
    second_person_name, second_person_gender);

  return TRUE;
}

//------------------------------------------------------------------------------
/// TODO:
///
///
/// @return 
//
void showRelationship(Person *persons_array, char const *first_person_name, BOOL
 first_person_gender, char const *second_person_name, BOOL second_person_gender)
{
  Person *first_person = findPerson(persons_array, first_person_name, 
    first_person_gender);
  Person *second_person = findPerson(persons_array, second_person_name,
    second_person_gender); 
  if(first_person == NULL || second_person == NULL)
  {
    showError(ERROR_RELATIONSHIP_PERSON_DOES_NOT_EXISTS);
  }
  else if(first_person == second_person)
  {
    showError(ERROR_BOTH_PEOPLE_ARE_THE_SAME);
  }
  else
  {
    switch (checkRelation(first_person, second_person))
    {
      case RELATION_SISTER:
      showSuccessMessage(MSG_RELATION_PEOPLE_ARE_RELATED);
      printf("%s [%c] is the sister of %s [%c].\n", first_person->name_,
        (first_person->gender_ == 1) ? 'f' : 'm', second_person->name_,
        (second_person->gender_ == 1) ? 'f' : 'm');
      break;
      case RELATION_BROTHER:
      showSuccessMessage(MSG_RELATION_PEOPLE_ARE_RELATED);
      printf("%s [%c] is the brother of %s [%c].\n", first_person->name_,
        (first_person->gender_ == 1) ? 'f' : 'm', second_person->name_,
        (second_person->gender_ == 1) ? 'f' : 'm');
      break;
      case RELATION_MOTHER:
      showSuccessMessage(MSG_RELATION_PEOPLE_ARE_RELATED);
      if(first_person->mother_ != second_person)
      {
        printf("%s [%c] is the mother of %s [%c].\n", first_person->name_,
        (first_person->gender_ == 1) ? 'f' : 'm', second_person->name_,
        (second_person->gender_ == 1) ? 'f' : 'm');
      }
      else
      {
        printf("%s [%c] is the mother of %s [%c].\n", second_person->name_,
        (second_person->gender_ == 1) ? 'f' : 'm', first_person->name_,
        (first_person->gender_ == 1) ? 'f' : 'm');
      }
      break;
      case RELATION_FATHER:
      showSuccessMessage(MSG_RELATION_PEOPLE_ARE_RELATED);
      if(first_person->father_ != second_person)
      {
        printf("%s [%c] is the father of %s [%c].\n", first_person->name_,
        (first_person->gender_ == 1) ? 'f' : 'm', second_person->name_,
        (second_person->gender_ == 1) ? 'f' : 'm');
      }
      else
      {
        printf("%s [%c] is the father of %s [%c].\n", second_person->name_,
        (second_person->gender_ == 1) ? 'f' : 'm', first_person->name_,
        (first_person->gender_ == 1) ? 'f' : 'm');
      }
      break;
      case RELATION_GRANDMOTHER_FIRST_PERSON:
      showSuccessMessage(MSG_RELATION_PEOPLE_ARE_RELATED);
      printf("%s [%c] is the grandmother of %s [%c].\n", first_person->name_,
      (first_person->gender_ == 1) ? 'f' : 'm', second_person->name_,
      (second_person->gender_ == 1) ? 'f' : 'm');
      break;
      case RELATION_GRANDMOTHER_SECOND_PERSON:
      showSuccessMessage(MSG_RELATION_PEOPLE_ARE_RELATED);
      printf("%s [%c] is the grandmother of %s [%c].\n", second_person->name_,
      (second_person->gender_ == 1) ? 'f' : 'm', first_person->name_,
      (first_person->gender_ == 1) ? 'f' : 'm');
      break;
      case RELATION_GRANDFATHER_FIRST_PERSON:
      showSuccessMessage(MSG_RELATION_PEOPLE_ARE_RELATED);
      printf("%s [%c] is the grandfather of %s [%c].\n", first_person->name_,
      (first_person->gender_ == 1) ? 'f' : 'm', second_person->name_,
      (second_person->gender_ == 1) ? 'f' : 'm');
      break;
      case RELATION_GRANDFATHER_SECOND_PERSON:
      showSuccessMessage(MSG_RELATION_PEOPLE_ARE_RELATED);
      printf("%s [%c] is the grandfather of %s [%c].\n", second_person->name_,
      (second_person->gender_ == 1) ? 'f' : 'm', first_person->name_,
      (first_person->gender_ == 1) ? 'f' : 'm');
      break;
      case ERROR_RELATION_NOT_RELATED:
      showError(ERROR_RELATION_NOT_RELATED);
      break;
      default:
      showError(ERROR_RELATION_NOT_RELATED);
      break;
    }
  }
}

int checkRelation(Person *first_person, Person *second_person)
{
  if((first_person->mother_ == NULL && first_person->father_ == NULL) && (second_person->mother_ == NULL && second_person->father_ == NULL))
  {
    return ERROR_RELATION_NOT_RELATED;
  }

  if(first_person->gender_ == 1 && ((first_person->mother_ != NULL && second_person->mother_ != NULL && first_person->mother_ == second_person->mother_) || (first_person->father_ != NULL && second_person->father_ != NULL && first_person->father_ == second_person->father_)))
  {
    return RELATION_SISTER;
  }
  else if(first_person->gender_ == 0 && ((first_person->mother_ != NULL && second_person->mother_ != NULL && first_person->mother_ == second_person->mother_) || (first_person->father_ != NULL && second_person->father_ != NULL && first_person->father_ == second_person->father_)))
  {
    return RELATION_BROTHER;
  }
  else if((first_person->gender_ == 1 && second_person->mother_ != NULL && first_person == second_person->mother_) || (second_person->gender_ == 1 && first_person->mother_ != NULL && second_person == first_person->mother_))
  {
    return RELATION_MOTHER;
  }
  else if((first_person->gender_ == 0 && second_person->father_ != NULL && first_person == second_person->father_) || (second_person->gender_ == 0 && first_person->father_ != NULL && second_person == first_person->father_))
  {
    return RELATION_FATHER;
  }
  else if((first_person->gender_ == 1 && second_person->mother_ != NULL && second_person->mother_->mother_ != NULL  && first_person == second_person->mother_->mother_) || (first_person->gender_ == 1 && second_person->father_ != NULL && second_person->father_->mother_ != NULL && first_person == second_person->father_->mother_))
  {
    return RELATION_GRANDMOTHER_FIRST_PERSON;
  }
  else if((second_person->gender_ == 1 && first_person->mother_ != NULL && first_person->mother_->mother_ != NULL && second_person == first_person->mother_->mother_) 
    || (second_person->gender_ == 1 && first_person->father_ != NULL && first_person->father_->mother_ != NULL && second_person == first_person->father_->mother_))
  {
    return RELATION_GRANDMOTHER_SECOND_PERSON;
  }
  else if((first_person->gender_ == 0 && second_person->mother_ != NULL && second_person->mother_->father_ != NULL && first_person == second_person->mother_->father_) || 
    (first_person->gender_ == 0 && second_person->father_ != NULL && second_person->father_->father_ != NULL && first_person == second_person->father_->father_))
  {
    return RELATION_GRANDFATHER_FIRST_PERSON;
  }
  else if((second_person->gender_ == 0 && first_person->mother_ != NULL && first_person->mother_->father_ != NULL && second_person == first_person->mother_->father_) || 
    (second_person->gender_ == 0 && first_person->father_ != NULL && first_person->father_->father_ != NULL && second_person == first_person->father_->father_))
  {
    return RELATION_GRANDFATHER_SECOND_PERSON;
  }

  return 0;
}

//------------------------------------------------------------------------------
///
/// Add Relationship, call coresponding functions for every possible
/// realtionship that can be registerd by our programm
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
void addRelationship(char const *first_person_name, BOOL first_person_gender, 
  char const *second_person_name, BOOL second_person_gender, char const
  *relationship, Person **array_of_persons)
{
  if(strcmp(first_person_name, second_person_name) == 0 && first_person_gender
   == second_person_gender)
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
        addMother(first_person_name, first_person_gender, second_person_name,
         second_person_gender, relationship, array_of_persons);
      }
    }
    if(strcmp(relationship, "father" ) == 0)
    {
      if(first_person_gender != 0)
      {
        showError(ERROR_SEX_DOES_NOT_MATCH);
      }
      else
      {
        addFather(first_person_name, first_person_gender, second_person_name,
        second_person_gender, relationship, array_of_persons);
      }
    }
    if(strcmp(relationship, "mgm" ) == 0)
    {
      if(first_person_gender != 1)
      {
        showError(ERROR_SEX_DOES_NOT_MATCH);
      }
      else
      {
        addMgm(first_person_name, first_person_gender, second_person_name,
        second_person_gender, relationship, array_of_persons);
      }
    }   
    if(strcmp(relationship, "fgm" ) == 0)
    {
      if(first_person_gender != 1)
      {
        showError(ERROR_SEX_DOES_NOT_MATCH);
      }
      else
      {
        addFgm(first_person_name, first_person_gender, second_person_name,
        second_person_gender, relationship, array_of_persons);
      }
    }
    if(strcmp(relationship, "mgf" ) == 0)
    {
      if(first_person_gender != 0)
      {
        showError(ERROR_SEX_DOES_NOT_MATCH);
      }
      else
      {
        addMgf(first_person_name, first_person_gender, second_person_name,
        second_person_gender, relationship, array_of_persons);
      }
    }
    if(strcmp(relationship, "fgf" ) == 0)
    {
      if(first_person_gender != 0)
      {
        showError(ERROR_SEX_DOES_NOT_MATCH);
      }
      else
      {
        addFgf(first_person_name, first_person_gender, second_person_name,
        second_person_gender, relationship, array_of_persons);
      }
    }   
  }
}
//------------------------------------------------------------------------------
///
/// addMother
/// Add person and persons mother
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
void addMother(char const *first_person_name, BOOL first_person_gender, char 
const *second_person_name, BOOL second_person_gender, char const *relationship,
Person **array_of_persons)
{
  if(findPerson(*array_of_persons, second_person_name, second_person_gender) != NULL)
  {
    Person *child = findPerson(*array_of_persons, second_person_name, second_person_gender);

    if(child->mother_ == NULL || nameIsUnknown(child->mother_->name_))
    {
      if(child->mother_ != NULL && nameIsUnknown(child->mother_->name_))
      {
        strcpy(child->mother_->name_, first_person_name);
      }
      else
      {
        if(findPerson(*array_of_persons, first_person_name, first_person_gender) != NULL)
        {
          child->mother_ = findPerson(*array_of_persons, first_person_name, first_person_gender);
        }
        else
        {
          int number_of_persons = numberOfPersons(*array_of_persons);
          if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
          {
            Person *buffer = realloc(*array_of_persons, sizeof(Person)*
              (number_of_persons * 5));
            if(buffer == NULL)
            {
              free(*array_of_persons);
              showError(ERROR_OUT_OF_MEMORY);
              exit(ERROR_OUT_OF_MEMORY);
            }
            *array_of_persons = buffer;
          }  
          child = findPerson(*array_of_persons, second_person_name, second_person_gender);
          strcpy((*array_of_persons + number_of_persons)->name_, first_person_name);
          (*array_of_persons + number_of_persons)->gender_ = first_person_gender;
          (*array_of_persons + number_of_persons)->mother_ = NULL;
          (*array_of_persons + number_of_persons)->father_ = NULL;
          (*array_of_persons + (number_of_persons + 1))->gender_ = 3;
          child->mother_ = (*array_of_persons + number_of_persons);
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
    int number_of_persons = numberOfPersons(*array_of_persons);
    if (number_of_persons == 0)
    {
      strcpy((*array_of_persons)->name_, second_person_name);
      (*array_of_persons)->gender_ = second_person_gender;
      (*array_of_persons)->father_ = NULL;
      strcpy((*array_of_persons + 1)->name_, first_person_name);
      (*array_of_persons + 1)->gender_ = 1;
      (*array_of_persons + 1)->mother_ = NULL;
      (*array_of_persons + 1)->father_ = NULL;
      (*array_of_persons + 2)->gender_ = 3;
      (*array_of_persons)->mother_ = (*array_of_persons + 1);
    }
    else
    {
      if(findPerson(*array_of_persons, first_person_name, first_person_gender) != NULL)
      {
        if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
        {
          Person *buffer = realloc(*array_of_persons, sizeof(Person)*
            (number_of_persons * 5));
          if(buffer == NULL)
          {
            free(*array_of_persons);
            showError(ERROR_OUT_OF_MEMORY);
            exit(ERROR_OUT_OF_MEMORY);
          }
          *array_of_persons = buffer;
        } 
        strcpy((*array_of_persons + number_of_persons)->name_, second_person_name);
        (*array_of_persons + number_of_persons)->gender_ = second_person_gender;
        (*array_of_persons + number_of_persons)->father_ = NULL;
        (*array_of_persons + number_of_persons)->mother_ = findPerson(*array_of_persons, first_person_name, first_person_gender);
        (*array_of_persons + (number_of_persons + 1))->gender_ = 3;

      }
      else
      {
        if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
        {
          Person *buffer = realloc(*array_of_persons, sizeof(Person)*
            (number_of_persons * 5));
          if(buffer == NULL)
          {
            free(*array_of_persons);
            showError(ERROR_OUT_OF_MEMORY);
            exit(ERROR_OUT_OF_MEMORY);
          }
          *array_of_persons = buffer;
        }  
        strcpy((*array_of_persons + number_of_persons)->name_, first_person_name);
        (*array_of_persons + number_of_persons)->gender_ = 1;
        (*array_of_persons + number_of_persons)->mother_ = NULL;
        (*array_of_persons + number_of_persons)->father_ = NULL;
        strcpy((*array_of_persons + (number_of_persons + 1))->name_, second_person_name);
        (*array_of_persons + (number_of_persons + 1))->gender_ = second_person_gender;
        (*array_of_persons + (number_of_persons + 1))->mother_ = (*array_of_persons + number_of_persons);
        (*array_of_persons + (number_of_persons + 1))->father_ = NULL;
        (*array_of_persons + (number_of_persons + 2))->gender_ = 3;
      }
    }
  }
}

//------------------------------------------------------------------------------
///
/// addFather
/// Add person and persons father
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
void addFather(char const *first_person_name, BOOL first_person_gender, char
 const *second_person_name, BOOL second_person_gender, char const *relationship,
 Person **array_of_persons) {
  if(findPerson(*array_of_persons, second_person_name, second_person_gender) != NULL)
  {
    Person *child = findPerson(*array_of_persons, second_person_name, second_person_gender);

    if(child->father_ == NULL || nameIsUnknown(child->father_->name_))
    {
      if(child->father_ != NULL && nameIsUnknown(child->father_->name_))
      {
        strcpy(child->father_->name_, first_person_name);
      }
      else
      {
        if(findPerson(*array_of_persons, first_person_name, first_person_gender) != NULL)
        {
          child->father_ = findPerson(*array_of_persons, first_person_name, first_person_gender);
        }
        else
        {
          int number_of_persons = numberOfPersons(*array_of_persons);
          if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
          {
            Person *buffer = realloc(*array_of_persons, sizeof(Person)*
              (number_of_persons * 5));
            if(buffer == NULL)
            {
              free(*array_of_persons);
              showError(ERROR_OUT_OF_MEMORY);
              exit(ERROR_OUT_OF_MEMORY);
            }
            *array_of_persons = buffer;
          }  
          child = findPerson(*array_of_persons, second_person_name, second_person_gender);
          strcpy((*array_of_persons + number_of_persons)->name_, first_person_name);
          (*array_of_persons + number_of_persons)->gender_ = first_person_gender;
          (*array_of_persons + number_of_persons)->mother_ = NULL;
          (*array_of_persons + number_of_persons)->father_ = NULL;
          (*array_of_persons + (number_of_persons + 1))->gender_ = 3;
          child->father_ = (*array_of_persons + number_of_persons);
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
    int number_of_persons = numberOfPersons(*array_of_persons);
    if (number_of_persons == 0)
    {
      strcpy((*array_of_persons)->name_, second_person_name);
      (*array_of_persons)->gender_ = second_person_gender;
      (*array_of_persons)->mother_ = NULL;
      strcpy((*array_of_persons + 1)->name_, first_person_name);
      (*array_of_persons + 1)->gender_ = 0;
      (*array_of_persons + 1)->mother_ = NULL;
      (*array_of_persons + 1)->father_ = NULL;
      (*array_of_persons + 2)->gender_ = 3;
      (*array_of_persons)->father_ = (*array_of_persons + 1);
    }
    else
    {
      if(findPerson(*array_of_persons, first_person_name, first_person_gender) != NULL)
      {
        if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
        {
          Person *buffer = realloc(*array_of_persons, sizeof(Person)*
            (number_of_persons * 5));
          if(buffer == NULL)
          {
            free(*array_of_persons);
            showError(ERROR_OUT_OF_MEMORY);
            exit(ERROR_OUT_OF_MEMORY);
          }
          *array_of_persons = buffer;
        } 
        strcpy((*array_of_persons + number_of_persons)->name_, second_person_name);
        (*array_of_persons + number_of_persons)->gender_ = second_person_gender;
        (*array_of_persons + number_of_persons)->mother_ = NULL;
        (*array_of_persons + number_of_persons)->father_ = findPerson(*array_of_persons, first_person_name, first_person_gender);
        (*array_of_persons + (number_of_persons + 1))->gender_ = 3;

      }
      else
      {
        if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
        {
          Person *buffer = realloc(*array_of_persons, sizeof(Person)*
            (number_of_persons * 5));
          if(buffer == NULL)
          {
            free(*array_of_persons);
            showError(ERROR_OUT_OF_MEMORY);
            exit(ERROR_OUT_OF_MEMORY);
          }
          *array_of_persons = buffer;
        }  
        strcpy((*array_of_persons + number_of_persons)->name_, first_person_name);
        (*array_of_persons + number_of_persons)->gender_ = 1;
        (*array_of_persons + number_of_persons)->mother_ = NULL;
        (*array_of_persons + number_of_persons)->father_ = NULL;
        strcpy((*array_of_persons + (number_of_persons + 1))->name_, second_person_name);
        (*array_of_persons + (number_of_persons + 1))->gender_ = second_person_gender;
        (*array_of_persons + (number_of_persons + 1))->father_ = (*array_of_persons + number_of_persons);
        (*array_of_persons + (number_of_persons + 1))->mother_ = NULL;
        (*array_of_persons + (number_of_persons + 2))->gender_ = 3;
      }
    }
  }
}

//------------------------------------------------------------------------------
///
/// addMgm
/// Add person and persons mother-grandmother
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
void addMgm(char const *first_person_name, BOOL first_person_gender, char const
 *second_person_name, BOOL second_person_gender, char const *relationship, Person **array_of_persons)
{
    if(findPerson(*array_of_persons, second_person_name, second_person_gender) != NULL)
    {
      Person *child = findPerson(*array_of_persons, second_person_name, second_person_gender);
      if(child->mother_ != NULL)
      {
        if(child->mother_->mother_ == NULL)
        {
          int number_of_persons = numberOfPersons(*array_of_persons);
          if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
          {
            Person *buffer = realloc(*array_of_persons, sizeof(Person)*
              (number_of_persons * 5));
            if(buffer == NULL)
            {
              free(*array_of_persons);
              showError(ERROR_OUT_OF_MEMORY);
              exit(ERROR_OUT_OF_MEMORY);
            }
            *array_of_persons = buffer;
          } 
          strcpy((*array_of_persons + number_of_persons)->name_,
          first_person_name);
          (*array_of_persons + number_of_persons)->gender_ = first_person_gender;
          (*array_of_persons + number_of_persons)->mother_ = NULL;
          (*array_of_persons + number_of_persons)->father_ = NULL;
          (*array_of_persons + (number_of_persons + 1))->gender_ = 3;
          child->mother_->mother_ = (*array_of_persons + number_of_persons);
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
        int number_of_persons = numberOfPersons(*array_of_persons);
        child->mother_ = addUnknownPerson(*array_of_persons, first_person_gender);
        number_of_persons++;
        if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
        {
          Person *buffer = realloc(*array_of_persons, sizeof(Person)*
            (number_of_persons * 5));
          if(buffer == NULL)
          {
            free(*array_of_persons);
            showError(ERROR_OUT_OF_MEMORY);
            exit(ERROR_OUT_OF_MEMORY);
          }
          *array_of_persons = buffer;
        } 
        strcpy((*array_of_persons + number_of_persons)->name_, first_person_name);
        (*array_of_persons + number_of_persons)->gender_ = first_person_gender;
        (*array_of_persons + number_of_persons)->father_ = NULL;
        (*array_of_persons + (number_of_persons + 1))->gender_ = 3;
        child->mother_->mother_ = (*array_of_persons + number_of_persons);
      }
    }
    else
    {
      int number_of_persons = numberOfPersons(*array_of_persons);
      Person *mother = addUnknownPerson(*array_of_persons, 1);
      number_of_persons++;
      if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
      {
        Person *buffer = realloc(*array_of_persons, sizeof(Person)*
          (number_of_persons * 5));
        if(buffer == NULL)
        {
          free(*array_of_persons);
          showError(ERROR_OUT_OF_MEMORY);
          exit(ERROR_OUT_OF_MEMORY);
        }
        *array_of_persons = buffer;
      }      
      strcpy((*array_of_persons + number_of_persons)->name_, second_person_name);
      (*array_of_persons + number_of_persons)->gender_ = second_person_gender;
      (*array_of_persons + number_of_persons)->father_ = NULL;
      strcpy((*array_of_persons + (number_of_persons + 1))->name_,
       first_person_name);
      (*array_of_persons + (number_of_persons + 1))->gender_ = first_person_gender;
      (*array_of_persons + (number_of_persons + 1))->father_ = NULL; 
      (*array_of_persons + (number_of_persons + 1))->mother_ = NULL;
      (*array_of_persons + (number_of_persons + 2))->gender_ = 3;
      (*array_of_persons + number_of_persons)->mother_ = mother;
      mother->mother_ = (*array_of_persons + (number_of_persons + 1));
    } 
}

//------------------------------------------------------------------------------
///
/// addFgm
/// Add person and persons father-grandmother
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
void addFgm(char const *first_person_name, BOOL first_person_gender, char const
 *second_person_name, BOOL second_person_gender, char const *relationship,
 Person **array_of_persons)
{
    if(findPerson(*array_of_persons, second_person_name, second_person_gender) != NULL)
    {
      Person *child = findPerson(*array_of_persons, second_person_name, second_person_gender);
      if(child->father_ != NULL)
      {
        if(child->father_->mother_ == NULL)
        {
          int number_of_persons = numberOfPersons(*array_of_persons);
          if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
          {
            Person *buffer = realloc(*array_of_persons, sizeof(Person)*
              (number_of_persons * 5));
            if(buffer == NULL)
            {
              free(*array_of_persons);
              showError(ERROR_OUT_OF_MEMORY);
              exit(ERROR_OUT_OF_MEMORY);
            }
            *array_of_persons = buffer;
          } 
          strcpy((*array_of_persons + number_of_persons)->name_,
          first_person_name);
          (*array_of_persons + number_of_persons)->gender_ = first_person_gender;
          (*array_of_persons + number_of_persons)->mother_ = NULL;
          (*array_of_persons + number_of_persons)->father_ = NULL;
          (*array_of_persons + (number_of_persons + 1))->gender_ = 3;

          child->father_->mother_ = (*array_of_persons + number_of_persons);

          printf("%s\n", (*array_of_persons + number_of_persons)->name_);
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
        child->father_ = addUnknownPerson(*array_of_persons, 0);
        int number_of_persons = numberOfPersons(*array_of_persons);
        if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
        {
          Person *buffer = realloc(*array_of_persons, sizeof(Person)*
            (number_of_persons * 5));
          if(buffer == NULL)
          {
            free(*array_of_persons);
            showError(ERROR_OUT_OF_MEMORY);
            exit(ERROR_OUT_OF_MEMORY);
          }
          *array_of_persons = buffer;
        } 
        strcpy((*array_of_persons + number_of_persons)->name_, first_person_name);
        (*array_of_persons + number_of_persons)->gender_ = first_person_gender;
        (*array_of_persons + number_of_persons)->mother_ = NULL;
        (*array_of_persons + (number_of_persons + 1))->gender_ = 3;
        child->father_->mother_ = (*array_of_persons + number_of_persons);
      }
    }
    else
    {
      Person *father = addUnknownPerson(*array_of_persons, 0);
      int number_of_persons = numberOfPersons(*array_of_persons);
      if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
      {
        Person *buffer = realloc(*array_of_persons, sizeof(Person)*
          (number_of_persons * 5));
        if(buffer == NULL)
        {
          free(*array_of_persons);
          showError(ERROR_OUT_OF_MEMORY);
          exit(ERROR_OUT_OF_MEMORY);
        }
        *array_of_persons = buffer;
      }      
      strcpy((*array_of_persons + number_of_persons)->name_, second_person_name);
      (*array_of_persons + number_of_persons)->gender_ = second_person_gender;
      (*array_of_persons + number_of_persons)->mother_ = NULL;
      strcpy((*array_of_persons + (number_of_persons + 1))->name_,
       first_person_name);
      (*array_of_persons + (number_of_persons + 1))->gender_ = first_person_gender;
      (*array_of_persons + (number_of_persons + 1))->father_ = NULL; 
      (*array_of_persons + (number_of_persons + 1))->mother_ = NULL;
      (*array_of_persons + (number_of_persons + 2))->gender_ = 3;
      (*array_of_persons + number_of_persons)->father_ = father;
      (*array_of_persons + (number_of_persons + 1))->father_->mother_ = (*array_of_persons + (number_of_persons + 1));
    } 
}

//------------------------------------------------------------------------------
///
/// addMgf
/// Add person and persons mother-grandfather
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
void addMgf(char const *first_person_name, BOOL first_person_gender, char const
 *second_person_name, BOOL second_person_gender, char const *relationship, 
 Person **array_of_persons)
{
    if(findPerson(*array_of_persons, second_person_name, second_person_gender) != NULL)
    {
      Person *child = findPerson(*array_of_persons, second_person_name, second_person_gender);
      if(child->mother_ != NULL)
      {
        if(child->mother_->father_ == NULL)
        {
          int number_of_persons = numberOfPersons(*array_of_persons);
          if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
          {
            Person *buffer = realloc(*array_of_persons, sizeof(Person)*
              (number_of_persons * 5));
            if(buffer == NULL)
            {
              free(*array_of_persons);
              showError(ERROR_OUT_OF_MEMORY);
              exit(ERROR_OUT_OF_MEMORY);
            }
            *array_of_persons = buffer;
          } 
          strcpy((*array_of_persons + number_of_persons)->name_,
          first_person_name);
          (*array_of_persons + number_of_persons)->gender_ = first_person_gender;
          (*array_of_persons + number_of_persons)->mother_ = NULL;
          (*array_of_persons + number_of_persons)->father_ = NULL;
          (*array_of_persons + (number_of_persons + 1))->gender_ = 3;
          child->mother_->father_ = (*array_of_persons + number_of_persons);
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
        int number_of_persons = numberOfPersons(*array_of_persons);
        child->mother_ = addUnknownPerson(*array_of_persons, first_person_gender);
        number_of_persons++;
        if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
        {
          Person *buffer = realloc(*array_of_persons, sizeof(Person)*
            (number_of_persons * 5));
          if(buffer == NULL)
          {
            free(*array_of_persons);
            showError(ERROR_OUT_OF_MEMORY);
            exit(ERROR_OUT_OF_MEMORY);
          }
          *array_of_persons = buffer;
        } 
        strcpy((*array_of_persons + number_of_persons)->name_, first_person_name);
        (*array_of_persons + number_of_persons)->gender_ = first_person_gender;
        (*array_of_persons + number_of_persons)->father_ = NULL;
        (*array_of_persons + (number_of_persons + 1))->gender_ = 3;
        child->mother_->father_ = (*array_of_persons + number_of_persons);
      }
    }
    else
    {
      int number_of_persons = numberOfPersons(*array_of_persons);
      Person *mother = addUnknownPerson(*array_of_persons, 1);
      number_of_persons++;
      if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
      {
        Person *buffer = realloc(*array_of_persons, sizeof(Person)*
          (number_of_persons * 5));
        if(buffer == NULL)
        {
          free(*array_of_persons);
          showError(ERROR_OUT_OF_MEMORY);
          exit(ERROR_OUT_OF_MEMORY);
        }
        *array_of_persons = buffer;
      }      
      strcpy((*array_of_persons + number_of_persons)->name_, second_person_name);
      (*array_of_persons + number_of_persons)->gender_ = second_person_gender;
      (*array_of_persons + number_of_persons)->father_ = NULL;
      strcpy((*array_of_persons + (number_of_persons + 1))->name_,
       first_person_name);
      (*array_of_persons + (number_of_persons + 1))->gender_ = first_person_gender;
      (*array_of_persons + (number_of_persons + 1))->father_ = NULL; 
      (*array_of_persons + (number_of_persons + 1))->mother_ = NULL;
      (*array_of_persons + (number_of_persons + 2))->gender_ = 3;
      (*array_of_persons + number_of_persons)->mother_ = mother;
      mother->father_ = (*array_of_persons + (number_of_persons + 1));
    } 
}

//------------------------------------------------------------------------------
///
/// addFgf
/// Add person and persons father-grandfather
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
void addFgf(char const *first_person_name, BOOL first_person_gender, char const
 *second_person_name, BOOL second_person_gender, char const *relationship,
 Person **array_of_persons)
{
    if(findPerson(*array_of_persons, second_person_name, second_person_gender) != NULL)
    {
      Person *child = findPerson(*array_of_persons, second_person_name, second_person_gender);
      if(child->father_ != NULL)
      {
        if(child->father_->father_ == NULL)
        {
          int number_of_persons = numberOfPersons(*array_of_persons);
          if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
          {
            Person *buffer = realloc(*array_of_persons, sizeof(Person)*
              (number_of_persons * 5));
            if(buffer == NULL)
            {
              free(*array_of_persons);
              showError(ERROR_OUT_OF_MEMORY);
              exit(ERROR_OUT_OF_MEMORY);
            }
            *array_of_persons = buffer;
          } 
          strcpy((*array_of_persons + number_of_persons)->name_,
          first_person_name);
          (*array_of_persons + number_of_persons)->gender_ = first_person_gender;
          (*array_of_persons + number_of_persons)->mother_ = NULL;
          (*array_of_persons + number_of_persons)->father_ = NULL;
          (*array_of_persons + (number_of_persons + 1))->gender_ = 3;
          child->father_->father_ = (*array_of_persons + number_of_persons);
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
        int number_of_persons = numberOfPersons(*array_of_persons);
        child->father_ = addUnknownPerson(*array_of_persons, first_person_gender);
        number_of_persons++;
        if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
        {
          Person *buffer = realloc(*array_of_persons, sizeof(Person)*
            (number_of_persons * 5));
          if(buffer == NULL)
          {
            free(*array_of_persons);
            showError(ERROR_OUT_OF_MEMORY);
            exit(ERROR_OUT_OF_MEMORY);
          }
          *array_of_persons = buffer;
        } 
        strcpy((*array_of_persons + number_of_persons)->name_, first_person_name);
        (*array_of_persons + number_of_persons)->gender_ = first_person_gender;
        (*array_of_persons + number_of_persons)->mother_ = NULL;
        (*array_of_persons + (number_of_persons + 1))->gender_ = 3;
        child->father_->father_ = (*array_of_persons + number_of_persons);
      }
    }
    else
    {
      int number_of_persons = numberOfPersons(*array_of_persons);
      Person *father = addUnknownPerson(*array_of_persons, 0);
      number_of_persons++;
      if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
      {
        Person *buffer = realloc(*array_of_persons, sizeof(Person)*
          (number_of_persons * 5));
        if(buffer == NULL)
        {
          free(*array_of_persons);
          showError(ERROR_OUT_OF_MEMORY);
          exit(ERROR_OUT_OF_MEMORY);
        }
        *array_of_persons = buffer;
      }      
      strcpy((*array_of_persons + number_of_persons)->name_, second_person_name);
      (*array_of_persons + number_of_persons)->gender_ = second_person_gender;
      (*array_of_persons + number_of_persons)->mother_ = NULL;
      strcpy((*array_of_persons + (number_of_persons + 1))->name_,
       first_person_name);
      (*array_of_persons + (number_of_persons + 1))->gender_ = first_person_gender;
      (*array_of_persons + (number_of_persons + 1))->father_ = NULL; 
      (*array_of_persons + (number_of_persons + 1))->mother_ = NULL;
      (*array_of_persons + (number_of_persons + 2))->gender_ = 3;
      (*array_of_persons + number_of_persons)->father_ = father;
      father->father_ = (*array_of_persons + (number_of_persons + 1));
    } 
}

//------------------------------------------------------------------------------
///
/// Add Relation  unknown person 
/// Add person which we dont't know "?1 [m/f]"
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
  if(number_of_persons > (INIT_PERSONS_ARRAY_SIZE - 5))
  {
    Person *buffer = realloc(array_of_persons, sizeof(Person)*
      (number_of_persons * 5));
    if(buffer == NULL)
    {
      free(array_of_persons);
      showError(ERROR_OUT_OF_MEMORY);
      exit(ERROR_OUT_OF_MEMORY);
    }
    array_of_persons = buffer;
  } 
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
/// Wait for input
/// put on screen "esp>" while program is running, and close program if EOF 
/// make free on close
///
/// @param person_array
///
/// @return 
//
void waitForInput(Person **persons_array)
{
  char input_command[INPUT_COMMAND_LENGHT] = "init"; // Just initalisation in
  // order to make sure that our variable is there
  while(TRUE)
  {
    printf("esp> ");
    fgets(input_command,INPUT_COMMAND_LENGHT, stdin);
    if(strlen(input_command) != 1 && !feof(stdin))
    {
        parseInput(input_command, persons_array);
    }
    if (feof(stdin))
    {
      free(*persons_array);
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
/// Use array of persons, sort it and write persons to the file in right format
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
    Person *persons_sorted = sortPersons(persons_to_write);
    while((persons_sorted + counter)->gender_ != 3)
    {
      if((persons_sorted + counter)->mother_ != NULL)
      {
        fprintf(file_stream, "  \"%s [%c]\" -> \"%s [%c]\";\n", (persons_sorted 
         + counter)->name_, ((persons_sorted + counter)->gender_ == TRUE) ?
        'f' : 'm',
          ((persons_sorted + counter)->mother_)->name_, 'f');
      }
      if((persons_sorted + counter)->father_ != NULL)
      {
        fprintf(file_stream, "  \"%s [%c]\" -> \"%s [%c]\";\n", 
          (persons_sorted + counter)->name_,
          (((persons_sorted + counter)->gender_) == TRUE) ? 'f' : 'm', 
          ((persons_sorted + counter)->father_)->name_, 'm');
      }
      if((persons_sorted + counter)->father_ == NULL && (persons_sorted + 
        counter)->mother_ == NULL)
      {
        fprintf(file_stream, "  \"%s [%c]\";\n", (persons_sorted + 
          counter)->name_, ((persons_sorted + counter)->gender_ == TRUE) ?
           'f' : 'm');
      }
      counter++;
    }
    fprintf(file_stream, "}");
    fclose(file_stream);
    free(persons_sorted);
    showSuccessMessage(MSG_SUCCESS_CREATING_DOT_FILE);
    return TRUE;
  }
  else if(!fileExists(file_name))
  {
    FILE *file_stream;
    int counter = 0;
    file_stream = fopen(file_name, "wb");
    fprintf(file_stream, "digraph FamilyTree\n");
    fprintf(file_stream, "{\n");
    Person *persons_sorted = sortPersons(persons_to_write);
    while((persons_sorted + counter)->gender_ != 3)
    {
      if((persons_sorted + counter)->mother_ != NULL)
      {
        fprintf(file_stream, "  \"%s [%c]\" -> \"%s [%c]\";\n", (persons_sorted
          + counter)->name_, ((persons_sorted + counter)->gender_ == TRUE) ? 'f'
        : 'm', ((persons_sorted + counter)->mother_)->name_, 'f');
      }
      if((persons_sorted + counter)->father_ != NULL)
      {
        fprintf(file_stream, "  \"%s [%c]\" -> \"%s [%c]\";\n", (persons_sorted
         + counter)->name_, 
          (((persons_sorted + counter)->gender_) == TRUE) ? 'f' : 'm', 
          ((persons_sorted + counter)->father_)->name_, 'm');
      }
      if((persons_sorted + counter)->father_ == NULL && (persons_sorted + 
        counter)->mother_ == NULL)
      {
        fprintf(file_stream, "  \"%s [%c]\";\n",(persons_sorted + 
          counter)->name_, ((persons_sorted + counter)->gender_ == TRUE) ? 
        'f' : 'm');
      }
      counter++;
    }
    fprintf(file_stream, "}");
    showSuccessMessage(MSG_SUCCESS_CREATING_DOT_FILE);
    free(persons_sorted);
    fclose(file_stream);
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
///
/// Store file into memory
/// Convert .dot file content into long string
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
        fclose(file_stream);
        exit(ERROR_FILE_COULD_NOT_BE_READ);
      }
      if(fseek(file_stream, 0L, SEEK_END) == 0)
      {
          long buffer_size = ftell(file_stream);
          if(buffer_size == -1)
          {
            showError(ERROR_OUT_OF_MEMORY);
            fclose(file_stream);
            exit(ERROR_OUT_OF_MEMORY);
          }
          file_content = (char*)malloc(sizeof(char)*(buffer_size + 1));
          if(file_content == NULL)
          {
            showError(ERROR_OUT_OF_MEMORY);
            fclose(file_stream);
            exit(ERROR_OUT_OF_MEMORY);
          }
          if(fseek(file_stream, 0L, SEEK_SET) != 0)
          {
            free(file_content);
            showError(ERROR_FILE_COULD_NOT_BE_READ);
            fclose(file_stream);
            exit(ERROR_FILE_COULD_NOT_BE_READ);
          }
          size_t new_lenght = fread(file_content, sizeof(char), buffer_size, 
            file_stream);
          if(ferror(file_stream) != 0)
          {
            free(file_content);
            showError(ERROR_FILE_COULD_NOT_BE_READ);
            fclose(file_stream);
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
/// We are sorting array of persons, and giving new sorted array back
///
/// @param persons
///
/// @return new sorted array
//
Person *sortPersons(Person *persons)
{
  int number_of_persons = numberOfPersons(persons);
  int counter;
  Person *person_placeholder = malloc(sizeof(Person));
  int switch_counter;
  Person *persons_sorted = malloc(sizeof(Person)*(number_of_persons + 1));
  for (counter = 0; counter < number_of_persons; counter++)
  {
    copyPerson(persons_sorted + counter,persons + counter);
  }
  persons_sorted[number_of_persons].gender_ = 3;
  for(counter = 0; counter < (number_of_persons - 1); counter++)
  {
    for(switch_counter = counter + 1; switch_counter < number_of_persons;
      switch_counter++)
    {
      if(strcmp((persons_sorted + counter)->name_,(persons_sorted +
        switch_counter)->name_) > 0)
      {
        copyPerson(person_placeholder, persons_sorted + counter);
        copyPerson(persons_sorted + counter, persons_sorted + switch_counter);
        copyPerson(persons_sorted + switch_counter, person_placeholder);
      }
      else if(strcmp((persons_sorted + counter)->name_,(persons_sorted +
        switch_counter)->name_) == 0 && (persons_sorted + counter)->gender_ < 
        (persons_sorted
       +
        switch_counter)->gender_)
      {
        copyPerson(person_placeholder, persons_sorted + counter);
        copyPerson(persons_sorted + counter, persons_sorted + switch_counter);
        copyPerson(persons_sorted + switch_counter, person_placeholder);
      }
      else if(namesArePartiallyEqual((persons_sorted + counter)->name_,
        (persons_sorted + switch_counter)->name_) && (strlen((persons_sorted +
         counter)->name_) > strlen((persons_sorted + switch_counter)->name_)))
      {
        copyPerson(person_placeholder, persons_sorted + counter);
        copyPerson(persons_sorted + counter, persons_sorted + switch_counter);
        copyPerson(persons_sorted + switch_counter, person_placeholder);
      }
    }
  }

  free(person_placeholder);
  return persons_sorted;
}
//------------------------------------------------------------------------------
///
/// Check if two persons have the same name untill " "(whitspaces)
/// function needed for sorting
/// 
/// @param first_name
/// @param second_name
/// 
/// @return TRUE/FALSE
//
BOOL namesArePartiallyEqual(char const *first_name, char const *second_name)
{
  char first_name_placeholder[MAX_NAME_LENGHT]; //This had to be done, due to
  // strtok changing the original string
  char second_name_placeholder[MAX_NAME_LENGHT];
  char *first_name_token;
  char *second_name_token;
  strcpy(first_name_placeholder,first_name);
  strcpy(second_name_placeholder,second_name);

  first_name_token = strtok(first_name_placeholder," ");
  second_name_token = strtok(second_name_placeholder," ");

  if(strcmp(first_name_token, second_name_token) == 0)
  {
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
///
/// List Persons
/// list all persons from given array
///
/// @param persons
///
/// @return TRUE/FALSE (false if no person has been found)
//
BOOL listPersons(Person *persons)
{
  int counter = 0;
  Person *persons_sorted = sortPersons(persons);
  while((persons_sorted + counter)->gender_  != 3)
  {
    printf("%s %s\n", (persons_sorted + counter)->name_, ((persons_sorted + 
      counter)->gender_
    == 1) ? "[f]" : "[m]");
    counter++;
  }
  free(persons_sorted);
  if(counter < 1)
  {
    return FALSE;
  }
  return TRUE;
}

//------------------------------------------------------------------------------
///
/// Copy person
/// copy person from one location to another
///
/// @param first_person
/// @param second_person
///
/// @return TRUE
/// NOTE: could've been void function ? 
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
/// count the number of persons in a given array
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
/// Find person
/// find person in an array
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
    if(strcmp((persons + counter)->name_, name) == 0 && (persons + counter)->
      gender_ == gender)
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
    printf("[ERR] Wrong usage - add <namePerson1> [m/f] "
      "<relation> <namePerson2>[m/f].\n");
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
    case ERROR_WRONG_RELATIONSHIP_USAGE:
    printf("[ERR] Wrong usage - relationship <namePerson1> [m/f] <namePerson2>"
    " [m/f].\n");
    break;
    case ERROR_RELATIONSHIP_PERSON_DOES_NOT_EXISTS:
    printf("[ERR] At least one person does not exist yet.\n");
    break;
    case ERROR_RELATION_NOT_RELATED:
    printf("There is no relationship between them.\n");
  }
}

//------------------------------------------------------------------------------
///
/// Show Success Message
/// print success Message
///
/// @param msg_code
///
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
    case MSG_RELATION_PEOPLE_ARE_RELATED:
    printf("The two people are related.\n");
    break;
  }
}