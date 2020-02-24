#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

char *sys_cmd [4] = {"exit", "cmd1", "cmd2", "cmd3"};
//获取命令代号
int get_cmd_id(char *cmd)
{
    int i;
    for (i = 0; i < 4; i++) {
        if (strcmp(cmd, sys_cmd[i]) == 0) {
            return i;
        }
    }
	//没有命令则返回-1
    return -1;
}
//创建子进程
void shell_fork(int cmd_id)
{
    //获取pid
    pid_t pid = fork();
    //创建进程失败
    if (pid < 0) {
        printf("fork error\n");
        exit(0);
        //创建子进程成功
    } else if (pid == 0) {
        //在子进程中执行相应的命令
        switch (cmd_id) {
        case 1:
            execl("./cmd1", "", NULL);
            break;
        case 2:
            execl("./cmd2", "", NULL);
            break;
        case 3:
            execl("./cmd3", "", NULL);
            break;
        }
        //执行出错
        printf("execl error\n");
        exit(0);
    } else {
        return;
    }
}

int main(void)
{
    char cmd[50];
    int cmd_id = -1;
    while (1) {
        printf("shell >> ");
        scanf("%s", cmd);
        cmd_id = get_cmd_id(cmd);
        if (cmd_id == -1) {
            printf("command not found\n");
        } else if (cmd_id == 0) {
            exit(0);
        } else {
            shell_fork(cmd_id);
        }
        wait(NULL);
    }
}
/*
相关函数
fork()创建子进程
int execl(const char * path, const char * arg, ...);
程序替换函数
execl()用来执行参数path 字符串所代表的文件路径, 接下来的参数代表执行该文件时传递过去的argv(0), argv[1], …, 最后一个参数必须用空指针(NULL)作结束. 

*/
