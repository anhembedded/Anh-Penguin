

### **Module 6: Ghi Log (Logging) 📜**

#### **6.1. Tầm quan trọng của Logging 📊**

* **Lý thuyết:** Ghi log là quá trình các chương trình ghi lại các thông báo về hoạt động của chúng. Các thông báo này có thể là:
  * **Lỗi (Errors):** Các vấn đề nghiêm trọng cần được xử lý.
  * **Cảnh báo (Warnings):** Các tình huống không mong muốn nhưng chương trình vẫn có thể tiếp tục.
  * **Thông tin (Informational):** Các sự kiện quan trọng trong hoạt động bình thường của chương trình.
  * **Gỡ lỗi (Debug):** Thông tin chi tiết dành cho lập trình viên để theo dõi luồng thực thi và giá trị biến.
* **Mục đích:**
  * **Gỡ lỗi:** Xác định nguyên nhân của lỗi hoặc hành vi không mong muốn.
  * **Giám sát:** Theo dõi hiệu suất và trạng thái của hệ thống.
  * **Kiểm tra (Auditing):** Ghi lại các sự kiện quan trọng cho mục đích bảo mật hoặc tuân thủ.
  * **Phân tích:** Thu thập dữ liệu để phân tích xu hướng hoặc cải thiện ứng dụng.
* **Vị trí Log:** Các thông báo log thường được ghi vào console, các file log chuyên dụng (ví dụ: `/var/log/messages`, `/var/log/syslog`), hoặc được gửi qua mạng đến một server log tập trung.

#### **6.2. Hệ thống Syslog (The Syslog System) 🌳**

* **Lý thuyết:** Syslog là một giao thức chuẩn và một hệ thống dịch vụ log trong Linux (và Unix-like systems). Nó cho phép các chương trình gửi thông báo log đến một daemon log tập trung (thường là `syslogd`, `rsyslogd`, hoặc `journald` trên các hệ thống hiện đại). Daemon này sau đó sẽ xử lý các thông báo dựa trên cấu hình (ví dụ: ghi vào file, gửi qua mạng, bỏ qua).
* **Hàm `syslog()`:** Hàm chính để gửi thông báo log đến hệ thống syslog.
  **C++**

  ```
  #include <syslog.h> // For syslog, LOG_*, openlog, closelog, setlogmask
  // void syslog(int priority, const char *message, arguments...);
  ```

  * **`priority`** : Là sự kết hợp (bitwise OR `|`) của một **mức độ nghiêm trọng (severity level)** và một  **giá trị cơ sở (facility value)** .
  * **Severity Levels (Mức độ nghiêm trọng - từ cao đến thấp):**
    * `LOG_EMERG`: Emergency (khẩn cấp, hệ thống không dùng được).
    * `LOG_ALERT`: Alert (cần hành động ngay).
    * `LOG_CRIT`: Critical (lỗi nghiêm trọng, ví dụ: lỗi phần cứng).
    * `LOG_ERR`: Error (lỗi).
    * `LOG_WARNING`: Warning (cảnh báo).
    * `LOG_NOTICE`: Notice (điều kiện bình thường nhưng đáng chú ý).
    * `LOG_INFO`: Informational (thông tin chung).
    * `LOG_DEBUG`: Debug (thông tin gỡ lỗi).
  * **Facility Values (Giá trị cơ sở - nguồn gốc thông báo):**
    * `LOG_USER`: Thông báo từ các ứng dụng người dùng (mặc định).
    * `LOG_DAEMON`: Thông báo từ các daemon hệ thống.
    * `LOG_LOCAL0` đến `LOG_LOCAL7`: Dành cho việc sử dụng tùy chỉnh của quản trị viên.
  * **`message`** : Một chuỗi định dạng giống `printf`, có thể chứa các specifier chuyển đổi.
  * **Đặc biệt:** `%m` sẽ được thay thế bằng chuỗi mô tả lỗi tương ứng với giá trị hiện tại của `errno`. Rất hữu ích để ghi log lỗi hệ thống.
* **Hàm `openlog()`:**

  * `void openlog(const char *ident, int logopt, int facility);`
  * Thiết lập các tùy chọn cho các cuộc gọi `syslog()` tiếp theo.
    * `ident`: Một chuỗi tiền tố (thường là tên chương trình) sẽ được thêm vào mỗi thông báo log.
    * `logopt`: Các tùy chọn hành vi (bitwise OR):
      * `LOG_PID`: Thêm PID của tiến trình vào thông báo log.
      * `LOG_CONS`: Gửi thông báo đến console nếu không thể gửi đến syslog daemon.
      * `LOG_NDELAY`: Mở kết nối đến syslog daemon ngay lập tức.
      * `LOG_ODELAY`: Mở kết nối khi gọi `syslog()` lần đầu (mặc định).
    * `facility`: Giá trị cơ sở mặc định cho các cuộc gọi `syslog()` tiếp theo.
* **Hàm `closelog()`:**

  * `void closelog(void);`
  * Đóng kết nối đến syslog daemon và giải phóng tài nguyên.
* **Hàm `setlogmask()`:**

  * `int setlogmask(int maskpri);`
  * Kiểm soát mức độ ưu tiên của các thông báo log sẽ được chấp nhận bởi `syslog()`. Các thông báo có mức độ ưu tiên thấp hơn `maskpri` sẽ bị bỏ qua.
  * Sử dụng macro `LOG_MASK(priority)` (cho một mức ưu tiên cụ thể) hoặc `LOG_UPTO(priority)` (cho tất cả các mức ưu tiên đến và bao gồm `priority`).
* **Liên hệ Embedded Linux:**

  * **Gỡ lỗi và Giám sát Từ xa:** Trên thiết bị nhúng không có màn hình, syslog là cách chính để nhận thông báo về hoạt động của ứng dụng. Bạn có thể cấu hình syslog để gửi log qua mạng (syslog-ng, rsyslog) đến một máy chủ log trung tâm.
  * **Phân tích sự cố:** Các log được ghi với dấu thời gian và mức độ nghiêm trọng giúp bạn dễ dàng phân tích nguyên nhân khi thiết bị gặp sự cố.
  * **Tối ưu tài nguyên:** Sử dụng `setlogmask()` để tắt các thông báo `LOG_DEBUG` trong môi trường sản phẩm, giúp giảm tải I/O và tiết kiệm không gian lưu trữ.
* **Ví dụ (C++): `syslog_example.cpp` - Sử dụng Syslog**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <syslog.h> // For syslog, openlog, closelog, setlogmask, LOG_*
  #include <unistd.h> // For getpid
  #include <cstdio>   // For fopen, fclose
  #include <cstdlib>  // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>  // For strerror
  #include <errno.h>  // For errno

  int main() {
      // Khởi tạo syslog. "my_app_logger" là định danh, LOG_PID thêm PID, LOG_USER là cơ sở mặc định.
      openlog("my_app_logger", LOG_PID | LOG_CONS, LOG_USER);
      std::cout << "INFO    : Syslog initialized. Check /var/log/syslog or journalctl for messages." << std::endl;

      // --- Gửi các loại thông báo khác nhau ---
      syslog(LOG_INFO, "Application started. PID: %d", getpid());
      std::cout << "INFO    : Sent INFO message to syslog." << std::endl;

      syslog(LOG_WARNING, "Configuration file 'config.txt' not found. Using default settings.");
      std::cout << "INFO    : Sent WARNING message to syslog." << std::endl;

      // Thử mở một file không tồn tại để tạo lỗi và dùng %m
      FILE *f = fopen("non_existent_file_for_log_test.txt", "r");
      if (!f) {
          // %m sẽ tự động thay thế bằng strerror(errno)
          syslog(LOG_ERR, "Failed to open file 'non_existent_file_for_log_test.txt': %m");
          std::cout << "INFO    : Sent ERROR message to syslog with %m (errno description)." << std::endl;
      } else {
          fclose(f);
          std::cout << "INFO    : Successfully opened file (unexpected in this test)." << std::endl;
      }

      syslog(LOG_DEBUG, "This is a DEBUG message. PID: %d", getpid());
      std::cout << "INFO    : Sent DEBUG message to syslog." << std::endl;

      // --- Kiểm soát logmask ---
      std::cout << "\nINFO    : Setting logmask to only show messages up to LOG_NOTICE." << std::endl;
      int old_logmask = setlogmask(LOG_UPTO(LOG_NOTICE)); // Chỉ hiển thị NOTICE trở lên
      std::cout << "INFO    : Old logmask was: " << old_logmask << std::endl;

      syslog(LOG_DEBUG, "This DEBUG message should NOT appear due to logmask. PID: %d", getpid());
      std::cout << "INFO    : Sent another DEBUG message (should be filtered by logmask)." << std::endl;

      syslog(LOG_NOTICE, "This NOTICE message SHOULD appear. PID: %d", getpid());
      std::cout << "INFO    : Sent a NOTICE message (should appear)." << std::endl;

      // Khôi phục logmask về ban đầu (tùy chọn)
      setlogmask(old_logmask);
      std::cout << "INFO    : Logmask restored to original value." << std::endl;

      syslog(LOG_DEBUG, "This DEBUG message should appear again after restoring logmask. PID: %d", getpid());
      std::cout << "INFO    : Sent a final DEBUG message (should appear again)." << std::endl;


      // Đóng syslog
      closelog();
      std::cout << "INFO    : Syslog connection closed." << std::endl;
      std::cout << "INFO    : Program finished. Use 'journalctl -f -t my_app_logger' or 'tail -f /var/log/syslog' to view logs." << std::endl;

      return EXIT_SUCCESS;
  }
  ```

  * **Cách biên dịch:**
    **Bash**

    ```
    g++ syslog_example.cpp -o syslog_example
    ```
  * **Cách chạy và kiểm tra log:**
    **Bash**

    ```bash
    ./syslog_example
    # Mở một terminal khác và kiểm tra log:
    journalctl -f -t my_app_logger # Dành cho hệ thống dùng systemd (phổ biến hiện nay)
    # Hoặc:
    tail -f /var/log/syslog       # Dành cho hệ thống dùng rsyslogd/syslogd truyền thống
    # Hoặc:
    tail -f /var/log/messages     # Tùy thuộc cấu hình syslog của bạn
    ```
  * **Phân tích:** Quan sát cách các thông báo log xuất hiện trong hệ thống log của bạn, bao gồm định danh chương trình, PID, và cách `setlogmask` lọc bỏ các thông báo có mức độ ưu tiên thấp hơn.

---

### **Câu hỏi Tự đánh giá Module 6 🤔**

1. Giải thích tầm quan trọng của việc ghi log trong môi trường lập trình nhúng. Nêu ít nhất ba mục đích chính của logging.
2. Syslog là gì? Nêu vai trò của syslog daemon trong hệ thống log.
3. Khi gọi `syslog()`, tham số `priority` được tạo thành từ những thành phần nào? Nêu ví dụ về một mức độ nghiêm trọng và một giá trị cơ sở.
4. Bạn muốn các thông báo log của chương trình `my_daemon` của bạn luôn bao gồm PID của nó và được gửi đến syslog daemon ngay lập tức. Viết lệnh `openlog()` phù hợp.
5. Giải thích cách `%m` hoạt động trong chuỗi định dạng của `syslog()`.
6. Bạn muốn chỉ hiển thị các thông báo log có mức độ nghiêm trọng từ `LOG_WARNING` trở lên (tức là `WARNING`, `ERR`, `CRIT`, `ALERT`, `EMERG`). Viết lệnh `setlogmask()` phù hợp.

---

### **Bài tập Thực hành Module 6 ✍️**

1. **Chương trình Ghi Log Đơn giản với các Cấp độ:**
   * Viết một chương trình C++ (`simple_logger.cpp`) mà:
     * Sử dụng `openlog()` với định danh là tên chương trình và `LOG_PID`.
     * Ghi các thông báo log với các mức độ nghiêm trọng khác nhau: `LOG_INFO`, `LOG_WARNING`, `LOG_ERR`, `LOG_DEBUG`.
     * Trong đó, thông báo `LOG_ERR` phải là kết quả của một lỗi thực tế (ví dụ: cố gắng mở một file không tồn tại) và sử dụng `%m`.
     * Sử dụng `setlogmask()` để thử bật/tắt các thông báo `LOG_DEBUG`.
     * Đóng syslog bằng `closelog()`.
   * **Thử thách:**
     * Tạo một hàm wrapper `my_log(LogLevel level, const std::string& message)` để đơn giản hóa việc gọi `syslog()`.
     * Chạy chương trình và kiểm tra output trong file log hệ thống.
2. **Chương trình Daemon Ghi Log Tối thiểu (Nâng cao):**
   * Viết một chương trình C++ (`minimal_daemon_logger.cpp`) mà:

     * Thực hiện `fork()` hai lần để trở thành một daemon (tiến trình nền, không có terminal điều khiển).
     * Trong tiến trình daemon, sử dụng `openlog()` để cấu hình syslog.
     * Ghi một thông báo `LOG_INFO` khi khởi động.
     * Trong một vòng lặp vô hạn, cứ mỗi 5 giây, ghi một thông báo `LOG_DEBUG` hoặc `LOG_INFO` (ví dụ: "Daemon still running...").
     * Cài đặt trình xử lý tín hiệu `SIGTERM` để khi nhận được tín hiệu này, daemon sẽ thoát một cách duyên dáng (ghi một thông báo `LOG_INFO` cuối cùng và `closelog()`).
   * **Cách chạy:**
     **Bash**

     ```bash
     g++ minimal_daemon_logger.cpp -o minimal_daemon_logger
     ./minimal_daemon_logger &  # Chạy nền
     ps -ax | grep minimal_daemon_logger # Tìm PID của daemon
     kill <PID_của_daemon> # Gửi SIGTERM để dừng daemon
     ```
   * **Thử thách:** Quan sát log để thấy daemon khởi động, các thông báo định kỳ, và thông báo khi nó dừng.

---
