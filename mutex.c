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

#define MAX 10000

int number;//全局变量,两个线程都来访问这个全局变量
//在全局变量区，创建两个mutex
pthread_mutex_t mutex;

void* myfunc1(void* arg)
{
	for(int i=0;i<MAX;i++)
	{
		//访问全局变量之前就lock
		pthread_mutex_lock(&mutex);//lock
		int cur=number;
		cur++;
		number=cur;
		printf("thread No.1, thread_id=%ld, num=%d\n",pthread_self(),number);
		pthread_mutex_unlock(&mutex);//unlock
		usleep(10);//sleep 10us,模拟交替数数的现象
	}
	return NULL;
}

void* myfunc2(void* arg)
{
	for(int i=0;i<MAX;i++)
	{
		pthread_mutex_lock(&mutex);//lock
		int cur=number;
		cur++;
		number=cur;
		printf("thread No.2, thread_id=%ld, num=%d\n",pthread_self(),number);
		pthread_mutex_unlock(&mutex);//unlock
		usleep(10);//sleep 10us
	}
	return NULL;
}


int main(int argc,char* argv[])
{
	pthread_t p1,p2;//两个子线程的线程id变量
	//创建两个子线程
	pthread_mutex_init(&mutex,NULL);//不设置mutex属性，NULL
		

	pthread_create(&p1,NULL,myfunc1,NULL);
	pthread_create(&p2,NULL,myfunc2,NULL);
	//阻塞，等待回收子线程资源
	pthread_join(p1,NULL);//一次回收一个子线程
	pthread_join(p1,NULL);
	
	pthread_mutex_destroy(&mutex);//释放mutex占用的资源
	return 0;
}
