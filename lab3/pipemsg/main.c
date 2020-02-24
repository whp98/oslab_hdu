//进程管道通信实验
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
//定义每次最大写入量
#define BUF_MAX_SIZE 8192
//检查是否存在
#define CHECK(x)                                            \
    do {                                                    \
        if (!(x)) {                                         \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x);                                     \
            exit(-1);                                       \
        }                                                   \
    } while (0)

//主要函数
int main(int argc, char **argv) {
    int pipefd[2], pid, i = 0;
    int flag = 0;
    ssize_t n;
    char buf[BUF_MAX_SIZE];
    char str[BUF_MAX_SIZE];
    //定义三个有名信号量
    sem_t *write_mutex;
    sem_t *read_mutex1;
    sem_t *read_mutex2;
    //创建信号量
    //可读可写
    write_mutex = sem_open("pipe_test_wm", O_CREAT | O_RDWR, 0666, 0);
    read_mutex1 = sem_open("pipe_test_rm_1", O_CREAT | O_RDWR, 0666, 0);
    read_mutex2 = sem_open("pipe_test_rm_2", O_CREAT | O_RDWR, 0666, 0);
    //全0写入
    memset(buf, 0, BUF_MAX_SIZE);
    memset(str, 0, BUF_MAX_SIZE);
    //创建管道并检查管道创建情况
    CHECK(pipe(pipefd) >= 0);
    //检查进程创建
    CHECK((pid = fork()) >= 0);
    //创建第一个儿子进程
    if (pid == 0) {
        int count = 0;
        //关闭写入描述符
        close(pipefd[0]);
        int flags = fcntl(pipefd[1], F_GETFL);
        //管道默认是阻塞写，通过`fcntl`设置成非阻塞写，在管道满无法继续写入时返回-EAGAIN，作为循环终止条件
        fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK);
        //写入
        while (!flag) {
            n = write(pipefd[1], buf, BUF_MAX_SIZE);
            //如果返回-1那么就写满了
            if (n == -1) {
                flag = 1;
            } else {
                count++;
                printf("子进程写入 %dB\n", n);
            }
        }
        printf("管道大小是： %dKB\n", (count * BUF_MAX_SIZE) / 1024);
        exit(0);
    }

    CHECK((pid = fork()) >= 0);
    if (pid == 0) {
        sem_wait(write_mutex);
        close(pipefd[0]);
        n = write(pipefd[1], "这是子进程2\n", 100);
        printf("子进程二写入 %dB\n", n);
        sem_post(write_mutex);
        sem_post(read_mutex1);
        exit(0);
    }

    CHECK((pid = fork()) >= 0);
    if (pid == 0) {
        sem_wait(write_mutex);
        close(pipefd[0]);
        n = write(pipefd[1], "这是子进程3\n", 50);
        printf("子进程三写入 %dB\n", n);
        sem_post(write_mutex);
        sem_post(read_mutex2);
        exit(0);
    }
    //父进程的操作
    wait(0);
    close(pipefd[1]);
    int flags = fcntl(pipefd[0], F_GETFL);
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
    while (!flag) {
        n = read(pipefd[0], str, BUF_MAX_SIZE);
        if (n == -1) {
            flag = 1;
        } else {
            printf("%dB 父进程读出\n", n);
        }
    }

    sem_post(write_mutex);
    sem_wait(read_mutex1);
    sem_wait(read_mutex2);
    n = read(pipefd[0], str, BUF_MAX_SIZE);
    printf("%dB 读出\n", n);
    for (i = 0; i < n; i++) {
        printf("%c", str[i]);
    }

    sem_close(write_mutex);
    sem_close(read_mutex1);
    sem_close(read_mutex2);
    sem_unlink("pipe_test_wm");
    sem_unlink("pipe_test_rm_1");
    sem_unlink("pipe_test_rm_2");
    return 0;
}
