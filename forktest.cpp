#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
int main(int argc,char *argv[])
{
	pid_t pids [1];
	int pos;
    pid_t p = fork();
	if (!p) {
		printf("Child pid %d\n", getpid());
		int row = 4;
		exit(row);
	}
	pids[0] = p;
	printf("This is parent\n");
	pid_t id = wait(&pos);
	printf("This is pos: %d\n", pos/255);
	printf("This is the pid: %d\n", id);
	printf("This is the pid in array: %d\n", pids[0]);
	return 0;
}
