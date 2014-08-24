//
//  brainfuck.c
//
//  This is a bit of nice readable C code that interperets
//  a given Brainfuck program on the fly. It's probably not
//  the fastest out there, but it's worked in all of my tests.
//
//  Created by Paul Hollinsky on 8/23/14.
//  Copyright (c) 2014 Paul Hollinsky. All rights reserved.
//

#include <stdio.h>	// Used for the input and output
#include <stdlib.h>	// Used for file reading
#include <string.h> // Used for string lengths
#include <time.h>	// Used for benchmarking

char array[30000]; // Uses the original spec of a 30k position array
char *ptr = array;

int time_program = 0; // Set to true if you want to time the execution of the program

char program[30000]; // String containing the program to be executed
unsigned long program_length;

int find_end_position(char* program, int start_character)
{
	int current_position = start_character + 1;
	int levels = 0;
	while (current_position <= program_length) // Stop searching if we're going to hit the end of the program.
	{
		if (program[current_position] == ']')
		{
			if (levels == 0) // This makes sure it isn't the wrong ending brace
			{
				return current_position - 1;
			}
			else
			{
				levels--; // If this is an inner brace, we go up a level.
			}
		}
		else if (program[current_position] == '[')
		{
			levels++;
		}
		current_position++; // Didn't find it, check the next character.
	}
	return -1;
}

int exec(char* program, int current_position)
{
	if (program[current_position] == '>') // Move pointer right
        ++ptr;
    else if (program[current_position] == '<') // Move pointer left
        --ptr;
    else if (program[current_position] == '+') // Increment value
        ++*ptr;
    else if (program[current_position] == '-') // Decrement value
        --*ptr;
    else if (program[current_position] == '.') // Output character
        putchar(*ptr);
    else if (program[current_position] == ',') // Input character
        *ptr = getchar();
    else if (program[current_position] == '[') // The start of a loop
    {
        int start_position = current_position + 1; // The first instruction in the loop
		int end_position = find_end_position(program, current_position);
		if (end_position == -1)
		{
			printf("ERROR: No ending match for loop brace on position %d.\n", current_position);
			exit(EXIT_FAILURE);
		}
		char loop[end_position - start_position]; // Creates a string to hold the loop as a seperate 'program'
		int line_counter = 0;
		for(int i = start_position; i <= end_position; i++)
		{
			loop[line_counter] = program[i]; // Copies the loop into it's own program array
			line_counter++;
		}
		unsigned long loop_length = end_position - start_position + 1;
        while (*ptr)
        {
            for (int i = 0; i < loop_length; i++) //
			{
				i = exec(loop, i);
			}
        }
		return end_position + 1; // Changes the position to be right after the loop
    }
	return current_position; // Doesn't change the current position
}

int main(int argc, char *argv[])
{
	char * buffer = 0;
	long length;
	FILE * file = fopen (argv[1], "rb"); // Choose the file from the program's arguments
	
	if (file) // This whole block reads the file into memory
	{
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		buffer = malloc(length);
		if (buffer)
		{
			fread(buffer, 1, length, file);
		}
		fclose(file);
	}
	
	if (buffer)
	{
		strcpy(program, buffer); // Copy the buffer into the program variable
	}
	else
	{
		printf("Please specify a file to run.\n");
	}
	
	clock_t begin = 0, end;
	double time_spent;
	if (time_program)
	{
		begin = clock();
	}
    program_length = strlen(program); // Figures out how long the program is
    for (int i = 0; i < program_length; i++) // Loops over the program
    {
        i = exec(program, i); // Processes the current character
		// i is reset by exec in case exec encounters a loop
		// A loop will result in a different counter when it finishes looping.
    }
	if (time_program)
	{
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		printf("\nTime spent: %f seconds\n", time_spent);
	}
    return 0;
}