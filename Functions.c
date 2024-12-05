#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"

int ReadAndStoreFontData(char *FileName, Character **characters, size_t *Num_of_characters)
{
    FILE *file;
    file = fopen(FileName,"r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    char buffer[50]; //buffer for each line
    *Num_of_characters = 0;
    int buffer2[3];

    // First Scan of the file to get number of caracters to later allocate to arrays of structures
    while(!feof(file))
    {
        fgets(buffer, 50, file);
        sscanf(buffer, "%d %d %d", &buffer2[0], &buffer2[1], &buffer2[2]);
        if( buffer2[0] == 999 )
        {
            (*Num_of_characters)++;
        }
    }
    fclose(file);

    // Allocate memory to the array of characters
    *characters = calloc( *Num_of_characters, sizeof( Character ) );//(Character *) malloc( Num_of_characters * sizeof(*characters) );
    if (*characters == NULL)
    {
        perror("Failed to allocate memory");
        return 2;
    }

    file = fopen(FileName,"r");
    if (file == NULL)
    {
        perror("Error opening file");
        free(*characters);
        return 1;
    }
 
    int i = 0, j = 0, buffer3[3];
    // Second Scan of the file to fill the array of structures with coordinates
    while(!feof(file))
    {
        fgets(buffer, 50, file);
        sscanf(buffer, "%d %d %d", &buffer3[0], &buffer3[1], &buffer3[2]);
        if( buffer3[0] == 999 )
        {
            (*characters)[j].ASCII_Code = buffer3[1];
            (*characters)[j].n_lines = buffer3[2];
            (*characters)[j].line = calloc( (size_t)buffer3[2], sizeof( Coordinates ) );
            if (*characters == NULL)
            {
                perror("Failed to allocate memory");
                return 2;
            }
            buffer3[2] = 0;
            i = 0;
            j++;
        }else
        {
            sscanf(buffer, "%f %f %d", &(*characters)[j-1].line[i].fontX, &(*characters)[j-1].line[i].fontY, &(*characters)[j-1].line[i].P);
            i++;
        }
    }
    fclose(file);
    return 0;
}

int freeCharacters(Character *characters, size_t *Num_of_characters)
{
    for (int k = 0; k < Num_of_characters; k++)
    {
        free(characters[k].line);
    }
    free(characters);
    return 0;
}