#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>
//#include <windows.h>
#include "rs232.h"
#include "serial.h"

#include "Functions.h"

#define bdrate 115200               /* 115200 baud */

void SendCommands (char *buffer );

int main()
{
    // read start
    char *FileName = "SingleStrokeFont.txt";//"TestData.txt";
    size_t Num_of_characters;
    Character *characters;

    int F_test = ReadAndStoreFontData(FileName, &characters, &Num_of_characters);
    if(F_test != 0)
    {
        perror("Error in ReadAndStoreFontData()");
        return 1;
    }

    // Print the characters
    for (int n = 0; n < (int)Num_of_characters; n++)
    {
        printf("\nCharacter: %c\tn_lines: %d\n", characters[n].ASCII_Code, characters[n].n_lines);
        for (int l = 0; l < characters[n].n_lines; l++)
        {
            printf("%d %d %d\n", (int)characters[n].line[l].fontX, (int)characters[n].line[l].fontY, (int)characters[n].line[l].P);
        }
    }
    printf("\nNumber of characters: %zu\n", Num_of_characters);
    // read end

    // Get font size form the user
    int FontSize;
    float Scale;
    F_test = GetFontSizeAndScale(&FontSize, &Scale);
    if(F_test != 0)
    {
        printf("Error in GetFontSizeAndScale()");
        return 1;
    }

    printf("Font size and scale %d\t%f\n", FontSize, Scale);

    // get text file name from the user with the text to be printed out
    char TextFileName[100];
    int count = 0;
    FILE *TextFile;
    do
    {
        printf("\nPlease provide the name of the file with the text to be printed (include the .txt):\n");
        scanf("%s", TextFileName);
        // open the file 
        TextFile = fopen(TextFileName,"r");
        count++;
    }while(TextFile == NULL && count<10);

    if (TextFile == NULL)
    {
        perror("Error opening the text file");
        return 1;
    }

    // scan the file for the number of characters
    int ch;
    size_t letterCount = 0;
    while ((ch = fgetc(TextFile)) != EOF)
    {
        letterCount++;
        //printf("\n%d\t%c",ch, ch);
    }
    fclose(TextFile);
    CharacterGCode *charactersGCode;
    charactersGCode = calloc( letterCount, sizeof( CharacterGCode ) ); // allocate memory to array of structures for each character in the text file to be filed out later with gcode
    
    //char mode[]= {'8','N','1',0};
    char buffer[100];

    // If we cannot open the port then give up immediately
    if ( CanRS232PortBeOpened() == -1 )
    {
        printf ("\nUnable to open the COM port (specified in serial.h) ");
        exit (0);
    }

    // Time to wake up the robot
    printf ("\nAbout to wake up the robot\n");

    // We do this by sending a new-line
    sprintf (buffer, "\n");
     // printf ("Buffer to send: %s", buffer); // For diagnostic purposes only, normally comment out
    PrintBuffer (&buffer[0]);
    Sleep(100);

    // This is a special case - we wait  until we see a dollar ($)
    WaitForDollar();

    printf ("\nThe robot is now ready to draw\n");

    //These commands get the robot into 'ready to draw mode' and need to be sent before any writing commands
    sprintf (buffer, "G1 X0 Y0 F1000\n");
    SendCommands(buffer);
    sprintf (buffer, "M3\n");
    SendCommands(buffer);
    sprintf (buffer, "S0\n");
    SendCommands(buffer);

    float OffsetX = 0.0, OffsetY = 0.0, CharacterX = 0.0, DrawingX = 0.0;
    int letter = 0, WordsInLine = 0, s = 0;
    // compare ch to characters.
    // open generategcode with that characters. as inpput
    // put that g code into buffer and send
    // Read the test text file leter by letter and and convert coordinates for that letter into g code
    TextFile = fopen(TextFileName,"r");
    while((ch = fgetc(TextFile)) != EOF ) //!feof(TextFile))
    {
        //ch = fgetc(TextFile);
        //printf("letter: %d/t ch: %s", letter, ch);
        for ( int m = 0; m < (int)Num_of_characters; m++ )
        {
            if( characters[m].ASCII_Code == ch )
            {
                F_test = GenerateGCode(m, letter, &charactersGCode, &characters, Scale, OffsetX, OffsetY, &CharacterX);
                if(F_test != 0)
                {
                    printf("Error in GetFontSizeAndScale()");
                    return 1;
                }
            }
        }
        if(ch == 32)// end of word
        {
            WordsInLine++;

            if( (DrawingX > 100) || (ch = 10) || (ch = 13))
            {
                OffsetY = OffsetY + 5.0 + (float)FontSize;
                DrawingX = 0;
                WordsInLine = 1;
            }
            OffsetX = DrawingX;
                    //send gcode to the robot
        while(s < letter)
        {
            for (int o = 0; o < charactersGCode[s].n_lines; o++)
            {
                //memset(buffer, '\0', sizeof(buffer));
                //strcat(buffer, charactersGCode[s].line[o].S);
                //strcat(buffer, charactersGCode[s].line[o].G);
                //strcat(buffer, charactersGCode[s].line[o].X);
                //strcat(buffer, charactersGCode[s].line[o].Y);
                //sscanf(buffer, "%s\n %s %s %s\n", charactersGCode[s].line[o].S, charactersGCode[s].line[o].G, charactersGCode[s].line[o].X, charactersGCode[s].line[o].Y);
                sprintf (buffer, "%s\n%s %s %s", charactersGCode[s].line[o].S, charactersGCode[s].line[o].G, charactersGCode[s].line[o].X, charactersGCode[s].line[o].Y);
                SendCommands(buffer);
            }
            s++;
        }
        }else if( DrawingX >= 100 && WordsInLine <= 1)
        {
            printf(" Word length is larger than drawing length\n ");
            return 1;
        }
        letter++;
    }
/*
    // These are sample commands to draw out some information - these are the ones you will be generating.
    sprintf (buffer, "G0 X-13.41849 Y0.000\n");
    SendCommands(buffer);
    sprintf (buffer, "S1000\n");
    SendCommands(buffer);
    sprintf (buffer, "G1 X-13.41849 Y-4.28041\n");
    SendCommands(buffer);
    sprintf (buffer, "G1 X-13.41849 Y0.0000\n");
    SendCommands(buffer);
    sprintf (buffer, "G1 X-13.41089 Y4.28041\n");
    SendCommands(buffer);
    sprintf (buffer, "S0\n");
    SendCommands(buffer);
    sprintf (buffer, "G0 X-7.17524 Y0\n");
    SendCommands(buffer);
    sprintf (buffer, "S1000\n");
    SendCommands(buffer);
    sprintf (buffer, "G0 X0 Y0\n");
    SendCommands(buffer);
*/

    // Before we exit the program we need to close the COM port
    CloseRS232Port();
    printf("Com port now closed\n");

    // Close text file
    fclose(TextFile);
    // Free alocated memory
    F_test = freeCharacters(characters, charactersGCode, &Num_of_characters);
    if(F_test != 0)
    {
        perror("Error in freeCharacters()");
        return 1;
    }

    return (0);
}

// Send the data to the robot - note in 'PC' mode you need to hit space twice
// as the dummy 'WaitForReply' has a getch() within the function.
void SendCommands (char *buffer)
{
    //printf ("Buffer to send: %s", buffer); // For diagnostic purposes only, normally comment out
    PrintBuffer (&buffer[0]);
    WaitForReply();
    Sleep(100); // Can omit this when using the writing robot but has minimal effect
    // getch(); // Omit this once basic testing with emulator has taken place
}
