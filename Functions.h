#include <stdio.h>
#include <stdlib.h>
#ifndef Functions_H_INCLUDED
#define Functions_H_INCLUDED

typedef struct {
    float fontX;
    float fontY;
    int P;
} Coordinates; //struct of coordinates in a single line for a single character

typedef struct {
    char *name; //array of structures (eg. name[1] = )
    struct Coordinates *line; //length = number of lines of coordinates for each character
} Character; // struct of all coordinates for a single character

int ReadAndStoreFontData( char *FileName, struct FontData *Character );

#endif //Functions_H_INCLUDED