#include <stdio.h>
#include <stdlib.h>

#include "rs232.h"
#include "serial.h"
#include "functions.h"

#define bdrate 115200               /* 115200 baud */

void SendCommands (char *buffer );

int main()
{
    // **********************************************************************
    // Read and store font data

    char *FileName = "SingleStrokeFont.txt"; //create variable with the name of the file with single stroke font data
    size_t Num_of_characters; //total number of characters which data will be stored
    Character *characters; //initiate the array of structs to store the single stroke font data for each character

    int F_test = ReadAndStoreFontData(FileName, &characters, &Num_of_characters); //read and store font data in created struct
    if(F_test != 0) // test if the function executed correctly
    {
        perror("Error in ReadAndStoreFontData()");
        return 1;
    }

    //**************************************************************************
    // Get font size form the user

    int FontSize;
    float Scale;
    F_test = GetFontSizeAndScale(&FontSize, &Scale); // get the font size from the user and calculate the scale to scale the GCode
    if(F_test != 0) // test if the function executed correctly
    {
        printf("Error in GetFontSizeAndScale()");
        return 1;
    }

    printf("Font size and scale %d\t%f\n", FontSize, Scale); // print out the font and scale 

    //******************************************************************************
    // Get text file name from the user with the text to be printed out

    char TextFileName[100];
    int count = 0;
    FILE *TextFile;

    do{
        printf("\nPlease provide the name of the file with the text to be printed (include the .txt):\n");
        scanf("%s", TextFileName);
        // open the file 
        TextFile = fopen(TextFileName,"r");
        count++;
    }while(TextFile == NULL && count<10);

    if (TextFile == NULL) // test if the file opened correctly
    {
        perror("Error opening the text file");
        return 1;
    }

    //***********************************************************************
    // Scan the file for the number of characters to be printed out

    int ch; // character buffer
    size_t letterCount = 0;
    while ((ch = fgetc(TextFile)) != EOF)
    {
        letterCount++;
        //printf("\n%d\t%c",ch, ch); // print out the text for diagnostics
    }
    fclose(TextFile);

    // Initiate and alocate memory to the array of structures used to store gcode for each character to be printed out
    CharacterGCode *charactersGCode;
    charactersGCode = calloc( letterCount, sizeof( CharacterGCode ) );
    
    //**************************************************************************** */
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

    //********************************************************************************** */
    // Generate the GCode and send it to the robot to be printed out
    float OffsetX = 0.0, OffsetY = 0.0, CharacterX = 0.0;
    int letter = 0, s = 0;
    TextFile = fopen(TextFileName,"r");

    while(1) // scan the TextFile letter by letter
    {
        ch = fgetc(TextFile); //buffer storing the letter being processed in the current iteration of the loop

        if (ch == 32) // if read character is a space " "
        {
            for ( int m = 0; m < (int)Num_of_characters; m++ ) // generate GCode
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
            OffsetX = CharacterX; // offset the writing for next letter by largest X coordinate of the last read and stored character
            letter++;
            while( s < letter ) //send code to the robot
            {
                for (int o = 0; o < charactersGCode[s].n_lines; o++) // send the generated GCode through the buffer line by line to the robot
                {
                    sprintf (buffer, "%s %s %s %s\n", charactersGCode[s].line[o].S, charactersGCode[s].line[o].G, charactersGCode[s].line[o].X, charactersGCode[s].line[o].Y);
                    SendCommands(buffer);
                }  
                s++;
            }
        }else if (ch == 10) // if the read character is ASCII code for a new line
        {
            for ( int m = 0; m < (int)Num_of_characters; m++ ) // generate GCode
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
            letter++;
            while( s < letter ) //send code to the robot
            {
                for (int o = 0; o < charactersGCode[s].n_lines; o++)
                {
                    sprintf (buffer, "%s %s %s %s\n", charactersGCode[s].line[o].S, charactersGCode[s].line[o].G, charactersGCode[s].line[o].X, charactersGCode[s].line[o].Y);
                    SendCommands(buffer);
                }  
                s++;
            }
            OffsetX = 0; // reset X to the start of the writing area
            CharacterX = 0;
            OffsetY = OffsetY - (float)((5.0 + (float)FontSize)); // offset the Y to write on the next line
        }else // procesing all other characters
        {
            for ( int m = 0; m < (int)Num_of_characters; m++ ) // generate GCode
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
            OffsetX = CharacterX;
            letter++;
        }

        if( feof(TextFile) ) // end the while(1) loop at the end of text file and send the final GCode
        {
            while( s < letter ) //send code to the robot
            {
                for (int o = 0; o < charactersGCode[s].n_lines; o++)
                {
                    sprintf (buffer, "%s %s %s %s\n", charactersGCode[s].line[o].S, charactersGCode[s].line[o].G, charactersGCode[s].line[o].X, charactersGCode[s].line[o].Y);
                    SendCommands(buffer);
                }  
                s++;
            }
            break;
        }
    }

    // Close text file
    fclose(TextFile);

    //*********************************************************************************************** */
    // Before we exit the program we need to close the COM port
    CloseRS232Port();
    printf("Com port now closed\n");

    //************************************************************************************************** */
    // Free alocated memory
    F_test = freeCharacters(characters, charactersGCode, &Num_of_characters);
    if(F_test != 0) // test if the function executed correctly
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
