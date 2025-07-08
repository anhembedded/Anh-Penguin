### 📖 Mục tiêu và Kiến trúc Dự án

#### 4.1. Bài toán 🎯

Xây dựng một hệ thống gồm:

  * **Nhiều tiến trình Producer**: Cùng nhau sản xuất dữ liệu (ví dụ: các gói tin, các bản ghi) và đưa vào một bộ đệm chung.
  * **Một tiến trình Consumer**: Lấy dữ liệu từ bộ đệm chung đó và xử lý (ví dụ: in ra màn hình, ghi vào file).

**Yêu cầu cốt lõi:**

  * Hệ thống phải hoạt động an toàn, không xảy ra **race condition** (dữ liệu bị ghi đè hoặc đọc sai).
  * Producer phải chờ nếu bộ đệm đã đầy.
  * Consumer phải chờ nếu bộ đệm đang rỗng.
  * Trao đổi dữ liệu phải có hiệu năng cao.

#### 4.2. Kiến trúc Hệ thống 🏛️

Để giải quyết bài toán này, chúng ta sẽ kết hợp các công cụ đã học:

1.  **POSIX Shared Memory**: Dùng để tạo ra **bộ đệm vòng (circular buffer)** chung. Đây là kênh truyền dữ liệu chính, đảm bảo tốc độ cao vì không cần sao chép qua kernel.
2.  **POSIX Semaphores**: Dùng làm các "tín hiệu giao thông" để điều phối việc truy cập vào bộ đệm. Chúng ta cần 3 semaphores:
      * `mutex`: Một **binary semaphore** (giá trị khởi tạo là 1). Nó hoạt động như một ổ khóa, đảm bảo tại một thời điểm chỉ có một tiến trình (producer hoặc consumer) được phép thay đổi các biến quản lý bộ đệm (`in`, `out`).
      * `empty_slots`: Một **counting semaphore** (giá trị khởi tạo là `BUFFER_SIZE`). Nó đếm số ô trống trong bộ đệm. Producer sẽ phải `wait` (chờ) trên semaphore này trước khi ghi.
      * `full_slots`: Một **counting semaphore** (giá trị khởi tạo là 0). Nó đếm số ô đã có dữ liệu. Consumer sẽ phải `wait` (chờ) trên semaphore này trước khi đọc.

#### 4.3. Luồng hoạt động Logic

**Producer Process:**

```
while (true) {
    item = produce_item();         // 1. Tạo dữ liệu
    sem_wait(empty_slots);         // 2. Chờ cho đến khi có ít nhất một ô trống
    sem_wait(mutex);               // 3. Khóa bộ đệm để ghi
    
    // --- Vùng găng ---
    add_item_to_buffer(item);      // 4. Ghi dữ liệu vào bộ đệm
    // --- Hết vùng găng ---

    sem_post(mutex);               // 5. Mở khóa bộ đệm
    sem_post(full_slots);          // 6. Báo hiệu rằng đã có thêm một ô đầy
}
```

**Consumer Process:**

```
while (true) {
    sem_wait(full_slots);          // 1. Chờ cho đến khi có ít nhất một ô đầy dữ liệu
    sem_wait(mutex);               // 2. Khóa bộ đệm để đọc

    // --- Vùng găng ---
    item = remove_item_from_buffer(); // 3. Đọc dữ liệu từ bộ đệm
    // --- Hết vùng găng ---

    sem_post(mutex);               // 4. Mở khóa bộ đệm
    sem_post(empty_slots);         // 5. Báo hiệu rằng đã có thêm một ô trống
    
    consume_item(item);            // 6. Xử lý dữ liệu
}
```

-----

### 💻 Triển khai Code (C++)

Chúng ta sẽ tạo một tiến trình chính (`main`) để khởi tạo tài nguyên và tạo ra các tiến trình con (Producers và Consumer).

#### `common.h`

```cpp
#ifndef COMMON_H
#define COMMON_H

#include <semaphore.h>

const int BUFFER_SIZE = 10;

// Tên của các đối tượng IPC
const char* SHM_NAME = "/prod_cons_shm";
const char* SEM_MUTEX_NAME = "/prod_cons_mutex";
const char* SEM_EMPTY_NAME = "/prod_cons_empty";
const char* SEM_FULL_NAME = "/prod_cons_full";

// Dữ liệu được trao đổi
struct DataItem {
    int producer_id;
    int value;
};

// Cấu trúc của vùng nhớ chia sẻ
struct SharedBuffer {
    DataItem buffer[BUFFER_SIZE];
    int in;  // Chỉ số để producer ghi vào
    int out; // Chỉ số để consumer đọc ra
};

#endif
```

#### `main.cpp` (Orchestrator)

```cpp
#include <iostream>
#include <vector>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "common.h"

void run_producer(int id);
void run_consumer();

int main() {
    // === 1. Khởi tạo tài nguyên ===
    // Dọn dẹp các lần chạy trước nếu có
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_MUTEX_NAME);
    sem_unlink(SEM_EMPTY_NAME);
    sem_unlink(SEM_FULL_NAME);

    // Tạo Shared Memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedBuffer));
    SharedBuffer* shared_data = (SharedBuffer*)mmap(0, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shared_data->in = 0;
    shared_data->out = 0;
    
    // Tạo Semaphores
    sem_t* mutex = sem_open(SEM_MUTEX_NAME, O_CREAT, 0666, 1);
    sem_t* empty_slots = sem_open(SEM_EMPTY_NAME, O_CREAT, 0666, BUFFER_SIZE);
    sem_t* full_slots = sem_open(SEM_FULL_NAME, O_CREAT, 0666, 0);

    // === 2. Tạo các tiến trình Producer và Consumer ===
    const int NUM_PRODUCERS = 3;
    std::vector<pid_t> pids;

    // Tạo Producers
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            run_producer(i + 1);
            exit(0);
        }
        pids.push_back(pid);
    }

    // Tạo Consumer
    pid_t consumer_pid = fork();
    if (consumer_pid == 0) { // Child process
        run_consumer();
        exit(0);
    }
    pids.push_back(consumer_pid);

    // === 3. Chờ tất cả kết thúc ===
    std::cout << "[Main] Waiting for all processes to finish..." << std::endl;
    for (pid_t pid : pids) {
        waitpid(pid, NULL, 0);
    }

    // === 4. Dọn dẹp tài nguyên ===
    std::cout << "[Main] All processes finished. Cleaning up." << std::endl;
    munmap(shared_data, sizeof(SharedBuffer));
    close(shm_fd);
    sem_close(mutex);
    sem_close(empty_slots);
    sem_close(full_slots);
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_MUTEX_NAME);
    sem_unlink(SEM_EMPTY_NAME);
    sem_unlink(SEM_FULL_NAME);
    
    return 0;
}

// ----- Định nghĩa hàm cho Producer và Consumer -----
// (Code cho các hàm này sẽ được đặt ở đây hoặc trong các file riêng)

void run_producer(int id) {
    // Mở lại các tài nguyên đã tạo
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    SharedBuffer* shared_data = (SharedBuffer*)mmap(0, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    sem_t* mutex = sem_open(SEM_MUTEX_NAME, 0);
    sem_t* empty_slots = sem_open(SEM_EMPTY_NAME, 0);
    sem_t* full_slots = sem_open(SEM_FULL_NAME, 0);

    for (int i = 0; i < 5; ++i) { // Mỗi producer tạo 5 item
        sleep(rand() % 3);
        DataItem item;
        item.producer_id = id;
        item.value = i;

        sem_wait(empty_slots);
        sem_wait(mutex);
        
        shared_data->buffer[shared_data->in] = item;
        shared_data->in = (shared_data->in + 1) % BUFFER_SIZE;
        std::cout << "[Producer " << id << "] Produced item " << i << std::endl;

        sem_post(mutex);
        sem_post(full_slots);
    }

    // Dọn dẹp
    munmap(shared_data, sizeof(SharedBuffer));
    close(shm_fd);
    sem_close(mutex);
    sem_close(empty_slots);
    sem_close(full_slots);
}

void run_consumer() {
    // Mở lại các tài nguyên
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    SharedBuffer* shared_data = (SharedBuffer*)mmap(0, sizeof(SharedBuffer), PROT_READ, MAP_SHARED, shm_fd, 0);
    sem_t* mutex = sem_open(SEM_MUTEX_NAME, 0);
    sem_t* empty_slots = sem_open(SEM_EMPTY_NAME, 0);
    sem_t* full_slots = sem_open(SEM_FULL_NAME, 0);

    for (int i = 0; i < 15; ++i) { // Consumer sẽ tiêu thụ 15 item
        sem_wait(full_slots);
        sem_wait(mutex);

        DataItem item = shared_data->buffer[shared_data->out];
        shared_data->out = (shared_data->out + 1) % BUFFER_SIZE;
        std::cout << "\t[Consumer] Consumed item from Producer " << item.producer_id << " with value " << item.value << std::endl;

        sem_post(mutex);
        sem_post(empty_slots);

        sleep(rand() % 2);
    }
    
    // Dọn dẹp
    munmap(shared_data, sizeof(SharedBuffer));
    close(shm_fd);
    sem_close(mutex);
    sem_close(empty_slots);
    sem_close(full_slots);
}
```

**Cách chạy:**

1.  **Biên dịch:**
    ```bash
    g++ main.cpp -o multi_proc_app -lrt -lpthread
    ```
      * `-lrt`: Cần cho POSIX IPC.
      * `-lpthread`: Thường cần cho các hàm `sem_...` dù chúng ta không dùng Pthreads.
2.  **Chạy:**
    ```bash
    ./multi_proc_app
    ```
3.  **Quan sát Output**: Anh sẽ thấy log của các Producer và Consumer xen kẽ nhau, nhưng Consumer sẽ luôn đọc được dữ liệu hợp lệ.

-----

### 🧩 Mở rộng và Ứng dụng Thực tế

  * **Kịch bản thực tế**:

      * **Producers**: Các tiến trình đọc dữ liệu từ các nguồn khác nhau trên xe hơi: một tiến trình đọc dữ liệu từ **CAN bus**, một tiến trình đọc dữ liệu từ **cảm biến Lidar**, một tiến trình đọc **tọa độ GPS**.
      * **Shared Buffer**: Một "trung tâm dữ liệu" tốc độ cao trong bộ nhớ.
      * **Consumer**: Một tiến trình **Data Logger** ghi tất cả dữ liệu này xuống bộ nhớ lưu trữ (SSD) một cách tuần tự và có trật tự để phục vụ cho việc phân tích sau này (post-analysis) hoặc gỡ lỗi.

  * **Hướng Mở rộng**:

    1.  **Shutdown một cách duyên dáng**: Làm thế nào để Consumer biết khi nào tất cả Producer đã làm xong và có thể dừng lại?
          * **Giải pháp**: Sau khi các Producer kết thúc, tiến trình chính có thể ghi một "viên thuốc độc" (`poison pill`) vào buffer - một `DataItem` có giá trị đặc biệt (ví dụ `producer_id = -1`). Khi Consumer đọc được item này, nó sẽ biết cần phải thoát.
    2.  **Giám sát hiệu năng**: Thêm trường `timestamp` vào `DataItem` để đo độ trễ - tức là thời gian một item nằm trong buffer từ lúc được sản xuất đến lúc được tiêu thụ.
    3.  **Sử dụng Mutex trong Shared Memory**: Thay vì dùng Named Semaphore, anh có thể đặt một **Pthread Mutex** ngay bên trong `SharedBuffer`. Điều này yêu cầu khởi tạo mutex với các thuộc tính đặc biệt để nó có thể được chia sẻ giữa các tiến trình (`PTHREAD_PROCESS_SHARED`). Đây là một kỹ thuật nâng cao hơn.