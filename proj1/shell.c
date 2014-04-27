#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

//This function allows for character replacement within a string
//Used to add spaces between redirection symbols
void replace(char * orig, char * search,  char * repl) {
  char buffer[1024];
  char * ch;
  
  //If the string does not contain the substring, return
  if(!(ch = strstr(orig, search)))
    return;
  
  //Replace the substring with a new substring
  strncpy(buffer, orig, ch-orig);
  buffer[ch-orig] = 0;
  sprintf(buffer+(ch - orig), "%s%s", repl, ch + strlen(search));
  orig[0] = 0;
  strcpy(orig, buffer);

  //Do this function recurrsively until all substrings are replaced
  return replace(orig, search, repl);
}

//This is where all the parsing of the user input happens
int getCommand(char *** retrn, int retrnVal, int *redirt, char **fileOut, char **fileIn, char **fileErr, int *wait) {

  int argc, freebls;
  char *buffer, **argv, pipe = 0;

  *wait = 0;
  argc = 0;
  freebls = 5;
  argv = (char **)malloc(5*sizeof(char *));
  if(*redirt != 16) printf("sish:>");
  scanf("%ms", &buffer);

  //This checks for a EOF or CNTR-D input from the user
  if(buffer == NULL)
    return -1;

  argv[argc] = buffer;
  argc++;
  freebls--;
  
  while(getchar() != '\n' && pipe == 0) {

    if(freebls == 0) {
      argv = (char **) realloc(argv, 5*sizeof(char *));
      freebls = 5;
    }

    scanf("%ms", &buffer);
    
    //If redirection TO a file is indicated
    if(strcmp(buffer, ">") == 0) {
      if(getchar() != '\n') {
	*redirt = 1;
	//Scan in and store the file name
	scanf("%ms", fileOut);
      }
    }
    //If redirection FROM a file is indicated
    else if(strcmp(buffer, "<") == 0) {
      if(getchar() != '\n') {
	*redirt = 4;
	//Scan in and store the file name
	scanf("%ms", fileIn);
      }
    }
    //If background process is indicated
    else if(strcmp(buffer, "&") == 0) {
      *wait = 1;
    }
    //If piping is indicated
    else if(strcmp(buffer, "|") == 0) {
      pipe = 1;
      *redirt = 16;
    }
    //Otherwise, treat it as a command or argument
    else {
      argv[argc] = buffer;
      argc++;
      freebls--;
    }
  }
  
  if(freebls == 0) 
    argv = (char **) realloc(argv, sizeof(char));
  
  //Set the last variable in the array to NULL for searching purposes
  argv[argc] = NULL;
  *retrn = argv;
  return argc;
}

//This frees up variables
void freeA(int argc, char **argv) {
  for(;argc >= 0; argc--)
    free(argv[argc]);
  free(argv);
}

//This functions checks for the status of the child function
void child(pid_t new) {
  static int *array = NULL;
  static int nbpro = 0;
  static int freesp = 5;
  int i, sonStatus, pid;
  if(array == NULL) array = (int *)malloc(5*sizeof(int));

  if(new == 1) {
    free(array);
    return;
  }

  if(new != 0) {
    if(freesp == 0) {
      array = (int *)realloc(array, 5*sizeof(int));
      freesp = 5;
    }

    array[nbpro] = new;
    nbpro++;
    freesp--;
  }

  for(i = 0; i < nbpro; i++) {
    if((pid = waitpid(array[i], &sonStatus, WNOHANG)) == array[i]) {
      array[i] = array[nbpro-1];
      array[nbpro-1] = 0;
      nbpro--;
      freesp++;
    }
    else if(pid == -1)
      printf("ERROR: waitpid failed.");
  }
}

int main(void){

  char **argv, *fileIn = NULL, *fileOut = NULL, *fileErr = NULL;
  int argc, sonStatus, retrnVal = 0, redirt = 0, out = 1, in = 0,  wait = 0, tube[2];
  pid_t pid;

  while(1){
    child(0);
    argc = getCommand(&argv, retrnVal, &redirt, &fileOut, &fileIn, &fileErr, &wait);

    //If EOF or CTRL-D has been entered, the program exits
    if(argc == -1) {
      return 0;
    }

    //If the user enters "exit", the program exits
    if(strcmp(argv[0], "exit") == 0) {
      freeA(argc, argv);
      child(1);
      return 0;
    }
    else {
      pid = fork();
      switch(pid) {
	//Make sure the fork was successful
        case -1:
	  printf("ERROR: Unable to fork.");
	  freeA(argc, argv);
	  return 1;
	  break;
        case 0:
	  //Check to see if any sort of redirection occurs
	  if(redirt > 0) {
	    //If there is redirection TO a file
	    if(fileOut != NULL) {
	      //Open the file
	      out = open(fileOut, O_RDWR | O_CREAT, 0644);
	      //Check that the file was successfully opened
	      if(out == -1) {
		printf("ERROR: Unable to open file ");
		exit(2);
	      }
	      //Redirect, and check if redirection is successful
	      if(dup2(out, STDOUT_FILENO) == -1) {
		printf("ERROR: Unable to redirect ");
		exit(3);
	      }
	      close(out);
	    }
	    //If there is a redirect FROM a file
	    if(fileIn != NULL) {
	      //Open the file
	      in = open(fileIn, O_RDONLY);
	      //Check that the file was successfully opened
	      if(in == -1) {
		printf("ERROR: Unable to open file ");
		exit(2);
	      }
	      //Redirect, and check if redirection is successful
	      if(dup2(in, STDIN_FILENO) == -1) {
		printf("ERROR: Unable to redirect ");
		exit(2);
	      }
	      close(in);
	      //break;
	    }
	    //If piping
	    if(redirt == 16) {
	      //Check to make sure making the pipe was successful
	      if(pipe(tube) == -1) {
		printf("ERROR: Unable to create pipe ");
		exit(4);
	      }
	      //Fork
	      switch(fork()) {
		//Check to make sure fork was successful
	        case -1:
		  printf("ERROR: Unable to fork !");
		  return 1;
		  break;
		//If fork is successful
	        case 0:
		  close(tube[0]);
		  //Redirect, and make sure redirection is successful
		  if(dup2(tube[1], STDOUT_FILENO) == -1) {
		    printf("ERROR: Unable to redirect ");
		    exit(3);
		  }
		  close(tube[1]);

		  //Execute the command
		  execvp(argv[0], argv);
		  //If the above function returns, an error occured
		  printf("ERROR: Command not found : %s\n", argv[0]);
		  freeA(argc, argv);
		  return -1;
		  break;
	        default:
		  argc = getCommand(&argv, retrnVal, &redirt, &fileOut, &fileIn, &fileErr, &wait);
		  close(tube[1]);
		  //Redirect, and make sure redirection is successful
		  if(dup2(tube[0], STDIN_FILENO) == -1) {
		    printf("ERROR: Unable to redirect ");
		    exit(3);
		  }
		  close(tube[0]);
		  
		  //Execute the command
		  execvp(argv[0], argv);
		  //If the above function returns, an error occured
		  printf("ERROR: Command not found : %s\n", argv[0]);
		  freeA(argc, argv);
		  break;
	      }
	    }
	  }
	  //Execute the command
	  execvp(argv[0], argv);
	  //If the above function returns, an error occured
	  printf("ERROR: Command not found : %s\n", argv[0]);
	  freeA(argc, argv);
	  return -1;

        default:
	  if(redirt == 16)
	    argc = getCommand(&argv, retrnVal, &redirt, &fileOut, &fileIn, &fileErr, &wait);

	  //Wait for child process to finish
	  redirt = 0;
	  if(wait == 0) {
	    waitpid(pid, &sonStatus, 0);
	    retrnVal = WEXITSTATUS(sonStatus);
	  }
	  else
	    child(pid);
	  break;
      }
    }

    //Free up all the variables
    freeA(argc, argv);
    free(fileIn);
    free(fileOut);
    free(fileErr);
    fileIn = NULL;
    fileOut = NULL;
    fileErr = NULL;

  }
}
