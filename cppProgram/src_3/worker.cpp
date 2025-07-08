#include <iostream>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include "common.hpp"

int main() {
    // Mở semaphore đã tồn tại
    sem_t* my_sem = sem_open(SEM_NAME, 0);
    if (my_sem == SEM_FAILED) {
        perror("sem_open (Worker)");
        return 1;
    }

    std::cout << "[Worker] Đang làm việc trong 3 giây..." << std::endl;
    sleep(3);
    std::cout << "[Worker] Gửi tín hiệu cho Waiter." << std::endl;
    sem_post(my_sem);
    sem_close(my_sem);

    return 0;
}