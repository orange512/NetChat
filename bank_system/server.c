/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月12日 星期二 20时38分08秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<fcntl.h>
#include<string.h>
#include<signal.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<fcntl.h>
#define ERR_EXIT(m)\
        do{\
            perror(m),exit(-1);\
        }while(0)
struct account
{
    int id;
    char name[20];
    double money;
};
struct msg
{
    long mtype;
    struct account acc;
};
void add_aounnt(int sig,siginfo_t  *info,void *p )
{
    printf("接受到信号%d\n",sig);
    key_t key1 = ftok(".",100);
    key_t key2 = ftok(".",200);

    if(key1 == -1&&key2 == -1)
        ERR_EXIT("ftok");
    int msgid1 = msgget(key1,0);
    if(msgid1 == -1)
        ERR_EXIT("msgid1");
    int msgid2 = msgget(key2,0);
    if(msgid2 == -1)
        ERR_EXIT("msgid2");

    struct msg msg1 = {};
    printf("before msgrcv\n");
    int ret = msgrcv(msgid2,&msg1,sizeof(msg1)-8,1,0);
    printf("ret = %d\n",ret);
    if(ret == -1)
        ERR_EXIT("msgrcv");

    int fd1 = open("./account.txt",O_RDWR);
    char a[10];
    int i = 0;
    int rcv = read(fd1,a,sizeof(a));
    if(rcv == -1)
        ERR_EXIT("read");
    printf("读取信息成功\n");
    sscanf(a,"%d",&i);
    msg1.acc.id = i + 1;
    printf("id = %d\n",msg1.acc.id);
    i = i+1;
    lseek(fd1,SEEK_SET,0);

    int w = write(fd1,&i,4);
    if(w == -1)
        ERR_EXIT("write");
    char buf[20];
    sprintf(buf,"%d.dat",i);
    int fd2 = open(buf,O_CREAT|O_WRONLY|O_TRUNC,0666);
    write(fd2,&msg1,sizeof(msg1)-8);
    int s = msgsnd(msgid1,&msg1,sizeof(msg1)-8,0);
    if(s == -1)
        ERR_EXIT("msgsend");
    printf("写入队列成功\n");
    printf("id = %d\n",msg1.acc.id);
    close(fd1);
    close(fd2);
}
int main()
{
    printf("PID = %d",getpid());
    key_t key1 = ftok(".",100);
    key_t key2 = ftok(".",200);
    if(key1 == -1&&key2 == -1)
        ERR_EXIT("ftok");
    int msgid1 = msgget(key1,0666|IPC_CREAT);
    if(msgid1 == -1)
        ERR_EXIT("msgid1");
    int msgid2 = msgget(key2,0666|IPC_CREAT);
    if(msgid2 == -1)
        ERR_EXIT("msgid2");
    printf("创建成功\n");
    struct sigaction action = {};
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = add_aounnt;
    sigaction(30,&action,NULL);
    while(1);

}
