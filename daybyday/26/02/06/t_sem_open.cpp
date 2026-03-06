#include <fcntl.h>           /* For O_* constants */
#include <iostream>
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

// g++ t_sem_open.cpp -o t_sem_open -lpthread

int main() {
    // 命名信号量
    sem_t *sem = sem_open("/mysem", O_CREAT, 0644, 1);
    std::cout << "Now there is sem.mysem in /dev/shm directory" << std::endl;
    sem_wait(sem);
    sem_post(sem);
    sem_close(sem);     // 关闭当前进程对信号量的访问
    return 0;
}