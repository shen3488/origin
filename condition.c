#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>//unix std
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>//pthread.h

//生产者线程和消费者线程共享的数据---链表
typedef struct node
{
	int data;
	struct node* next;
}Node;

//定义一根指针指向链表的头部
Node* head=NULL;

//为线程同步准备一把mutex
pthread_mutex_t mutex;

//为阻塞线程准备一个条件变量
pthread_cond_t cond;

void* producer(void* arg)
{
	while(1)
	{
		//链表数据存放在线程共享的堆中
		Node* pnew=(Node*)malloc(sizeof(Node));
		//节点的初始化
		pnew->data=rand()%1000;//0-999

		//----->开始操作共享数据
		pthread_mutex_lock(&mutex);
		pnew->next=head;//头插法，head指针前移动
		head=pnew;
		printf("===produce:%ld--->%d",pthread_self(),pnew->data);
		pthread_mutex_unlock(&mutex);
		//----->结束操作共享数据
		
		//通知条件变量解除阻塞
		pthread_cond_broadcast(&cond);

		sleep(rand()%3);//3s内随机休眠
	}
	return NULL;
}

void* consumer(void* arg)
{
	while(1)
	{
		//开始操作共享数据
		pthread_mutex_lock(&mutex);
		if(head==NULL)
		{
			//continue;忙轮询太消耗系统资源
			pthread_cond_wait(&cond,&mutex);//第二个参数是一个mutex*
			//解除阻塞之后对参数mutex*对应的锁重新加锁
			
		}
		Node* pdel=head;
		head=head->next;
		printf("|||consume:%ld--->%d",pthread_self(),pdel->data);
		//结束操作共享数据
		free(pdel);
		pthread_mutex_unlock(&mutex);	
	}
	return NULL;
}


int main(int argc,char* argv[])
{
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond,NULL);

	pthread_t p1,p2;//存放进程ID
	//创建生产者进程
	pthread_create(&p1,NULL,producer,NULL);
	//创建消费者进程
	pthread_create(&p2,NULL,consumer,NULL);
	
	//阻塞回收子线程 
	pthread_join(p1,NULL);
	pthread_join(p2,NULL);
	
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	return 0;
}
