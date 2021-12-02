/*

	Name: Chidera Nwankwo

*/

// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017, 2021 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports 10 arguments

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  //keeps track of the last 15 processes that ran
  pid_t pidArray[15];

  //logs the last 15 commands used by user
  char history[15][MAX_COMMAND_SIZE];
  
  // counter for how many times a process has been run, helps flush out oldest process in pidArray
  int j = 0;

  // counter for how many commands used by user
  int k = 0;

  while( 1 )
  {
   
    //empties the command line input before user can type
    memset(cmd_str, '\0', MAX_COMMAND_SIZE);

    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    //empties the breakup of the command line arguments
    memset(token, '\0', MAX_NUM_ARGUMENTS);

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }
    
    // prints another prompt if the user types a blank line
    if(token[0] == NULL)
    {
	continue;
    }

    // terminates shell upon user request
    else if(strcmp(token[0],"quit") == 0 || strcmp(token[0],"exit") == 0)
    {
    	return 0;
    }

    //changes directory of shell	    
    else if(strcmp(token[0],"cd") == 0)
    {
	//directory goes back one layer
	if(token[1] != NULL && strcmp(token[1], "..") == 0)
	{
		chdir("..");
	}
	
	else
	{
		//directory changes to whatever the next argument on the command line is and returns error if directory does not exist
		if(chdir(token[1]) == -1)
		{
			printf("%s: Invalid Directory\n", token[1]);
		}
	}

	continue;
    }

    //shows the IDs of the last 15 processes that ran
    else if(strcmp(token[0], "showpids") == 0)
    {		
	for(int i=0; i < j; i++)
	{
		printf("%d: %d\n", i, pidArray[i]);
	}	
	
	continue; 
    }

    else if(strcmp(token[0], "history") == 0)
    {
	for(int i=0; i<k; i++)
	{
		printf("%d: %s\n", i, history[i]);
	}
	
	continue;
    }

    //makes our command argument the previous one saved in the history array
    else if(token[0][0] == '!')
    {
	// checks if there is a command in the nth place to be called
	if(k == 0 || NULL)
	{
		printf("Command not in history.\n");
		continue;
	}

    	strcpy(token[0], history[token[0][1] - '0']);
    }

    //creates a child process adds new process ID to the ID storage array
    pid_t pid = fork();
   
     
    //if the process ID array is full, then shift all values down to drop off oldest value and add newest one at the top
    if(j>=15)
    {
    	for(int i = 0; i<15; i++)
        {
        	pidArray[i] = pidArray[i+1];
        }

	//points j to the end of the array for the new value to be stored
	j = 14;

    }
    
    //if the history array is full, then shift all values down and add the newest value at the top
    if(k>=15)
    {
	for(int i =0; i<15; i++)
	{
		strcpy(history[i], history [i+1]);
	}

	//points to the end of the history array
	k = 14;

    }

    // adds newest process ID to the storage array
    pidArray[j] = pid;
    j++;

    // adds current command to the command storage array
    strcpy(history[k], token[0]);
    k++;

    // code only executes if child process is currently running
    if(pid == 0 ) 
    {
	//counter returns a non-zero value if the user input command is executed, 0 if no command is found
    	int counter = 0;

	//if the command would not result in an error, then run the command for real
	if(execlp(token[0], token[0], token[1], token[2], token[3], token[4], token[5], token[6], token[7], token[8], token[9], NULL)!=-1)
	{
		execlp(token[0], token[0], token[1], token[2], token[3], token[4], token[5], token[6], token[7], token[8], token[9], NULL);
		counter++;
	}

	if(execl("/usr/local/bin", token[0], token[1], token[2], token[3], token[4], token[5], token[6], token[7], token[8], token[9], NULL)!=-1)
	{
		execl("/usr/local/bin", token[0], token[1], token[2], token[3], token[4], token[5], token[6], token[7], token[8], token[9], NULL);
		counter++;
	}

	if(execl("/usr/bin", token[0], token[1], token[2], token[3], token[4], token[5], token[6], token[7], token[8], token[9], NULL)!=-1)
	{
		execl("/usr/bin", token[0], token[1], token[2], token[3], token[4], token[5], token[6], token[7], token[8], token[9], NULL);
		counter++;
	}

	if(execl("/bin", token[0], token[1], token[2], token[3], token[4], token[5], token[6], token[7], token[8], token[9], NULL)!=-1)
	{
		execl("/bin", token[0], token[1], token[2], token[3], token[4], token[5], token[6], token[7], token[8], token[9], NULL);
		counter++;
	}
	
	//returns an error to the user only after searching all directories
	if(counter == 0)
	{
		printf("%s: Command not found.\n", token[0]);
	}
	
	//exits the child process and resumes the parent process
	exit(0);
    }

    else
    {
	// makes parent process wait until the child process is finished executing
    	int status;
	wait(&status);
    } 
 }
 
 return 0;

}
