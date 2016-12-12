#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct _Person_
{
    char name_[256];
    short gender_;
    struct _Person_ *mother_;
    struct _Person_ *father_;
}Person;

void testFunkcija(Person ***person_array_pointers)
{
  printf("%s\n", (*person_array_pointers[0])->name_);

  (*person_array_pointers[1]) = malloc(sizeof(Person));
  strcpy((*person_array_pointers[1])->name_,"stefan");

   *person_array_pointers = realloc(**person_array_pointers,sizeof(Person*)*3);

}

int main(int argc, char const *argv[])
{
  Person **person_array_pointers = malloc(sizeof(Person*)*5);
  Person ***person_array_pointer = &person_array_pointers;

  person_array_pointers[0] = (Person*)malloc(sizeof(Person));
  strcpy((person_array_pointers[0])->name_,"djordje");
  (person_array_pointers[0])->gender_ = 0;
  printf("%s\n", person_array_pointers[0]->name_);

  testFunkcija(person_array_pointer);


  printf("%s\n", person_array_pointers[1]->name_);
  return 0;
}

