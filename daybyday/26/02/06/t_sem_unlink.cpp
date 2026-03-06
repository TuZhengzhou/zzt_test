#include <semaphore.h>
#include <iostream>

// g++ t_sem_unlink.cpp -o t_sem_unlink -lpthread

int main() {
    std::cout << "Now we close semaphore 'mysem'" << std::endl;
    sem_unlink("mysem");
    std::cout << "Now there is no sem.mysem in /dev/shm directory" << std::endl;
}