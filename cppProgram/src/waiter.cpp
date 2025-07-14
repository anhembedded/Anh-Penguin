#include "common.hpp"
#include <unistd.h>
#include <stdlib.h>

int main() {
    
    sem_t* my_sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);

    if (my_sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }
    std::cout << "[Waiter] Đang đợi tín hiệu từ Worker..." << std::endl;
    sem_wait(my_sem);
    std::cout << "[Waiter] Đã nhận được tín hiệu! Tiếp tục công việc." << std::endl;
    // Đóng kết nối
    sem_close(my_sem);

    // Xóa semaphore khỏi hệ thống
    sem_unlink(SEM_NAME);
    return 0;
}