#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define N 12

extern char **environ;

char *allowed[N] = {"cp","touch","mkdir","ls","pwd","cat","grep","chmod","diff","cd","exit","help"};

int isAllowed(const char*cmd) {
	for (int i = 0; i < 12; i++){
	    if (strcmp(cmd, allowed[i]) == 0) return 1;
	}

	return 0;
}

int main() {

    char line[256];

    posix_spawnattr_t attr;
    pid_t pid;
    int status;

    while (1) {

	fprintf(stderr,"rsh>");

	if (fgets(line,256,stdin)==NULL) continue;
	if (strcmp(line,"\n")==0) continue;
	line[strlen(line)-1]='\0';

	char* lineCpy = (char *)malloc(sizeof(char));
	char* command = (char *)malloc(sizeof(char));
	char* token = (char *)malloc(sizeof(char));

	strcpy(lineCpy, line);

	command = strtok(lineCpy, " ");

	int valid = isAllowed(command);

	if(valid == 0){
	    printf("NOT ALLOWED!\n");
	    continue;
	}

	char** argv = (char **)malloc(21*sizeof(char*));
	int numArgs = 0;
	int i = 1;

	argv[0] = command;
	token = strtok(NULL, " ");
	while(token){
	    argv[i] = token;
	    i++;
	    numArgs++;
	    token = strtok(NULL, " ");
	}
	argv[i] = NULL;

	if (strcmp(command, "cd") == 0){
	    if (numArgs > 1){
		printf("-rsh: cd: too many arguments");
	    }
	    chdir(argv[1]);
	} else if (strcmp(command, "exit") == 0) {
	    break;
	} else if (strcmp(command, "help") == 0) {
	    printf("The allowed commands are: \n");
	    for (int i = 0; i < 12; i++){
		printf("%d. ", i+1);
		printf("%s\n", allowed[i]);
	    }
	}else {
	    posix_spawnattr_init(&attr);
	    if(posix_spawnp(&pid, command, NULL, &attr, argv, environ) != 0){
		perror("spawn failed");
		exit(EXIT_FAILURE);
	    }
	    if (waitpid(pid, &status, 0) == -1){
		perror("waitpid failed");
		exit(EXIT_FAILURE);
	    }
	    posix_spawnattr_destroy(&attr);
	}
    }
    return 0;
}
