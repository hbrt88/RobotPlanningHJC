#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"


int GenerateGCode(int m, int letter, CharacterGCode **charactersGCode, Character **characters, float Scale, float OffsetX, float OffsetY, float *CharacterX)
{
    (*charactersGCode)[letter].n_lines = (*characters)[m].n_lines;
    float ScaledX, ScaledY;
    (*charactersGCode)[letter].line = calloc( (size_t)(*characters)[m].n_lines, sizeof( GCodeCoordinates ) );
    if ((*charactersGCode)[letter].line == NULL)
    {
        perror("Failed memory allocation");
        return 1;
    }

    for( int i = 0; i < (*characters)[m].n_lines; i++)
    {
        ScaledX = ((*characters)[m].line[i].fontX) * Scale + OffsetX;
        ScaledY = ((*characters)[m].line[i].fontY) * Scale + OffsetY;
        
        if(i == 0)
        {
            sprintf((*charactersGCode)[letter].line[i].X, "X%f", 0.0);
            sprintf((*charactersGCode)[letter].line[i].Y, "Y%f", 0.0);
            sprintf((*charactersGCode)[letter].line[i].G, "G%d", 1);
            sprintf((*charactersGCode)[letter].line[i].S, "S%d", 0);
        }else
        {
            sprintf((*charactersGCode)[letter].line[i].X, "X%f", ScaledX);
            sprintf((*charactersGCode)[letter].line[i].Y, "Y%f", ScaledY);
            sprintf((*charactersGCode)[letter].line[i].G, "G%d", ((*characters)[m].line[i].P));
            sprintf((*charactersGCode)[letter].line[i].S, "S%d", ((*characters)[m].line[i].P * 1000));
        }

        if( ScaledX > *CharacterX )
        {
            *CharacterX = ScaledX;
        }
    }
    return 0;
}

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
    *characters = calloc( *Num_of_characters, sizeof( Character ) );
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

int GetFontSizeAndScale( int *FontSize, float *Scale )
{
    int count = 0;
    do
    {
        if(count >= 10)
        {
            printf("User didnt provie correct size of font 10 times\n");
            return 1;
        }
        printf("\nPlease provide the font size of the printed text:\n");
        scanf("%d", FontSize);
        count++;
    }while((*FontSize>10) || (*FontSize<4));
    
    *Scale = (float)*FontSize/18;
    return 0;
}

int freeCharacters(Character *characters, CharacterGCode *charactersGCode, size_t *Num_of_characters)
{
    for (int k = 0; k < (int)*Num_of_characters; k++)
    {
        free(characters[k].line);
    }

    for (int l = 0; l < (int)sizeof(charactersGCode); l++)
    {
        free(characters[l].line);
    }
    free(characters);
    free(charactersGCode);
    return 0;
}