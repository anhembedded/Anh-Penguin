### **Module 7: Các Chủ đề Nâng cao (`fcntl` & `mmap`) 🚀**

#### **7.1. `fcntl()`: Điều khiển File Descriptor Nâng cao 🛠️**

* **Lý thuyết:** Hàm **`fcntl()`** (file control) là một System Call đa năng, được sử dụng để thực hiện nhiều thao tác điều khiển và truy vấn nâng cao trên một  **File Descriptor đã mở** . Nó cho phép bạn kiểm soát các thuộc tính của File Descriptor hoặc file mà nó trỏ tới.

  * **Syntax:**
    **C++**

    ```cpp
    #include <fcntl.h> // For fcntl, F_DUPFD, FD_CLOEXEC, etc.
    int fcntl(int fildes, int cmd, ... /* arg */);
    ```
  * **`fildes`** : File Descriptor cần thao tác.
  * **`cmd`** : Một mã lệnh xác định hành động cần thực hiện. Các lệnh này được định nghĩa trong `<fcntl.h>`.
  * **`... (arg)`** : Một đối số tùy chọn, kiểu `long` hoặc con trỏ, tùy thuộc vào lệnh `cmd`.
  * **Giá trị trả về:** Giá trị trả về phụ thuộc vào lệnh `cmd`. Thường là `0` cho thành công, `-1` cho thất bại (`errno` được thiết lập).
* **Các lệnh `fcntl()` phổ biến:**

  1. **Sao chép File Descriptor (`F_DUPFD`, `F_DUPFD_CLOEXEC`) 👯:**
     * `fcntl(fildes, F_DUPFD, newfd)`: Trả về một File Descriptor mới có giá trị số nguyên bằng hoặc lớn hơn `newfd`, là bản sao của `fildes`.
     * `fcntl(fildes, F_DUPFD_CLOEXEC, newfd)`: Tương tự `F_DUPFD`, nhưng tự động đặt cờ `FD_CLOEXEC` cho File Descriptor mới.
  2. **Lấy/Đặt Cờ File Descriptor (`F_GETFD`, `F_SETFD`) 🚩:**
     * `fcntl(fildes, F_GETFD)`: Trả về các cờ của File Descriptor.
     * `fcntl(fildes, F_SETFD, flags)`: Đặt các cờ cho File Descriptor.
     * **Cờ quan trọng nhất:** `FD_CLOEXEC` (Close-on-exec). Nếu cờ này được đặt, File Descriptor sẽ **tự động đóng** khi tiến trình gọi một trong các hàm `exec()` (ví dụ: `execlp`, `execve`). Điều này rất quan trọng để ngăn chặn các file descriptor không mong muốn bị kế thừa bởi các chương trình con sau `exec`.
  3. **Lấy/Đặt Cờ Trạng thái File (`F_GETFL`, `F_SETFL`) 📊:**
     * `fcntl(fildes, F_GETFL)`: Trả về các cờ trạng thái file và chế độ truy cập (ví dụ: `O_RDONLY`, `O_WRONLY`, `O_RDWR`, `O_APPEND`, `O_NONBLOCK`).
     * `fcntl(fildes, F_SETFL, flags)`: Đặt các cờ trạng thái file.
     * **Ứng dụng phổ biến:** Chuyển đổi một File Descriptor từ chế độ chặn (blocking) sang **không chặn (non-blocking)** bằng cách thêm cờ `O_NONBLOCK`.
       **C++**

       ```
       int flags = fcntl(fd, F_GETFL, 0); // Lấy cờ hiện tại
       if (flags == -1) { /* error */ }
       flags |= O_NONBLOCK; // Thêm cờ O_NONBLOCK
       if (fcntl(fd, F_SETFL, flags) == -1) { /* error */ }
       ```
     * **Lưu ý:** Bạn không thể thay đổi chế độ truy cập (`O_RDONLY`, `O_WRONLY`) hoặc quyền hạn file bằng `F_SETFL`.
  4. **Quản lý Khóa File Tư vấn (Advisory File Locking - `F_GETLK`, `F_SETLK`, `F_SETLKW`) 🔐:**
     * `fcntl()` cũng là System Call chính để thực hiện khóa file, cho phép đồng bộ hóa quyền truy cập vào các vùng file giữa các tiến trình. (Chúng ta đã học chi tiết về File Locking trong chương "Data Management").
     * `F_GETLK`: Kiểm tra khóa.
     * `F_SETLK`: Đặt/giải phóng khóa (không chặn).
     * `F_SETLKW`: Đặt/giải phóng khóa (có chặn).
* **Liên hệ Embedded Linux:**

  * **Kiểm soát kế thừa FD:** Đảm bảo các tiến trình con không kế thừa các FD nhạy cảm hoặc không cần thiết bằng `FD_CLOEXEC`.
  * **I/O không chặn:** Chuyển đổi các FD sang chế độ `O_NONBLOCK` khi bạn cần thực hiện I/O mà không muốn chương trình bị treo (ví dụ: đọc từ cổng serial, socket mà không biết dữ liệu có sẵn hay không).
  * **Khóa file:** Đồng bộ hóa truy cập vào các file cấu hình hoặc dữ liệu trên thiết bị nhúng khi có nhiều daemon cùng truy cập.
* **Ví dụ (C++): Sử dụng `fcntl()` để đặt cờ `FD_CLOEXEC` và `O_NONBLOCK`**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <fcntl.h>    // For fcntl, O_RDONLY, FD_CLOEXEC, O_NONBLOCK, F_GETFL, F_SETFL, F_GETFD, F_SETFD
  #include <unistd.h>   // For fork, execvp, close, read, sleep
  #include <sys/wait.h> // For waitpid
  #include <errno.h>    // For errno
  #include <string.h>   // For strerror

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

  int main() {
      const char *test_file = "fcntl_test.txt";
      int fd;

      // Tạo một file để test
      fd = open(test_file, O_RDWR | O_CREAT | O_TRUNC, 0644);
      if (fd == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Failed to create test file: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Created test file: " + std::string(test_file) + " with FD " + std::to_string(fd));

      // --- Ví dụ 1: Đặt cờ FD_CLOEXEC ---
      AppLogger::log(AppLogger::INFO_L, "--- Demonstrating FD_CLOEXEC ---");
      int flags = fcntl(fd, F_GETFD);
      if (flags == -1) {
          AppLogger::log(AppLogger::ERROR_L, "F_GETFD failed: " + std::string(strerror(errno)));
          close(fd);
          unlink(test_file);
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::INFO_L, "Current FD flags: " + std::to_string(flags) + (flags & FD_CLOEXEC ? " (FD_CLOEXEC set)" : " (FD_CLOEXEC not set)"));

      // Đặt cờ FD_CLOEXEC
      flags |= FD_CLOEXEC;
      if (fcntl(fd, F_SETFD, flags) == -1) {
          AppLogger::log(AppLogger::ERROR_L, "F_SETFD failed: " + std::string(strerror(errno)));
          close(fd);
          unlink(test_file);
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "FD_CLOEXEC set for FD " + std::to_string(fd) + ".");

      // Fork và exec một chương trình con (ví dụ: 'ls')
      // File descriptor 'fd' sẽ tự động đóng trong tiến trình con sau exec
      pid_t pid = fork();
      if (pid == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Fork failed: " + std::string(strerror(errno)));
          close(fd);
          unlink(test_file);
          return EXIT_FAILURE;
      } else if (pid == 0) { // Child process
          AppLogger::log(AppLogger::INFO_L, "Child Process (PID: " + std::to_string(getpid()) + "): Executing 'ls -l'.");
          // Thử mở lại file test_file.txt trong child, nó sẽ có FD mới nếu FD cũ đã đóng
          int child_fd = open(test_file, O_RDONLY);
          if (child_fd == -1) {
              AppLogger::log(AppLogger::WARNING_L, "Child: File " + std::string(test_file) + " is NOT inherited, needs to be reopened. Error: " + strerror(errno));
          } else {
              AppLogger::log(AppLogger::SUCCESS_L, "Child: File " + std::string(test_file) + " IS inherited (unexpected if FD_CLOEXEC set). New FD: " + std::to_string(child_fd));
              close(child_fd);
          }
          char* args[] = { (char*)"ls", (char*)"-l", (char*)"/proc/self/fd", nullptr };
          execvp(args[0], args); // execvp sẽ đóng 'fd' này
          AppLogger::log(AppLogger::ERROR_L, "Execvp failed in child: " + std::string(strerror(errno)));
          _exit(EXIT_FAILURE);
      } else { // Parent process
          int status;
          waitpid(pid, &status, 0);
          AppLogger::log(AppLogger::INFO_L, "Parent: Child process finished. Now closing original FD " + std::to_string(fd));
          close(fd); // Đóng FD trong parent
          AppLogger::log(AppLogger::SUCCESS_L, "FD " + std::to_string(fd) + " closed in parent.");
      }

      // --- Ví dụ 2: Đặt cờ O_NONBLOCK ---
      AppLogger::log(AppLogger::INFO_L, "\n--- Demonstrating O_NONBLOCK ---");
      int nonblock_fd = open("/dev/stdin", O_RDONLY); // Mở stdin (terminal)
      if (nonblock_fd == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Failed to open /dev/stdin: " + std::string(strerror(errno)));
          unlink(test_file);
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Opened /dev/stdin with FD " + std::to_string(nonblock_fd));

      // Lấy cờ hiện tại
      flags = fcntl(nonblock_fd, F_GETFL, 0);
      if (flags == -1) {
          AppLogger::log(AppLogger::ERROR_L, "F_GETFL failed for nonblock_fd: " + std::string(strerror(errno)));
          close(nonblock_fd);
          unlink(test_file);
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::INFO_L, "Current flags for /dev/stdin: " + std::to_string(flags) + (flags & O_NONBLOCK ? " (O_NONBLOCK set)" : " (O_NONBLOCK not set)"));

      // Đặt cờ O_NONBLOCK
      flags |= O_NONBLOCK;
      if (fcntl(nonblock_fd, F_SETFL, flags) == -1) {
          AppLogger::log(AppLogger::ERROR_L, "F_SETFL failed to set O_NONBLOCK: " + std::string(strerror(errno)));
          close(nonblock_fd);
          unlink(test_file);
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "O_NONBLOCK set for FD " + std::to_string(nonblock_fd) + ".");

      char buffer[10];
      AppLogger::log(AppLogger::INFO_L, "Attempting to read from /dev/stdin (non-blocking). Type something quickly!");
      sleep(1); // Give user a moment to type

      ssize_t bytes_read = read(nonblock_fd, buffer, sizeof(buffer));
      if (bytes_read == -1) {
          if (errno == EAGAIN || errno == EWOULDBLOCK) {
              AppLogger::log(AppLogger::WARNING_L, "Read returned EAGAIN/EWOULDBLOCK: No data immediately available (expected).");
          } else {
              AppLogger::log(AppLogger::ERROR_L, "Read failed: " + std::string(strerror(errno)));
          }
      } else if (bytes_read == 0) {
          AppLogger::log(AppLogger::WARNING_L, "Read returned 0 bytes (EOF or no data).");
      } else {
          buffer[bytes_read] = '\0';
          AppLogger::log(AppLogger::SUCCESS_L, "Read " + std::to_string(bytes_read) + " bytes: '" + std::string(buffer) + "'");
      }

      close(nonblock_fd);
      unlink(test_file); // Dọn dẹp
      return EXIT_SUCCESS;
  }
  ```

#### **7.2. `mmap()`: Ánh xạ Bộ nhớ File (Memory-Mapped Files) 🧠**

* **Lý thuyết:** Hàm **`mmap()`** (memory map) cung cấp một cách mạnh mẽ để **ánh xạ (map)** một phần hoặc toàn bộ nội dung của một file vào không gian địa chỉ bộ nhớ ảo của tiến trình. Sau khi ánh xạ, bạn có thể truy cập và thao tác với nội dung file như thể nó là một mảng trong bộ nhớ, sử dụng các con trỏ và toán tử `[]` thông thường, thay vì phải dùng `read()`/`write()`.

  * **Syntax:**
    **C++**

    ```
    #include <sys/mman.h> // For mmap, munmap, msync
    void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
    ```
  * **`addr`** : Địa chỉ gợi ý nơi Kernel nên ánh xạ file vào bộ nhớ ảo của tiến trình. Thường đặt là `0` (hoặc `NULL`) để Kernel tự động chọn địa chỉ phù hợp.
  * **`len`** : Độ dài (tính bằng byte) của vùng file cần ánh xạ vào bộ nhớ.
  * **`prot` (Protection - Quyền bảo vệ):** Xác định quyền truy cập vào vùng bộ nhớ được ánh xạ (bitwise OR các cờ):

    * `PROT_READ`: Cho phép đọc.
    * `PROT_WRITE`: Cho phép ghi.
    * `PROT_EXEC`: Cho phép thực thi (nếu vùng chứa mã lệnh).
    * `PROT_NONE`: Không cho phép truy cập.
  * **`flags` (Cờ):** Kiểm soát cách vùng bộ nhớ được ánh xạ và cách các thay đổi được phản ánh:

    * `MAP_SHARED`: Các thay đổi được thực hiện trong bộ nhớ sẽ được **ghi trở lại file** và hiển thị cho các tiến trình khác cũng ánh xạ cùng file đó. (Dùng cho IPC hoặc lưu trữ bền vững).
    * `MAP_PRIVATE`: Các thay đổi chỉ có hiệu lực cục bộ trong không gian bộ nhớ của tiến trình hiện tại và  **không được ghi trở lại file gốc** . (Dùng cho các bản sao riêng tư của file).
    * `MAP_FIXED`: Yêu cầu Kernel ánh xạ vào địa chỉ `addr` cụ thể (ít dùng, không portable).
  * **`fildes`** : File Descriptor của file đã mở cần ánh xạ.
  * **`off`** : Offset (tính bằng byte) từ đầu file nơi bắt đầu ánh xạ. Thường là `0` để ánh xạ từ đầu file.
  * **Giá trị trả về:** Con trỏ `void*` tới đầu vùng bộ nhớ được ánh xạ nếu thành công, `MAP_FAILED` nếu thất bại (`errno` được thiết lập).
* **`msync()`: Đồng bộ hóa thay đổi 🔄**

  * **Lý thuyết:** Khi bạn thay đổi dữ liệu trong vùng bộ nhớ được ánh xạ, các thay đổi đó có thể không được ghi ngay lập tức xuống file vật lý. `msync()` ép buộc các thay đổi phải được ghi (hoặc đọc) từ file.
  * **Syntax:** `int msync(void *addr, size_t len, int flags);`
  * **`addr`** : Địa chỉ bắt đầu của vùng bộ nhớ cần đồng bộ hóa.
  * **`len`** : Độ dài của vùng cần đồng bộ hóa.
  * **`flags`** :
  * `MS_ASYNC`: Ghi không đồng bộ (trả về ngay).
  * `MS_SYNC`: Ghi đồng bộ (chờ ghi hoàn tất).
  * `MS_INVALIDATE`: Đọc lại dữ liệu từ file vào bộ nhớ (loại bỏ các thay đổi chưa được ghi).
* **`munmap()`: Giải phóng ánh xạ 🗑️**

  * **Lý thuyết:** Giải phóng vùng bộ nhớ đã được ánh xạ bởi `mmap()`.
  * **Syntax:** `int munmap(void *addr, size_t len);`
  * **`addr`** : Địa chỉ trả về bởi `mmap()`.
  * **`len`** : Độ dài của vùng đã ánh xạ.
* **Ưu điểm của `mmap()`:**

  * **Hiệu suất cao:** Truy cập file nhanh như truy cập bộ nhớ RAM, vì Kernel tự động xử lý việc đọc/ghi từ đĩa vào bộ nhớ nền. Tránh overhead của `read()`/`write()` System Calls lặp đi lặp lại.
  * **IPC hiệu quả:** Là một trong những cơ chế IPC hiệu quả nhất để chia sẻ dữ liệu lớn giữa các tiến trình (kết hợp với `MAP_SHARED`).
  * **Đơn giản hóa code:** Thao tác với file thông qua con trỏ và mảng đơn giản hơn nhiều so với việc quản lý buffer và offset thủ công.
* **Hạn chế của `mmap()`:**

  * **Phức tạp hơn:** Yêu cầu hiểu biết về bộ nhớ ảo và cách Kernel quản lý trang.
  * **Xử lý lỗi:** Lỗi truy cập bộ nhớ (ví dụ: truy cập ngoài vùng ánh xạ) sẽ gây `SIGSEGV`.
  * **Đồng bộ hóa:** Nếu nhiều tiến trình/luồng cùng ghi vào vùng nhớ chia sẻ, cần cơ chế đồng bộ hóa riêng (mutex, semaphore) để tránh race conditions.
* **Liên hệ Embedded Linux:**

  * **Truy cập file cấu hình/dữ liệu lớn:** Ánh xạ các file chứa dữ liệu lớn (ví dụ: lookup tables, firmware images) vào bộ nhớ để truy cập nhanh chóng.
  * **IPC tốc độ cao:** Chia sẻ dữ liệu giữa các daemon hoặc ứng dụng khác nhau trên thiết bị nhúng mà không cần sao chép dữ liệu.
  * **Tương tác với phần cứng (Memory-mapped I/O):** Trong một số trường hợp (ví dụ: driver), `mmap()` có thể được dùng để ánh xạ các thanh ghi phần cứng vào không gian địa chỉ của tiến trình để truy cập trực tiếp.
* **Ví dụ (C++): Sử dụng `mmap()` để cập nhật file cấu trúc**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <vector>
  #include <sys/mman.h> // For mmap, munmap, msync, PROT_*, MAP_*
  #include <fcntl.h>    // For open, O_RDWR, O_CREAT, O_TRUNC
  #include <unistd.h>   // For close, unlink, ftruncate
  #include <stdio.h>    // For sprintf (C-style string formatting)
  #include <stdlib.h>   // For EXIT_FAILURE, EXIT_SUCCESS
  #include <errno.h>    // For errno
  #include <string.h>   // For strerror

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

  // Định nghĩa cấu trúc bản ghi
  typedef struct {
      int integer_value;
      char string_data[24];
  } RECORD;

  #define NRECORDS 10 // Số lượng bản ghi

  int main() {
      const char *filename = "records.dat";
      int fd;
      RECORD *mapped_records; // Con trỏ tới vùng bộ nhớ được ánh xạ
      size_t file_size = NRECORDS * sizeof(RECORD);

      // --- 1. Tạo và ghi dữ liệu ban đầu vào file ---
      AppLogger::log(AppLogger::INFO_L, "--- Creating initial records.dat file ---");
      fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
      if (fd == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Failed to open file: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      }

      // Đảm bảo file có đủ kích thước cho NRECORDS
      if (ftruncate(fd, file_size) == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Failed to truncate file: " + std::string(strerror(errno)));
          close(fd);
          unlink(filename);
          return EXIT_FAILURE;
      }

      // Ghi dữ liệu ban đầu bằng cách ánh xạ bộ nhớ
      mapped_records = (RECORD *)mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
      if (mapped_records == MAP_FAILED) {
          AppLogger::log(AppLogger::CRITICAL_L, "mmap failed for initial write: " + std::string(strerror(errno)));
          close(fd);
          unlink(filename);
          return EXIT_FAILURE;
      }

      for (int i = 0; i < NRECORDS; ++i) {
          mapped_records[i].integer_value = i;
          sprintf(mapped_records[i].string_data, "RECORD-%02d", i);
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Initial records written to file via mmap.");

      // Đồng bộ hóa ngay lập tức để đảm bảo dữ liệu được ghi vào đĩa
      if (msync(mapped_records, file_size, MS_SYNC) == -1) {
          AppLogger::log(AppLogger::WARNING_L, "msync failed after initial write: " + std::string(strerror(errno)));
      }

      // Giải phóng ánh xạ sau khi ghi (chúng ta sẽ ánh xạ lại để đọc/sửa)
      if (munmap(mapped_records, file_size) == -1) {
          AppLogger::log(AppLogger::WARNING_L, "munmap failed after initial write: " + std::string(strerror(errno)));
      }
      close(fd); // Đóng file descriptor

      // --- 2. Đọc và sửa đổi bản ghi bằng mmap ---
      AppLogger::log(AppLogger::INFO_L, "\n--- Reading and modifying records via mmap ---");
      fd = open(filename, O_RDWR); // Mở lại file
      if (fd == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Failed to reopen file: " + std::string(strerror(errno)));
          unlink(filename);
          return EXIT_FAILURE;
      }

      // Ánh xạ lại file vào bộ nhớ
      mapped_records = (RECORD *)mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
      if (mapped_records == MAP_FAILED) {
          AppLogger::log(AppLogger::CRITICAL_L, "mmap failed for modification: " + std::string(strerror(errno)));
          close(fd);
          unlink(filename);
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "File mapped to memory at address: " + std::to_string(reinterpret_cast<long long>(mapped_records)));

      // Đọc và in ra bản ghi số 5
      int record_index_to_check = 5;
      AppLogger::log(AppLogger::INFO_L, "Record " + std::to_string(record_index_to_check) + " (before modification): Integer = " + std::to_string(mapped_records[record_index_to_check].integer_value) + ", String = '" + mapped_records[record_index_to_check].string_data + "'");

      // Sửa đổi bản ghi số 5
      mapped_records[record_index_to_check].integer_value = 500;
      sprintf(mapped_records[record_index_to_check].string_data, "MODIFIED-%03d", 500);
      AppLogger::log(AppLogger::INFO_L, "Modified Record " + std::to_string(record_index_to_check) + ".");

      // Đọc và in ra bản ghi số 5 sau khi sửa đổi
      AppLogger::log(AppLogger::INFO_L, "Record " + std::to_string(record_index_to_check) + " (after modification): Integer = " + std::to_string(mapped_records[record_index_to_check].integer_value) + ", String = '" + mapped_records[record_index_to_check].string_data + "'");

      // Đồng bộ hóa thay đổi trở lại file
      AppLogger::log(AppLogger::INFO_L, "Synchronizing changes to file (MS_ASYNC)...");
      if (msync(mapped_records, file_size, MS_ASYNC) == -1) {
          AppLogger::log(AppLogger::WARNING_L, "msync failed during modification: " + std::string(strerror(errno)));
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Changes synchronized.");

      // Giải phóng ánh xạ
      if (munmap(mapped_records, file_size) == -1) {
          AppLogger::log(AppLogger::WARNING_L, "munmap failed: " + std::string(strerror(errno)));
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Memory unmapped.");
      close(fd); // Đóng file descriptor

      // --- 3. Xác nhận thay đổi bằng cách đọc file thông thường ---
      AppLogger::log(AppLogger::INFO_L, "\n--- Verifying changes by reading file normally ---");
      fd = open(filename, O_RDONLY);
      if (fd == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Failed to open file for verification: " + std::string(strerror(errno)));
          unlink(filename);
          return EXIT_FAILURE;
      }
      RECORD read_record;
      // Di chuyển đến bản ghi số 5 và đọc
      if (lseek(fd, record_index_to_check * sizeof(RECORD), SEEK_SET) == -1) {
          AppLogger::log(AppLogger::ERROR_L, "lseek failed for verification: " + std::string(strerror(errno)));
      } else {
          ssize_t bytes_read = read(fd, &read_record, sizeof(RECORD));
          if (bytes_read == sizeof(RECORD)) {
              AppLogger::log(AppLogger::SUCCESS_L, "Verified Record " + std::to_string(record_index_to_check) + " from file: Integer = " + std::to_string(read_record.integer_value) + ", String = '" + read_record.string_data + "'");
          } else {
              AppLogger::log(AppLogger::ERROR_L, "Failed to read record for verification. Bytes read: " + std::to_string(bytes_read));
          }
      }
      close(fd);

      // Dọn dẹp
      unlink(filename);
      AppLogger::log(AppLogger::INFO_L, "\nFile " + std::string(filename) + " cleaned up.");
      return EXIT_SUCCESS;
  }
  ```

---

### **Câu hỏi Tự đánh giá Module 7 🤔**

1. Hàm `fcntl()` được sử dụng để làm gì? Nêu 3 loại thao tác khác nhau mà `fcntl()` có thể thực hiện.
2. Giải thích ý nghĩa của cờ `FD_CLOEXEC`. Tại sao việc đặt cờ này lại quan trọng trong các chương trình sử dụng `fork()` và `exec()`?
3. Bạn có một File Descriptor `my_socket_fd` đang ở chế độ blocking. Viết đoạn code C++ sử dụng `fcntl()` để chuyển nó sang chế độ non-blocking.
4. Giải thích khái niệm "Memory-Mapped Files" và lợi ích chính của `mmap()` so với các thao tác `read()`/`write()` truyền thống.
5. Phân biệt giữa `MAP_SHARED` và `MAP_PRIVATE` khi sử dụng `mmap()`. Khi nào bạn sẽ dùng mỗi loại?
6. Tại sao cần sử dụng `msync()` sau khi sửa đổi dữ liệu trong một vùng bộ nhớ được ánh xạ bằng `mmap()` với `MAP_SHARED`? Phân biệt `MS_ASYNC` và `MS_SYNC`.
7. Nêu một trường hợp sử dụng thực tế của `mmap()` trong lập trình Embedded Linux.

---

### **Bài tập Thực hành Module 7 ✍️**

1. **Chương trình Kiểm soát `FD_CLOEXEC`:**
   * Viết một chương trình C++ (`fd_cloexec_test.cpp`) mà:
     * Tạo một file `temp_fd_test.txt`.
     * Mở file này và lấy File Descriptor `fd_test`.
     * **Không đặt** cờ `FD_CLOEXEC` cho `fd_test`.
     * `fork()` một tiến trình con.
     * Trong tiến trình con:
       * `execvp()` lệnh `ls -l /proc/self/fd`.
       * Quan sát output để xem `fd_test` có xuất hiện trong danh sách các File Descriptor của tiến trình con sau `exec` không.
     * Lặp lại các bước trên, nhưng lần này **đặt cờ `FD_CLOEXEC`** cho `fd_test` trước khi `fork()` và `execvp()`.
     * Quan sát sự khác biệt trong output của `ls -l /proc/self/fd`.
     * Dọn dẹp file `temp_fd_test.txt`.
2. **Chương trình I/O Không Chặn trên Terminal:**
   * Viết một chương trình C++ (`non_blocking_input.cpp`) mà:
     * Mở `stdin` (FD 0) ở chế độ non-blocking bằng `fcntl()`.
     * Trong một vòng lặp vô hạn, cố gắng đọc một ký tự từ `stdin` bằng `read()`.
     * Nếu `read()` trả về `-1` với `errno` là `EAGAIN` hoặc `EWOULDBLOCK` (nghĩa là không có input ngay lập tức), in ra "No input yet..." và ngủ 1 giây.
     * Nếu `read()` đọc được ký tự, in ra ký tự đó và số byte đọc được.
     * Nếu `read()` trả về `0` (EOF, ví dụ: `Ctrl+D`), thoát chương trình.
     * **Thử thách:** Tích hợp `select()` (kiến thức từ chương Sockets) để chờ input một cách hiệu quả hơn thay vì vòng lặp `read()` liên tục.
3. **Chương trình Chia sẻ Bộ nhớ File giữa các Tiến trình (`mmap` IPC):**
   * Viết hai chương trình C++ riêng biệt:
     * **`mmap_writer.cpp`:**
       * Tạo một file `shared_data.bin` với kích thước đủ lớn (ví dụ: 1KB).
       * Ánh xạ file này vào bộ nhớ với `PROT_READ | PROT_WRITE` và `MAP_SHARED`.
       * Ghi một chuỗi "Hello from Writer [timestamp]" vào đầu vùng nhớ ánh xạ.
       * Dùng `msync()` với `MS_SYNC` để đảm bảo dữ liệu được ghi xuống đĩa.
       * Ngủ 5 giây.
       * Ghi một chuỗi khác "Writer finished [timestamp]".
       * Giải phóng ánh xạ và đóng file.
     * **`mmap_reader.cpp`:**
       * Mở file `shared_data.bin`.
       * Ánh xạ file này vào bộ nhớ với `PROT_READ` và `MAP_SHARED`.
       * Trong một vòng lặp, đọc và in ra nội dung của vùng nhớ ánh xạ mỗi 1 giây.
       * Quan sát cách nội dung thay đổi khi `mmap_writer.cpp` ghi dữ liệu.
       * Dừng khi bạn nhấn `Ctrl+C`.
   * **Cách chạy:**
     1. Biên dịch cả hai chương trình.
     2. Chạy `mmap_reader.cpp` trong một terminal.
     3. Trong terminal khác, chạy `mmap_writer.cpp`.
     4. Quan sát output của `mmap_reader.cpp`.
   * **Thử thách:** Thêm một `pthread_mutex_t` vào đầu vùng nhớ chia sẻ và sử dụng nó để đồng bộ hóa việc đọc/ghi giữa writer và reader, tránh race conditions.

---

Chúc mừng bạn đã hoàn thành toàn bộ các Module về "Working with Files" trong Linux Programming! Đây là một khối lượng kiến thức khổng lồ và rất quan trọng. Hãy dành thời gian để thực hành các bài tập nâng cao này. Khi bạn đã sẵn sàng, chúng ta có thể chuyển sang một chủ đề mới hoặc ôn lại bất kỳ phần nào bạn muốn!
