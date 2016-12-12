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
#define INIT_PERSONS_ARRAY_SIZE 2

typedef struct _Person_
{
    char name_[MAX_NAME_LENGHT];
    BOOL gender_;
    struct _Person_ *mother_;
    struct _Person_ *father_;
}Person;

// forward declaration
Person **parseDotFile(char *file_content);
// forward declaration
void showError(short error_code);
// forward declaration
void showSuccessMessage(short msg_code);
// forward declaration
char *storeFileIntoMemory(const char *file_name);
// forward declaration
Person *findPerson(Person **persons, char const *name, BOOL gender);
// forward declaration
BOOL listPersons(Person **persons);
// forward declaration
BOOL parseSingleFileLine(char *line_to_parse, char *name, BOOL *gender_b, char
*parrent_name, BOOL *parrent_gender_b);
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
    char *file_content = storeFileIntoMemory("black_fam-tree.dot");
    Person **persons_array = parseDotFile(file_content);
    
    return SUCCESS_PROGRAM_CLOSED;
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
/// List of Persons
/// with command list get we all person stored in memory
///
/// @param persons
///
/// @return TRUE/FALSE
//
BOOL listPersons(Person **persons)
{
  int counter = 0;
  //sortPersons(persons);
  while((*persons+counter)->gender_  != 3)
  {
    printf("%s %s\n", (*persons + counter)->name_, ((*persons + counter)->gender_
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
/// We are parsing the file contents that have been loaded into memory, into
/// string
///
/// @param file_content string, with dot file data to be parsed
///
/// @return array_of_persons, array of persons and their data, dynamicly
/// allocated
//
Person **parseDotFile(char *file_content)
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

    Person **array_of_persons = calloc((lines_separated_counter*10),sizeof(Person*)); //Big chunk of memory for persons, it'll be
    // reallocated later

    array_of_persons[(lines_separated_counter*10)-1] = malloc(sizeof(Person));

    array_of_persons[(lines_separated_counter*10)-1]->gender_ = 3;

    for(counter = 2; counter < lines_separated_counter; counter++ )
    {
        if(parseSingleFileLine(lines_separated[counter], name, &gender, parrent_name, &parrent_gender) == FALSE)
        {
            showError(ERROR_FILE_COULD_NOT_BE_READ);
            exit(ERROR_FILE_COULD_NOT_BE_READ);
        }
        name[strlen(name)-1] = '\0'; // NOTE: Getting rid of last empty space
        if(findPerson(array_of_persons, name, gender) == NULL)
        {
            array_of_persons[number_of_persons] = malloc(sizeof(Person));
            strcpy(array_of_persons[number_of_persons]->name_,name);
            array_of_persons[number_of_persons]->gender_ = gender;
            if(parrent_name[0] != '\0')
            {
                if(findPerson(array_of_persons,parrent_name,parrent_gender) == NULL)
                {
                    array_of_persons[number_of_persons + 1] = malloc(sizeof(Person));
                    strcpy(array_of_persons[number_of_persons+1]->name_,parrent_name);
                    array_of_persons[number_of_persons+1]->gender_ = parrent_gender;
                    if(parrent_gender == TRUE)
                    {
                        array_of_persons[number_of_persons]->mother_ = array_of_persons[number_of_persons+1];
                    }
                    else if(parrent_gender == FALSE)
                    {
                        array_of_persons[number_of_persons]->father_ = array_of_persons[number_of_persons+1];
                    }
                    number_of_persons++;
                }
                else
                {
                    Person *parrent = findPerson(array_of_persons,parrent_name,parrent_gender);
                    if(parrent_gender == TRUE)
                    {
                        array_of_persons[number_of_persons]->mother_ = parrent;
                    }
                    else if(parrent_gender == FALSE)
                    {
                        array_of_persons[number_of_persons]->father_ = parrent;
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
                        array_of_persons[number_of_persons] = malloc(sizeof(Person));
                        strcpy(array_of_persons[number_of_persons]->name_,parrent_name);
                        array_of_persons[number_of_persons]->gender_ = parrent_gender;
                        new_temp_person->mother_ = array_of_persons[number_of_persons];
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
                        array_of_persons[number_of_persons] = malloc(sizeof(Person));
                        strcpy(array_of_persons[number_of_persons]->name_,parrent_name);
                        array_of_persons[number_of_persons]->gender_ = parrent_gender;
                        new_temp_person->father_ = array_of_persons[number_of_persons];
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
    array_of_persons[number_of_persons] = (Person*)malloc(sizeof(Person));
    array_of_persons[number_of_persons]->gender_ = 3; // This is like \0 in string
    // so we know where our array ends
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
/// Find person
/// search for person
///
/// @param persons
/// @param name
/// @param gender
///
/// @return finded person or NULL
//
Person *findPerson(Person **persons, char const  *name, BOOL gender)
{
    int counter = 0;
    while(*(persons + counter)->gender_ != 3 && (*persons + counter) != NULL)
    {
        if(strcmp((*persons + counter)->name_, name) == 0 && (*persons + counter)->gender_ == gender)
        {
            return (*persons + counter);
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