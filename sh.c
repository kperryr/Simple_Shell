#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "sh.h"



size_t len = 256;

int sh( int argc, char **argv, char **envp )
{
  char *input_cmd[12] =     { "exit",      //1
		      	    "which",     //2
		            "where", 	   //3
		            "cd",        //4
		            "pwd",	   //5
		            "list",      //6
		            "pid",       //7
		            "kill",      //8
		            "prompt",    //9
		            "printenv",  //10
		            "setenv",    //11
		            '\0'};

  prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandline = calloc(MAX_CANON, sizeof(char));
  char *cmmd = calloc(MAX_CANON, sizeof(char));
  char *command, *arg, *commandpath, *p, *pwd, *owd, *word;
  char **args = calloc(MAXARGS, sizeof(char*));
  int uid, g, status, argsct, index, go = 1, cmd_found = 0, i = 0;
  struct passwd *password_entry;
  char *homedir;
  struct pathelement *pathlist;
  

  uid = getuid();
  password_entry = getpwuid(uid);               /* get passwd info */
  homedir = password_entry->pw_dir;		/* Home directory to start
						  out with*/
     
  if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
  {
    perror("getcwd");
    exit(2);
  }
  owd = calloc(strlen(pwd) + 1, sizeof(char));
  memcpy(owd, pwd, strlen(pwd));
  

  /* Put PATH into a linked list */
  pathlist = get_path();
  
  
  while ( go )
  {
    
 /*  // freezes loop when ignored
    if (signal(SIGTSTP, sighand) == SIG_ERR) 
	    perror("SIGTSTP error");
  
    if(signal(SIGINT, sighand)== SIG_ERR) 
	    perror("SIGINT error");
    
    if(signal(SIGTERM, sighand) == SIG_ERR)
	    perror("SIGTERM error");
    
*/	//control D/EOF  STILL needs to be done
  
        if ((pwd = getcwd(NULL,256)) == NULL)
               perror("getcwd() error:");

  	cmd_found = 0; 
     /* print your prompt */
	printf("\n%s [%s]> ", prompt, pwd);

    /* get command line and process */

	if(getline(&commandline,&len, stdin) != -1){
	
		if(signal(SIGINT, SIG_DFL) == SIG_ERR)
		       	perror("SIGINT error");

		commandline[strlen(commandline)-1] = '\0';
		
		word = strtok(commandline, " \n");
		i = 0;
		while(word != NULL){
			
			args[i] = word;
			word = strtok(NULL, " \n");
				i++;
		}
		args[i] = '\0';
	
   
		/* check for each built in command and implement */
		for( int k = 0; k<11; k++){
			if(strcmp(input_cmd[k], args[0]) == 0){
				cmd_found = 1;
				index = k+1;
				continue;
				}
			}
		if( cmd_found == 1){
			printf("Executing built-in [%s]\n", args[0]);
			switch(index){
				case 1://free(cmmd);
					free(commandline);
					free(pathlist);
					free(args);
					free(owd);
					exit_shell();
					break;
				case 2: 
					cmmd = which(args, pathlist);
					if (cmmd != NULL)
						printf("%s\n",cmmd);
					break;
				case 3: where(args, pathlist);
					break;
				case 4: changedir(args);
					break;
				case 5: if(args[1] == NULL)
						pwdirect();
					else
						fprintf(stderr, "cwd: too many args\n");
					break;
				case 6: list(args);
					break;
				case 7: printpid();
					break;
				case 8: kill_pid(args);
					break;
				case 9: prompt_(args);
					break;
				case 10: printenv(args,envp);
					 break;
				case 11: setenv_(args,envp,pathlist);
					 break;
				default: printf("Mismatch error of what command was used\n");
					 break;
			}


     /*  else  program to exec */
    
       /* find it */
       /* do fork(), execve() and waitpid() */

       /* else
		if((pid = fork()) < 0){
			fprintf(stderr, "Error\n");
		}else if (pid = 0){
			printf("Executing  [%s]\n",args[0]);
			if(strchr(args[0],'/') != NULL0){
				if(access(args[0], X_OR) == 0){
					execve(args[0],args,NULL);
				}else{
				fprintf(stderr, "Failed to execute path\n");
				}
			}else{ 
				args[0] = which( args, pathlist);
				if( -1 == excve (args[0], (char**) args,NULL)
					perror ("child process");
				
			}
		}else{
			waitpid(pid,&status,0);
			if (WIFEXITED(status)){
				printf("Exit status:%d",status); "FIXXXX"
			}else{
				printf("Child: Abnormal Exit\n"); */
	
     	 }else{
     		   fprintf(stderr, "%s: Command not found.\n", args[0]); 
    	 }
     }	
  }
  return 0;
} /* sh() */

/*blocks signals to continue loop*/
void sighand( int sig){
	signal(sig,SIG_IGN);
	printf("\nBLOCKED\n");
}

/* exits out of shell. No parameters or returned values */
void exit_shell(){

	exit(0);
}


/* loop through pathlist until finding command and return it.  Return NULL 
   when not found. Parameters are the command to be found and the pathlist*/
char *which(char **args, struct pathelement *p )
{
	char *cmds;
	if(args[1] == NULL){
		fprintf(stderr, "Which: too few args\n");
		return NULL;
	}else if(args[2] == NULL){
		while(p){
			sprintf(cmds,"%s/%s", p->element,args[1]);
			if(access(cmds,X_OK) == 0){

				cmds[strlen(cmds)] = '\0';
				return cmds;
			}
			p = p->next;
		}

		fprintf(stderr, "Which: Command %s not found\n", args[1]);
		return NULL;
	}else{
		fprintf(stderr, "Which: too many args\n");
		return NULL;
	}
	
} /* which() */


/* loop through pathlist until finding command everywhere it is executable. Return
   NULL when not found. Parameters are the command to be found and the pathlist*/
char * where(char **args, struct pathelement *p )
{

	char *cmds;
	int check;
	char *f = "found";
	if(args[1] == NULL){
		fprintf(stderr, "Which: too few args\n");
		return NULL;
	}else if(args[2] == NULL){
		while(p){
			sprintf(cmds,"%s/%s", p->element,args[1]);
			if(access(cmds,F_OK) == 0){
				printf("%s\n", cmds);
				memset(cmds,0,strlen(cmds));
				check = 1;
			}
			p = p->next;
		}
		if (check == 1){
			return f;
		}
		fprintf(stderr, "Which: Command %s not found\n", args[1]);
		return NULL;
	}else{
		fprintf(stderr, "Which: too many args\n");
		return NULL;
	}
} /* where() */


/* Changes cwd depending on user input.  Returns nothing. Parameter holds the input
   to decide what the directory is being changed to*/
void changedir( char **args){
	char *tmp = getcwd(NULL,256);


	if(args[2] == NULL){

		if(args[1] == NULL){ //HOME
		        char *home;
			home = getenv("HOME");
			if(chdir(home) != 0)
				perror(" chdir() error");
		}else if(strcmp(args[1],"-") == 0){ //PREVIOUS
			if(chdir("..") != 0)
				perror("chdir() error:");
			char *new = getcwd(NULL,256);
			if(strcmp(tmp,new) == 0)
				fprintf(stderr, "cd: no previous directory found\n");
		}else if(strchr(args[1],'/') == 0){ //ENTRY
			if(chdir(args[1]) != 0)
				perror("chdir() error:");
		}else{
			fprintf(stderr, "cd: Invalid directroy entry\n");
		}
	}else if ( args[1] == NULL){
		fprintf(stderr, "cd: too few args\n");	
	}else{
		fprintf(stderr, "cd: too many args\n");
	}

}/*changedir()*/

/* Prints current working directory. Returns nothing and has no parameters */
void pwdirect(){
	char *pwdir;

    		if ((pwdir = getcwd(NULL,256)) == NULL){
      			perror("getcwd() error");
    		}else{
      			printf("CWD: %s\n", pwdir);
  		}
}/*pwdirect()*/

/* list all files in a given directory. Returns integer 1 if error,
   0 if success. Parameter is the directories inputed by the user */

int list ( char **args )
{
  
	int check;
	if( args[1] == NULL){ //cwd directory
		struct dirent *d1;
		DIR *dir = opendir(".");
		if(dir == NULL){
			perror("opendir() error");
			return 1;
		}
		while((d1 = readdir(dir)) != NULL){
			printf("%s\n", d1->d_name);
			check = 1;
		}

		if (check == 0)
			printf("No files where found in CWD\n");
		closedir(dir);
	}else{
		int k = 1; //starts at first directory
		while(args[k] != NULL){ //several directories
			check = 0;
			struct dirent *d2;
			DIR *dirr = opendir(args[k]);
			if (dirr == NULL){
				perror("opendir() error");
				return 1;
			}
			printf("%s: ", args[k]);
			while((d2 = readdir(dirr)) != NULL){
			//	if (!strcmp(d2->d_name,".") || !strcmp(d2->d_name,"..")){	
		
					printf("%s\n", d2->d_name);
					check = 1;
			//	}	
				
			}
			if (check == 0)
				printf("No files where found in %s\n", args[k]);
			closedir(dirr);
			k++;
		}
		
	}
	return 0;
							
} /* list() */


/* Prints parent and Child pid. Returns nothing and has no parameters */
void printpid(){

	printf("Child PID: %d\n", getpid());

	printf("Parent PID: %d\n", getppid());
}/* printpid() */


/* Kills given process given by shell user. Returns nothing. Parameters
   consist pid number to kill and an optional signal number*/
void kill_pid(char **args){
	
	char *str;
	int sig;
	int check;
	int invalid_entry;	
	
	if(args[2] == NULL && args[1] != NULL){ //just PID
		pid = atoi(args[1]); //convert to int
		if(0 == kill(pid,0)) 
			kill(pid,15); //SIGTERM
		else
			perror("Invalid PID");
	
	}else if(strchr(args[1],'-') != NULL){ //if sig num
		pid = atoi(args[2]); //convert PID to int
		char num[] = "1234567890";
		char letter[] = "abcdefghijklmnopqrstuvwxyz";
		for(int j = 0; j < 10; j++){ //check for numbers
			if( strchr(args[1],num[j]) !=NULL ){
				check = 1;
				continue;
			}
		}
		for(int k = 0; k<26 ; k++){ //check for invalid letters
			if(strchr(args[1],letter[k]) != NULL){
				invalid_entry = 1;
				continue;
			}
		}
		if(invalid_entry != 1 && check == 1 ){ //if only numbers
			sig = strtol(args[1],&str,10); 
			sig = sig * -1; //make postive due to dash

			if(0 == kill(pid,0))
				kill(pid,sig);
			else
				perror("Invalid PID");

		}else{
			fprintf(stderr,"kill: invalid signal number\n");
		}
	}else{
		fprintf(stderr, "kill: invalid command arguments\n");
	}
} /* kill()*/


/* changes prompt string given by shell user. Returns nothing. Parameters
   consist of either a prompt or NULL charater to be prompted*/
void prompt_(char **args){
	char p[256];
	if (args[1] != NULL && args[2] == NULL){ //if prompt given
						 // as command
		prompt = args[1];
	}else if(args[1] != NULL && args[2] != NULL){ 
	
		fprintf(stderr, "prompt: too many args\n");
	}else{
		printf("\nInput prompt prefix:");
		if(fgets(p,256,stdin) == NULL){
			fprintf(stderr, "unable to read user input\n");
		}else{
			strtok(p,"\n");
			prompt = p;
		}	
	}
	
}/* prompt() */

/* Prints enviromental varibles as inputed by the user. Returns nothing */
void printenv( char **args, char **envp){
	int k = 0;
	if(args[1] == NULL){ //prints all env
		while(envp[k] != NULL){
			printf("%s\n", envp[k]);
			k++;
		}
	}else if(args[2] == NULL){ //prints specific env
		if(getenv(args[1]) != NULL)
			printf("%s:%s\n", args[1], getenv(args[1]));
		else
			fprintf(stderr,"getenv: variable does not exist\n");
	}else{
		fprintf(stderr,"printenv: too many arguments\n");
	}
}/* printenv() */


/* Prints, sets or changes enviromental varibles as inputed by the user
   Returns nothing */
void setenv_(char **args, char **envp, struct pathelement *p){ //still need to add error checks
	int k = 0;
	if (args[1] == NULL){
		while(envp[k] != NULL){
			printf("%s\n",envp[k]);
			k++;
		}
	}else if (args[2] == NULL){ //empty string values
		if(getenv(args[1])){ //already exist
			if(strcmp(args[1],"PATH") == 0)
				fprintf(stderr,"PATH needs a value\n");// add more code for getpath()
			
			else if(strcmp(args[1],"HOME") == 0)
				fprintf(stderr,"HOME needs a value\n");
			
			else
				setenv(args[1],"",1);
		}else{
			setenv(args[1],"",0); //create new env
		}
	}else if(args[3] == NULL){ //contains a string value
		if( getenv(args[1])){  //already exist

			if(strcmp(args[1],"PATH") == 0){
				struct pathelement *prev;
				while(p != NULL){
					prev = p->next;
					free(p);
					p = prev;
				}//make sure this freed  the oringal linked list
				
				setenv(args[1],args[2],1); //call getpath ?
				
			}else if(strcmp(args[1],"HOME") == 0){
				setenv(args[1],args[2],1); //call getpath?- run tests
			}else{
				setenv(args[1],args[2],1); //create new env
			}
		}else{
			setenv(args[1],args[2],0);
		}
	}else{
		fprintf(stderr,"setenv: too many args\n");
	}
}/* setenv() */

