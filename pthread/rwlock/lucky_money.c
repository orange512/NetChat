/*************************************************************************
	> File Name: mutex.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月14日 星期四 22时33分04秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<signal.h>
#include<string.h>
#include<time.h>
#include"link_list.h"
int remainMoney = -1;//剩余红包钱
int left_MoneyPacksize;//剩余红包个数
Node *pnode;//创建链表指针
pthread_cond_t cond;
pthread_rwlock_t rwlock;
int getMoney;//获取的钱
double Lucky_money = 0.0;
int RAND(int n,int m)//生成n到m之内的随机数
{
    return rand()%(m-n+1)+n; 
}

int getRandomMoney()//获取随机金额红包
{
    if(remainMoney/left_MoneyPacksize == 1)
    {
        return remainMoney/left_MoneyPacksize;
    }
    int money;
    if(left_MoneyPacksize == 1)
    {
        money =   remainMoney;
        remainMoney -= money;
        return money;
    }
    money = remainMoney*RAND(1,199)/100/left_MoneyPacksize;
    remainMoney -= money;
    left_MoneyPacksize--;
    return money;
}
void destroy(pthread_t tid)//销毁抢完红包的线程
{
    pthread_join(tid,0);
   // printf("%ld exit\n",tid);
}
void* task(void *p)//抢红包
{
    pthread_rwlock_wrlock(&rwlock);
    if(remainMoney == 0)
    {
        pthread_rwlock_unlock(&rwlock);
        destroy(pthread_self());
    }
    else
    {
        getMoney = getRandomMoney();//获取金额
    //    printf("取走%d.%d%d元\n",getMoney/100,(getMoney/10)%10,getMoney%10);
        tail_Insert(pnode,getRandomString(5),getMoney);//写入链表    
        pthread_rwlock_unlock(&rwlock);
        destroy(pthread_self());//先抢完的人先结束
    }
}
void* creat_money(void *p)//生产红包
{
    remainMoney =(int)(*(double*)p * 100);
    pthread_rwlock_wrlock(&rwlock);
    remainMoney = remainMoney;
    printf("红包已放入\n");
    pthread_rwlock_wrlock(&rwlock);
    return NULL;

}

int main()
{
    pnode = init();
    int k;
    double user_money;
    int err;
    pthread_rwlock_init(&rwlock,0);
    pthread_t tid;
    srand((unsigned)time(0));

    while(1)
    {
        printf("请输入红包金额和红包数量(注：红包数量最多3000个)\n");
        scanf("%lf %d",&user_money,&left_MoneyPacksize);
        if(user_money*100/left_MoneyPacksize >= 1)
        {
            k = left_MoneyPacksize;
            break;
        }
        else
        printf("对不起，您的红包金额过少，请重新输入\n");
    }
    pthread_create(&tid,0,creat_money,(void *)(&user_money));
    pthread_join(tid,0);

    sleep(1);
    pthread_t *p_id =(pthread_t *) malloc(sizeof(pthread_t) * k * 2);
    memset(p_id,0,sizeof(p_id));
    for(int i = 0 ; i  < k*2;i++)//创建抢红包人数
    {
        if(err = pthread_create(&p_id[i],0,task,0)!= 0)
            fprintf(stderr,"%s\n",strerror(err));
    }


    printf("\n1秒后开抢.....\n");
    sleep(1);
    print(pnode);//打印抢到钱的人
    Max(pnode);//显示最多抢的人

    free(pnode);
    free(p_id);
    pthread_cond_destroy(&cond);
    return 0;
}
