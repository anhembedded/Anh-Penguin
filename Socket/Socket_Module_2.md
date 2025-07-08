
### **Module 2: Bước ra Mạng với Sockets AF\_INET (TCP/IP) 🌐**

**Mục tiêu**: Module này sẽ chuyển đổi kiến thức từ giao tiếp nội bộ sang mạng thực sự. Anh sẽ học cách sử dụng Sockets với bộ giao thức TCP/IP, làm quen với các khái niệm quan trọng như địa chỉ IP, port, và vấn đề "byte ordering".

#### 📖 **Lý thuyết chi tiết**

  * **Domain `AF_INET`**:

      * Đây là domain dành cho giao thức Internet phiên bản 4 (IPv4), phổ biến nhất là **TCP/IP**. Hầu hết mọi giao tiếp mạng mà anh biết (web, email, game...) đều dùng domain này.
      * Địa chỉ của một socket `AF_INET` được xác định bởi một cặp: **địa chỉ IP** và **số port**.
          * **Địa chỉ IP (IP Address)**: Một số 32-bit, dùng để định danh một máy tính (host) trên mạng. Nó thường được viết dưới dạng "dotted quad" (ví dụ: `192.168.1.10`). Địa chỉ `127.0.0.1` (còn gọi là `localhost`) là một địa chỉ đặc biệt chỉ về chính máy đó, rất hữu ích để kiểm thử.
          * **Số Port (Port Number)**: Một số 16-bit (từ 0 đến 65535), dùng để định danh một ứng dụng hoặc một dịch vụ cụ thể trên máy tính đó. Một máy tính có thể chạy nhiều dịch vụ cùng lúc, mỗi dịch vụ lắng nghe trên một port riêng. Ví dụ: port 80 cho Web (HTTP), 22 cho SSH.
          * **Lưu ý**: Các port dưới 1024 thường được dành cho các dịch vụ hệ thống và đòi hỏi quyền root để `bind`. Khi phát triển, chúng ta nên chọn các port lớn hơn 1024.

  * **Vấn đề "Byte Ordering" (Thứ tự Byte)**:

      * Đây là một khái niệm **cực kỳ quan trọng** trong lập trình mạng. Các kiến trúc máy tính khác nhau lưu trữ các số nhiều byte (như `short` 16-bit, `long` 32-bit) theo thứ tự khác nhau:
          * **Little-Endian** (ví dụ: Intel x86, AMD64): Byte có trọng số thấp nhất được lưu ở địa chỉ bộ nhớ thấp nhất.
          * **Big-Endian** (ví dụ: PowerPC, SPARC, các hệ thống mạng cũ): Byte có trọng số cao nhất được lưu ở địa chỉ bộ nhớ thấp nhất.
      * Để đảm bảo các máy tính có thể giao tiếp chính xác, một **Thứ tự Byte Mạng (Network Byte Order)** tiêu chuẩn đã được định nghĩa, và nó chính là **Big-Endian**.
      * **Các hàm chuyển đổi**: Trước khi gửi một số (port, địa chỉ IP) vào mạng, anh **phải** chuyển nó từ thứ tự byte của máy mình (Host Byte Order) sang Network Byte Order.
          * `htons()`: **H**ost **to** **N**etwork **S**hort (16-bit, dùng cho **port**).
          * `htonl()`: **H**ost **to** **N**etwork **L**ong (32-bit, dùng cho **địa chỉ IP**).
          * `ntohs()`, `ntohl()`: Ngược lại, chuyển từ Network sang Host khi nhận dữ liệu.

  * **Cấu trúc địa chỉ mới**: Khi làm việc với `AF_INET`, chúng ta sử dụng `struct sockaddr_in` thay vì `struct sockaddr_un`.

    ```cpp
    struct sockaddr_in {
        sa_family_t    sin_family; // Luôn là AF_INET
        in_port_t      sin_port;   // Số port (phải dùng htons())
        struct in_addr sin_addr;   // Địa chỉ IP (phải dùng htonl())
    };
    ```

#### 💻 **Code mẫu (C++)**

Chuyển đổi ví dụ từ Module 1 sang `AF_INET`, chạy trên `localhost` với port `9999`.

**`server_inet.cpp`**

```cpp
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h> // Cho sockaddr_in
#include <unistd.h>

const int PORT = 9999;

int main() {
    // 1. Tạo socket với domain AF_INET
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { perror("socket"); return 1; }

    // 2. Chuẩn bị địa chỉ và `bind`
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    // INADDR_ANY: Chấp nhận kết nối từ bất kỳ địa chỉ IP nào của máy. Rất hữu dụng.
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(PORT); // Chuyển port sang network byte order
    
    if (bind(server_fd, (const sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind"); return 1;
    }
    
    // 3. Listen và Accept (logic không đổi)
    listen(server_fd, 5);
    std::cout << "[Server] Waiting for connection on port " << PORT << std::endl;
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) { perror("accept"); return 1; }
    std::cout << "[Server] Client connected!" << std::endl;

    // 4. Giao tiếp và Đóng (logic không đổi)
    char buffer[256] = {0};
    read(client_fd, buffer, sizeof(buffer));
    std::cout << "[Server] Received: " << buffer << std::endl;
    write(client_fd, "Hello from INET Server!", 24);
    
    close(client_fd);
    close(server_fd);

    return 0;
}
```

**`client_inet.cpp`**

```cpp
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // Cho inet_addr
#include <unistd.h>

const int PORT = 9999;
const char* SERVER_IP = "127.0.0.1";

int main() {
    // 1. Tạo socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Chuẩn bị địa chỉ server
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    // inet_addr sẽ chuyển đổi địa chỉ IP dạng chuỗi sang dạng số và network byte order
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP); 

    // 3. Kết nối đến server
    if (connect(client_fd, (const sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect"); return 1;
    }
    std::cout << "[Client] Connected to server." << std::endl;

    // 4. Giao tiếp và Đóng (logic không đổi)
    write(client_fd, "Hello from INET Client!", 24);
    char buffer[256] = {0};
    read(client_fd, buffer, sizeof(buffer));
    std::cout << "[Client] Received: " << buffer << std::endl;
    close(client_fd);

    return 0;
}
```

#### 🧩 **Liên hệ Embedded Linux**

  * **Giao diện điều khiển từ xa (Remote CLI/Web)**: Một thiết bị nhúng như router, camera IP, hoặc bộ điều khiển công nghiệp thường chạy một server socket. Anh có thể viết một ứng dụng client trên máy tính để kết nối vào thiết bị qua mạng Ethernet hoặc Wi-Fi để gửi lệnh cấu hình (`set_param X=Y`), yêu cầu gửi trạng thái (`get_status`), hoặc ra lệnh reboot.
  * **Streaming Dữ liệu**: Một thiết bị đo lường (data logger) có thể liên tục gửi các gói dữ liệu đo được về một máy chủ trung tâm để xử lý và hiển thị. Giao thức TCP đảm bảo dữ liệu không bị mất mát hay sai thứ tự, điều rất quan trọng cho các chuỗi dữ liệu cần sự toàn vẹn.

-----

#### ❓ **Câu hỏi Ôn tập**

1.  So với `AF_UNIX`, một socket `AF_INET` cần hai thông tin gì để xác định một địa chỉ duy nhất trên mạng?
2.  Vấn đề "Byte Ordering" là gì và tại sao nó quan trọng trong lập trình mạng? Hàm `htons()` có chức năng gì và được dùng cho loại dữ liệu nào?
3.  Trong code server, tại sao chúng ta thường `bind` với địa chỉ `INADDR_ANY` thay vì một địa chỉ IP cụ thể như "192.168.1.10"?
4.  Điểm khác biệt chính trong các trường dữ liệu của `struct sockaddr_un` (Module 1) và `struct sockaddr_in` (Module 2) là gì?

#### 💻 **Bài tập Thực hành**

1.  **Bài 1 (Dễ): Client linh hoạt.**

      * Sửa đổi `client_inet.cpp` để nhận địa chỉ IP của server từ tham số dòng lệnh (`argv[1]`) thay vì hardcode chuỗi "127.0.0.1".

2.  **Bài 2 (Trung bình): Server thông tin hệ thống.**

      * Sửa đổi `server_inet.cpp` để khi có client kết nối, server sẽ chạy lệnh `uname -a` (sử dụng `popen()`), đọc kết quả trả về từ lệnh đó và gửi chuỗi kết quả cho client.
      * Client chỉ việc nhận và in chuỗi này ra màn hình.

3.  **Bài 3 (Khó): Server tra cứu từ điển.**

      * Tạo một file `dictionary.txt` có dạng:
        ```
        hello:xin chao
        world:the gioi
        socket:o cam
        ```
      * Server khi khởi động sẽ đọc file này và lưu vào một `std::map<std::string, std::string>`.
      * Client gửi một từ tiếng Anh (ví dụ: "socket").
      * Server nhận từ, tra cứu trong map và gửi lại định nghĩa tiếng Việt tương ứng. Nếu không tìm thấy, server gửi lại thông báo "Word not found".

-----
