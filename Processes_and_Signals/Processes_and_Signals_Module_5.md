# **Module 5: Luyện tập Tổng hợp & Ứng dụng 🧩**

#### **5.1. Ôn tập và Kết nối Kiến thức 🔗**

Hãy cùng điểm lại các chủ đề chính và mối liên hệ giữa chúng:

* **Module 1: Tiến trình là gì?**
  * **Kiến thức cốt lõi:** Định nghĩa tiến trình, cấu trúc (text, data, heap, stack), PID/PPID, bảng tiến trình, `ps`/`top`, tiến trình hệ thống (`init`/`systemd`), lập lịch và độ ưu tiên (`nice`).
  * **Kết nối:** Cung cấp nền tảng về cách các chương trình tồn tại và hoạt động trong Linux.
* **Module 2: Khởi động Tiến trình Mới**
  * **Kiến thức cốt lõi:** `system()` (chạy qua shell, chờ), `exec` family (thay thế ảnh tiến trình, không trả về), `fork()` (nhân bản tiến trình, tạo cha/con), mô hình `fork()` + `exec` (tạo tiến trình mới chạy chương trình khác), chuyển hướng I/O (`dup2`).
  * **Kết nối:** Các phương pháp để bắt đầu các tác vụ độc lập hoặc thay đổi chức năng của tiến trình hiện tại. `fork()` và `exec()` là cặp đôi vàng để tạo ra các tiến trình mới.
* **Module 3: Quản lý Tiến trình Con**
  * **Kiến thức cốt lõi:** `wait()`/`waitpid()` (chờ con, thu thập trạng thái, `WNOHANG` để không chặn), "Zombie Processes" (nguyên nhân và cách tránh: `wait()`, `SIG_IGN`, tiến trình mồ côi được `init` nhận nuôi).
  * **Kết nối:** Đảm bảo vòng đời của tiến trình con được quản lý đúng cách, ngăn chặn rò rỉ tài nguyên (bảng tiến trình) và giữ hệ thống ổn định.
* **Module 4: Signals (Tín hiệu)**
  * **Kiến thức cốt lõi:** Định nghĩa tín hiệu (thông báo không đồng bộ), các loại tín hiệu phổ biến (`SIGINT`, `SIGTERM`, `SIGKILL`, `SIGSEGV`, `SIGCHLD`), gửi tín hiệu (`kill`, `alarm`), xử lý tín hiệu (`signal` vs. `sigaction` - khuyến nghị), `sa_mask` (chặn tín hiệu trong handler), `sa_flags` (`SA_RESTART`, `SA_SIGINFO`), `sigsuspend` (tạm dừng an toàn).
  * **Kết nối:** Cơ chế chính để giao tiếp không đồng bộ giữa các tiến trình, xử lý lỗi cấp hệ thống, và điều khiển vòng đời của các daemon.

**Mối liên hệ tổng thể trong lập trình hệ thống nhúng:**

Trong môi trường nhúng, các khái niệm này thường được kết hợp để xây dựng các hệ thống mạnh mẽ và bền vững:

* **Daemonization:** Sử dụng `fork()` hai lần, `setsid()`, `chdir("/")`, `umask(0)`, `close()` các FD chuẩn để biến một chương trình thành daemon chạy nền.
* **Quản lý dịch vụ:** Một daemon chính có thể `fork()` và `exec()` các dịch vụ con. Nó sẽ `waitpid()` cho chúng hoặc bỏ qua `SIGCHLD` để tránh zombie.
* **Giao tiếp và điều khiển:** Các tiến trình có thể gửi `SIGTERM` để yêu cầu dừng duyên dáng, `SIGHUP` để tải lại cấu hình, hoặc `SIGUSR1`/`SIGUSR2` cho các sự kiện tùy chỉnh.
* **Phản ứng với lỗi:** Cài đặt `sigaction()` cho `SIGSEGV` để ghi log debug hoặc thực hiện dọn dẹp trước khi crash.
* **Tối ưu tài nguyên:** Giới hạn số lượng tiến trình con, quản lý zombie, và sử dụng `nice` để điều chỉnh độ ưu tiên.

---

#### **5.2. Câu hỏi Tổng hợp và Tình huống ❓**

1. Tình huống (Thiết kế Daemon Phản hồi):
   Bạn cần thiết kế một daemon cho một thiết bị IoT chạy Linux. Daemon này sẽ lắng nghe các lệnh từ mạng và thực hiện các tác vụ dài (ví dụ: cập nhật firmware, thu thập dữ liệu lớn). Daemon cần:

   * Chạy nền vĩnh viễn.
   * Phản hồi nhanh chóng với yêu cầu dừng (tắt máy duyên dáng).
   * Tải lại cấu hình mà không cần khởi động lại khi có thay đổi.
   * Có thể khởi chạy các công cụ bên ngoài để thực hiện các tác vụ phụ trợ, nhưng không được bị chặn bởi chúng và phải đảm bảo các công cụ này không trở thành zombie.
   * Ghi log tất cả các sự kiện quan trọng.

   Hãy mô tả chi tiết các bước và hàm bạn sẽ sử dụng để triển khai daemon này, bao gồm cả cách bạn sẽ xử lý các tín hiệu và quản lý tiến trình con.
2. **Phân biệt và Ứng dụng:**

   * Giải thích sự khác biệt giữa việc một tiến trình bị `kill -9` và bị `kill` (mặc định `SIGTERM`). Điều gì xảy ra với `SIGCHLD` trong cả hai trường hợp nếu đó là tiến trình con?
   * Khi bạn chạy `my_app < input.txt | grep "keyword" > output.txt`, giải thích các System Call liên quan đến `fork()`, `exec()`, `pipe()`, và `dup2()` mà shell thực hiện để thiết lập pipeline này.
   * Bạn có một chương trình C++ cần khởi chạy một công cụ bên ngoài và đợi nó hoàn thành, nhưng không muốn chương trình chính bị treo nếu công cụ đó không bao giờ kết thúc. Bạn sẽ sử dụng `wait()` hay `waitpid()` với tùy chọn nào?
3. **Gỡ lỗi Tiến trình:**

   * Một ứng dụng của bạn tạo ra nhiều tiến trình con. Sau một thời gian, bạn thấy hệ thống chậm lại và lệnh `ps -al` hiển thị nhiều tiến trình con ở trạng thái `Z`.
     * Nguyên nhân của vấn đề này là gì?
     * Bạn sẽ sử dụng công cụ nào (từ giáo trình Debugging) để xác nhận các zombie này?
     * Bạn sẽ sửa code của mình như thế nào để ngăn chặn chúng? Nêu hai cách khác nhau.
4. **Tín hiệu và Tài nguyên:**

   * Giải thích tại sao việc sử dụng `sigsuspend()` để chờ tín hiệu lại hiệu quả hơn về mặt tài nguyên so với một vòng lặp `while(true) { sleep(1); }`.
   * Trong một chương trình đa luồng (POSIX Threads), khi một `SIGTERM` được gửi đến tiến trình, luồng nào sẽ nhận tín hiệu đó? Tại sao trình xử lý tín hiệu phải là `re-entrant`?

---

#### **5.3. Bài tập Thực hành Tổng hợp ✍️**

**Bài tập: Hệ thống Quản lý Dịch vụ Đơn giản (Simple Service Manager)**

Bạn sẽ xây dựng một công cụ dòng lệnh để quản lý các dịch vụ nền (background services) trên Linux. Hệ thống này sẽ bao gồm một chương trình quản lý (`service_manager`) và một chương trình dịch vụ mẫu (`mock_service`).

**Yêu cầu chương trình `service_manager.cpp`:**

1. **Xử lý Tham số Dòng lệnh:**
   * Sử dụng `getopt_long()` để xử lý các lệnh:
     * `./service_manager launch <service_name> <command_to_run...>`: Khởi chạy một dịch vụ.
     * `./service_manager stop <service_name>`: Dừng một dịch vụ.
     * `./service_manager status <service_name>`: Kiểm tra trạng thái của dịch vụ.
     * `./service_manager list`: Liệt kê tất cả các dịch vụ đang được quản lý.
     * `./service_manager help`: In ra hướng dẫn sử dụng.
   * **Lưu ý:** `command_to_run` có thể là một chuỗi lệnh có khoảng trắng, cần được xử lý đúng cách (ví dụ: `execvp` với mảng `char*[]`).
2. **Khởi chạy Dịch vụ (`launch`):**
   * Chương trình `service_manager` sẽ `fork()` hai lần để tạo một tiến trình con (cháu) trở thành daemon.
   * Tiến trình cháu sẽ:
     * Trở thành leader của phiên mới (`setsid()`).
     * Chuyển thư mục làm việc về `/` (`chdir("/")`).
     * Đặt `umask(0)`.
     * Đóng tất cả các file descriptor chuẩn (`stdin`, `stdout`, `stderr`).
     * Mở file log riêng cho dịch vụ (ví dụ: `/var/log/<service_name>.log`) ở chế độ append (`O_WRONLY | O_CREAT | O_APPEND`).
     * Chuyển hướng `stdout` và `stderr` của nó sang file log này bằng `dup2()`.
     * Ghi PID của nó vào một file PID (ví dụ: `/var/run/<service_name>.pid`).
     * Sử dụng `execvp()` để chạy `command_to_run`.
     * Nếu `execvp()` thất bại, ghi lỗi vào log và thoát.
   * Tiến trình `service_manager` gốc sẽ in ra thông báo "Service
3. **Dừng Dịch vụ (`stop`):**
   * Đọc PID của dịch vụ từ file PID (`/var/run/<service_name>.pid`).
   * Gửi `SIGTERM` đến PID đó.
   * Chờ tối đa 5 giây bằng `waitpid(PID, &status, 0)` để dịch vụ thoát.
   * Nếu dịch vụ không thoát sau 5 giây, gửi `SIGKILL`.
   * Xóa file PID.
   * Ghi log sự kiện dừng vào syslog (hoặc file log của `service_manager` nếu bạn muốn mở rộng).
4. **Trạng thái Dịch vụ (`status`):**
   * Kiểm tra sự tồn tại của file PID.
   * Nếu file PID tồn tại, đọc PID từ đó.
   * Sử dụng `kill(PID, 0)` (gửi tín hiệu 0, không làm gì ngoài kiểm tra quyền và sự tồn tại của tiến trình) để kiểm tra xem tiến trình có đang chạy không.
   * In ra "Service
5. **Liệt kê Dịch vụ (`list`):**
   * Quét thư mục `/var/run/` để tìm các file có đuôi `.pid`.
   * Đối với mỗi file `.pid` tìm thấy, đọc tên dịch vụ và PID.
   * Kiểm tra trạng thái của từng PID (như lệnh `status`).
   * In ra danh sách các dịch vụ và trạng thái của chúng.

**Yêu cầu chương trình `mock_service.cpp`:**

1. **Mô phỏng Dịch vụ:**
   * Nhận một tham số dòng lệnh là tên dịch vụ (ví dụ: `my_sensor_service`).
   * Trong vòng lặp vô hạn, cứ mỗi 3 giây, in ra một dòng "Service
   * **Xử lý Tín hiệu:**
     * Cài đặt `sigaction()` cho `SIGTERM`: Khi nhận được, in ra "Service
     * Cài đặt `sigaction()` cho `SIGHUP`: Khi nhận được, in ra "Service

**Các Module kiến thức chính được sử dụng:**

* **Process Management:** `fork()`, `execvp()`, `waitpid()`, `getpid()`, `setsid()`, `chdir()`, `close()`.
* **Signals:** `sigaction()` (cho `SIGTERM`, `SIGHUP`, `SIGCHLD`), `SIG_IGN`, `kill()`.
* **File I/O:** `open()`, `write()`, `read()`, `unlink()`, `std::ofstream`, `std::ifstream`.
* **Logging:** `syslog()`, `openlog()`, `closelog()`.
* **Command-line Arguments:** `getopt_long()`.
* **Error Handling:** `errno`, `strerror()`, `perror()`.
* **Thời gian:** `time()`, `localtime()`, `strftime()`.
* **Quét thư mục:** `opendir()`, `readdir()`, `closedir()` (cho lệnh `list`).

**Cấu trúc thư mục gợi ý:**

```
service_manager_project/
├── CMakeLists.txt
├── src/
│   ├── service_manager.cpp
│   └── mock_service.cpp
```

**Gợi ý `CMakeLists.txt`:**

**CMake**

```cmake
cmake_minimum_required(VERSION 3.17)
project(ServiceManagerProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

# Đặt thư mục đầu ra cho executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Thêm executable cho service_manager
add_executable(service_manager src/service_manager.cpp)
target_link_libraries(service_manager PRIVATE -lstdc++ ) # Cần cho C++ features, và có thể -D_GNU_SOURCE nếu dùng getopt_long

# Thêm executable cho mock_service
add_executable(mock_service src/mock_service.cpp)
target_link_libraries(mock_service PRIVATE -lstdc++ ) # Cần cho C++ features
```

**Quy trình Thực hiện:**

1. **Viết code cho `mock_service.cpp`** trước, đảm bảo nó hoạt động đúng và xử lý tín hiệu.
2. **Viết code cho `service_manager.cpp`** từng chức năng một (`launch`, `stop`, `status`, `list`).
3. **Biên dịch:** `cmake -S . -B build && cmake --build build`.
4. **Kiểm tra và Gỡ lỗi:**
   * Chạy `service_manager launch my_service_1 ./bin/mock_service my_service_1`
   * Kiểm tra log: `tail -f /var/log/my_service_1.log` (hoặc `journalctl -f -t my_service_1` nếu bạn chuyển hướng log sang syslog).
   * Kiểm tra trạng thái: `./bin/service_manager status my_service_1`
   * Dừng dịch vụ: `./bin/service_manager stop my_service_1`
   * Kiểm tra các trường hợp lỗi (ví dụ: stop một dịch vụ không chạy).
   * Sử dụng `ps -ax` và `ps -al` để kiểm tra tiến trình, đặc biệt là trạng thái `Z`.
   * Sử dụng `Valgrind` và `GDB` để gỡ lỗi nếu có vấn đề.
