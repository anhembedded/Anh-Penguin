

### **Module 7: Tài nguyên và Giới hạn (Resources and Limits) 📈**

#### **7.1. Giới hạn Tài nguyên là gì? (What are Resource Limits?) 🚫**

* **Lý thuyết:** Các chương trình chạy trên Linux đều phải tuân theo các  **giới hạn tài nguyên (resource limits)** . Các giới hạn này có thể xuất phát từ:
  * **Giới hạn vật lý của phần cứng:** Ví dụ: tổng dung lượng RAM, tốc độ CPU.
  * **Chính sách hệ thống:** Do quản trị viên đặt ra để ngăn chặn một tiến trình chiếm dụng quá nhiều tài nguyên và làm ảnh hưởng đến các tiến trình khác hoặc sự ổn định của hệ thống.
  * **Giới hạn triển khai:** Ví dụ: kích thước tối đa của một số nguyên, số lượng file descriptor tối đa mà một tiến trình có thể mở.
* **Mục đích:**
  * **Bảo vệ hệ thống:** Ngăn chặn các tiến trình độc hại hoặc lỗi gây ra tình trạng "từ chối dịch vụ" (Denial of Service - DoS) bằng cách chiếm hết tài nguyên.
  * **Phân bổ công bằng:** Đảm bảo các tiến trình chia sẻ tài nguyên một cách công bằng.
  * **Độ ổn định:** Giúp hệ thống hoạt động ổn định và dự đoán được.

#### **7.2. Các Loại Giới hạn Tài nguyên Phổ biến (`RLIMIT_*`) 📊**

* **Lý thuyết:** Các giới hạn tài nguyên được định nghĩa trong `<sys/resource.h>` và thường có tiền tố `RLIMIT_`.
  * `RLIMIT_CORE`: Kích thước tối đa của file core dump (khi chương trình crash).
  * `RLIMIT_CPU`: Thời gian CPU tối đa mà tiến trình có thể sử dụng (giây). Nếu vượt quá, `SIGXCPU` có thể được gửi.
  * `RLIMIT_DATA`: Kích thước tối đa của phân đoạn dữ liệu (data segment).
  * `RLIMIT_FSIZE`: Kích thước tối đa của file mà tiến trình có thể tạo (bytes). Nếu vượt quá, `SIGXFSZ` có thể được gửi.
  * `RLIMIT_NOFILE`: Số lượng File Descriptor tối đa mà tiến trình có thể mở đồng thời.
  * `RLIMIT_STACK`: Kích thước tối đa của stack (ngăn xếp).
  * `RLIMIT_AS`: Tổng không gian địa chỉ ảo tối đa (address space, bao gồm code, data, stack, heap).
* **Soft Limit (`rlim_cur`) và Hard Limit (`rlim_max`)** :
* Mỗi giới hạn tài nguyên có hai giá trị:
  * **Soft Limit (`rlim_cur` - current limit):** Là giới hạn "khuyên dùng". Tiến trình có thể tự tăng soft limit lên đến hard limit. Khi soft limit bị vượt quá, các hàm thư viện có thể bắt đầu trả về lỗi.
  * **Hard Limit (`rlim_max` - maximum limit):** Là giới hạn "cứng". Chỉ một tiến trình chạy với đặc quyền superuser (root) mới có thể tăng hard limit. Một tiến trình bình thường chỉ có thể giảm hard limit của chính nó.
* Khi hard limit bị vượt quá, Kernel có thể gửi tín hiệu để chấm dứt tiến trình.

#### **7.3. Lấy và Đặt Giới hạn Tài nguyên (`getrlimit()`, `setrlimit()`) 📈📉**

* **Lý thuyết:**

  * **`getrlimit()`:** Hàm dùng để lấy các giới hạn tài nguyên hiện tại của tiến trình.
    **C++**

    ```cpp
    #include <sys/resource.h> // For getrlimit, setrlimit, struct rlimit, RLIMIT_*
    int getrlimit(int resource, struct rlimit *rlim);
    ```

    * `resource`: Loại tài nguyên cần lấy giới hạn (`RLIMIT_CORE`, `RLIMIT_CPU`, v.v.).
    * `rlim`: Con trỏ tới cấu trúc `struct rlimit` để lưu thông tin giới hạn.
  * **`setrlimit()`:** Hàm dùng để đặt các giới hạn tài nguyên mới cho tiến trình.
    **C++**

    ```cpp
    #include <sys/resource.h>
    int setrlimit(int resource, const struct rlimit *rlim);
    ```

    * `resource`: Loại tài nguyên cần đặt giới hạn.
    * `rlim`: Con trỏ tới cấu trúc `struct rlimit` chứa các giới hạn mới.
  * **`struct rlimit`** :
    **C++**

  ```cpp
  struct rlimit {
      rlim_t rlim_cur; // Soft limit
      rlim_t rlim_max; // Hard limit
  };
  ```

  * `rlim_t` là một kiểu số nguyên (thường là `unsigned long`) đủ lớn để chứa các giới hạn.
  * **Giá trị trả về:** Cả hai hàm đều trả về `0` nếu thành công, `-1` nếu thất bại (`errno` được thiết lập).
* **Liên hệ Embedded Linux:**

  * **Kiểm soát bộ nhớ:** Đặt giới hạn `RLIMIT_AS`, `RLIMIT_DATA`, `RLIMIT_STACK` để ngăn chặn các ứng dụng nhúng tiêu thụ quá nhiều RAM, gây ra lỗi Out-Of-Memory (OOM) cho toàn hệ thống.
  * **Ngăn chặn rò rỉ FD:** Đặt giới hạn `RLIMIT_NOFILE` để phát hiện sớm các lỗi rò rỉ file descriptor.
  * **Đảm bảo độ bền:** Giới hạn thời gian CPU (`RLIMIT_CPU`) cho các tiến trình không quan trọng để chúng không làm treo hệ thống.
  * **Tối ưu hóa:** Điều chỉnh giới hạn để ứng dụng hoạt động hiệu quả trong môi trường tài nguyên hạn chế.
* **Ví dụ (C++): `resource_limits.cpp` - Lấy và Đặt Giới hạn Tài nguyên**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <sys/resource.h> // For getrlimit, setrlimit, struct rlimit, RLIMIT_*
  #include <unistd.h>       // For getpid, sleep
  #include <cstdio>         // For FILE, tmpfile, fprintf, ferror, fclose
  #include <cstdlib>        // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>        // For strerror
  #include <errno.h>        // For errno
  #include <cmath>          // For log (in work function)

  // Logger namespace (as defined in previous modules)
  namespace AppLogger {
      enum LogLevel { TRACE_L, DEBUG_L, INFO_L, SUCCESS_L, WARNING_L, ERROR_L, CRITICAL_L };
      static const std::map<LogLevel, std::string> LogLevelNames = {
          {TRACE_L,    "TRACE   "}, {DEBUG_L,    "DEBUG   "}, {INFO_L,     "INFO    "},
          {SUCCESS_L,  "SUCCESS "}, {WARNING_L,  "WARNING "}, {ERROR_L,    "ERROR   "},
          {CRITICAL_L, "CRITICAL"}
      };
      void log(LogLevel level, const std::string& message) {
          std::cout << LogLevelNames.at(level) << ": " << message << std::endl;
      }
  }

  // Hàm mô phỏng công việc (tạo file tạm, tính toán)
  void do_work() {
      FILE *f = tmpfile(); // Tạo file tạm thời
      if (!f) {
          AppLogger::log(AppLogger::ERROR_L, "Failed to create temporary file for work: " + std::string(strerror(errno)));
          return;
      }
      AppLogger::log(AppLogger::INFO_L, "Working: Writing to temporary file and doing calculations...");

      // Ghi dữ liệu vào file tạm
      for (int i = 0; i < 10000; ++i) {
          if (fprintf(f, "Do some output line %d\n", i) < 0) {
              if (ferror(f)) {
                  AppLogger::log(AppLogger::ERROR_L, "Error writing to temporary file (possibly FSIZE limit reached): " + std::string(strerror(errno)));
                  break; // Thoát vòng lặp nếu có lỗi ghi
              }
          }
      }
      fclose(f); // Đóng file tạm, nó sẽ tự động xóa

      // Thực hiện tính toán để tiêu thụ CPU
      double x = 4.5;
      for (int i = 0; i < 1000000; ++i) {
          x = log(x * x + 3.21);
      }
      AppLogger::log(AppLogger::INFO_L, "Working: Finished calculations. Final x: " + std::to_string(x));
  }

  int main() {
      struct rusage r_usage;
      struct rlimit r_limit;
      int priority;

      AppLogger::log(AppLogger::INFO_L, "--- Starting Resource Limits Demonstration ---");

      // --- 1. Thực hiện công việc và lấy mức sử dụng CPU ---
      AppLogger::log(AppLogger::INFO_L, "Performing initial work to measure CPU usage.");
      do_work();

      if (getrusage(RUSAGE_SELF, &r_usage) == -1) {
          AppLogger::log(AppLogger::ERROR_L, "getrusage failed: " + std::string(strerror(errno)));
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "CPU usage: User = " + std::to_string(r_usage.ru_utime.tv_sec) + "." + std::to_string(r_usage.ru_utime.tv_usec) +
                                              ", System = " + std::to_string(r_usage.ru_stime.tv_sec) + "." + std::to_string(r_usage.ru_stime.tv_usec));
      }

      // --- 2. Lấy độ ưu tiên hiện tại ---
      // Đặt errno về 0 trước khi gọi getpriority vì -1 là giá trị hợp lệ cho priority
      errno = 0; 
      priority = getpriority(PRIO_PROCESS, getpid());
      if (priority == -1 && errno != 0) {
          AppLogger::log(AppLogger::ERROR_L, "getpriority failed: " + std::string(strerror(errno)));
      } else {
          AppLogger::log(AppLogger::INFO_L, "Current priority (nice value): " + std::to_string(priority));
      }

      // --- 3. Lấy giới hạn kích thước file (RLIMIT_FSIZE) ---
      if (getrlimit(RLIMIT_FSIZE, &r_limit) == -1) {
          AppLogger::log(AppLogger::ERROR_L, "getrlimit (RLIMIT_FSIZE) failed: " + std::string(strerror(errno)));
      } else {
          AppLogger::log(AppLogger::INFO_L, "Current RLIMIT_FSIZE: soft = " + std::to_string(r_limit.rlim_cur) +
                                              ", hard = " + std::to_string(r_limit.rlim_max));
      }

      // --- 4. Đặt giới hạn kích thước file và thử lại công việc ---
      AppLogger::log(AppLogger::INFO_L, "\nINFO    : Setting a 2KB file size limit (soft) and 4KB (hard).");
      r_limit.rlim_cur = 2048; // 2 KB
      r_limit.rlim_max = 4096; // 4 KB

      if (setrlimit(RLIMIT_FSIZE, &r_limit) == -1) {
          AppLogger::log(AppLogger::ERROR_L, "setrlimit (RLIMIT_FSIZE) failed: " + std::string(strerror(errno)) + " (May require root privileges if trying to increase hard limit).");
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "File size limit set. Now performing work again (should hit limit).");
          do_work(); // Lần này sẽ gặp lỗi do giới hạn kích thước file
      }

      AppLogger::log(AppLogger::INFO_L, "--- Resource Limits Demonstration Finished ---");

      return EXIT_SUCCESS;
  }
  ```

  * **Cách biên dịch:**
    **Bash**

    ```bash
    g++ resource_limits.cpp -o resource_limits -lm # -lm để liên kết với thư viện toán học cho log()
    ```
  * **Cách chạy:**
    **Bash**

    ```
    ./resource_limits
    # Quan sát lỗi khi ghi file sau khi đặt giới hạn.
    # Thử chạy với 'nice -n 10 ./resource_limits' để thay đổi priority (nice value)
    ```
  * **Phân tích:** Bạn sẽ thấy chương trình báo lỗi khi cố gắng ghi quá 2KB vào file tạm sau khi giới hạn được đặt. Bạn cũng có thể quan sát `nice value` thay đổi khi chạy với lệnh `nice`.

#### **7.4. Độ ưu tiên Tiến trình (`getpriority()`, `setpriority()`) ⬆️⬇️**

* **Lý thuyết:** Kernel Linux sử dụng một hệ thống ưu tiên để quyết định tiến trình nào sẽ được cấp phát thời gian CPU.
  * **Giá trị Nice (Niceness):** Là một giá trị từ `-20` đến `19`.

    * `-20`: Ít tốt bụng nhất,  **độ ưu tiên cao nhất** .
    * `0`: Mặc định.
    * `19`: Tốt bụng nhất,  **độ ưu tiên thấp nhất** .
  * **`getpriority()`:** Lấy giá trị nice của một tiến trình, nhóm tiến trình, hoặc người dùng.
    **C++**

    ```cpp
    #include <sys/resource.h> // For getpriority, PRIO_*
    int getpriority(int which, id_t who);
    ```

    * `which`: Xác định `who` là gì (`PRIO_PROCESS`, `PRIO_PGRP`, `PRIO_USER`).
    * `who`: ID của tiến trình/nhóm/người dùng.
    * **Lưu ý:** Trả về `-1` nếu lỗi, nhưng `-1` cũng là một giá trị nice hợp lệ. Do đó, bạn phải kiểm tra `errno` sau khi gọi để phân biệt lỗi và giá trị `-1` hợp lệ.
  * **`setpriority()`:** Đặt giá trị nice mới.
    **C++**

    ```cpp
    #include <sys/resource.h>
    int setpriority(int which, id_t who, int priority);
    ```

    * `priority`: Giá trị nice mới.
    * **Lưu ý:** Người dùng bình thường chỉ có thể giảm độ ưu tiên (tăng giá trị nice) của các tiến trình của mình. Chỉ superuser (root) mới có thể tăng độ ưu tiên (giảm giá trị nice).
* **`getrusage()`:**
  * **Lý thuyết:** Lấy thông tin chi tiết về việc sử dụng tài nguyên (đặc biệt là thời gian CPU) của tiến trình hoặc các tiến trình con của nó.
    **C++**

    ```
    #include <sys/resource.h> // For getrusage, struct rusage, RUSAGE_*
    #include <sys/time.h>     // For struct timeval
    int getrusage(int who, struct rusage *r_usage);
    ```

    * `who`: `RUSAGE_SELF` (chỉ tiến trình hiện tại), `RUSAGE_CHILDREN` (bao gồm tiến trình con).
    * `struct rusage`: Chứa `ru_utime` (thời gian CPU trong user mode) và `ru_stime` (thời gian CPU trong kernel mode).
* **Liên hệ Embedded Linux:**
  * **Quản lý tác vụ thời gian thực mềm (Soft Real-Time):** Sử dụng `setpriority()` để đảm bảo các tác vụ quan trọng (ví dụ: thu thập dữ liệu cảm biến, điều khiển động cơ) có độ ưu tiên cao hơn các tác vụ nền (ví dụ: ghi log, giao tiếp mạng không quan trọng).
  * **Phân tích hiệu suất:** `getrusage()` giúp bạn đo lường chính xác thời gian CPU mà ứng dụng của bạn tiêu thụ, giúp tối ưu hóa mã.
  * **Ngăn chặn "starvation":** Đảm bảo các tiến trình không bị "đói" tài nguyên CPU do các tiến trình ưu tiên cao hơn.

---

### **Câu hỏi Tự đánh giá Module 7 🤔**

1. Giải thích sự khác biệt giữa "soft limit" (`rlim_cur`) và "hard limit" (`rlim_max`) của một giới hạn tài nguyên. Ai có thể tăng hard limit?
2. Bạn muốn giới hạn một chương trình không được tạo ra các file lớn hơn 10MB và không được mở quá 100 file cùng lúc. Viết đoạn code C++ sử dụng `setrlimit()` để thiết lập các giới hạn này.
3. Giá trị "niceness" của một tiến trình nằm trong khoảng nào? Giá trị nào cho thấy độ ưu tiên cao hơn?
4. Tại sao việc kiểm tra `errno` sau khi `getpriority()` trả về `-1` lại quan trọng?
5. Phân biệt giữa "user time" và "system time" trong `struct rusage`.

---

### **Bài tập Thực hành Module 7 ✍️**

1. **Chương trình Kiểm tra Giới hạn File Descriptor:**
   * Viết một chương trình C++ (`fd_limit_test.cpp`) mà:
     * Lấy và in ra giới hạn `RLIMIT_NOFILE` hiện tại (soft và hard).
     * Đặt soft limit của `RLIMIT_NOFILE` xuống một giá trị nhỏ (ví dụ: 10).
     * Thử mở một số lượng lớn file (ví dụ: 20 file `test_file_0.txt`, `test_file_1.txt`, ...).
     * Quan sát chương trình báo lỗi `EMFILE` (Too many open files) khi vượt quá soft limit mới.
     * Đảm bảo đóng tất cả các file đã mở trước khi thoát.
   * **Thử thách:** Sau khi đặt soft limit, thử tăng nó trở lại giá trị hard limit và quan sát kết quả.
2. **Chương trình Đo lường Thời gian CPU:**
   * Viết một chương trình C++ (`cpu_time_meter.cpp`) mà:
     * Có một hàm `perform_heavy_computation()` thực hiện một vòng lặp tính toán nặng (ví dụ: lặp 10 triệu lần phép toán `sqrt(log(x))`).
     * Trong `main()`, gọi `perform_heavy_computation()`.
     * Sau đó, sử dụng `getrusage()` để lấy và in ra "user time" và "system time" mà hàm tính toán đã tiêu thụ.
   * **Thử thách:**
     * Chạy chương trình này với `nice -n 10 ./cpu_time_meter` và `nice -n -10 ./cpu_time_meter` (cần `sudo` cho `-10`). Quan sát liệu "user time" và "system time" có thay đổi đáng kể không. Giải thích tại sao.
3. **Chương trình Quản lý Độ ưu tiên (Nâng cao):**
   * Viết một chương trình C++ (`priority_manager.cpp`) mà:
     * In ra PID của chính nó và độ ưu tiên (nice value) hiện tại.
     * Thử giảm độ ưu tiên của chính nó (tăng nice value, ví dụ: từ 0 lên 5).
     * In ra độ ưu tiên mới để xác nhận.
     * Thử tăng độ ưu tiên của chính nó (giảm nice value, ví dụ: từ 5 xuống 0 hoặc -5).
     * In ra độ ưu tiên mới.
   * **Lưu ý:** Bạn sẽ cần chạy chương trình với `sudo` để có thể tăng độ ưu tiên (giảm nice value). Xử lý lỗi `EPERM` nếu không có đủ quyền.

---

Chúc mừng bạn đã hoàn thành toàn bộ giáo trình "The Linux Environment"! Đây là một khối lượng kiến thức rất lớn và cực kỳ quan trọng cho bất kỳ lập trình viên Linux nào, đặc biệt là trong lĩnh vực nhúng.

Hãy dành thời gian để thực hành các bài tập này. Chúng sẽ củng cố kiến thức của bạn và giúp bạn tự tin hơn khi phát triển các ứng dụng Linux thực tế.

Bạn có muốn chuyển sang một giáo trình mới, hoặc ôn lại bất kỳ phần nào bạn muốn không?
