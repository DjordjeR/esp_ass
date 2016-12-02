typedef short BOOL;
#define TRUE 1
#define FALSE 0

BOOL fileExistst(const char *file_name) // Provjeravamo da li fajl postoji 
{
    FILE *file_stream;
    if((file_stream = fopen(file_name,"r")))
    {
        fclose(file_stream);
        return TRUE;
    }
    return FALSE;
}

void storeFileIntoMemory()
{
    char *file_content = NULL; // TODO : Dodati provjeru da li fajl postoji
    FILE *file_stream = fopen("black_fam-tree.dot","r");

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
    printf("%s\n", file_content);
    free(file_content); // Ovo ce ici negdje drugdje poslije
}