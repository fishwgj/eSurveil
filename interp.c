//
//  interp.c
//  sysmon
//
//  Created by wgj on 13-5-24.
//  Copyright (c) 2013年 wgj. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t child;
    int fdpipe;
    int fds[2];
    printf("%d, %d, %d\n",STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO);
    /*
    close(STDIN_FILENO);
    if (dup2(STDERR_FILENO,STDIN_FILENO)<0)
    {
        perror("dup2");
        exit(0);
    }*/
    fdpipe = pipe(fds);
    if (fdpipe < 0)
    {
        perror("pipe");
        exit(0);
    }
    
    child = fork();
    if (child <0)
    {
        perror("fork");
        exit(0);
    }
    
    if (child == 0)
    {
        printf("in child\n");
        if (dup2(fds[1], STDERR_FILENO)<0)
        {
            perror("dup2\nchild exit\n");
            exit(0);
        }
        char *exe_argv[] = { "dumpcap", "-Z", "0", NULL };
        execvp("dumpcap", exe_argv);
    }
    else
    {
        int status;
        char inbuf[512];
        pid_t pid;
        printf("in parent\n");
        if (dup2(fds[0], STDIN_FILENO)<0)
        {
            perror("dup2\nparent exit\n");
            exit(0);
        }
        while (0==(pid=waitpid(child, &status, WNOHANG)))
        {
            int len = 0, i;
            printf("child still running\n");
            len = read(0, inbuf, 512);
            inbuf[len] = 0;
            i = inbuf[1]<<16 | inbuf[2]<<8 | inbuf[3];
            printf("read %d bytes [%c][%d][%s] from child\n", len, inbuf[0], i, inbuf+4);
        }
        if (pid<0)
        {
            perror("waitpid");
            exit(0);
        }
        printf("child %d exit\n", pid);
    }
    return 0;
}