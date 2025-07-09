# **Module 1: Giới thiệu Sockets và Giao tiếp Local (AF\_UNIX) 🏠**

    **Mục tiêu**: Module này là bước khởi đầu, giúp anh làm quen với API của Sockets trong một môi trường đơn giản nhất, không bị phân tâm bởi các chi tiết mạng phức tạp như địa chỉ IP hay port
---

## 📖 **Lý thuyết chi tiết**

### 🔌 Socket là gì? (Giải thích như người thật việc thật)

Một **Socket** là "lỗ giao tiếp" giữa hai tiến trình — giống như ổ cắm điện: nếu hai bên cắm đúng vào nhau thì truyền dữ liệu được.

### 🧠 Tính chất

- Là **endpoint của kết nối hai chiều** (nghĩa là vừa gửi vừa nhận được)
- Có thể hoạt động:
  - **Trong nội bộ hệ thống** → ví dụ daemon ↔ GUI
  - **Qua mạng TCP/IP** → ví dụ điện thoại ↔ server cloud

### 🔄 So với IPC khác

| Loại            | Chỉ nội bộ máy | Giao tiếp qua mạng | Hai chiều? |
|-----------------|----------------|---------------------|------------|
| Pipe/FIFO       | ✅              | ❌                  | Một chiều  |
| Shared Memory   | ✅              | ❌                  | Phải tự đồng bộ |
| Message Queue   | ✅              | ❌                  | Hai chiều (dữ liệu thô) |
| **Socket**      | ✅              | ✅                  | ✅          |

---

## 🧩 Mô hình Client - Server qua Socket

### 📌 Các vai

| Thành phần | Hành vi |
|------------|---------|
| **Server** | Lắng nghe, tạo đường truyền |
| **Client** | Khởi tạo kết nối, yêu cầu dịch vụ |

> Nghĩ như tiệm trà sữa (server): chờ khách tới order.  
> Khách (client) chủ động tới gọi món.

---

### 📞 Nghi lễ kết nối — “The Socket Ceremony”

### 💻 Server

1. `socket()` → Tạo socket endpoint
2. `bind()` → Gắn địa chỉ (IP + port hoặc tên file nếu nội bộ)
3. `listen()` → Sẵn sàng nhận kết nối, tạo queue
4. `accept()` → Khi có client gọi đến, tạo đường dây riêng

### 📱 Client

1. `socket()` → Tạo socket
2. `connect()` → Gọi đến địa chỉ server đã `bind`

---

## 📦 Ví dụ ứng dụng thực tế

| Tình huống | Loại socket dùng |
|------------|------------------|
| Remote SSH | TCP socket (`AF_INET`) |
| Chat nội bộ máy | UNIX socket (`AF_UNIX`) |
| Service web | TCP socket (HTTP/WebSocket) |
| App ↔ Daemon Linux | UNIX domain socket |

---

## 🧠 Mở rộng: Các loại Socket

| Kiểu Domain     | Mô tả                                              |
|-----------------|----------------------------------------------------|
| `AF_INET`       | Socket TCP/UDP qua mạng                            |
| `AF_INET6`      | Giống như trên nhưng hỗ trợ IPv6                   |
| `AF_UNIX` / `AF_LOCAL` | Giao tiếp nội bộ máy dùng đường dẫn file       |
| `AF_PACKET`     | Raw socket tầng thấp (cấp MAC) — dùng cho sniffer  |
| `AF_NETLINK`    | Kernel ↔ Userspace (thường dùng với Linux subsystem) |

---

## 🛡️ AF_UNIX Socket – Giao tiếp “nội bộ cực nhanh”

- Giao tiếp giữa tiến trình trong cùng máy
- Dùng path như `/tmp/sock_daemon`
- Không qua network stack → giảm latency, tiết kiệm CPU

### 📦 Dùng khi nào?

- Embedded Linux: app ↔ background daemon
- App muốn nói chuyện với system service mà không qua TCP

---

## 🔧 Demo code

- Một **UNIX socket server** trong C/C++
- Một **client** gửi thông điệp như `"Sensor ON"` → nhận lại `"ACK"`

Hoặc Anh muốn chuyển sang **Socket trong Python**, **gRPC**, hoặc build flow chart mô hình `accept()` dùng `select()` để scale? Nói phát là quẩy liền 😎

### 💻 **Code mẫu (C++)**

Chơi lớn rồi Anh ơi 😎! Đây là hướng dẫn **từng bước một**, thật kỹ càng, để hiểu và mở rộng chương trình Client/Server qua UNIX Domain Socket trong C++ — một loại IPC cực nhanh và gọn gàng khi giao tiếp nội bộ hệ thống.

---

#### 🎯 Mục tiêu

Xây dựng 2 tiến trình:

- **Server**: mở socket tại đường dẫn `/tmp/my_app.socket`, lắng nghe kết nối
- **Client**: kết nối đến socket đó, gửi tin nhắn, nhận phản hồi

---

#### ✨ Bước 1: Hiểu nền tảng UNIX Domain Socket

- Là **socket nội bộ máy**: không qua TCP/IP
- Dùng `AF_UNIX` làm domain
- Đường dẫn socket là một **file đặc biệt** trong hệ thống Linux (`/tmp/...`)
- Giao tiếp kiểu **SOCK_STREAM** (như TCP): tin cậy, hai chiều

➡️ Cơ chế này hoạt động tốt giữa app ↔ daemon, module ↔ service nội bộ máy

---

#### 🛠️ Bước 2: Viết Server từng bước

##### ➊ Dọn socket cũ (nếu có) → tránh lỗi “Address already in use”

```cpp
unlink(SOCKET_PATH);
```

##### ➋ Tạo socket

```cpp
int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
```

- `AF_UNIX`: dùng địa chỉ file
- `SOCK_STREAM`: kết nối đáng tin cậy
- `0`: chọn giao thức mặc định (usually zero)

##### ➌ Bind socket tới đường dẫn file

```cpp
sockaddr_un server_addr;
server_addr.sun_family = AF_UNIX;
strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr));
```

➡️ Điều này gắn socket vào file `/tmp/my_app.socket` trên hệ thống

##### ➍ Lắng nghe kết nối

```cpp
listen(server_fd, 5); // tối đa 5 kết nối chờ
```

##### ➎ Chấp nhận kết nối từ client

```cpp
int client_fd = accept(server_fd, NULL, NULL);
```

- Lệnh này sẽ **block** cho đến khi client kết nối

##### ➏ Giao tiếp

```cpp
read(client_fd, buffer, sizeof(buffer));
write(client_fd, "Hello from Server!", 19);
```

##### ➐ Dọn dẹp tài nguyên

```cpp
close(client_fd);
close(server_fd);
unlink(SOCKET_PATH);
```

---

#### 🤖 Bước 3: Viết Client từng bước

##### ➊ Tạo socket

```cpp
int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
```

##### ➋ Chuẩn bị địa chỉ của server

```cpp
sockaddr_un server_addr;
server_addr.sun_family = AF_UNIX;
strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
```

##### ➌ Kết nối tới server

```cpp
connect(client_fd, (sockaddr*)&server_addr, sizeof(server_addr));
```

##### ➍ Giao tiếp

```cpp
write(client_fd, "Hello from Client!", 19);
read(client_fd, buffer, sizeof(buffer));
```

##### ➎ Đóng kết nối

```cpp
close(client_fd);
```

---

## 🔎 Mở rộng kiến thức

### 💬 Giao tiếp lặp lại (loop)

- Có thể bọc đoạn đọc/ghi trong vòng lặp để duy trì kết nối lâu hơn
- Server có thể xử lý nhiều client bằng `select()` hoặc `fork()`

### 🧩 Truyền Struct thay vì char*

- Serialize struct bằng `memcpy()` hoặc `std::span`
- Đảm bảo tương thích cấu trúc khi truyền giữa các tiến trình

### 🔐 Phân quyền truy cập socket

- Đặt permission: `chmod("/tmp/my_app.socket", 0600);`
- Giới hạn user được phép giao tiếp với daemon

### 🧠 Dọn socket đúng cách

- Nếu server crash, file socket vẫn tồn tại → `unlink()` rất cần!
- Có thể dùng `atexit()` hoặc signal handler để đảm bảo cleanup

---

## 🔬 So sánh với TCP Socket

| Tiêu chí        | UNIX Socket       | TCP Socket        |
|------------------|-------------------|-------------------|
| Phạm vi           | Nội bộ máy        | Qua mạng          |
| Hiệu năng         | Rất cao           | Có overhead mạng  |
| Giao tiếp         | AF_UNIX + file    | AF_INET + IP:Port |
| Dùng khi nào?     | App ↔ daemon, local API | Remote service, distributed system |

---

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

- **Giao tiếp giữa các Daemons**: Trong một thiết bị nhúng, anh có thể có một `log_daemon` (dịch vụ ghi log) và một `app_daemon` (dịch vụ ứng dụng chính). `app_daemon` có thể gửi các bản tin log đến `log_daemon` thông qua một `AF_UNIX` socket. Nó nhanh, hiệu quả và không yêu cầu cấu hình mạng phức tạp.
- **API cho các tiến trình User-space**: Một driver hoặc một dịch vụ hệ thống chạy với quyền root có thể mở một `AF_UNIX` socket để các ứng dụng user-space không có quyền cao có thể giao tiếp và yêu cầu dịch vụ một cách an toàn, thay vì phải dùng các cơ chế phức tạp hơn như `ioctl`.

---

Khi nào anh thấy ổn với nội dung của Module 1, hãy cho tôi biết để chúng ta tiếp tục với **Module 2: Bước ra Mạng với Sockets AF\_INET (TCP/IP)** nhé.

#### ❓ **Câu hỏi Ôn tập**

1. Đặc điểm nào làm cho Sockets khác biệt cơ bản so với các cơ chế IPC khác như Pipes hay Shared Memory?
2. Mô tả chuỗi 4 system call chính mà một tiến trình Server phải thực hiện để sẵn sàng nhận kết nối. Nêu ngắn gọn mục đích của từng hàm.
3. Trong ví dụ về `AF_UNIX` socket, tại sao lại có lệnh `unlink(SOCKET_PATH)` ở đầu và cuối chương trình `server_unix.cpp`? Điều gì sẽ xảy ra nếu thiếu nó?
4. Tại sao hàm `accept()` lại trả về một file descriptor **mới** (`client_fd`) thay vì sử dụng luôn file descriptor ban đầu (`server_fd`) để giao tiếp?

#### 💻 **Bài tập Thực hành**

1. **Bài 1 (Dễ): "Echo" Server.**

   - Sửa đổi `server_unix.cpp` để sau khi nhận được tin nhắn từ client, nó sẽ gửi lại **chính xác** tin nhắn đó cho client.
   - Sửa đổi `client_unix.cpp` để nó gửi một chuỗi do người dùng nhập từ bàn phím.
2. **Bài 2 (Trung bình): Server Tính toán.**

   - Client sẽ gửi một chuỗi có dạng `"ADD 5 10"` hoặc `"MUL 3 4"`.
   - Server nhận chuỗi, phân tích cú pháp để nhận diện lệnh (`ADD`/`MUL`) và các toán hạng.
   - Server thực hiện phép tính và gửi lại kết quả (dạng chuỗi) cho Client. Ví dụ: `"Result: 15"`.
3. **Bài 3 (Khó): Trao đổi cấu trúc.**

   - Định nghĩa một `struct` chung, ví dụ `struct SystemInfo { int cpu_usage; int mem_free; };`.
   - Sửa đổi Client để gửi `struct` này đến Server (lưu ý về `sizeof`).
   - Server nhận `struct`, in các trường dữ liệu ra màn hình, và gửi lại một thông báo xác nhận.
