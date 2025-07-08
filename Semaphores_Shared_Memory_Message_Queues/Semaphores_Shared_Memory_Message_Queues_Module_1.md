
* **Module 1:** Đồng bộ hóa tiến trình với POSIX Semaphores
* **Module 2:** Trao đổi dữ liệu tốc độ cao với POSIX Shared Memory
* **Module 3:** Giao tiếp có cấu trúc với POSIX Message Queues
* **Module 4:** Project tổng hợp: Xây dựng hệ thống Producer-Consumer đa tiến trình

Bắt đầu với module đầu tiên nào\!

---

### **Module 1: Đồng bộ hóa tiến trình với POSIX Semaphores**

**Semaphore** là một trong những công cụ đồng bộ hóa (synchronization) kinh điển và mạnh mẽ nhất trong lập trình hệ thống. Hãy tưởng tượng nó như một người điều phối tài nguyên, đảm bảo rằng các tiến trình (processes) truy cập vào tài nguyên dùng chung một cách có trật tự và không gây ra xung đột.

---

### 📖 Lý thuyết chi tiết

#### 1\. Semaphore là gì?

Về cốt lõi, **POSIX Semaphore** là một biến số nguyên được bảo vệ bởi kernel, chỉ có thể được truy cập thông qua các thao tác nguyên tử (atomic operations). Nó dùng để giải quyết hai bài toán chính:

* **Loại trừ tương hỗ (Mutual Exclusion):** Đảm bảo tại một thời điểm chỉ có một số lượng tiến trình nhất định (thường là một) có thể truy cập vào một đoạn mã nguy hiểm (critical section).
* **Đồng bộ hóa sự kiện (Event Synchronization):** Một tiến trình có thể chờ một sự kiện xảy ra ở một tiến trình khác. Ví dụ: Tiến trình A phải chờ Tiến trình B tính toán xong dữ liệu mới có thể bắt đầu xử lý.

Аналогия dễ hiểu nhất là một bãi đỗ xe có `N` chỗ trống.

* **Giá trị của Semaphore** chính là số chỗ trống hiện có.
* Khi một xe muốn vào bãi (`sem_wait`), nó kiểm tra xem còn chỗ không.
  * Nếu còn (`N > 0`), nó đi vào và số chỗ trống giảm đi 1 (`N--`).
  * Nếu hết chỗ (`N = 0`), nó phải xếp hàng chờ bên ngoài cho đến khi có xe khác ra.
* Khi một xe rời bãi (`sem_post`), nó giải phóng một chỗ, và số chỗ trống tăng lên 1 (`N++`). Nếu có xe đang chờ, một trong số chúng sẽ được phép vào.

#### 2\. Các loại Semaphore và Thao tác chính

Có hai loại Semaphore chính:

* **Binary Semaphore:** Giá trị chỉ có thể là 0 hoặc 1. Hoạt động rất giống một `Mutex` (Mutual Exclusion Lock), dùng để bảo vệ một tài nguyên duy nhất.
* **Counting Semaphore:** Giá trị có thể là bất kỳ số nguyên không âm nào. Dùng để quản lý một nhóm gồm `N` tài nguyên giống hệt nhau.

Các thao tác cơ bản trên POSIX Semaphore:

| Thao tác          | Mô tả (Hành động nguyên tử)                                                                                                                                                                                                   | Tên kinh điển (Dijkstra)     |
| :----------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :------------------------------ |
| `sem_wait()`     | Giảm giá trị của semaphore đi 1. Nếu giá trị trở thành âm (hoặc trước đó đã là 0), tiến trình gọi sẽ bị**block** (khóa lại) cho đến khi semaphore được tăng lên bởi một tiến trình khác. | `P` (Proberen - to test)      |
| `sem_post()`     | Tăng giá trị của semaphore lên 1. Nếu có bất kỳ tiến trình nào đang bị block trên semaphore này, một trong số chúng sẽ được**unblock** (mở khóa) và tiếp tục thực thi.                            | `V` (Verhogen - to increment) |
| `sem_trywait()`  | Phiên bản không khóa (non-blocking) của `sem_wait()`. Thử giảm giá trị, nhưng nếu phải block, nó sẽ trả về lỗi `EAGAIN` ngay lập tức thay vì chờ.                                                           | -                               |
| `sem_getvalue()` | Lấy giá trị hiện tại của semaphore.                                                                                                                                                                                            | -                               |

#### 3\. Named vs. Unnamed Semaphores

Đây là một điểm quan trọng trong POSIX:

* **Named Semaphores (Semaphore có tên):**
  * **Đặc điểm:** Chúng được xác định bởi một cái tên (ví dụ: `/my_semaphore`). Tên này tồn tại trong một hệ thống file ảo (thường là trong `/dev/shm/`).
  * **API:** `sem_open()`, `sem_close()`, `sem_unlink()`.
  * **Trường hợp sử dụng:** Lý tưởng cho việc đồng bộ hóa giữa các **tiến trình không liên quan** (unrelated processes), ví dụ hai chương trình được chạy riêng biệt từ terminal.
* **Unnamed Semaphores (Semaphore không tên / Memory-based):**
  * **Đặc điểm:** Chúng không có tên, chỉ là một object `sem_t` tồn tại trong bộ nhớ của tiến trình.
  * **API:** `sem_init()`, `sem_destroy()`.
  * **Trường hợp sử dụng:** Thường dùng để đồng bộ hóa giữa các **luồng (threads)** trong cùng một tiến trình, hoặc giữa các tiến trình có quan hệ cha-con (`fork()`) và chia sẻ vùng nhớ chung. Chúng nhanh hơn vì không cần truy cập hệ thống file.

#### 4\. So sánh với các cơ chế khác

* **Semaphore vs. Mutex:**
  * **Ownership:** Một `Mutex` có khái niệm "chủ sở hữu". Luồng nào đã khóa (`lock`) `Mutex` thì **chỉ có luồng đó** mới có thể mở khóa (`unlock`).
  * **Signaling:** `Semaphore` không có khái niệm chủ sở hữu. Bất kỳ tiến trình/luồng nào cũng có thể gọi `sem_post()` để "giải phóng" semaphore, ngay cả khi nó không phải là tiến trình đã gọi `sem_wait()`. Điều này làm cho `Semaphore` trở thành một cơ chế **signaling** mạnh mẽ. Một tiến trình có thể báo hiệu cho một tiến trình khác rằng một sự kiện đã hoàn tất.
* **POSIX vs. RTOS Semaphores (ví dụ FreeRTOS):**
  * **Khái niệm:** Hoàn toàn tương tự (Binary, Counting, Take/Wait, Give/Post).
  * **API:** Khác nhau. Ví dụ, trong FreeRTOS, bạn dùng `xSemaphoreCreateBinary()`, `xSemaphoreTake()`, `xSemaphoreGive()`.
  * **Context:** Trong RTOS, semaphore được thiết kế cho môi trường thời gian thực, nhấn mạnh vào tính xác định (determinism) và độ trễ thấp. Cơ chế scheduler của RTOS sẽ tương tác chặt chẽ với semaphore.
* **POSIX vs. Windows Semaphores:**
  * **Khái niệm:** Tương tự.
  * **API:** Windows có bộ API riêng: `CreateSemaphore()`, `WaitForSingleObject()`, `ReleaseSemaphore()`.
  * **Đặc điểm:** Các object đồng bộ hóa của Windows thường phức tạp hơn, đi kèm với các thuộc tính bảo mật (security attributes) và có thể được dùng trong nhiều kịch bản khác nhau (ví dụ `WaitForMultipleObjects`).

---

### 💻 Code mẫu (C++)

Chúng ta sẽ xây dựng một ví dụ kinh điển: **Producer-Consumer** sử dụng **Named Semaphores**. Kịch bản này rất phổ biến trong các hệ thống nhúng.

* **Producer:** Một tiến trình tạo ra dữ liệu và đưa vào một buffer chung.
* **Consumer:** Một tiến trình khác lấy dữ liệu từ buffer đó để xử lý.
* **Buffer:** Một vùng nhớ chia sẻ (chúng ta sẽ "giả lập" nó bằng file trong ví dụ này, và sẽ thay bằng `Shared Memory` thật ở module sau).

Chúng ta cần 2 semaphores để điều phối:

1. `sem_full`: Đếm số lượng item có trong buffer. Consumer sẽ `wait` trên semaphore này.
2. `sem_empty`: Đếm số lượng chỗ trống trong buffer. Producer sẽ `wait` trên semaphore này.
3. Và một `mutex` (dùng binary semaphore) để bảo vệ việc đọc/ghi vào buffer.

**File `common.h`:**

```cpp
#pragma once

#include <string>

const char* SEM_FULL_NAME = "/sem_full";
const char* SEM_EMPTY_NAME = "/sem_empty";
const char* SEM_MUTEX_NAME = "/sem_mutex";
const std::string BUFFER_FILE = "buffer.txt";
const int BUFFER_SIZE = 5;
```

**File `producer.cpp`:**

```cpp
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include "common.h"

int main() {
    // O_CREAT: Create semaphores if they don't exist
    // 0666: Read/write permissions for all users
    // Initial values:
    // sem_full: 0 (buffer is initially empty)
    // sem_empty: BUFFER_SIZE (buffer has N empty slots)
    // sem_mutex: 1 (mutex is initially unlocked)
    sem_t* sem_full = sem_open(SEM_FULL_NAME, O_CREAT, 0666, 0);
    sem_t* sem_empty = sem_open(SEM_EMPTY_NAME, O_CREAT, 0666, BUFFER_SIZE);
    sem_t* sem_mutex = sem_open(SEM_MUTEX_NAME, O_CREAT, 0666, 1);

    if (sem_full == SEM_FAILED || sem_empty == SEM_FAILED || sem_mutex == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    for (int i = 0; i < 20; ++i) {
        // Wait for an empty slot in the buffer
        sem_wait(sem_empty);

        // Wait for exclusive access to the buffer
        sem_wait(sem_mutex);

        // ---- CRITICAL SECTION START ----
        std::ofstream buffer(BUFFER_FILE, std::ios::app);
        int item = rand() % 100;
        buffer << item << std::endl;
        std::cout << "Produced: " << item << std::endl;
        buffer.close();
        // ---- CRITICAL SECTION END ----

        // Release exclusive access
        sem_post(sem_mutex);

        // Signal that a new item is available
        sem_post(sem_full);

        sleep(1); // Simulate work
    }

    sem_close(sem_full);
    sem_close(sem_empty);
    sem_close(sem_mutex);

    // Clean up the semaphores from the system
    sem_unlink(SEM_FULL_NAME);
    sem_unlink(SEM_EMPTY_NAME);
    sem_unlink(SEM_MUTEX_NAME);

    return 0;
}
```

**File `consumer.cpp`:**

```cpp
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include "common.h"

int main() {
    // Open existing semaphores
    sem_t* sem_full = sem_open(SEM_FULL_NAME, 0);
    sem_t* sem_empty = sem_open(SEM_EMPTY_NAME, 0);
    sem_t* sem_mutex = sem_open(SEM_MUTEX_NAME, 0);

    if (sem_full == SEM_FAILED || sem_empty == SEM_FAILED || sem_mutex == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }
  
    // Clear the buffer file at the start
    std::ofstream ofs(BUFFER_FILE, std::ofstream::out | std::ofstream::trunc);
    ofs.close();


    for (int i = 0; i < 20; ++i) {
        // Wait for an item to be available in the buffer
        sem_wait(sem_full);

        // Wait for exclusive access to the buffer
        sem_wait(sem_mutex);

        // ---- CRITICAL SECTION START ----
        std::ifstream buffer(BUFFER_FILE);
        std::vector<int> items;
        int val;
        while(buffer >> val) {
            items.push_back(val);
        }
        buffer.close();

        if (!items.empty()) {
            int consumed_item = items.front();
            items.erase(items.begin());
            std::cout << "Consumed: " << consumed_item << std::endl;

            // Rewrite the buffer file
            std::ofstream out_buffer(BUFFER_FILE, std::ios::trunc);
            for(int item : items) {
                out_buffer << item << std::endl;
            }
            out_buffer.close();
        }
        // ---- CRITICAL SECTION END ----

        // Release exclusive access
        sem_post(sem_mutex);

        // Signal that an empty slot is now available
        sem_post(sem_empty);

        usleep(1500000); // Simulate work (1.5 seconds)
    }

    sem_close(sem_full);
    sem_close(sem_empty);
    sem_close(sem_mutex);

    return 0;
}
```

**Cách biên dịch và chạy:**
Mở 2 terminal.

```bash
# Terminal 1 & 2: Compile
# -lrt: link with the real-time library (needed for sem_open)
# -lpthread: link with the pthread library
g++ producer.cpp -o producer -lrt -lpthread
g++ consumer.cpp -o consumer -lrt -lpthread

# Terminal 1: Run consumer first so it can clear the buffer
./consumer

# Terminal 2: Run producer
./producer
```

Bạn sẽ thấy Producer và Consumer hoạt động nhịp nhàng, điều phối bởi các semaphores.

---

### 🧩 Liên hệ Embedded Linux

Trong bối cảnh Embedded Linux, vai trò của Semaphore càng trở nên quan trọng:

1. **Quản lý tài nguyên phần cứng:** Đây là ứng dụng phổ biến nhất.

   * **Ví dụ:** Một bus I2C hoặc SPI được chia sẻ bởi nhiều tiến trình. Một tiến trình muốn giao tiếp với một sensor trên bus phải `wait()` một semaphore. Sau khi hoàn thành, nó `post()` semaphore để tiến trình khác có thể sử dụng. Điều này ngăn ngừa việc hai tiến trình cùng lúc ghi/đọc trên bus gây nhiễu loạn dữ liệu.
   * Trong dự án automotive, đây có thể là việc truy cập vào CAN bus, LIN bus, hoặc một module an ninh phần cứng (HSM - Hardware Security Module).
2. **Đồng bộ hóa giữa User-space và Kernel-space:**

   * Một driver trong kernel có thể xử lý một ngắt (interrupt) từ phần cứng (ví dụ: có dữ liệu mới từ ADC).
   * Sau khi xử lý xong và đưa dữ liệu vào buffer, driver có thể `post()` một semaphore mà một tiến trình user-space đang `wait()`.
   * Tiến trình user-space sẽ được đánh thức để xử lý dữ liệu mới, thay vì phải liên tục thăm dò (polling) trong một vòng lặp `while`, giúp tiết kiệm đáng kể CPU.
3. **Tối ưu hóa hệ thống:**

   * Trong các hệ thống nhúng có tài nguyên hạn chế (CPU, RAM), việc sử dụng các cơ chế blocking như `sem_wait()` hiệu quả hơn nhiều so với `polling` (spin-wait), vì nó cho phép HĐH chuyển CPU cho các tác vụ khác trong khi tiến trình đang chờ.
   * **Unnamed Semaphores** đặt trong vùng nhớ chia sẻ (`shared memory`) là lựa chọn tối ưu về hiệu năng cho các tiến trình liên quan, vì chúng không tốn chi phí truy cập hệ thống file như **Named Semaphores**.

---

### 🤔 Câu hỏi Tự đánh giá

1. Sự khác biệt cốt lõi về "quyền sở hữu" (ownership) giữa `Mutex` và `Binary Semaphore` là gì?
2. Trong ví dụ Producer-Consumer ở trên, chuyện gì sẽ xảy ra nếu chúng ta khởi tạo `sem_full` với giá trị `BUFFER_SIZE` và `sem_empty` với giá trị `0`?
3. Khi nào bạn nên chọn `Named Semaphore` thay vì `Unnamed Semaphore` và ngược lại?
4. Lệnh `sem_unlink()` có vai trò gì? Nếu producer thoát mà không gọi `sem_unlink()`, điều gì sẽ xảy ra với các semaphore trên hệ thống?
5. Trong một hệ thống nhúng thời gian thực, tại sao việc một tác vụ có độ ưu tiên cao bị block trên một semaphore mà một tác vụ có độ ưu tiên thấp đang giữ lại là một vấn đề nguy hiểm? (Gợi ý: Priority Inversion).

### 🏋️ Bài tập Thực hành

1. **Mở rộng Producer-Consumer:**

   * Sửa đổi code để chạy 2 producer và 1 consumer. Quan sát xem hệ thống còn hoạt động đúng không.
   * Thêm 1 producer và 2 consumer. Bạn cần thay đổi gì để đảm bảo các consumer không đọc trùng dữ liệu?
2. **Tạo ra Deadlock:**

   * Viết một chương trình với 2 tiến trình và 2 semaphore (A và B).
   * Tiến trình 1: `wait(A)` rồi `wait(B)`.
   * Tiến trình 2: `wait(B)` rồi `wait(A)`.
   * Chạy chương trình và quan sát hiện tượng "khóa chết" (deadlock). Giải thích tại sao nó xảy ra.
3. **Sử dụng Unnamed Semaphores:**

   * Viết lại ví dụ Producer-Consumer bằng cách sử dụng `fork()` để tạo ra tiến trình con.
   * Đặt các `sem_t` object và buffer trong một vùng nhớ chia sẻ (bạn có thể tìm hiểu trước về `mmap` với cờ `MAP_SHARED`, hoặc chờ module sau).
   * Sử dụng `sem_init()` và `sem_destroy()` thay vì `sem_open()`/`sem_close()`. So sánh sự phức tạp của hai cách tiếp cận.



  * **Tiến trình A (Waiter):** Chạy trước và **đợi** một tín hiệu.
  * **Tiến trình B (Worker):** Chạy sau, làm một việc gì đó (giả lập bằng `sleep`), rồi **gửi tín hiệu** cho tiến trình A tiếp tục.

Đây là bản chất của việc đồng bộ hóa sự kiện.

-----

### \#\# Hướng dẫn Step-by-Step sử dụng POSIX Semaphore

#### **Bước 1: Chuẩn bị môi trường & Include các thư viện cần thiết**

Để sử dụng POSIX semaphore, bạn cần include các header file sau. Hãy tạo 2 file `waiter.cpp` và `worker.cpp`.

```cpp
// waiter.cpp và worker.cpp đều cần các header này
#include <iostream>     // Cho std::cout
#include <semaphore.h>  // Thư viện chính của semaphore
#include <fcntl.h>      // Chứa các cờ O_CREAT
#include <unistd.h>     // Cho sleep()
```

#### **Bước 2: Đặt tên cho Semaphore**

Vì chúng ta đang giao tiếp giữa 2 tiến trình độc lập, chúng ta sẽ dùng **Named Semaphore**. Chúng cần một cái tên chung để "tìm thấy" nhau. Tên này phải bắt đầu bằng dấu `/`.

```cpp
// Thêm dòng này vào cả 2 file
const char* SEM_NAME = "/my_simple_semaphore";
```

#### **Bước 3: Mở (hoặc Tạo) Semaphore (`sem_open`)**

Đây là bước quan trọng nhất. Tiến trình nào chạy trước sẽ tạo ra semaphore, tiến trình sau chỉ cần mở nó. Hàm `sem_open` xử lý cả hai việc này.

**Trong `waiter.cpp` (tiến trình tạo):**

```cpp
// waiter.cpp
int main() {
    // 1. Tạo semaphore
    // O_CREAT: Tạo semaphore nếu nó chưa tồn tại.
    // 0666: Quyền truy cập (read/write cho tất cả user).
    // 0: Giá trị khởi tạo của semaphore. Rất quan trọng!
    //    Chúng ta muốn Waiter phải đợi, nên giá trị ban đầu là 0.
    sem_t* my_sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);

    if (my_sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }
    
    // Các bước tiếp theo sẽ ở đây...
}
```

  * **Phân tích `sem_open`:**
      * `SEM_NAME`: Tên đã định nghĩa.
      * `O_CREAT`: Cờ báo cho hệ điều hành "hãy tạo semaphore này nếu chưa có".
      * `0666`: Quyền truy cập file-system style.
      * `0`: **Giá trị ban đầu**. Vì giá trị là 0, bất kỳ ai gọi `sem_wait` ngay bây giờ sẽ bị block. Đây chính là điều chúng ta muốn cho `Waiter`.

#### **Bước 4: Đợi và Gửi tín hiệu (`sem_wait` và `sem_post`)**

Bây giờ là lúc sử dụng semaphore.

**Trong `waiter.cpp` (tiến trình đợi):**

```cpp
// waiter.cpp (phần tiếp theo trong hàm main)
std::cout << "[Waiter] Đang đợi tín hiệu từ Worker..." << std::endl;

// 2. Đợi tín hiệu
// Hàm này sẽ block (dừng) chương trình tại đây cho đến khi
// giá trị của semaphore > 0.
sem_wait(my_sem);

std::cout << "[Waiter] Đã nhận được tín hiệu! Tiếp tục công việc." << std::endl;
```

**Trong `worker.cpp` (tiến trình gửi tín hiệu):**

```cpp
// worker.cpp
int main() {
    // 1. Mở semaphore đã được tạo bởi Waiter.
    // Lưu ý: không có O_CREAT và không có giá trị khởi tạo.
    sem_t* my_sem = sem_open(SEM_NAME, 0);

    if (my_sem == SEM_FAILED) {
        perror("sem_open (Worker)");
        return 1;
    }

    std::cout << "[Worker] Đang làm việc trong 3 giây..." << std::endl;
    sleep(3); // Giả lập công việc

    // 2. Gửi tín hiệu
    // Hàm này tăng giá trị của semaphore lên 1 (từ 0 -> 1).
    // Hành động này sẽ đánh thức tiến trình Waiter đang bị block.
    std::cout << "[Worker] Gửi tín hiệu cho Waiter." << std::endl;
    sem_post(my_sem);
    
    // Các bước tiếp theo...
}
```

#### **Bước 5: Dọn dẹp (`sem_close` và `sem_unlink`)**

Sau khi dùng xong, chúng ta cần "đóng" kết nối tới semaphore và xóa nó khỏi hệ thống.

  * `sem_close(sem_t* sem)`: Đóng kết nối của tiến trình hiện tại tới semaphore. Giống như `fclose()`. **Mỗi tiến trình phải tự gọi nó.**
  * `sem_unlink(const char* name)`: Xóa hẳn semaphore ra khỏi hệ thống. Chỉ cần **một tiến trình gọi là đủ** (thường là tiến trình quản lý chính). Nếu không gọi, semaphore sẽ tồn tại cho đến khi bạn restart máy.

**Hoàn thiện code:**

**File `waiter.cpp` hoàn chỉnh:**

```cpp
#include <iostream>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

const char* SEM_NAME = "/my_simple_semaphore";

int main() {
    // Tạo semaphore với giá trị ban đầu là 0
    sem_t* my_sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (my_sem == SEM_FAILED) {
        perror("sem_open (Waiter)");
        return 1;
    }

    std::cout << "[Waiter] Đang đợi tín hiệu từ Worker..." << std::endl;

    // Wait - sẽ bị block ở đây
    sem_wait(my_sem);

    std::cout << "[Waiter] Đã nhận được tín hiệu! Dọn dẹp và thoát." << std::endl;

    // Đóng kết nối
    sem_close(my_sem);

    // Xóa semaphore khỏi hệ thống
    sem_unlink(SEM_NAME);

    return 0;
}
```

**File `worker.cpp` hoàn chỉnh:**

```cpp
#include <iostream>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

const char* SEM_NAME = "/my_simple_semaphore";

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
    
    // Post - tăng giá trị semaphore để giải phóng Waiter
    sem_post(my_sem);

    // Đóng kết nối
    sem_close(my_sem);

    return 0;
}
```
