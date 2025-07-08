### **Module 1: Giới thiệu Sockets và Giao tiếp Local (AF\_UNIX) 🏠**

**Mục tiêu**: Module này là bước khởi đầu, giúp anh làm quen với API của Sockets trong một môi trường đơn giản nhất, không bị phân tâm bởi các chi tiết mạng phức tạp như địa chỉ IP hay port.

#### 📖 **Lý thuyết chi tiết**

* **Socket là gì?**

  * **Socket** là một điểm cuối (endpoint) của một kênh giao tiếp hai chiều. Nó là một cơ chế cho phép các tiến trình giao tiếp với nhau, không chỉ trên cùng một máy mà còn qua mạng.
  * **So với Pipes/IPC khác**: Điểm khác biệt cốt lõi là Sockets được thiết kế ngay từ đầu để hỗ trợ **giao tiếp mạng**. Trong khi Pipes, Shared Memory, Message Queues chỉ hoạt động trên một máy duy nhất.
* **Mô hình Client-Server**

  * **Server**: Tiến trình "lắng nghe", chờ yêu cầu kết nối từ Client. Nó cung cấp một dịch vụ (service).
  * **Client**: Tiến trình "chủ động", khởi tạo kết nối đến Server để yêu cầu dịch vụ.
* **Quy trình giao tiếp cơ bản (The Socket "Ceremony")**

  1. **Server side**:
     * `socket()`: Tạo một socket mới, giống như xin cấp một số điện thoại chưa có số.
     * `bind()`: Gán cho socket một "tên" hoặc "địa chỉ" để Client có thể tìm thấy. Giống như đăng ký số điện thoại đó.
     * `listen()`: Bật điện thoại và lắng nghe cuộc gọi đến, tạo một hàng đợi cho các cuộc gọi nhỡ.
     * `accept()`: Nhấc máy khi có cuộc gọi. Thao tác này sẽ **block** (chờ) cho đến khi có Client kết nối, và nó tạo ra một đường dây **mới** dành riêng cho việc giao tiếp với Client đó.
  2. **Client side**:
     * `socket()`: Tạo một socket (lấy một chiếc điện thoại).
     * `connect()`: Quay số và kết nối đến địa chỉ của Server.
* **Domain `AF_UNIX` (hay `AF_LOCAL`)**:

  * Đây là "Socket nội bộ". Nó sử dụng hệ thống file của Linux làm không gian địa chỉ.
  * Địa chỉ của một socket `AF_UNIX` là một **đường dẫn file** trên đĩa (ví dụ: `/tmp/my_socket`).
  * Rất hữu ích để giao tiếp giữa các tiến trình trên cùng một máy với hiệu năng cao mà không cần đến network stack.

#### 💻 **Code mẫu (C++)**

Chúng ta sẽ xây dựng một cặp Client/Server đơn giản giao tiếp qua file socket `/tmp/my_app.socket`.

**`server_unix.cpp`**

```cpp
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

const char* SOCKET_PATH = "/tmp/my_app.socket";

int main() {
    // 1. Dọn dẹp socket cũ nếu có và tạo socket mới
    unlink(SOCKET_PATH);
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0); // SOCK_STREAM là cho kết nối tin cậy (như TCP)
    if (server_fd == -1) { perror("socket"); return 1; }

    // 2. Chuẩn bị địa chỉ và `bind`
    sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
  
    if (bind(server_fd, (const sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        return 1;
    }
    std::cout << "[Server] Bound to " << SOCKET_PATH << std::endl;

    // 3. Lắng nghe kết nối
    if (listen(server_fd, 5) == -1) { // Hàng đợi tối đa 5 kết nối
        perror("listen");
        return 1;
    }
  
    // 4. Chấp nhận kết nối (block tại đây)
    std::cout << "[Server] Waiting for connection..." << std::endl;
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) { perror("accept"); return 1; }
    std::cout << "[Server] Client connected!" << std::endl;

    // 5. Giao tiếp với client
    char buffer[256] = {0};
    read(client_fd, buffer, sizeof(buffer));
    std::cout << "[Server] Received: " << buffer << std::endl;
  
    write(client_fd, "Hello from Server!", 19);
  
    // 6. Đóng kết nối và dọn dẹp
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH); // Xóa file socket
  
    return 0;
}
```

**`client_unix.cpp`**

```cpp
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

const char* SOCKET_PATH = "/tmp/my_app.socket";

int main() {
    // 1. Tạo socket
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) { perror("socket"); return 1; }
  
    // 2. Chuẩn bị địa chỉ server
    sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // 3. Kết nối đến server
    std::cout << "[Client] Connecting to " << SOCKET_PATH << "..." << std::endl;
    if (connect(client_fd, (const sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        return 1;
    }
    std::cout << "[Client] Connected to server." << std::endl;

    // 4. Giao tiếp
    write(client_fd, "Hello from Client!", 19);

    char buffer[256] = {0};
    read(client_fd, buffer, sizeof(buffer));
    std::cout << "[Client] Received: " << buffer << std::endl;

    // 5. Đóng kết nối
    close(client_fd);

    return 0;
}
```

#### 🧩 **Liên hệ Embedded Linux**

* **Giao tiếp giữa các Daemons**: Trong một thiết bị nhúng, anh có thể có một `log_daemon` (dịch vụ ghi log) và một `app_daemon` (dịch vụ ứng dụng chính). `app_daemon` có thể gửi các bản tin log đến `log_daemon` thông qua một `AF_UNIX` socket. Nó nhanh, hiệu quả và không yêu cầu cấu hình mạng phức tạp.
* **API cho các tiến trình User-space**: Một driver hoặc một dịch vụ hệ thống chạy với quyền root có thể mở một `AF_UNIX` socket để các ứng dụng user-space không có quyền cao có thể giao tiếp và yêu cầu dịch vụ một cách an toàn, thay vì phải dùng các cơ chế phức tạp hơn như `ioctl`.

---

Khi nào anh thấy ổn với nội dung của Module 1, hãy cho tôi biết để chúng ta tiếp tục với **Module 2: Bước ra Mạng với Sockets AF\_INET (TCP/IP)** nhé.

#### ❓ **Câu hỏi Ôn tập**

1. Đặc điểm nào làm cho Sockets khác biệt cơ bản so với các cơ chế IPC khác như Pipes hay Shared Memory?
2. Mô tả chuỗi 4 system call chính mà một tiến trình Server phải thực hiện để sẵn sàng nhận kết nối. Nêu ngắn gọn mục đích của từng hàm.
3. Trong ví dụ về `AF_UNIX` socket, tại sao lại có lệnh `unlink(SOCKET_PATH)` ở đầu và cuối chương trình `server_unix.cpp`? Điều gì sẽ xảy ra nếu thiếu nó?
4. Tại sao hàm `accept()` lại trả về một file descriptor **mới** (`client_fd`) thay vì sử dụng luôn file descriptor ban đầu (`server_fd`) để giao tiếp?

#### 💻 **Bài tập Thực hành**

1. **Bài 1 (Dễ): "Echo" Server.**

   * Sửa đổi `server_unix.cpp` để sau khi nhận được tin nhắn từ client, nó sẽ gửi lại **chính xác** tin nhắn đó cho client.
   * Sửa đổi `client_unix.cpp` để nó gửi một chuỗi do người dùng nhập từ bàn phím.
2. **Bài 2 (Trung bình): Server Tính toán.**

   * Client sẽ gửi một chuỗi có dạng `"ADD 5 10"` hoặc `"MUL 3 4"`.
   * Server nhận chuỗi, phân tích cú pháp để nhận diện lệnh (`ADD`/`MUL`) và các toán hạng.
   * Server thực hiện phép tính và gửi lại kết quả (dạng chuỗi) cho Client. Ví dụ: `"Result: 15"`.
3. **Bài 3 (Khó): Trao đổi cấu trúc.**

   * Định nghĩa một `struct` chung, ví dụ `struct SystemInfo { int cpu_usage; int mem_free; };`.
   * Sửa đổi Client để gửi `struct` này đến Server (lưu ý về `sizeof`).
   * Server nhận `struct`, in các trường dữ liệu ra màn hình, và gửi lại một thông báo xác nhận.
