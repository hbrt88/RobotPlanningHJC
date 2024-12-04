#include <stdio.h>
#include <stdlib.h>
#include "Functions.h"

int ReadAndStoreFontData( char *FileName, struct FontData *Character[] )
{
    int line = 0, character = 0;
    FILE *file;
    file = fopen(FileName, "r");
    if (file == NULL)
    {
        printf("Error opening the file\n");
        return(0);
    }
    
    char buffer[50];
    int buffer_line[3];
    int Num_of_characters = 0;
    fgets(buffer, 200, file);
    while(!feof(file)) // count the number of characters to be stored in the array of structs
    {
        sscanf(buffer, "%d %d %d", &buffer_line[0], &buffer_line[1], &buffer_line[2]);
        if( buffer_line[3] == 999 )
        {
            Num_of_characters++;
        }
    }
    FontData Character[Num_of_characters];

    while(!feof(file))
    {
        FontData *Character = character + 
        if(buffer == "999")
        FontData *Character = character
        //check 999
    }
    return 1;
}