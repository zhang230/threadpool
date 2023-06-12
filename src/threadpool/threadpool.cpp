#include <threadpool.h>
#include <stdio.h>
#include <stdlib.h>


ThreadPool* threadPoolCreate(int threadNum, int queueSize)
{
    
    ThreadPool* pool = (ThreadPool *)malloc(sizeof(ThreadPool));
   
    if (pool == NULL)
    {
        printf("线程池对象创建失败");
        return NULL;
    }

    pool->threadID = (pthread_t *)malloc(threadNum * sizeof(pthread_t));
    pool->threadNum = threadNum;
    if (pool->threadID == NULL)
    {
        printf("线程池对象池子创建失败");
        return NULL;
    }
    if (pthread_mutex_init(&pool->mutexPool, NULL) != 0 ||
        pthread_cond_init(&pool->cond, NULL) != 0 ||
        pthread_cond_init(&pool->taskCond, NULL) != 0)
    {
        printf("线程初始化失败");
        return NULL;
    }
 

    pool->taskQ = (Task *)malloc(sizeof(Task) * queueSize);
 
    pool->queueCapacity = queueSize;
    pool->queueSize = 0;
    pool->queueFront = 0;
    pool->queueRear = 0;
    pool->shutdown = 0;//打开线程
    pool->alreadyHandNum = 0;
    for (int i = 0; i < threadNum; i++)
    {
        pthread_create(&pool->threadID[i], NULL, worker, pool);
    }
    return pool;
}

int threadPoolDestroy(ThreadPool* pool)
{
    
    pool->shutdown = 1; //关闭线程
    for (int i = 0; i < pool->threadNum; i++)
    {
        pthread_cancel(pool->threadID[i]);
    }
    return 0;
}

void threadPoolAdd(ThreadPool* pool, void(*func)(void *), void* args)
{
    
    pthread_mutex_lock(&pool->mutexPool);
    while (pool->queueSize == pool->queueCapacity)
    {
        pthread_cond_wait(&pool->taskCond, &pool->mutexPool);
    }

    pool->taskQ[pool->queueRear].func = func;
    pool->taskQ[pool->queueRear].args = args;
    pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;
    pool->queueSize++;

    pthread_cond_signal(&pool->cond);//唤醒worker线程

    pthread_mutex_unlock(&pool->mutexPool);
}

void* worker(void* args)
{
    ThreadPool* pool = (ThreadPool*)args;
    while (1)
    {
        
        pthread_mutex_lock(&pool->mutexPool);
      
        while (pool->queueSize == 0)
        {
            pthread_cond_wait(&pool->cond, &pool->mutexPool);//阻塞当前线程，等待pthread_cond_signal设置pool->cond条件唤醒当前线程执行, 并且释放pool->mutextPool锁, 所以在任务添加函数中，可以获得锁
        }

        Task task;
        task.func = pool->taskQ[pool->queueFront].func;
        task.args = pool->taskQ[pool->queueFront].args;
        
        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
        pool->queueSize--;

        task.func(task.args);

        pool->alreadyHandNum++;

        pthread_cond_signal(&pool->taskCond);
        pthread_mutex_unlock(&pool->mutexPool);
        
        if (pool->shutdown == 1)
        {
            pthread_exit(NULL);
        }
        
    }
    return NULL;
}
void threadExit(ThreadPool* pool)
{
    pthread_t tid = pthread_self();
    for (int i = 0; i < pool->threadNum; i++)
    {
        if (pool->threadID[i] == tid)
        {
            pool->threadID[i] = 0;
            break;
        }
    }
    pthread_exit(NULL);
}