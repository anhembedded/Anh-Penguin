### **Module 4: Giao tiếp không kết nối với Datagrams (UDP) 🚤**

**Mục tiêu**: Hiểu rõ về giao thức UDP và cách sử dụng socket `SOCK_DGRAM`. Anh sẽ học cách gửi và nhận các gói tin độc lập (datagrams) mà không cần thiết lập kết nối, đồng thời nhận thức được những ưu và nhược điểm của mô hình này.

#### 📖 **Lý thuyết chi tiết**

  * **Socket `SOCK_DGRAM` (UDP - User Datagram Protocol)**

      * Đây là sự đối lập hoàn toàn với `SOCK_STREAM` (TCP) mà chúng ta đã dùng. Hãy nghĩ về TCP như một cuộc gọi điện thoại (có kết nối, đảm bảo) và UDP như gửi một tấm bưu thiếp (không kết nối, không đảm bảo).
      * **Đặc điểm "3 Không"**:
        1.  **Không kết nối (Connectionless)**: Không có quá trình "bắt tay" `connect()` / `accept()` dài dòng. Mỗi gói tin (datagram) được gửi đi là một thực thể độc lập, tự nó chứa địa chỉ người nhận và được "phóng" vào mạng.
        2.  **Không tin cậy (Unreliable)**: Kernel và mạng không đảm bảo rằng gói tin sẽ đến nơi. Nó có thể bị mất dọc đường, và tầng ứng dụng sẽ không nhận được thông báo lỗi nào.
        3.  **Không đúng thứ tự (Unordered)**: Nếu anh gửi gói A rồi đến gói B, không có gì đảm bảo rằng bên nhận sẽ nhận được A trước B.

  * **Vậy tại sao lại dùng UDP?**

      * **Tốc độ (Fast)**: Không có độ trễ của việc thiết lập và duy trì kết nối. Dữ liệu được gửi đi ngay lập tức.
      * **Gọn nhẹ (Lightweight)**: Phần header của gói tin UDP nhỏ hơn nhiều so với TCP, giúp tiết kiệm băng thông.
      * **Hỗ trợ Broadcast/Multicast**: UDP cho phép gửi một gói tin đến tất cả các máy trong mạng LAN (broadcast) hoặc một nhóm máy cụ thể (multicast) một cách dễ dàng.

  * **API mới cho Datagrams**

      * `socket()`: Tham số `type` bây giờ là `SOCK_DGRAM`.
      * `bind()`: Server vẫn dùng `bind()` để "nghe" trên một port cụ thể.
      * **`sendto()`**: Thay cho `write()`. Gửi một datagram đến một địa chỉ cụ thể.
        ```cpp
        sendto(socket, data, data_len, flags, (sockaddr*)&dest_addr, sizeof(dest_addr));
        ```
      * **`recvfrom()`**: Thay cho `read()`. Nhận một datagram và đồng thời cho biết địa chỉ của người gửi là ai.
        ```cpp
        recvfrom(socket, buffer, buffer_len, flags, (sockaddr*)&sender_addr, &sender_addr_len);
        ```

#### 💻 **Code mẫu (C++)**

Ví dụ này xây dựng một "Echo Server" đơn giản. Server nhận một tin nhắn, in ra và gửi chính tin nhắn đó ngược lại cho client.

**`server_udp.cpp`**

```cpp
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

const int PORT = 9999;
const int BUFFER_SIZE = 1024;

int main() {
    // 1. Tạo socket UDP
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) { perror("socket"); return 1; }

    // 2. Bind server vào một port
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(sock_fd, (const sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return 1;
    }

    std::cout << "[UDP Server] Waiting for datagrams on port " << PORT << std::endl;

    while (true) {
        char buffer[BUFFER_SIZE];
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // 3. Nhận datagram (block tại đây)
        ssize_t n_bytes = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (sockaddr*)&client_addr, &client_len);
        if (n_bytes < 0) {
            perror("recvfrom");
            continue;
        }
        buffer[n_bytes] = '\0'; // Đảm bảo chuỗi kết thúc null

        std::cout << "[UDP Server] Received " << n_bytes << " bytes from " 
                  << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port)
                  << " - Message: " << buffer << std::endl;
        
        // 4. Gửi phản hồi (echo) lại đúng địa chỉ client đó
        sendto(sock_fd, buffer, n_bytes, 0, (const sockaddr*)&client_addr, client_len);
    }

    close(sock_fd);
    return 0;
}
```

**`client_udp.cpp`**

```cpp
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

const int PORT = 9999;
const char* SERVER_IP = "127.0.0.1";
const int BUFFER_SIZE = 1024;

int main() {
    // 1. Tạo socket UDP
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) { perror("socket"); return 1; }

    // 2. Chuẩn bị địa chỉ server
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // 3. Gửi datagram mà không cần `connect()`
    const char* message = "Hello UDP Server!";
    sendto(sock_fd, message, strlen(message), 0, (const sockaddr*)&server_addr, sizeof(server_addr));
    std::cout << "[UDP Client] Sent message: " << message << std::endl;

    // 4. Nhận phản hồi
    char buffer[BUFFER_SIZE];
    ssize_t n_bytes = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    if (n_bytes < 0) {
        perror("recvfrom");
        return 1;
    }
    buffer[n_bytes] = '\0';
    std::cout << "[UDP Client] Received reply: " << buffer << std::endl;

    close(sock_fd);
    return 0;
}
```

#### 🧩 **Liên hệ Embedded Linux**

UDP cực kỳ hữu dụng trong thế giới nhúng vì sự gọn nhẹ và hiệu quả của nó.

  * **Phát hiện dịch vụ (Service Discovery)**: Một thiết bị nhúng khi vừa khởi động có thể gửi một gói tin UDP broadcast ra mạng LAN để thông báo "Tôi là camera IP, địa chỉ của tôi là 192.168.1.100". Các phần mềm trên máy tính có thể lắng nghe gói tin này để tự động tìm thấy thiết bị mà không cần người dùng nhập IP thủ công.
  * **Streaming cảm biến**: Một cảm biến nhiệt độ có thể gửi nhiệt độ hiện tại mỗi 500ms qua UDP đến một máy chủ. Nếu một gói tin bị mất, không sao cả, vì nửa giây sau gói tin mới nhất sẽ đến. Việc này tiết kiệm tài nguyên hơn nhiều so với việc duy trì một kết nối TCP liên tục.
  * **Gửi Log**: Gửi các thông điệp log có độ ưu tiên thấp (ví dụ: DEBUG, INFO) đến một server trung tâm. Nó nhanh và không làm chậm ứng dụng chính. Nếu mất một vài log DEBUG cũng không phải là thảm họa.

-----

#### ❓ **Câu hỏi Ôn tập**

1.  Nêu ba đặc điểm chính của UDP (`SOCK_DGRAM`) khiến nó khác biệt với TCP (`SOCK_STREAM`).
2.  Tại sao UDP được gọi là "connectionless" (không kết nối)? Cặp hàm nào thay thế cho `connect()`/`accept()` và `read()`/`write()` trong giao tiếp UDP cơ bản?
3.  Trong `server_udp.cpp`, làm thế nào server biết được địa chỉ để gửi phản hồi lại cho client?
4.  Hãy nêu một ví dụ ứng dụng thực tế mà UDP là lựa chọn tốt hơn TCP và giải thích tại sao.

#### 💻 **Bài tập Thực hành**

1.  **Bài 1 (Dễ): Time Server.**

      * Tạo một UDP server. Khi nhận được bất kỳ datagram nào, server sẽ lấy thời gian hệ thống hiện tại, định dạng nó thành một chuỗi (sử dụng `ctime()` hoặc `strftime()`), và gửi chuỗi đó lại cho client. Client chỉ việc in ra thời gian nhận được.

2.  **Bài 2 (Trung bình): Broadcast Client.**

      * Tạo một client gửi một datagram đến địa chỉ broadcast của mạng LAN (ví dụ: `192.168.1.255`).
      * Anh sẽ cần bật một option cho socket để cho phép broadcast: `int broadcast = 1; setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));`.
      * Chạy nhiều server trên các máy khác nhau trong cùng mạng LAN để xem tất cả chúng có nhận được tin nhắn không.

3.  **Bài 3 (Khó): Giao thức tin cậy đơn giản trên UDP.**

      * Sửa đổi cặp client/server. Client gửi một tin nhắn có kèm theo một số thứ tự (sequence number) ở đầu.
      * Server sau khi nhận sẽ gửi lại một gói tin "ACK" (acknowledgement) cũng kèm theo số thứ tự đó.
      * Client sẽ đặt một timeout cho việc nhận (sử dụng `setsockopt` với `SO_RCVTIMEO`). Nếu không nhận được ACK trong khoảng thời gian timeout, nó sẽ gửi lại tin nhắn. (Đây là bài tập mô phỏng lại một phần nhỏ của cách TCP hoạt động).

-----
