#ifndef functions_h
#define functions_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    float fontX;
    float fontY;
    int P;
} Coordinates; //struct of coordinates in a single line for a single character

typedef struct {
    int ASCII_Code; // ASCII code of the character
    int n_lines;
    Coordinates *line; //length = number of n_lines of coordinates for each character
} Character; // struct of all coordinates for a single character

int ReadAndStoreFontData(char *FileName, Character **characters, size_t *Num_of_characters);
int GetFontSizeAndScale( int *FontSize, float *Scale );
int freeCharacters(Character *characters, size_t *Num_of_characters);
#endif // functions_h