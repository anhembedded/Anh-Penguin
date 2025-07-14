#include <iostream>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "shared_data.h"

int main()
{
    std::cout << "Start Write" << std::endl;
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        return 1;
    }
    std::cout << "[Writer] Shared memory object created." << std::endl;

    // 2. Thiết lập kích thước
    if (ftruncate(fd, sizeof(SharedData)) == -1) {
        perror("ftruncate");
        return 1;
    }
    std::cout << "[Writer] Shared memory object size set." << std::endl;

    // 3. Ánh xạ vào không gian địa chỉ
    void* ptr  = mmap(nullptr,
        sizeof(SharedData),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0);
    if(ptr == MAP_FAILED) {
        perror("MAP_FAILED mmap");
        return 1;
    }
    std::cout << "[Writer] Shared memory mapped." << std::endl;
    SharedData* shared_data = static_cast<SharedData*>(ptr);
    shared_data->counter = 100;
    strcpy(shared_data->message, "Hello from Writer!");

    std::cout << "[Writer] Data written. Waiting for reader..." << std::endl;
    sleep(10); // Đợi reader đọc

    // 5. Dọn dẹp
    munmap(ptr, sizeof(SharedData));
    close(fd);
    shm_unlink(SHM_NAME); // Xóa đối tượng khỏi hệ thống
    std::cout << "[Writer] Cleanup complete." << std::endl;

    return 0;

}