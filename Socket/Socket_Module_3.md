
### **Module 3: Xử lý nhiều Client đồng thời 👨‍👩‍👧‍👦**

**Mục tiêu**: Nâng cấp server từ Module 2 để có khả năng phục vụ nhiều client cùng một lúc. Anh sẽ học và so sánh hai kiến trúc kinh điển: tạo tiến trình mới cho mỗi client (`fork`) và giám sát I/O trên nhiều kênh (`select`).

#### 📖 **Lý thuyết chi tiết**

  * **Vấn đề của Server tuần tự (Sequential Server)**

      * Server ở Module 2 là một server tuần tự. Luồng hoạt động của nó là: `accept()` -\> `read()` -\> `write()` -\> `close()`.
      * Các hàm `accept()` và `read()` đều là **blocking calls** (lệnh chặn).
      * Khi server đang `read()` dữ liệu từ Client A, nó không thể quay lại `accept()` để chấp nhận kết nối từ Client B. Client B sẽ bị "treo" trong hàng đợi cho đến khi Client A được phục vụ xong. Đây là một điểm yếu chí mạng.

  * **Cách 1: `fork()` cho mỗi Client (Mô hình Đa tiến trình)**

      * **Logic**: Rất trực quan. Mỗi khi có một client mới, server sẽ "nhân bản" chính nó ra để tạo một "nhân viên" riêng phục vụ client đó.
      * **Quy trình**:
        1.  Vòng lặp chính của server chỉ làm một việc: gọi `accept()` để chờ client mới.
        2.  Ngay khi `accept()` thành công và trả về `client_fd`, server gọi `fork()`.
        3.  **Trong tiến trình con (Child Process)**:
              * Nó được thừa hưởng một bản sao của `client_fd`. Nhiệm vụ của nó là giao tiếp với client này.
              * Nó không cần đến `server_fd` (socket lắng nghe), vì vậy nó nên `close(server_fd)`.
              * Sau khi phục vụ xong, tiến trình con gọi `exit()`.
        4.  **Trong tiến trình cha (Parent Process)**:
              * Nhiệm vụ của nó là quay lại `accept()` càng nhanh càng tốt.
              * Nó không cần `client_fd` (vì con đã lo), vì vậy nó nên `close(client_fd)`.
      * **Vấn đề Zombie Process**: Khi một tiến trình con kết thúc, nó sẽ trở thành "zombie" nếu tiến trình cha không gọi `wait()` để thu dọn nó. Trong mô hình này, cha không thể `wait()` vì sẽ bị block.
      * **Giải pháp**: Dùng `signal(SIGCHLD, SIG_IGN);`. Lệnh này báo cho kernel rằng "tôi không quan tâm đến trạng thái kết thúc của các con tôi, ngài cứ tự động dọn dẹp chúng đi".
      * **Ưu/Nhược điểm**:
          * 👍 **Ưu điểm**: Logic đơn giản, dễ triển khai. Các client được xử lý độc lập, nếu một "nhân viên" bị crash cũng không ảnh hưởng đến những người khác.
          * 👎 **Nhược điểm**: Tốn tài nguyên. Việc tạo một tiến trình mới rất tốn kém về bộ nhớ và thời gian CPU. Khó chia sẻ trạng thái giữa các client.

  * **Cách 2: `select()` (Mô hình I/O Multiplexing)**

      * **Logic**: Thay vì tạo nhiều "nhân viên", chúng ta có một "siêu nhân viên" có khả năng theo dõi nhiều đường dây điện thoại cùng lúc.
      * **Quy trình**:
        1.  Server duy trì một **tập hợp (set)** các file descriptor (fd) cần theo dõi, bao gồm cả `server_fd` và tất cả các `client_fd` đã kết nối.
        2.  Server dùng hàm `select()` để yêu cầu kernel: "Hãy chặn tôi cho đến khi có hoạt động (dữ liệu đến) trên bất kỳ fd nào trong tập hợp này".
        3.  Khi `select()` trả về (tức là có hoạt động), server sẽ duyệt qua tập hợp để xem fd nào "sáng đèn":
              * Nếu là `server_fd`: Có client mới kết nối -\> `accept()` và thêm `client_fd` mới vào tập hợp cần theo dõi.
              * Nếu là một `client_fd`: Client cũ gửi dữ liệu -\> `read()` dữ liệu và xử lý. Nếu `read()` trả về 0, tức là client đã ngắt kết nối, ta sẽ đóng fd và loại nó khỏi tập hợp.
      * **Các hàm thao tác với `fd_set`**:
          * `FD_ZERO(&set)`: Xóa sạch một set.
          * `FD_SET(fd, &set)`: Thêm một `fd` vào set.
          * `FD_CLR(fd, &set)`: Loại bỏ một `fd` khỏi set.
          * `FD_ISSET(fd, &set)`: Kiểm tra xem một `fd` có nằm trong set (có "sáng đèn") không.
      * **Ưu/Nhược điểm**:
          * 👍 **Ưu điểm**: **Hiệu năng cực cao**, chỉ dùng một tiến trình duy nhất. Tiết kiệm tài nguyên và có khả năng mở rộng để xử lý hàng nghìn kết nối. Đây là nền tảng của các server hiện đại như Nginx, Node.js.
          * 👎 **Nhược điểm**: Logic code phức tạp hơn vì phải tự quản lý trạng thái của tất cả các kết nối.

#### 💻 **Code mẫu (C++)**

Cả hai server sau đều có thể hoạt động với `client_inet.cpp` từ Module 2.

**`server_fork.cpp` (Sử dụng `fork()`)**

```cpp
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h> // Cho signal

const int PORT = 9999;

void handle_client(int client_fd);

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    
    bind(server_fd, (const sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);
    
    // Quan trọng: Tự động dọn dẹp các tiến trình con đã kết thúc
    signal(SIGCHLD, SIG_IGN); 

    std::cout << "[Server] Waiting for connections..." << std::endl;
    while(true) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) { continue; }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) { // Đây là tiến trình CON
            close(server_fd); // Con không cần socket lắng nghe
            handle_client(client_fd);
            exit(0);
        } else { // Đây là tiến trình CHA
            close(client_fd); // Cha không cần socket giao tiếp của client này
        }
    }
    close(server_fd);
    return 0;
}

void handle_client(int client_fd) {
    std::cout << "[Child " << getpid() << "] Handling new client." << std::endl;
    char buffer[256] = {0};
    read(client_fd, buffer, sizeof(buffer));
    std::cout << "[Child " << getpid() << "] Received: " << buffer << std::endl;
    sleep(5); // Giả lập công việc xử lý tốn thời gian
    write(client_fd, "Handled by a child process!", 28);
    close(client_fd);
    std::cout << "[Child " << getpid() << "] Finished handling client." << std::endl;
}
```

**`server_select.cpp` (Sử dụng `select()`)**

```cpp
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <vector>
#include <algorithm>

const int PORT = 9999;

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    
    bind(server_fd, (const sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    fd_set active_fds, read_fds;
    FD_ZERO(&active_fds);
    FD_SET(server_fd, &active_fds);
    int max_fd = server_fd;

    std::cout << "[Server] Waiting for activity..." << std::endl;
    while(true) {
        read_fds = active_fds;
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select"); return 1;
        }

        for (int i = 0; i <= max_fd; ++i) {
            if (FD_ISSET(i, &read_fds)) { // Tìm fd có hoạt động
                if (i == server_fd) { // 1. Kết nối mới
                    int client_fd = accept(server_fd, NULL, NULL);
                    FD_SET(client_fd, &active_fds);
                    if (client_fd > max_fd) max_fd = client_fd;
                    std::cout << "Accepted new client on fd " << client_fd << std::endl;
                } else { // 2. Dữ liệu từ client cũ
                    char buffer[256] = {0};
                    int n_bytes = read(i, buffer, sizeof(buffer));
                    if (n_bytes <= 0) { // Client ngắt kết nối hoặc lỗi
                        std::cout << "Client on fd " << i << " disconnected." << std::endl;
                        close(i);
                        FD_CLR(i, &active_fds);
                    } else {
                        std::cout << "Received from fd " << i << ": " << buffer << std::endl;
                        write(i, "Server got your message", 24);
                    }
                }
            }
        }
    }
    return 0;
}
```

#### 🧩 **Liên hệ Embedded Linux**

  * **Mô hình `fork()`**: Thích hợp cho các dịch vụ không yêu cầu nhiều kết nối đồng thời và cần sự đơn giản, ví dụ một giao diện `telnet` đơn giản để debug trên thiết bị. Việc `fork` có thể quá nặng cho các MCU cấu hình thấp.
  * **Mô hình `select()`/`poll()`/`epoll()`**: **Bắt buộc** phải sử dụng cho các thiết bị nhúng hiện đại cần xử lý nhiều luồng dữ liệu. Ví dụ, một Gateway IoT cần phải:
      * Nhận dữ liệu từ các cảm biến qua TCP.
      * Nhận lệnh điều khiển từ cloud server qua một kết nối TCP khác.
      * Cung cấp một giao diện web cấu hình qua HTTP (cũng là TCP).
      * `select()` (và các biến thể hiệu năng cao hơn của nó như `epoll`) cho phép một tiến trình duy nhất xử lý tất cả các kênh I/O này một cách hiệu quả, không bị block, tiết kiệm tối đa tài nguyên hệ thống.

-----

#### ❓ **Câu hỏi Ôn tập**

1.  Đâu là hạn chế chính của server tuần tự trong Module 2 khi có nhiều client muốn kết nối?
2.  Trong mô hình `fork()`, tại sao tiến trình cha phải `close(client_fd)` và tiến trình con phải `close(server_fd)`?
3.  "Zombie process" là gì? Lệnh `signal(SIGCHLD, SIG_IGN)` giải quyết vấn đề này như thế nào?
4.  Mô hình I/O Multiplexing (dùng `select`) giải quyết bài toán đa client như thế nào và ưu điểm lớn nhất của nó so với mô hình `fork()` là gì?

#### 💻 **Bài tập Thực hành**

1.  **Bài 1 (`fork` model): Server bền bỉ.**

      * Sửa đổi hàm `handle_client` trong `server_fork.cpp`. Thay vì chỉ đọc/ghi một lần rồi thoát, hãy tạo một vòng lặp `while` để liên tục đọc và phản hồi client cho đến khi client gửi tin nhắn là "quit".

2.  **Bài 2 (`select` model): Server phát thanh (Broadcast).**

      * Sửa đổi `server_select.cpp`. Khi server nhận được tin nhắn từ một client, thay vì chỉ trả lời lại client đó, hãy gửi tin nhắn này tới **tất cả các client khác** đang kết nối. (Gợi ý: anh sẽ cần một cách để lưu trữ tất cả `client_fd` đang hoạt động, ví dụ như trong một `std::vector`).

3.  **Bài 3 (`select` model): Thêm Timeout.**

      * Sửa đổi `server_select.cpp`. Thay vì truyền `NULL` cho tham số timeout của `select`, hãy tạo một `struct timeval` và đặt timeout là 10 giây.
      * Nếu `select()` trả về 0 (tức là timeout), hãy in ra màn hình một thông báo như "Server is idle...". Lưu ý rằng anh phải khởi tạo lại `struct timeval` trong mỗi lần lặp.

-----
