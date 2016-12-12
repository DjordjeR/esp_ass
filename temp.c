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