#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int main(){

  while(1){
    
    printf("sish:>");
    char str[1024];
    char* cmd = NULL;
    char* args[512];
    int arg_cnt = 0;

    //gets the command line input from the user
    //if it finds an EOF or CTRL-D, the program ends
    if (gets(str) == NULL)
      break;

    //if the user types in "exit", the program ends 
    if(!strcmp(str, "exit"))
      break;

    //this breaks up the command line input into tokens
    //each word being one token
    char* tkn = strtok(str, " ");

    //this seperates the command to be executed (which is the first token)
    //and the arguments (which are the rest of the tokens, if they exist)
    while(tkn) {
      if(!cmd) cmd = tkn;
      args[arg_cnt] = tkn;
      arg_cnt++;

      tkn = strtok(NULL, " ");
    }

    //this sets the end of the argument list to NULL
    //so that we know it is the end of the list
    args[arg_cnt] = NULL;

    //this forks the process and runs the command inputted by the user
    //if the command does not exist, it prints out an error
    //but does not exit the program.
    pid_t pid = fork();
    if(!pid){
      execvp(cmd, args);
      exit(errno);
    }
    else{
      int status;
      waitpid(pid, &status, 0);
      printf("%d\n", status);
      if(status > 0)
	printf("ERROR: Command not found.\n");
    }
  }
  return 0;
}
