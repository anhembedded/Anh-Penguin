# **Module 2: Khóa File (File Locking) 🔒**

Trong Module này, chúng ta sẽ tìm hiểu về **khóa file (file locking)** – một cơ chế quan trọng cho phép các chương trình phối hợp quyền truy cập vào các file được chia sẻ. Điều này là cực kỳ cần thiết trong các hệ thống đa nhiệm hoặc đa người dùng để đảm bảo tính toàn vẹn và nhất quán của dữ liệu.

#### **2.1. Tại sao cần Khóa File? (Why File Locking?) 🤝**

* **Lý thuyết:** Trong môi trường đa nhiệm (multitasking) hoặc đa người dùng (multi-user), nhiều tiến trình có thể cố gắng truy cập và sửa đổi cùng một file đồng thời. Nếu không có cơ chế đồng bộ hóa, điều này có thể dẫn đến:
  * **Điều kiện tranh chấp (Race Conditions):** Các thao tác đọc và ghi bị xen kẽ một cách không mong muốn, dẫn đến dữ liệu bị hỏng hoặc không nhất quán.
  * **Dữ liệu không toàn vẹn (Data Inconsistency):** Một tiến trình đọc dữ liệu đang trong quá trình được tiến trình khác ghi dở, hoặc hai tiến trình cùng ghi vào cùng một vị trí, dẫn đến mất mát dữ liệu.
* **Mục đích của Khóa File:** Cung cấp một cách để các chương trình **thiết lập quyền kiểm soát** (establish control) đối với một file hoặc một phần của file, đảm bảo rằng dữ liệu được truy cập và sửa đổi một cách an toàn và có trật tự.
* **Khóa Tư vấn (Advisory Locks) vs. Khóa Bắt buộc (Mandatory Locks):**
  * **Khóa Tư vấn (Advisory Locks):** Đây là loại khóa phổ biến nhất trong Linux. Hệ điều hành **không tự động thực thi** khóa. Các chương trình phải **chủ động kiểm tra và tuân thủ** các khóa. Nếu một chương trình không tuân thủ quy tắc khóa (tức là không kiểm tra khóa trước khi truy cập), nó vẫn có thể đọc hoặc ghi vào file bị khóa.
  * **Khóa Bắt buộc (Mandatory Locks):** Loại khóa này ít phổ biến hơn và thường được bật thông qua việc mount filesystem với tùy chọn đặc biệt hoặc quyền hạn setgid. Hệ điều hành sẽ **thực thi** khóa và ngăn chặn truy cập vào file/vùng bị khóa ngay cả khi chương trình không kiểm tra.
* **Liên hệ Embedded Linux:** Cực kỳ quan trọng cho các thiết bị nhúng chạy nhiều daemon hoặc ứng dụng độc lập cần truy cập chung các file cấu hình, file log, hoặc dữ liệu cảm biến. Đảm bảo tính toàn vẹn của dữ liệu là chìa khóa cho độ tin cậy của hệ thống nhúng.

#### **2.2. Tạo File Khóa (Creating Lock Files) 🔑**

* **Lý thuyết:** Một phương pháp đơn giản (nhưng cần sự hợp tác) là sử dụng một file riêng biệt làm cờ khóa.

  * **Cách hoạt động:** Một tiến trình muốn truy cập tài nguyên sẽ cố gắng tạo một file có tên cụ thể (ví dụ: `/tmp/LCK.test`). Nó sử dụng hàm `open()` với các cờ đặc biệt để đảm bảo việc tạo file này là "atomic" (nguyên tử) và duy nhất.
  * **Các cờ `open()` quan trọng:**
    * `O_CREAT`: Tạo file nếu nó chưa tồn tại.
    * `O_EXCL`: Chỉ sử dụng với `O_CREAT`. Nếu file đã tồn tại, `open()` sẽ **thất bại** và trả về `-1` với `errno` là `EEXIST`. Điều này đảm bảo rằng bạn là người duy nhất đã tạo (và do đó có quyền kiểm soát) file khóa tại thời điểm đó.
  * Nếu `open()` thành công, tiến trình có thể giả định nó đã có khóa. Sau khi hoàn thành, nó sẽ `close()` và `unlink()` (xóa) file khóa.
  * **Hạn chế:** Đây là một cơ chế khóa **tư vấn** (advisory). Các chương trình  **phải hợp tác** ; nếu một chương trình bỏ qua việc kiểm tra/tạo file khóa, nó vẫn có thể truy cập tài nguyên. Ngoài ra, nếu chương trình giữ khóa bị crash trước khi xóa file khóa, file khóa có thể bị "kẹt" (stale lock), khiến tài nguyên bị khóa vĩnh viễn cho đến khi được xóa thủ công.
* **Liên hệ Embedded Linux:** Phù hợp cho các tài nguyên đơn giản hoặc khi cần cơ chế khóa "ngăn ngừa" giữa các ứng dụng đã biết cách hợp tác.
* **Ví dụ (C++): `lock1.cpp` - Tạo File Khóa Đơn giản**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <fcntl.h>    // For open, O_RDWR, O_CREAT, O_EXCL
  #include <unistd.h>   // For close, unlink
  #include <cstdlib>    // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>    // For strerror
  #include <errno.h>    // For errno

  // Logger namespace
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

  int main() {
      const char *lock_file_path = "/tmp/LCK.test";
      int file_desc;

      AppLogger::log(AppLogger::INFO_L, "Attempting to create lock file: " + std::string(lock_file_path));

      // Mở file với O_CREAT và O_EXCL để tạo khóa atomic
      // Quyền 0444 (r--r--r--) - chỉ đọc, vì nó chỉ là cờ báo hiệu
      file_desc = open(lock_file_path, O_RDWR | O_CREAT | O_EXCL, 0444);

      if (file_desc == -1) {
          if (errno == EEXIST) {
              AppLogger::log(AppLogger::WARNING_L, "Lock already present. File " + std::string(lock_file_path) + " already exists.");
          } else {
              AppLogger::log(AppLogger::ERROR_L, "Open failed for lock file: " + std::string(strerror(errno)));
          }
          return EXIT_FAILURE;
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "Open succeeded. Lock file " + std::string(lock_file_path) + " created.");
          // Giữ file mở để duy trì khóa
          AppLogger::log(AppLogger::INFO_L, "Holding lock for 5 seconds (Press Ctrl+C to stop this process).");
          sleep(5); // Giả vờ làm việc trong critical section

          // Sau khi hoàn thành, đóng và xóa file khóa
          AppLogger::log(AppLogger::INFO_L, "Releasing lock. Closing and unlinking lock file.");
          close(file_desc);
          unlink(lock_file_path);
          AppLogger::log(AppLogger::SUCCESS_L, "Lock released.");
      }

      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ lock1.cpp -o lock1
    ./lock1           # Lần đầu tiên, sẽ thành công
    ./lock1           # Lần thứ hai, sẽ thất bại với lỗi "Lock already present"
    rm /tmp/LCK.test  # Xóa file khóa để thử lại
    ```
* **Ví dụ (C++): `lock2.cpp` - File Khóa Hợp tác (Cooperative Lock Files)**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <fcntl.h>    // For open, O_RDWR, O_CREAT, O_EXCL
  #include <unistd.h>   // For close, unlink, sleep, getpid
  #include <cstdlib>    // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>    // For strerror
  #include <errno.h>    // For errno

  // Logger namespace
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

  const char *lock_file = "/tmp/LCK.test2";

  int main() {
      int file_desc;
      int tries = 10;
      pid_t my_pid = getpid(); // Lấy PID của tiến trình hiện tại

      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Starting lock cooperation test.");

      while (tries--) {
          file_desc = open(lock_file, O_RDWR | O_CREAT | O_EXCL, 0444);

          if (file_desc == -1) {
              AppLogger::log(AppLogger::WARNING_L, "Process " + std::to_string(my_pid) + ": Lock already present. Waiting... (" + strerror(errno) + ")");
              sleep(3); // Chờ 3 giây trước khi thử lại
          } else {
              // Critical section starts here
              AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": I have exclusive access.");
              sleep(1); // Giả vờ làm việc trong critical section
              close(file_desc); // Đóng file để giải phóng khóa
              unlink(lock_file); // Xóa file khóa
              AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Released lock.");
              // Critical section ends here
              sleep(2); // Giả vờ làm việc khác
          }
      }
      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Finished cooperation test.");
      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ lock2.cpp -o lock2
    rm -f /tmp/LCK.test2 # Đảm bảo file khóa không tồn tại trước khi chạy
    ./lock2 & ./lock2    # Chạy hai bản sao cùng lúc (một nền, một tiền cảnh)
    ```
  * **Phân tích:** Bạn sẽ thấy hai tiến trình (với các PID khác nhau) lần lượt giành được khóa và thông báo "I have exclusive access", trong khi tiến trình còn lại báo "Lock already present" và chờ.

#### **2.3. Khóa Vùng File (Locking Regions) 🔐**

* **Lý thuyết:** Khi bạn có một file lớn và muốn nhiều chương trình truy cập đồng thời các **phần khác nhau** của file mà không can thiệp vào nhau, hoặc cần phân biệt quyền khóa đọc/ghi, bạn sử dụng khóa vùng (file-segment locking).

  * Linux cung cấp hai cách chính: **`fcntl()`** và  **`lockf()`** . `fcntl()` là phương pháp mạnh mẽ và phổ biến hơn. `lockf()` là giao diện đơn giản hơn, thường được triển khai trên nền `fcntl()`.
  * **Không trộn lẫn `fcntl` và `lockf`** trên cùng một file, vì chúng sử dụng các triển khai khác nhau và có thể gây ra hành vi không xác định.
* **`fcntl()` cho Khóa Vùng:**

  * Hàm: `int fcntl(int fildes, int command, struct flock *flock_structure);` (như đã học trong Module 7 của giáo trình trước).
  * **`struct flock`:** Cấu trúc để định nghĩa khóa, chứa các thành viên quan trọng:
    * `l_type`: **Loại khóa** (`F_RDLCK` - khóa đọc chia sẻ, `F_WRLCK` - khóa ghi độc quyền, `F_UNLCK` - mở khóa).
    * `l_whence`: Vị trí gốc (`SEEK_SET`, `SEEK_CUR`, `SEEK_END`).
    * `l_start`: Offset từ `l_whence`.
    * `l_len`: Độ dài của vùng bị khóa.
    * `l_pid`: PID của tiến trình đang giữ khóa (chỉ được Kernel điền khi dùng `F_GETLK`).
  * **Các lệnh `fcntl()` liên quan đến khóa:**
    * `F_GETLK`: **Kiểm tra khóa.** Không đặt khóa. Nếu có khóa xung đột, nó điền thông tin về khóa xung đột vào `struct flock` (đặc biệt là `l_pid` của tiến trình giữ khóa). Nếu không có xung đột, `l_type` sẽ được đặt thành `F_UNLCK`.
    * `F_SETLK`: **Đặt hoặc giải phóng khóa (không chặn).** Nếu không thể đặt khóa ngay lập tức (do xung đột), nó trả về `-1` với `errno` là `EACCES` hoặc `EAGAIN`.
    * `F_SETLKW`: **Đặt hoặc giải phóng khóa (có chặn).** Nếu không thể đặt khóa ngay lập tức, tiến trình sẽ **chờ** cho đến khi có thể đặt được khóa hoặc bị gián đoạn bởi một tín hiệu.
  * **Quy tắc Khóa:**
    * **Khóa Đọc (`F_RDLCK`):** Cho phép **nhiều** tiến trình có khóa đọc trên cùng một vùng file. Tuy nhiên, không tiến trình nào được phép giữ khóa ghi trên vùng đó.
    * **Khóa Ghi (`F_WRLCK`):** Chỉ **một** tiến trình duy nhất có thể giữ khóa ghi trên một vùng. Không tiến trình nào khác có thể giữ khóa đọc hoặc khóa ghi trên vùng đó. Khóa ghi là độc quyền.
  * **Lưu ý:** Tất cả các khóa do một chương trình giữ trên một file sẽ tự động được giải phóng khi File Descriptor tương ứng được đóng, hoặc khi chương trình kết thúc.
* **Sử dụng `read()` và `write()` với Khóa:**

  * Luôn sử dụng các System Call cấp thấp **`read()`** và **`write()`** (thay vì `fread()`/`fwrite()`) khi làm việc với khóa vùng file.
  * Lý do: Các hàm `stdio` (`fread()`, `fwrite()`) sử dụng bộ đệm nội bộ (buffering) và có thể "đọc trước" (read-ahead) hoặc "ghi đệm" (write-behind) dữ liệu. Điều này có thể khiến dữ liệu trong bộ đệm của thư viện không khớp với dữ liệu thực tế trên đĩa (đã bị chương trình khác sửa đổi), dẫn đến lỗi khó lường và phá vỡ cơ chế khóa.
* **`lockf()` (Giao diện Đơn giản hơn):**

  * Hàm: `int lockf(int fildes, int function, off_t size_to_lock);`
  * Cung cấp giao diện đơn giản hơn cho việc khóa vùng, nhưng ít tính năng hơn `fcntl()`. Yêu cầu bạn tự `lseek()` đến vị trí bắt đầu vùng cần khóa.
  * `function`: `F_ULOCK` (mở khóa), `F_LOCK` (khóa độc quyền), `F_TLOCK` (kiểm tra và khóa độc quyền), `F_TEST` (kiểm tra khóa).
* **Liên hệ Embedded Linux:** Rất quan trọng khi cần truy cập đồng thời vào các file dữ liệu lớn (ví dụ: cơ sở dữ liệu nhúng tùy chỉnh, file log có cấu trúc) từ nhiều thành phần phần mềm khác nhau trên thiết bị nhúng.
* **Ví dụ (C++): `lock3.cpp` - Khóa File với `fcntl()` (Đặt khóa)**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <fcntl.h>    // For open, F_RDLCK, F_WRLCK, F_UNLCK, F_SETLK, struct flock
  #include <unistd.h>   // For close, sleep, getpid, write
  #include <cstdlib>    // For EXIT_FAILURE, EXIT_SUCCESS
  #include <cstring>    // For strerror, memset
  #include <errno.h>    // For errno

  // Logger namespace
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

  const char *test_file = "/tmp/test_lock_file";

  int main() {
      int file_desc;
      int res;
      pid_t my_pid = getpid();

      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Starting.");

      // 1. Mở hoặc tạo file
      file_desc = open(test_file, O_RDWR | O_CREAT, 0666); // r/w for owner, group, others
      if (file_desc == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Process " + std::to_string(my_pid) + ": Unable to open " + std::string(test_file) + " for read/write: " + strerror(errno));
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Opened " + std::string(test_file) + " with FD " + std::to_string(file_desc));

      // 2. Ghi một ít dữ liệu vào file để có nội dung để khóa
      for(int byte_count = 0; byte_count < 100; byte_count++) {
          (void)write(file_desc, "A", 1); // Ghi 100 ký tự 'A'
      }
      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Wrote 100 'A's to file.");

      // 3. Thiết lập vùng 1 với khóa đọc (F_RDLCK) từ byte 10 đến 29 (dài 20 byte)
      struct flock region_1;
      memset(&region_1, 0, sizeof(region_1));
      region_1.l_type = F_RDLCK;
      region_1.l_whence = SEEK_SET;
      region_1.l_start = 10;
      region_1.l_len = 20;

      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Attempting to set shared lock (F_RDLCK) on bytes 10-29.");
      res = fcntl(file_desc, F_SETLK, &region_1);
      if (res == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Process " + std::to_string(my_pid) + ": Failed to set shared lock on region 1: " + strerror(errno));
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Successfully set shared lock on region 1.");
      }

      // 4. Thiết lập vùng 2 với khóa ghi (F_WRLCK) từ byte 40 đến 49 (dài 10 byte)
      struct flock region_2;
      memset(&region_2, 0, sizeof(region_2));
      region_2.l_type = F_WRLCK;
      region_2.l_whence = SEEK_SET;
      region_2.l_start = 40;
      region_2.l_len = 10;

      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Attempting to set exclusive lock (F_WRLCK) on bytes 40-49.");
      res = fcntl(file_desc, F_SETLK, &region_2);
      if (res == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Process " + std::to_string(my_pid) + ": Failed to set exclusive lock on region 2: " + strerror(errno));
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Successfully set exclusive lock on region 2.");
      }

      // 5. Giữ khóa trong một thời gian
      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Holding locks for 60 seconds. You can run 'lock4.cpp' or 'lock5.cpp' in another terminal.");
      sleep(60); // Giữ khóa

      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Closing file and releasing all locks.");
      close(file_desc); // Đóng FD tự động giải phóng tất cả các khóa của tiến trình này
      unlink(test_file); // Xóa file
      AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": File closed and unlinked. All locks released.");

      return EXIT_SUCCESS;
  }
  ```

  * **Cách biên dịch:**
    **Bash**

    ```
    g++ lock3.cpp -o lock3
    ```
  * **Cách chạy:**
    **Bash**

    ```
    ./lock3 & # Chạy ở chế độ nền
    # Bạn sẽ thấy thông báo "Process XXX locking file"
    ```
* **Ví dụ (C++): `lock4.cpp` - Kiểm tra Khóa File (`F_GETLK`)**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <fcntl.h>    // For open, F_RDLCK, F_WRLCK, F_UNLCK, F_GETLK, struct flock
  #include <unistd.h>   // For close, getpid
  #include <cstdlib>    // For EXIT_FAILURE, EXIT_SUCCESS
  #include <cstring>    // For memset, strerror
  #include <errno.h>    // For errno

  // Logger namespace
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

  const char *test_file = "/tmp/test_lock_file";
  #define SIZE_TO_TRY 5 // Kích thước vùng cần kiểm tra

  // Hàm trợ giúp để hiển thị thông tin khóa
  void show_lock_info(const struct flock *to_show) {
      std::cout << "\tINFO    :   l_type: " << (to_show->l_type == F_RDLCK ? "F_RDLCK (Shared)" :
                                              (to_show->l_type == F_WRLCK ? "F_WRLCK (Exclusive)" : "F_UNLCK (Unlocked)"));
      std::cout << ", l_whence: " << (to_show->l_whence == SEEK_SET ? "SEEK_SET" :
                                       (to_show->l_whence == SEEK_CUR ? "SEEK_CUR" : "SEEK_END"));
      std::cout << ", l_start: " << (long long)to_show->l_start;
      std::cout << ", l_len: " << (long long)to_show->l_len;
      std::cout << ", l_pid: " << to_show->l_pid << std::endl;
  }

  int main() {
      int file_desc;
      int res;
      struct flock region_to_test;
      pid_t my_pid = getpid();

      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Starting lock test.");

      // 1. Mở file (phải đã được tạo bởi lock3.cpp)
      file_desc = open(test_file, O_RDWR);
      if (file_desc == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Process " + std::to_string(my_pid) + ": Unable to open " + std::string(test_file) + " (ensure lock3.cpp is running): " + strerror(errno));
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Opened " + std::string(test_file) + " with FD " + std::to_string(file_desc));

      // Vòng lặp kiểm tra các vùng khác nhau của file
      for (int start_byte = 0; start_byte < 100; start_byte += SIZE_TO_TRY) {
          AppLogger::log(AppLogger::INFO_L, "\nProcess " + std::to_string(my_pid) + ": --- Testing region from " + std::to_string(start_byte) + " to " + std::to_string(start_byte + SIZE_TO_TRY - 1) + " ---");

          // --- 3. Kiểm tra khóa ghi (F_WRLCK) ---
          memset(&region_to_test, 0, sizeof(region_to_test));
          region_to_test.l_type = F_WRLCK; // Loại khóa muốn kiểm tra
          region_to_test.l_whence = SEEK_SET;
          region_to_test.l_start = start_byte;
          region_to_test.l_len = SIZE_TO_TRY;
          region_to_test.l_pid = -1; // Đặt thành giá trị không hợp lệ để kiểm tra sự thay đổi

          AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Testing F_WRLCK (exclusive) on region " + std::to_string(start_byte) + " to " + std::to_string(start_byte + SIZE_TO_TRY - 1));
          res = fcntl(file_desc, F_GETLK, &region_to_test);
          if (res == -1) {
              AppLogger::log(AppLogger::ERROR_L, "Process " + std::to_string(my_pid) + ": F_GETLK failed: " + strerror(errno));
              close(file_desc);
              return EXIT_FAILURE;
          }

          if (region_to_test.l_type != F_UNLCK) { // Nếu l_type không phải F_UNLCK, có xung đột
              AppLogger::log(AppLogger::WARNING_L, "Process " + std::to_string(my_pid) + ": F_WRLCK would fail. Conflicting lock found:");
              show_lock_info(&region_to_test);
          } else {
              AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": F_WRLCK - Lock would succeed.");
          }

          // --- 4. Kiểm tra khóa đọc (F_RDLCK) ---
          memset(&region_to_test, 0, sizeof(region_to_test));
          region_to_test.l_type = F_RDLCK; // Loại khóa muốn kiểm tra
          region_to_test.l_whence = SEEK_SET;
          region_to_test.l_start = start_byte;
          region_to_test.l_len = SIZE_TO_TRY;
          region_to_test.l_pid = -1; // Reset pid

          AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Testing F_RDLCK (shared) on region " + std::to_string(start_byte) + " to " + std::to_string(start_byte + SIZE_TO_TRY - 1));
          res = fcntl(file_desc, F_GETLK, &region_to_test);
          if (res == -1) {
              AppLogger::log(AppLogger::ERROR_L, "Process " + std::to_string(my_pid) + ": F_GETLK failed: " + strerror(errno));
              close(file_desc);
              return EXIT_FAILURE;
          }

          if (region_to_test.l_type != F_UNLCK) { // Nếu l_type không phải F_UNLCK, có xung đột
              AppLogger::log(AppLogger::WARNING_L, "Process " + std::to_string(my_pid) + ": F_RDLCK would fail. Conflicting lock found:");
              show_lock_info(&region_to_test);
          } else {
              AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": F_RDLCK - Lock would succeed.");
          }
      }

      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Closing file.");
      close(file_desc);
      return EXIT_SUCCESS;
  }
  ```

  * **Cách biên dịch:**
    **Bash**

    ```
    g++ lock4.cpp -o lock4
    ```
  * **Cách chạy và phân tích:**

    1. Đảm bảo `lock3.cpp` đang chạy nền: `./lock3 &`
    2. Chạy `lock4.cpp`: `./lock4`
    3. **Phân tích output:**
       * Bạn sẽ thấy nhiều vùng được báo "Lock would succeed" (vì `lock3` không khóa chúng).
       * Tại vùng 10-29 (khóa đọc của `lock3`):
         * Test `F_WRLCK` (khóa ghi): Sẽ báo "Lock would fail" với `l_type 0` (F_RDLCK), PID là của `lock3`. Đúng, khóa đọc của `lock3` ngăn khóa ghi của `lock4`.
         * Test `F_RDLCK` (khóa đọc): Sẽ báo "Lock would succeed". Đúng, nhiều khóa đọc được phép.
       * Tại vùng 40-49 (khóa ghi của `lock3`):
         * Test `F_WRLCK` (khóa ghi): Sẽ báo "Lock would fail" với `l_type 1` (F_WRLCK), PID của `lock3`. Đúng, khóa ghi ngăn khóa ghi khác.
         * Test `F_RDLCK` (khóa đọc): Sẽ báo "Lock would fail" với `l_type 1` (F_WRLCK), PID của `lock3`. Đúng, khóa ghi ngăn khóa đọc.

#### **2.4. Khóa Cạnh tranh (Competing Locks) 🥊**

* **Lý thuyết:** Khi nhiều tiến trình cùng cố gắng đặt khóa trên cùng một vùng file, chúng sẽ "cạnh tranh" nhau.

  * **Hành vi `F_SETLK` (không chặn):** Nếu không thể đặt khóa, nó trả về lỗi ngay lập tức.
  * **Hành vi `F_SETLKW` (có chặn):** Nếu không thể đặt khóa, tiến trình sẽ bị **tạm dừng (block)** cho đến khi khóa có thể được đặt hoặc bị gián đoạn bởi một tín hiệu.
* **Deadlock (Tắc nghẽn) 💀:** Một tình huống nguy hiểm khi hai hoặc nhiều tiến trình đang chờ tài nguyên mà các tiến trình khác đang giữ, tạo ra một chu kỳ chờ đợi vô hạn.

  * **Ví dụ:** Tiến trình A khóa Vùng 1, muốn khóa Vùng 2. Tiến trình B khóa Vùng 2, muốn khóa Vùng 1. Cả A và B đều bị chặn chờ.
  * **Giải pháp:**
    * Luôn cố gắng đặt các khóa theo một **thứ tự nhất quán** (ví dụ: luôn khóa byte nhỏ trước byte lớn).
    * Sử dụng `F_SETLK` (không chặn) và xử lý lỗi bằng cách thử lại sau, hoặc sử dụng timeout thay vì `F_SETLKW`.
    * Khóa một vùng lớn hơn để tránh chia nhỏ quá mức.
  * Kernel Linux không tự động phát hiện và phá vỡ deadlock cho các khóa `fcntl` (không như các hệ quản trị cơ sở dữ liệu thương mại).
* **Ví dụ (C++): `lock5.cpp` - Khóa Cạnh tranh và `F_SETLKW`**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <fcntl.h>    // For open, F_RDLCK, F_WRLCK, F_UNLCK, F_SETLK, F_SETLKW, struct flock
  #include <unistd.h>   // For close, sleep, getpid
  #include <cstdlib>    // For EXIT_FAILURE, EXIT_SUCCESS
  #include <cstring>    // For memset, strerror
  #include <errno.h>    // For errno

  // Logger namespace
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

  const char *test_file = "/tmp/test_lock_file";

  int main() {
      int file_desc;
      int res;
      struct flock region_to_lock;
      pid_t my_pid = getpid();

      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Starting lock competition test.");

      // 1. Mở file (phải đã được tạo và khóa bởi lock3.cpp)
      file_desc = open(test_file, O_RDWR);
      if (file_desc == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Process " + std::to_string(my_pid) + ": Unable to open " + std::string(test_file) + " (ensure lock3.cpp is running): " + strerror(errno));
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Opened " + std::string(test_file) + " with FD " + std::to_string(file_desc));

      // --- Thử khóa đọc một vùng không bị khóa ---
      memset(&region_to_lock, 0, sizeof(region_to_lock));
      region_to_lock.l_type = F_RDLCK; region_to_lock.l_whence = SEEK_SET; region_to_lock.l_start = 0; region_to_lock.l_len = 5;
      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Trying F_RDLCK on region 0-4.");
      res = fcntl(file_desc, F_SETLK, &region_to_lock);
      if (res == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Process " + std::to_string(my_pid) + ": Failed to lock region 0-4: " + strerror(errno));
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Obtained lock on region 0-4.");
      }

      // --- Thử mở khóa một vùng mà mình đã khóa (hoặc không) ---
      memset(&region_to_lock, 0, sizeof(region_to_lock));
      region_to_lock.l_type = F_UNLCK; region_to_lock.l_whence = SEEK_SET; region_to_lock.l_start = 0; region_to_lock.l_len = 5;
      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Trying F_UNLCK on region 0-4.");
      res = fcntl(file_desc, F_SETLK, &region_to_lock);
      if (res == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Process " + std::to_string(my_pid) + ": Failed to unlock region 0-4: " + strerror(errno));
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Unlocked region 0-4.");
      }

      // --- Thử khóa ghi một vùng bị khóa đọc bởi lock3.cpp (bytes 10-29) ---
      memset(&region_to_lock, 0, sizeof(region_to_lock));
      region_to_lock.l_type = F_WRLCK; region_to_lock.l_whence = SEEK_SET; region_to_lock.l_start = 16; region_to_lock.l_len = 5;
      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Trying F_WRLCK on region 16-20 (conflicts with lock3's RDLCK on 10-29).");
      res = fcntl(file_desc, F_SETLK, &region_to_lock);
      if (res == -1) {
          AppLogger::log(AppLogger::WARNING_L, "Process " + std::to_string(my_pid) + ": Failed to lock region 16-20 (expected): " + strerror(errno));
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Obtained lock on region 16-20 (unexpected).");
      }

      // --- Thử khóa đọc một vùng bị khóa ghi bởi lock3.cpp (bytes 40-49) ---
      memset(&region_to_lock, 0, sizeof(region_to_lock));
      region_to_lock.l_type = F_RDLCK; region_to_lock.l_whence = SEEK_SET; region_to_lock.l_start = 40; region_to_lock.l_len = 10;
      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Trying F_RDLCK on region 40-49 (conflicts with lock3's WRLCK on 40-49).");
      res = fcntl(file_desc, F_SETLK, &region_to_lock);
      if (res == -1) {
          AppLogger::log(AppLogger::WARNING_L, "Process " + std::to_string(my_pid) + ": Failed to lock region 40-49 (expected): " + strerror(errno));
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Obtained lock on region 40-49 (unexpected).");
      }

      // --- Thử khóa ghi một vùng với F_SETLKW (có chờ) ---
      memset(&region_to_lock, 0, sizeof(region_to_lock));
      region_to_lock.l_type = F_WRLCK; region_to_lock.l_whence = SEEK_SET; region_to_lock.l_start = 16; region_to_lock.l_len = 5;
      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Trying F_WRLCK with WAIT (F_SETLKW) on region 16-20.");
      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Will block until lock3.cpp releases its lock.");

      res = fcntl(file_desc, F_SETLKW, &region_to_lock); // Sẽ chặn tại đây
      if (res == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Process " + std::to_string(my_pid) + ": F_SETLKW failed: " + strerror(errno));
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Obtained lock on region 16-20 after waiting.");
      }

      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Ending. Closing file.");
      close(file_desc);
      return EXIT_SUCCESS;
  }
  ```

  * **Cách biên dịch:**
    **Bash**

    ```
    g++ lock5.cpp -o lock5
    ```
  * **Cách chạy và phân tích:**

    1. Đảm bảo `lock3.cpp` đang chạy nền: `./lock3 &`
    2. Chạy `lock5.cpp`: `./lock5`
    3. Quan sát output: Bạn sẽ thấy `lock5` báo lỗi khi cố gắng khóa các vùng đang bị khóa độc quyền bởi `lock3`. Sau đó, `lock5` sẽ bị chặn (không in output nào) khi nó gọi `F_SETLKW` trên vùng `16-20`.
    4. Sau 60 giây, `lock3` sẽ kết thúc và giải phóng các khóa của nó. Ngay lập tức, `lock5` sẽ được "giải tỏa" (unblocked), giành được khóa và tiếp tục chạy.
    5. Output minh họa rõ ràng cách `F_SETLKW` hoạt động và các xung đột khóa.

---

### **Câu hỏi Tự đánh giá Module 2 🤔**

1. Giải thích sự khác biệt giữa Khóa Tư vấn (Advisory Locks) và Khóa Bắt buộc (Mandatory Locks) trong Linux. Loại nào phổ biến hơn và tại sao?
2. Bạn muốn tạo một file khóa đơn giản. Viết lệnh `open()` với các cờ `oflags` phù hợp để đảm bảo việc tạo file này là atomic và duy nhất. Điều gì xảy ra nếu file đã tồn tại?
3. Khi sử dụng `fcntl()` để khóa vùng file, phân biệt `F_RDLCK` và `F_WRLCK`. Quy tắc nào áp dụng khi nhiều tiến trình cố gắng giành được các loại khóa này trên cùng một vùng?
4. Tại sao việc sử dụng `read()` và `write()` (System Calls) được khuyến nghị hơn `fread()` và `fwrite()` (Standard I/O Library) khi làm việc với khóa vùng file?
5. Giải thích sự khác biệt về hành vi giữa `F_SETLK` và `F_SETLKW`. Nêu một tình huống bạn sẽ dùng `F_SETLK` và một tình huống dùng `F_SETLKW`.
6. `Deadlock` là gì trong ngữ cảnh khóa file? Nêu một ví dụ về cách `deadlock` có thể xảy ra và một cách cơ bản để phòng tránh nó.

---

### **Bài tập Thực hành Module 2 ✍️**

1. **Chương trình Khóa Tài nguyên Hợp tác (Cải tiến):**
   * Dựa trên `lock2.cpp` của bạn:
     * Thêm một hàm `int acquire_lock(const char *lock_file_path)` và `void release_lock(int file_desc, const char *lock_file_path)`.
     * Hàm `acquire_lock` sẽ cố gắng `open()` file khóa với `O_CREAT | O_EXCL`. Nếu thành công, trả về FD. Nếu thất bại, nó sẽ **thử lại định kỳ** (ví dụ: mỗi 1 giây, tối đa 10 lần) cho đến khi có thể lấy được khóa.
     * Hàm `release_lock` sẽ `close()` FD và `unlink()` file khóa.
     * Thay thế vòng lặp `while (tries--)` bằng một vòng lặp vô hạn và kiểm tra `acquire_lock`.
     * Khi tiến trình giữ khóa, nó sẽ làm việc (in PID và `sleep(1)`) và sau đó giải phóng khóa.
   * **Thử thách:** Chạy 3-4 bản sao của chương trình này cùng lúc (`./my_coop_lock & ./my_coop_lock & ./my_coop_lock`). Quan sát cách chúng hợp tác.
2. **Chương trình Mô phỏng Client/Server với Khóa Vùng:**
   * Viết hai chương trình C++: `server_locker.cpp` và `client_accessor.cpp`.
   * **`server_locker.cpp`:**
     * Tạo một file dữ liệu `shared_data.bin` có kích thước 1000 bytes, điền toàn bộ bằng ký tự 'A'.
     * Đặt một khóa ghi (`F_WRLCK`) trên vùng từ byte 0 đến 99 và một khóa đọc (`F_RDLCK`) trên vùng từ byte 100 đến 199.
     * Server sẽ giữ các khóa này và đi vào vòng lặp vô hạn để chờ tín hiệu `SIGTERM` để thoát (sử dụng `sigaction` và cờ toàn cục).
   * **`client_accessor.cpp`:**
     * Nhận 3 tham số dòng lệnh: `<offset>`, `<length>`, `<lock_type>` ("read" hoặc "write").
     * Mở `shared_data.bin`.
     * Cố gắng đặt khóa (`F_SETLK`) theo `<lock_type>` trên vùng từ `<offset>` với `<length>`.
     * Nếu khóa thành công, in ra "Acquired lock on [offset]-[length] for [lock_type]".
     * Nếu khóa thất bại, in ra "Failed to acquire lock on [offset]-[length] for [lock_type]: [lý do xung đột]".
     * Nếu khóa thành công, chương trình sẽ ghi 10 byte ký tự 'B' vào vùng đó (nếu là khóa ghi) hoặc đọc 10 byte (nếu là khóa đọc) và in ra.
     * Sau đó, giải phóng khóa và thoát.
   * **Thử thách:**
     * Chạy `server_locker.cpp` nền.
     * Chạy `client_accessor.cpp` với các tham số khác nhau để kiểm tra sự tương tác của các khóa. Ví dụ:
       * `./client_accessor 0 5 read`
       * `./client_accessor 10 5 write` (sẽ lỗi)
       * `./client_accessor 100 10 write` (sẽ lỗi)
       * `./client_accessor 150 10 read` (sẽ thành công)

---

Bạn đã hoàn thành Module 1 về Quản lý Bộ nhớ. Module 2 về Khóa File này sẽ trang bị cho bạn các kỹ năng để đảm bảo tính toàn vẹn dữ liệu trong các ứng dụng đa nhiệm. Hãy dành thời gian để hiểu và thực hành thật kỹ. Khi bạn đã sẵn sàng, hãy cho tôi biết để chuyển sang  **Module 3: Cơ sở dữ liệu DBM** !
