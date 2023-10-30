#include "sh.h"
#include <signal.h>
#include <stdio.h>

void sig_handler(int signal); 


int main( int argc, char **argv, char **envp )
{
  /* put signal set up stuff here */
 // prompt = calloc(PROMPTMAX, sizeof(char));
  return sh(argc, argv, envp);
  
}

void sig_hand(int signal)
{
  /* define your signal handler */
	printf("BLOCKED\n");
			
}

