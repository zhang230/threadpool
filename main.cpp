#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <threadpool.h>

static int a = 0;
void func(void *args)
{
    int* flag = (int *)args;
    if (*flag == 0)
    {
        a++;
    }
    else
    {
        a--;
    }
    return ;
}

int main(int argc, char** args)
{

    int threadNum = 16; //线程数量
    int queueSize = 1e7;//任务队列大小
    ThreadPool* pool = threadPoolCreate(threadNum, queueSize);
    int add = 0;
    int sub = 1;
    int taskNum = 1e7;  //添加的任务数
    clock_t s = clock();
    for (int i = 0; i < taskNum; i++)
    {
        if (i < taskNum / 2)
        {
            threadPoolAdd(pool, func, (void *)&add);
        }
        else
        {
            threadPoolAdd(pool, func, (void *)&sub);
        }
    }
    clock_t e = clock();
    // sleep(30);
    while (pool->alreadyHandNum != taskNum);

    threadPoolDestroy(pool);
    printf("money a is %d\nhand time %lf\n", a, (double)(e - s) / CLOCKS_PER_SEC);
    return 0;
}