//
// Created by steins_xin on 23-10-23.
//
#include <pthread.h>
#include <thread>
#include <cstdio>
#include <unistd.h>
#include <mutex>


using namespace std;

// 定义一个共享计数器
int counter = 0;
bool start = false;

pthread_t thread_1;             // 线程1
pthread_t thread_2;             // 线程2

// 定义互斥锁
pthread_mutex_t Mutex;
pthread_cond_t Cond;

void * Thread_1(void *Void){
    while (1){
        pthread_mutex_lock(&Mutex);
        // 访问共享资源（计数器）
        counter++;
        printf("Thread_1\n");
        start = true;
        pthread_cond_signal(&Cond);
        // 解锁
        pthread_mutex_unlock(&Mutex);
    }

}

void * Thread_2(void *Void){
    while (1){
        // 加锁
        pthread_mutex_lock(&Mutex);

        while (!start) {
            pthread_cond_wait(&Cond, &Mutex);
        }
        start = false;

        // 访问共享资源（计数器）
        counter++;
        sleep(1);
        printf("Thread_2\n");
        // 解锁
        pthread_mutex_unlock(&Mutex);
    }

}

int main()
{
    pthread_create(&thread_1, NULL, Thread_1, NULL);
    pthread_create(&thread_2, NULL, Thread_2, NULL);

    // 等待线程1和线程2完成
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);

    // 销毁互斥锁
    pthread_mutex_destroy(&Mutex);
    return 0;
}