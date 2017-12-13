/*************************************************************************
	> File Name: epoll_server.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月26日 星期二 21时58分07秒
 ************************************************************************/
#include<stdio.h>
#include<string.h>
#include<sys/types.h>  /* See NOTES */
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>

int main()
{
    int listenfd,conn,sockfd;
    int efd;//句柄
    struct epoll_event tmp,events[1024];
    int nready;
    if((listenfd = socket(PF_INET,SOCK_STREAM,0)) < 0)
        perror("socket");

    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(51880);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //inet_aton("127.0.0.1",&serveraddr.sin_addr);

    int on = 1;

    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))< 0)//地址重复利用
       perror("setsockopt");
    if(bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) < 0)
       perror("bind");
    if(listen(listenfd,SOMAXCONN) < 0)
       perror("listen");

    struct sockaddr_in peeraddr;
    socklen_t peer_len;
    char buff[1024];
    efd = epoll_create(1024);//创建epoll句柄
    tmp.events = EPOLLIN;//处理输入信息
    tmp.data.fd = listenfd;//监听事件
    epoll_ctl(efd,EPOLL_CTL_ADD,listenfd,&tmp);//添加事件
    for(;;)
    {
         nready = epoll_wait(efd,events,1024,-1);
        for(int i = 0;i< nready;i++)
        {
            if(events[i].data.fd == listenfd)
            {
                peer_len = sizeof(peeraddr);
                conn = accept(listenfd,(struct sockaddr *)&peeraddr,&peer_len);
                printf("IP = %s PORT = %d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
                if(conn < 0)
                {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                tmp.events = EPOLLIN;
                tmp.data.fd = conn;
                epoll_ctl(efd,EPOLL_CTL_ADD,conn,&tmp);
            }
            else
            {
                if(events[i].events & EPOLLIN)
                {
                    if((sockfd = events[i].data.fd) < 0)
                        continue;
                    int ret = recv(sockfd,buff,sizeof(buff),0);
                    if(ret == -1)
                    {
                        perror("read");
                    }
                    if(ret == 0)
                    {
                        printf("client close\n");
                        events[i].data.fd = -1;
                        close(sockfd);
                    }
                    printf("recv cli data:%s\n",buff);
                    send(sockfd,buff,ret-1,0);
                }
            }
           
        }
    }
    
}

