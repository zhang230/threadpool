#pragma once
#include <pthread.h>


typedef struct Task
{
    void (*func)(void* args);
    void* args;
}Task;


typedef struct ThreadPool 
{
    
    Task* taskQ;
    int queueCapacity;  //容量
    int queueSize;      //当前任务的个数
    int queueFront;
    int queueRear;
    int alreadyHandNum; //已经处理了的任务数量
    
    pthread_t* threadID;    //工作线程
    int threadNum;  //线程数量
    int shutdown;   //是否关闭线程
    
    pthread_mutex_t  mutexPool;    //锁住整个线程池
    pthread_cond_t cond; //唤醒条件
    pthread_cond_t taskCond; //唤醒条件
    
}ThreadPool;

ThreadPool* threadPoolCreate(int threadNum, int queueSize);

int threadPoolDestroy(ThreadPool* pool);
void threadExit(ThreadPool* pool);

void threadPoolAdd(ThreadPool* pool, void(*func)(void *), void* args);

void* worker(void* args);

