### **Module 5: Project Tổng hợp - Xây dựng Chat Server Đa Người Dùng 💬**

**Mục tiêu**: Vận dụng kiến thức về Sockets `AF_INET` và I/O Multiplexing (`select`) để xây dựng một Chat Server đa người dùng. Server có khả năng xử lý nhiều client kết nối đồng thời. Khi một client gửi tin nhắn, server sẽ "phát thanh" (broadcast) tin nhắn đó đến tất cả các client khác.

#### 📖 **Kiến trúc và Logic Dự án**

  * **Công nghệ cốt lõi**:

      * **Giao thức**: TCP (`SOCK_STREAM`) để đảm bảo tin nhắn được gửi đi một cách tin cậy.
      * **Mô hình Server**: Sử dụng `select()` để quản lý I/O từ nhiều client trong một tiến trình duy nhất, đảm bảo hiệu năng cao.

  * **Luồng hoạt động của Server**:

    1.  Khởi tạo một socket lắng nghe (`server_fd`) như trong Module 2.
    2.  Sử dụng một `fd_set` (`active_fds`) để lưu tất cả các file descriptor cần theo dõi, ban đầu chỉ có `server_fd`.
    3.  Vào vòng lặp `while(true)` và gọi `select()` để chờ hoạt động.
    4.  Khi `select()` trả về, kiểm tra xem fd nào có hoạt động:
          * **Nếu là `server_fd`**: Có kết nối mới. Gọi `accept()` để tạo `client_fd` mới. Thêm `client_fd` này vào `active_fds`. Thông báo có client mới tham gia.
          * **Nếu là một `client_fd`**: Có tin nhắn từ một client.
            a. Gọi `read()` để đọc tin nhắn.
            b. **Nếu `read()` trả về \> 0**: Duyệt qua tất cả các fd trong `active_fds`, gửi tin nhắn vừa nhận được cho tất cả các `client_fd` khác (trừ `server_fd` và chính người gửi).
            c. **Nếu `read()` trả về 0 hoặc \< 0**: Client đã ngắt kết nối. Thông báo client đã rời đi, `close()` fd đó và dùng `FD_CLR` để loại nó khỏi `active_fds`.

  * **Luồng hoạt động của Client**:

      * Client cũng cần xử lý I/O từ 2 nguồn gần như đồng thời:
        1.  **Người dùng**: Đọc input từ bàn phím (`stdin`).
        2.  **Server**: Nhận tin nhắn broadcast từ server.
      * Đây là một kịch bản hoàn hảo để áp dụng `select()` ngay tại phía client\! Client sẽ `select()` trên `stdin` (fd = 0) và `socket` của nó.

#### 💻 **Code mẫu (C++)**

**`chat_server.cpp`**

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include <cstring>

const int PORT = 9999;
const int BUFFER_SIZE = 1024;

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // ... (code bind và listen giống Module 3) ...
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 10);

    fd_set active_fds, read_fds;
    FD_ZERO(&active_fds);
    FD_SET(server_fd, &active_fds);
    int max_fd = server_fd;

    std::cout << "[Chat Server] Started on port " << PORT << ". Waiting for clients..." << std::endl;

    while (true) {
        read_fds = active_fds;
        select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        for (int i = 0; i <= max_fd; ++i) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == server_fd) { // 1. Kết nối mới
                    int client_fd = accept(server_fd, NULL, NULL);
                    FD_SET(client_fd, &active_fds);
                    if (client_fd > max_fd) max_fd = client_fd;
                    std::cout << "New client connected on fd " << client_fd << std::endl;
                } else { // 2. Tin nhắn từ client
                    char buffer[BUFFER_SIZE] = {0};
                    int bytes_read = read(i, buffer, BUFFER_SIZE);

                    if (bytes_read <= 0) { // Client ngắt kết nối
                        std::cout << "Client on fd " << i << " disconnected." << std::endl;
                        close(i);
                        FD_CLR(i, &active_fds);
                    } else { // Broadcast tin nhắn
                        std::cout << "Broadcasting message from fd " << i << std::endl;
                        for (int j = 0; j <= max_fd; ++j) {
                            if (FD_ISSET(j, &active_fds)) {
                                // Gửi cho tất cả trừ server và chính người gửi
                                if (j != server_fd && j != i) {
                                    write(j, buffer, bytes_read);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
```

**`chat_client.cpp`**

```cpp
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <string>

const int PORT = 9999;
const char* SERVER_IP = "127.0.0.1";

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    // ... (code connect giống Module 2) ...
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    connect(sock_fd, (sockaddr*)&server_addr, sizeof(server_addr));

    std::cout << "Connected to chat server. You can start typing." << std::endl;

    while (true) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds); // Theo dõi STDIN (bàn phím)
        FD_SET(sock_fd, &read_fds);       // Theo dõi socket từ server

        select(sock_fd + 1, &read_fds, NULL, NULL, NULL);

        if (FD_ISSET(STDIN_FILENO, &read_fds)) { // 1. Người dùng gõ phím
            std::string line;
            std::getline(std::cin, line);
            if (!line.empty()) {
                write(sock_fd, line.c_str(), line.length());
            }
        }

        if (FD_ISSET(sock_fd, &read_fds)) { // 2. Có tin nhắn từ server
            char buffer[1024] = {0};
            int bytes_read = read(sock_fd, buffer, sizeof(buffer));
            if (bytes_read <= 0) {
                std::cout << "Server disconnected." << std::endl;
                break;
            }
            std::cout << buffer << std::endl;
        }
    }

    close(sock_fd);
    return 0;
}
```

#### 🧩 **Liên hệ, Thảo luận và Hướng Mở rộng**

  * **Ứng dụng thực tế**:

      * **Hệ thống cảnh báo (Alert System)**: Trong một nhà máy, một "server" trung tâm nhận cảnh báo từ nhiều cảm biến (các "client"). Khi một cảm biến gửi cảnh báo nguy hiểm (ví dụ: nhiệt độ quá cao), server sẽ broadcast ngay lập tức cảnh báo đó đến các trạm giám sát (các client khác) để nhân viên có thể xử lý.
      * **Phòng họp ảo cho thiết bị (Device Collaboration)**: Nhiều thiết bị nhúng trong một hệ thống phức tạp (như trên một chiếc xe tự lái) cần trao đổi trạng thái cho nhau. Một thiết bị có thể broadcast trạng thái "Tôi đang thực hiện cập nhật firmware, vui lòng không gửi yêu cầu" cho tất cả các thiết bị khác.

  * **Hạn chế và Hướng Mở rộng**:

    1.  **Danh tính Người gửi**: Hiện tại, client nhận được tin nhắn nhưng không biết ai đã gửi.
          * **Bài tập 1**: Sửa đổi server. Khi nhận được tin nhắn từ client `i`, hãy tạo một chuỗi mới có dạng `[Client <i>]: <tin nhắn>` rồi mới broadcast.
    2.  **Nickname**: Sử dụng số file descriptor để định danh thì không thân thiện.
          * **Bài tập 2**: Thiết kế một giao thức đơn giản. Khi client mới kết nối, tin nhắn đầu tiên nó gửi phải là nickname. Server sẽ lưu nickname này vào một `std::map<int, std::string>` (ánh xạ từ fd sang nickname). Khi broadcast, server sẽ dùng nickname thay vì số fd.
    3.  **Thoát một cách duyên dáng**:
          * **Bài tập 3**: Nếu client gõ `/quit`, client sẽ đóng kết nối và thoát. Server khi phát hiện client ngắt kết nối (`read` trả về 0), sẽ tìm nickname tương ứng và broadcast tin nhắn `--- [Nickname] has left the chat. ---` cho những người còn lại.

-----

#### ❓ **Câu hỏi Ôn tập**

1.  Tại sao `select()` là công cụ lý tưởng cho cả server và client trong ứng dụng chat này?
2.  Trong server, khi `select()` trả về, làm thế nào để phân biệt giữa một yêu cầu kết nối mới và một tin nhắn từ client đã kết nối?
3.  Trong client, làm thế nào để xử lý đồng thời việc người dùng gõ phím và việc nhận tin nhắn từ server mà không làm chương trình bị "treo"?
4.  Mô tả cách server xử lý khi một client ngắt kết nối. Tại sao việc `close()` file descriptor và `FD_CLR()` nó khỏi `active_fds` lại quan trọng?
