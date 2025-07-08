### 📖 Lý thuyết chi tiết

#### 2.1. POSIX Shared Memory là gì? 🚀

**Shared Memory (Bộ nhớ chia sẻ)** là cơ chế cho phép hai hay nhiều tiến trình cùng **ánh xạ (map)** một vùng **bộ nhớ vật lý (physical RAM)** vào không gian **địa chỉ ảo (virtual address space)** của riêng chúng.

  * **Hiệu năng Cực cao**: Sau khi được ánh xạ, các tiến trình có thể đọc và ghi vào vùng nhớ này như thể nó là một con trỏ được cấp phát bởi `malloc`. Mọi thay đổi từ một tiến trình sẽ được các tiến trình khác "thấy" ngay lập tức.
  * **Zero-Copy (hoặc Near Zero-Copy)**: Dữ liệu không cần phải được sao chép từ user space của tiến trình A sang kernel space, rồi lại từ kernel space sang user space của tiến trình B. Việc loại bỏ các bước sao chép trung gian này giúp **giảm thiểu độ trễ (latency)** và **tiết kiệm chu kỳ CPU (CPU cycles)**, khiến nó trở thành lựa chọn lý tưởng để trao đổi lượng dữ liệu lớn (ví dụ: buffer hình ảnh, ma trận dữ liệu cảm biến).

#### 2.2. So sánh: POSIX vs. System V Shared Memory

Anh đã biết về System V, vậy hãy xem POSIX làm cho mọi thứ trở nên trực quan hơn như thế nào.

| Đặc điểm | System V Shared Memory | POSIX Shared Memory |
| :--- | :--- | :--- |
| **API Chính** | `shmget()`, `shmat()`, `shmdt()`, `shmctl()` | `shm_open()`, `ftruncate()`, `mmap()`, `munmap()`, `shm_unlink()` |
| **Định danh** | `key_t` (từ `ftok()`) | Một **chuỗi tên** có dạng `"/my_shm"` (giống tên file) |
| **Tư duy** | Hoạt động như một đối tượng IPC "trừu tượng" của kernel. | Hoạt động giống như một **file** trong hệ thống. Anh "mở" nó, "thay đổi kích thước" nó, và "ánh xạ" nó. |
| **Kết quả** | `shmat()` trả về con trỏ tới vùng nhớ. | `mmap()` trả về con trỏ tới vùng nhớ. |
| **Khuyến nghị** | Dùng khi cần tương tác với hệ thống cũ. | **Lựa chọn hàng đầu cho các dự án mới** vì API nhất quán và dễ hiểu hơn. |

#### 2.3. Quy trình làm việc với POSIX Shared Memory

Đây là các bước chuẩn để thiết lập và sử dụng POSIX Shared Memory.

1.  **`shm_open()`**: Tạo hoặc mở một đối tượng bộ nhớ chia sẻ.

    ```cpp
    int fd = shm_open("/my_shared_mem", O_CREAT | O_RDWR, 0666);
    ```

      * Hàm này trả về một **file descriptor** giống như khi mở file, nhưng đối tượng này thực chất nằm trong một hệ thống file ảo (thường là `/dev/shm`).
      * `/my_shared_mem`: Tên phải bắt đầu bằng dấu `/`.
      * `O_CREAT`: Tạo nếu chưa tồn tại.

2.  **`ftruncate()`**: **Thiết lập kích thước** cho đối tượng bộ nhớ chia sẻ.

    ```cpp
    ftruncate(fd, sizeof(MySharedData));
    ```

      * **Đây là bước cực kỳ quan trọng và hay bị bỏ quên.** Một đối tượng mới được tạo ra có kích thước bằng 0. Anh phải "kéo dài" nó ra đúng kích thước cần dùng.

3.  **`mmap()`**: **Ánh xạ** đối tượng vào không gian địa chỉ của tiến trình.

    ```cpp
    void* ptr = mmap(NULL, sizeof(MySharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ```

      * `mmap` là "phép màu" biến file descriptor thành một con trỏ mà anh có thể sử dụng.
      * `MAP_SHARED`: Cờ quan trọng nhất, đảm bảo các thay đổi được chia sẻ với các tiến trình khác.

4.  **Sử dụng con trỏ**: Giờ đây `ptr` có thể được ép kiểu và sử dụng như bất kỳ con trỏ nào khác.

5.  **Dọn dẹp**:

      * `munmap()`: Hủy ánh xạ vùng nhớ khỏi tiến trình.
      * `close()`: Đóng file descriptor.
      * `shm_unlink()`: **Xóa đối tượng bộ nhớ chia sẻ khỏi hệ thống.** Chỉ cần một tiến trình (thường là tiến trình tạo ra nó) gọi hàm này sau khi mọi người đã dùng xong.

⚠️ **Lưu ý cực kỳ quan trọng**: Shared Memory **KHÔNG** cung cấp bất kỳ cơ chế **đồng bộ hóa (synchronization)** nào. Nó chỉ là một vùng nhớ thô. Nếu nhiều tiến trình cùng ghi vào đó, anh sẽ gặp **race condition**. Vì vậy, Shared Memory hầu như luôn được sử dụng **kết hợp** với một cơ chế đồng bộ hóa khác như **Semaphore** hoặc **Mutex**.

-----

### 💻 Code mẫu (C++)

Ví dụ này mô phỏng mô hình **Producer (Writer) / Consumer (Reader)**. Writer tạo ra dữ liệu và Reader đọc dữ liệu đó.

#### `shared_data.h` (Dùng chung cho cả hai)

```cpp
#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <cstdint>

// Dữ liệu cần chia sẻ
struct SharedData {
    uint32_t counter;
    char message[128];
};

// Tên của đối tượng shared memory
const char* SHM_NAME = "/my_app_shm";

#endif
```

#### `writer.cpp` (Producer)

```cpp
#include <iostream>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "shared_data.h"

int main() {
    // 1. Tạo đối tượng shared memory
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
    std::cout << "[Writer] Shared memory size set." << std::endl;

    // 3. Ánh xạ vào không gian địa chỉ
    void* ptr = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    std::cout << "[Writer] Shared memory mapped." << std::endl;

    // 4. Sử dụng vùng nhớ
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
```

#### `reader.cpp` (Consumer)

```cpp
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "shared_data.h"

int main() {
    sleep(1); // Chờ writer tạo xong

    // 1. Mở đối tượng shared memory đã có
    int fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (fd == -1) {
        perror("shm_open");
        return 1;
    }
    std::cout << "[Reader] Shared memory object opened." << std::endl;

    // 2. Ánh xạ (không cần ftruncate vì chỉ đọc)
    void* ptr = mmap(NULL, sizeof(SharedData), PROT_READ, MAP_SHARED, fd, 0);
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
```

**Cách chạy:**

1.  **Biên dịch:**
    ```bash
    g++ writer.cpp -o writer -lrt
    g++ reader.cpp -o reader -lrt
    ```
      * Lưu ý cờ `-lrt` (real-time library) là cần thiết cho các hàm POSIX IPC.
2.  **Mở 2 terminal:**
      * **Terminal 1:** `./writer`
      * **Terminal 2:** (Trong lúc writer đang đợi 10s) `./reader`
3.  **Kết quả:** Terminal 2 (Reader) sẽ in ra dữ liệu mà Terminal 1 (Writer) đã ghi.

-----

### 🧩 Liên hệ Embedded Linux

Đây là nơi Shared Memory thực sự tỏa sáng.

  * **Xử lý luồng Video/Camera**: Trong một hệ thống xe tự lái, một tiến trình (`camera_capture`) nhận dữ liệu thô từ camera MIPI-CSI và **đẩy thẳng các khung hình (frames)** vào một vùng nhớ chia sẻ lớn (vài chục MB). Một tiến trình thứ hai (`ai_inference`) đọc các khung hình từ vùng nhớ đó để chạy các thuật toán nhận diện làn đường, vật cản. Việc này tránh hoàn toàn việc sao chép các frame video khổng lồ, giúp hệ thống đáp ứng thời gian thực.

  * **Tổng hợp dữ liệu cảm biến (Sensor Fusion)**:

      * **Tiến trình A**: Đọc dữ liệu từ IMU (gia tốc, con quay) ở tần số 800Hz và ghi vào shared memory.
      * **Tiến trình B**: Đọc dữ liệu từ GPS ở tần số 10Hz và cũng ghi vào shared memory.
      * **Tiến trình C (`kalman_filter`)**: Đọc dữ liệu từ cả hai nguồn trong shared memory để tính toán ra vị trí và hướng của xe một cách chính xác. Độ trễ cực thấp của shared memory là yếu tố sống còn cho các thuật toán điều khiển.

  * **Giao tiếp với Hardware DMA**: Trong các ứng dụng hiệu năng cao, bộ điều khiển **DMA (Direct Memory Access)** có thể được cấu hình để ghi dữ liệu từ ngoại vi (ví dụ: ADC, SPI) trực tiếp vào một vùng nhớ vật lý, vùng nhớ này sau đó được ánh xạ vào các tiến trình user-space dưới dạng shared memory. Đây là đỉnh cao của cơ chế "zero-copy", khi CPU thậm chí không cần tham gia vào việc di chuyển dữ liệu.

---

## 🧠 Cú pháp của `mmap()`

```cpp
#include <sys/mman.h>

void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
```

| Tham số       | Ý nghĩa                                                                 |
|---------------|-------------------------------------------------------------------------|
| `addr`        | Địa chỉ bắt đầu ánh xạ (thường để `NULL` để kernel tự chọn)             |
| `length`      | Kích thước vùng ánh xạ (số byte)                                        |
| `prot`        | Quyền truy cập: `PROT_READ`, `PROT_WRITE`, `PROT_EXEC`, `PROT_NONE`     |
| `flags`       | Kiểu ánh xạ: `MAP_SHARED`, `MAP_PRIVATE`, `MAP_ANONYMOUS`, v.v.         |
| `fd`          | File descriptor của file cần ánh xạ (hoặc `-1` nếu dùng `MAP_ANONYMOUS`)|
| `offset`      | Vị trí bắt đầu trong file (phải chia hết cho kích thước trang)          |

---

## 🔧 Ví dụ đơn giản

```cpp
int fd = open("data.txt", O_RDONLY);
char* mapped = (char*) mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
```

➡️ File `data.txt` sẽ được ánh xạ vào bộ nhớ, và ta có thể đọc nội dung như đọc mảng `mapped[i]`.

---

## 🧹 Giải phóng vùng ánh xạ

```cpp
munmap(mapped, 4096);
```

---

## 📦 Một số flags phổ biến

| Flag              | Ý nghĩa                                                                 |
|-------------------|-------------------------------------------------------------------------|
| `MAP_SHARED`      | Các tiến trình khác có thể thấy thay đổi trong vùng ánh xạ              |
| `MAP_PRIVATE`     | Copy-on-write — thay đổi không ảnh hưởng đến file gốc                   |
| `MAP_ANONYMOUS`   | Không ánh xạ vào file nào — dùng để cấp phát bộ nhớ tạm                 |
| `MAP_FIXED`       | Yêu cầu ánh xạ đúng tại địa chỉ `addr` (cẩn thận khi dùng)              |

---

## 🔄 So sánh với `read()` truyền thống

| Cách truy cập     | Ưu điểm của `mmap()`                          |
|-------------------|-----------------------------------------------|
| `read()`          | Phải copy dữ liệu từ kernel vào user space    |
| `mmap()`          | Truy cập trực tiếp vào vùng bộ nhớ ánh xạ     |
|                   | Tối ưu cho file lớn, chia sẻ giữa tiến trình  |

---

