## 🧱 1. Khởi tạo và cấu hình socket

| API            | Mô tả |
|----------------|------|
| `socket()`     | Tạo một socket mới |
| `bind()`       | Gán địa chỉ (IP/port hoặc file path) cho socket |
| `listen()`     | Đặt socket vào trạng thái lắng nghe (server) |
| `connect()`    | Kết nối đến địa chỉ từ client |
| `accept()`     | Chấp nhận kết nối từ client (server) |
| `getsockname()`| Lấy địa chỉ gán cho socket |
| `getpeername()`| Lấy địa chỉ của peer đã kết nối |

---

## 📡 2. Gửi và nhận dữ liệu

| API              | Mô tả |
|------------------|------|
| `send()`         | Gửi dữ liệu qua socket |
| `recv()`         | Nhận dữ liệu từ socket |
| `sendto()`       | Gửi dữ liệu đến địa chỉ cụ thể (UDP) |
| `recvfrom()`     | Nhận dữ liệu từ địa chỉ cụ thể (UDP) |
| `write()` / `read()` | Dùng được với socket như file descriptor |

---

## 🔧 3. Cấu hình socket nâng cao

| API               | Mô tả |
|-------------------|------|
| `setsockopt()`    | Thiết lập tùy chọn cho socket (timeout, reuse, v.v.) |
| `getsockopt()`    | Lấy giá trị tùy chọn hiện tại |
| `fcntl()`         | Thiết lập chế độ non-blocking, FD flags |
| `ioctl()`         | Điều khiển thiết bị mạng (ít dùng hơn) |

---

## 🔁 4. Đa luồng / đa kết nối

| API              | Mô tả |
|------------------|------|
| `select()`       | Chờ nhiều socket cùng lúc (I/O multiplexing) |
| `poll()`         | Giống `select()` nhưng linh hoạt hơn |
| `epoll_*()`      | Cơ chế hiệu năng cao cho server lớn |
| `fork()` / `pthread_create()` | Tạo tiến trình hoặc luồng để xử lý socket song song |

---

## 🧩 5. Đóng và dọn dẹp

| API            | Mô tả |
|----------------|------|
| `shutdown()`   | Đóng một chiều hoặc cả hai chiều của socket |
| `close()`      | Đóng hoàn toàn socket |
| `unlink()`     | Xóa file socket (AF_UNIX) khỏi hệ thống |

---

## 🧠 6. Các struct và hằng số quan trọng

| Thành phần         | Mô tả |
|--------------------|------|
| `sockaddr_in`      | Địa chỉ IPv4 |
| `sockaddr_in6`     | Địa chỉ IPv6 |
| `sockaddr_un`      | Địa chỉ UNIX domain socket |
| `AF_INET`, `AF_UNIX`, `AF_INET6` | Loại socket |
| `SOCK_STREAM`, `SOCK_DGRAM`      | Kiểu kết nối (TCP/UDP) |
| `SOL_SOCKET`, `SO_REUSEADDR`, `SO_RCVTIMEO` | Tùy chọn socket |

---

## 🔐 7. Bảo mật và quyền truy cập

| API / kỹ thuật     | Mô tả |
|--------------------|------|
| `chmod()`          | Đặt quyền truy cập cho file socket (AF_UNIX) |
| `SO_PEERCRED`      | Lấy UID/GID của tiến trình peer |
| `getsockopt()`     | Dùng để truy xuất thông tin bảo mật |

---

## 📦 8. Các biến thể socket đặc biệt

| Loại socket        | Mô tả |
|--------------------|------|
| `AF_PACKET`        | Raw socket tầng thấp (MAC layer) |
| `AF_NETLINK`       | Kernel ↔ Userspace IPC |
| `AF_BLUETOOTH`     | Giao tiếp Bluetooth |
| `AF_CAN`           | Giao tiếp mạng CAN (ô tô, công nghiệp) |

---

## 🧱 1. Tạo và cấu hình socket

### 🔧 `int socket(int domain, int type, int protocol);`

| Tham số      | Ý nghĩa |
|--------------|--------|
| `domain`     | Loại socket: `AF_INET`, `AF_UNIX`, `AF_INET6`, v.v. |
| `type`       | Kiểu kết nối: `SOCK_STREAM` (TCP), `SOCK_DGRAM` (UDP), `SOCK_RAW` |
| `protocol`   | Giao thức cụ thể, thường là `0` (mặc định theo `domain` + `type`) |

---

### 🧷 `int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);`

| Tham số      | Ý nghĩa |
|--------------|--------|
| `sockfd`     | FD của socket (trả về từ `socket()`) |
| `addr`       | Cấu trúc địa chỉ gán vào socket (`sockaddr_in` hoặc `sockaddr_un`) |
| `addrlen`    | Kích thước của cấu trúc địa chỉ |

➡️ Gắn địa chỉ cho socket — bắt buộc với server

---

### 🎧 `int listen(int sockfd, int backlog);`

| Tham số      | Ý nghĩa |
|--------------|--------|
| `sockfd`     | Socket FD vừa được `bind()` |
| `backlog`    | Số lượng tối đa kết nối chờ trong hàng đợi |

➡️ Đưa socket vào trạng thái lắng nghe (chỉ dùng cho `SOCK_STREAM`)

---

### ☎️ `int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);`

| Tham số      | Ý nghĩa |
|--------------|--------|
| `sockfd`     | FD đang lắng nghe |
| `addr`       | Trả về địa chỉ của client kết nối |
| `addrlen`    | Trả về kích thước của `addr` |

➡️ Server gọi khi có kết nối, trả về FD giao tiếp riêng

---

### 📞 `int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);`

| Tham số      | Ý nghĩa |
|--------------|--------|
| `sockfd`     | FD của client socket |
| `addr`       | Địa chỉ server cần kết nối |
| `addrlen`    | Kích thước địa chỉ |

---

## 📡 2. Giao tiếp dữ liệu

### 📤 `ssize_t send(int sockfd, const void *buf, size_t len, int flags);`

| Tham số   | Ý nghĩa |
|-----------|--------|
| `sockfd`  | FD đã kết nối |
| `buf`     | Dữ liệu gửi đi |
| `len`     | Số byte gửi |
| `flags`   | Cờ (thường là `0`, có thể là `MSG_NOSIGNAL`, `MSG_DONTWAIT`) |

---

### 📥 `ssize_t recv(int sockfd, void *buf, size_t len, int flags);`

| Tham số   | Ý nghĩa |
|-----------|--------|
| `sockfd`  | FD đã kết nối |
| `buf`     | Bộ đệm nhận dữ liệu |
| `len`     | Kích thước bộ đệm |
| `flags`   | Cờ điều khiển nhận dữ liệu |

---

## 🔍 3. Socket option và cấu hình nâng cao

### ⚙️ `int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);`

| Tham số      | Ý nghĩa |
|--------------|--------|
| `sockfd`     | FD của socket |
| `level`      | Tầng cấu hình (`SOL_SOCKET`, `IPPROTO_TCP`, ...) |
| `optname`    | Tên tùy chọn (`SO_REUSEADDR`, `SO_RCVTIMEO`, ...) |
| `optval`     | Giá trị tùy chọn |
| `optlen`     | Kích thước giá trị tùy chọn |

➡️ Ví dụ: Cho phép reuse port, đặt timeout đọc, v.v.

---

## 🔁 4. I/O multiplexing

### 🔄 `int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);`

| Tham số      | Ý nghĩa |
|--------------|--------|
| `nfds`       | FD lớn nhất + 1 |
| `readfds`    | Danh sách FD kiểm tra có thể đọc |
| `writefds`   | Kiểm tra có thể ghi |
| `exceptfds`  | Kiểm tra ngoại lệ |
| `timeout`    | Timeout cho việc chờ |

➡️ Cho phép server xử lý nhiều kết nối cùng lúc

---

## 🧹 5. Đóng và kết thúc kết nối

### 🔌 `int shutdown(int sockfd, int how);`

| Tham số      | Ý nghĩa |
|--------------|--------|
| `sockfd`     | FD đã kết nối |
| `how`        | Loại shutdown: `SHUT_RD`, `SHUT_WR`, `SHUT_RDWR` |

➡️ Dừng một chiều hoặc cả hai chiều của socket

---

### ❌ `int close(int sockfd);`

- Đóng socket hoàn toàn
- Dọn dẹp FD, kernel resources

---

## 📦 6. Các struct địa chỉ phổ biến

### 🌐 `struct sockaddr_in` — IPv4

```cpp
struct sockaddr_in {
  sa_family_t sin_family; // AF_INET
  uint16_t sin_port;      // Port (network byte order)
  struct in_addr sin_addr; // IP address
};
```

➡️ Dùng cho TCP/UDP qua mạng

---

### 🏠 `struct sockaddr_un` — UNIX Domain

```cpp
struct sockaddr_un {
  sa_family_t sun_family; // AF_UNIX
  char sun_path[108];     // Đường dẫn file socket
};
```

➡️ Dùng cho giao tiếp nội bộ máy

---

## 🎯 Kết luận

Socket API là xương sống của IPC và giao tiếp mạng trong Linux. Nếu Anh nắm được các hàm trên, kèm cách cấu hình `struct`, `FD`, `select()` hay `setsockopt()`, thì việc viết server/client hay module system không còn là vấn đề nữa 😎

---
