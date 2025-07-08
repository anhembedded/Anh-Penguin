#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "shared_data.h"

int main()
{
    sleep(1);
    // 1. Mở đối tượng shared memory đã có
    int fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (fd == -1) {
        perror("shm_open");
        return 1;
    }
    std::cout << "[Reader] Shared memory object opened." << std::endl;
     // 2. Ánh xạ (không cần ftruncate vì chỉ đọc)
    void* ptr = mmap(NULL,
        sizeof(SharedData),
        PROT_READ,
        MAP_SHARED,
        fd,
        0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    std::cout << "[Reader] Shared memory mapped." << std::endl;
    // 3. Sử dụng vùng nhớ
    SharedData* shared_data = static_cast<SharedData*>(ptr);
    std::cout << "[Reader] Reading data..." << std::endl;
    std::cout << "  - Counter: " << shared_data->counter << std::endl;
    std::cout << "  - Message: " << shared_data->message << std::endl;

    // 4. Dọn dẹp
    munmap(ptr, sizeof(SharedData));
    close(fd);
    std::cout << "[Reader] Cleanup complete." << std::endl;

    return 0;

}