
### 📖 Lý thuyết chi tiết

#### 3.1. POSIX Message Queues là gì? 📬

**POSIX Message Queues (Hàng đợi tin nhắn POSIX)** là một cơ chế IPC cho phép các tiến trình trao đổi các gói dữ liệu có kích thước cố định, được gọi là **tin nhắn (messages)**, một cách bất đồng bộ.

Hãy tưởng tượng nó như một hệ thống **hộp thư công cộng** do kernel quản lý.

  * **Hộp thư (`mqd_t`)**: Mỗi hàng đợi là một hộp thư có tên duy nhất.
  * **Gửi thư (`mq_send`)**: Bất kỳ tiến trình nào biết tên hộp thư đều có thể "gửi thư" (một gói tin) vào đó.
  * **Nhận thư (`mq_receive`)**: Tiến trình sở hữu hộp thư có thể "nhận thư" từ đó. Nếu hộp thư rỗng, tiến trình sẽ chờ cho đến khi có thư mới.
  * **Thứ tự & Ưu tiên**: Thư không được xử lý theo kiểu vào trước ra trước (FIFO) một cách mù quáng. Mỗi thư được gửi đi kèm một **mức độ ưu tiên (priority)**. Các thư có độ ưu tiên cao hơn sẽ luôn được nhận trước.

#### 3.2. So sánh: POSIX vs. System V Message Queues

| Đặc điểm | System V Message Queues | POSIX Message Queues |
| :--- | :--- | :--- |
| **API Chính** | `msgget()`, `msgsnd()`, `msgrcv()` | `mq_open()`, `mq_send()`, `mq_receive()` |
| **Định danh** | `key_t` (từ `ftok()`) | Chuỗi tên có dạng `"/my_queue"` |
| **Lọc tin nhắn** | Nhận tin nhắn theo **`mtype`** (một trường `long` trong message). Rất linh hoạt. | Nhận tin nhắn theo **`priority`** (số nguyên). Tin nhắn có `priority` cao nhất sẽ được nhận trước. |
| **Thông báo** | Không hỗ trợ trực tiếp. Phải tự thăm dò (polling). | **Hỗ trợ thông báo (`mq_notify`)**: Có thể yêu cầu kernel báo hiệu (ví dụ, gửi signal) khi có tin nhắn mới, giúp tiết kiệm CPU. |
| **File Descriptor** | Không, dùng `msqid` riêng biệt. | **Có**, `mq_open()` trả về một message queue descriptor, hoạt động tương tự file descriptor. |
| **Khuyến nghị** | Dùng cho hệ thống cũ hoặc khi cần lọc tin nhắn theo `type`. | **Lựa chọn hàng đầu cho dự án mới** vì API đơn giản và có tính năng `priority`, `notify`. |

#### 3.3. Quy trình làm việc với POSIX Message Queues

1.  **`mq_open()`**: Tạo hoặc mở một message queue.

    ```cpp
    struct mq_attr attr;
    attr.mq_maxmsg = 10; // Tối đa 10 tin nhắn trong hàng đợi
    attr.mq_msgsize = sizeof(MyMessage); // Kích thước tối đa mỗi tin nhắn

    mqd_t mqd = mq_open("/my_mq", O_CREAT | O_RDWR, 0666, &attr);
    ```

      * Hàm này trả về một **message queue descriptor (`mqd_t`)**.
      * `struct mq_attr` cho phép anh định nghĩa các thuộc tính của hàng đợi khi tạo mới.

2.  **`mq_send()`**: Gửi một tin nhắn vào hàng đợi.

    ```cpp
    MyMessage msg;
    // ... điền dữ liệu vào msg ...
    mq_send(mqd, (const char*)&msg, sizeof(msg), 10); // Gửi với priority 10
    ```

      * Hàm này là **non-blocking** theo mặc định (nếu hàng đợi đầy, nó sẽ báo lỗi ngay).

3.  **`mq_receive()`**: Nhận một tin nhắn từ hàng đợi.

    ```cpp
    char buffer[sizeof(MyMessage)];
    unsigned int prio;
    ssize_t bytes_read = mq_receive(mqd, buffer, sizeof(buffer), &prio);
    ```

      * Hàm này sẽ **block** nếu hàng đợi rỗng.
      * Nó sẽ nhận tin nhắn có **độ ưu tiên cao nhất**.
      * Giá trị `prio` (tùy chọn) sẽ được điền với độ ưu tiên của tin nhắn nhận được.

4.  **Dọn dẹp**:

      * `mq_close()`: Đóng message queue descriptor đối với tiến trình hiện tại.
      * `mq_unlink()`: Xóa message queue khỏi hệ thống. Chỉ cần một tiến trình gọi sau khi tất cả đã dùng xong.

-----

### 💻 Code mẫu (C++)

Ví dụ mô phỏng một **Server** nhận các yêu cầu từ nhiều **Client** thông qua một message queue.

#### `common.h` (Dùng chung)

```cpp
#ifndef COMMON_H
#define COMMON_H

#include <string>

const char* QUEUE_NAME = "/my_server_mq";
const int MAX_MESSAGES = 10;
const int MAX_MSG_SIZE = 256;

struct ClientRequest {
    int client_id;
    char command[MAX_MSG_SIZE - sizeof(int)];
};

#endif
```

#### `server.cpp` (Receiver)

```cpp
#include <iostream>
#include <mqueue.h>
#include <cstring>
#include "common.h"

int main() {
    mqd_t mqd;
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = sizeof(ClientRequest);
    attr.mq_curmsgs = 0;

    // 1. Tạo message queue
    mqd = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (mqd == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }
    std::cout << "[Server] Message queue is ready." << std::endl;

    // 2. Vòng lặp nhận tin nhắn
    while (true) {
        ClientRequest request;
        unsigned int priority;

        ssize_t bytes_read = mq_receive(mqd, (char*)&request, sizeof(request), &priority);
        if (bytes_read == -1) {
            perror("mq_receive");
            break;
        }

        std::cout << "[Server] Received request from Client " << request.client_id
                  << " with priority " << priority << ": '" << request.command << "'" << std::endl;

        if (strcmp(request.command, "exit") == 0) {
            break;
        }
    }

    // 3. Dọn dẹp
    mq_close(mqd);
    mq_unlink(QUEUE_NAME);
    std::cout << "[Server] Shutting down." << std::endl;

    return 0;
}
```

#### `client.cpp` (Sender)

```cpp
#include <iostream>
#include <mqueue.h>
#include <cstring>
#include <cstdlib>
#include "common.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <priority> <message>" << std::endl;
        return 1;
    }

    mqd_t mqd;
    ClientRequest request;
    
    // 1. Mở message queue đã có
    mqd = mq_open(QUEUE_NAME, O_WRONLY);
    if (mqd == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    // 2. Chuẩn bị tin nhắn
    request.client_id = getpid();
    strncpy(request.command, argv[2], sizeof(request.command) - 1);
    request.command[sizeof(request.command) - 1] = '\0';
    
    unsigned int priority = atoi(argv[1]);

    // 3. Gửi tin nhắn
    if (mq_send(mqd, (const char*)&request, sizeof(request), priority) == -1) {
        perror("mq_send");
        return 1;
    }

    std::cout << "[Client " << request.client_id << "] Sent message with priority " << priority << std::endl;

    // 4. Đóng kết nối
    mq_close(mqd);

    return 0;
}
```

**Cách chạy:**

1.  **Biên dịch:**
    ```bash
    g++ server.cpp -o server -lrt
    g++ client.cpp -o client -lrt
    ```
2.  **Mở 2 terminal:**
      * **Terminal 1:** Chạy server trước. Nó sẽ chờ tin nhắn.
        ```bash
        ./server
        ```
      * **Terminal 2:** Gửi các tin nhắn từ client với các độ ưu tiên khác nhau.
        ```bash
        ./client 10 "Get temperature"   # Priority 10
        ./client 30 "SHUTDOWN NOW"      # Priority 30
        ./client 10 "Get humidity"      # Priority 10
        ./client 20 "Reboot module A"   # Priority 20
        ./client 1 "exit"               # Priority 1
        ```
3.  **Kết quả:** Quan sát ở **Terminal 1**, anh sẽ thấy server nhận các tin nhắn **không theo thứ tự gửi**, mà theo thứ tự **ưu tiên từ cao đến thấp**. Tin "SHUTDOWN NOW" sẽ được xử lý trước cả tin "Get temperature" dù được gửi sau.

-----

### 🧩 Liên hệ Embedded Linux

  * **Hệ thống xử lý lệnh (Command Processor)**: Trong một thiết bị nhúng, một tiến trình giao diện người dùng (UI) hoặc một tiến trình nhận lệnh từ mạng có thể gửi các lệnh vào message queue của một tiến trình điều khiển trung tâm. Tiến trình điều khiển chỉ cần chờ và xử lý lệnh từ queue của nó. Điều này giúp **tách rời (decouple)** logic giao diện và logic điều khiển, làm cho code dễ quản lý và bảo trì hơn.

  * **Quản lý tác vụ ưu tiên**: Đây là ứng dụng đắt giá nhất. Hãy tưởng tượng một robot tự hành:

      * Tin nhắn "Báo cáo vị trí" được gửi với `priority = 10`.
      * Tin nhắn "Xoay camera sang trái" được gửi với `priority = 20`.
      * Tin nhắn "**DỪNG KHẨN CẤP do phát hiện vật cản**" được gửi với `priority = 30` (cao nhất).
      * Tiến trình điều khiển robot sẽ luôn xử lý lệnh dừng khẩn cấp trước tiên, bất kể nó đang bận báo cáo vị trí hay xoay camera. Điều này đảm bảo an toàn và phản ứng nhanh cho hệ thống.

  * **Giao tiếp tiết kiệm năng lượng với `mq_notify`**: Thay vì để một tiến trình liên tục chạy vòng lặp `while(true)` để gọi `mq_receive` (gây tốn CPU), anh có thể dùng `mq_notify` để đăng ký một signal. Tiến trình có thể đi vào trạng thái ngủ. Khi có tin nhắn mới đến, kernel sẽ gửi signal để "đánh thức" tiến trình dậy xử lý. Kỹ thuật này cực kỳ quan trọng cho các thiết bị chạy bằng pin.