#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

void* busy_thread(void* arg) {
    int id = *(int*)arg;
    printf("Thread %d: Starting, will run for 0.5 seconds\n", id);
    
    // 忙等待 0.5 秒，模拟长时间运行
    clock_t start = clock();
    while((clock() - start) * 1000 / CLOCKS_PER_SEC < 500) {
        // 忙循环
    }
    
    printf("Thread %d: Finished busy work\n", id);
    return NULL;
}

void* blocking_thread(void* arg) {
    int id = *(int*)arg;
    printf("Thread %d: Starting, will block after 0.1 seconds\n", id);
    
    // 先运行一小会儿
    clock_t start = clock();
    while((clock() - start) * 1000 / CLOCKS_PER_SEC < 100) {
        // 短时间忙循环
    }
    
    printf("Thread %d: Now blocking for 1 second...\n", id);
    sleep(1);  // 这里会立即触发切换
    printf("Thread %d: Woke up\n", id);
    
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;
    
    printf("Main: Creating threads (thread 1 = busy, thread 2 = blocking)\n");
    
    pthread_create(&t1, NULL, busy_thread, &id1);
    pthread_create(&t2, NULL, blocking_thread, &id2);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Main: All threads completed\n");
    return 0;
}