# **Module 3: Named Pipes (FIFOs) 📁💧**

#### **3.1. Named Pipes (FIFOs) là gì? 💡**

* **Lý thuyết:**
  * **FIFO** là viết tắt của **F**irst **I**n, **F**irst **O**ut (Vào trước, ra trước), đây là nguyên tắc hoạt động của pipe.
  * **Named Pipe (FIFO)** là một loại pipe đặc biệt có một **tên (name)** và tồn tại như một **file đặc biệt** trong hệ thống file của Linux (có đường dẫn, ví dụ: `/tmp/my_fifo`).
  * Không giống như unnamed pipes (được tạo bằng `pipe()`) chỉ tồn tại trong bộ nhớ Kernel và chỉ có thể được sử dụng bởi các tiến trình có quan hệ cha-con, Named Pipes có thể được sử dụng để giao tiếp giữa **bất kỳ hai (hoặc nhiều hơn) tiến trình không liên quan (unrelated processes)** nào trên cùng một hệ thống.
  * Mặc dù nó xuất hiện như một file trong hệ thống file, nó không lưu trữ dữ liệu trên đĩa. Dữ liệu được truyền qua FIFO vẫn nằm trong bộ đệm Kernel, giống như unnamed pipes.
* **Minh họa (Named Pipe):**
  * Trong hình trên, `Process A` và `Process B` không cần có quan hệ cha-con. Chúng giao tiếp thông qua một FIFO có tên là `/tmp/my_fifo` trên filesystem.
* **Liên hệ Embedded Linux:** FIFOs cực kỳ hữu ích trên các thiết bị nhúng để:
  * Tạo các kênh giao tiếp đơn giản giữa các daemon chạy độc lập hoặc các ứng dụng khác nhau.
  * Cho phép các script shell hoặc các tiện ích hệ thống tương tác với ứng dụng C/C++ của bạn thông qua một "file" quen thuộc.
  * Là một giải pháp IPC nhẹ hơn so với Sockets cho giao tiếp cục bộ.

#### **3.2. Tạo và Truy cập FIFOs 🔑**

* **Lý thuyết:**

  * **Tạo FIFO (`mkfifo()`):**
    * Bạn có thể tạo Named Pipe từ dòng lệnh hoặc từ chương trình.
    * **Từ dòng lệnh:** `mkfifo /tmp/my_fifo` (hoặc `mknod /tmp/my_fifo p`).
    * **Từ chương trình C/C++:**
      **C++**

      ```
      #include <sys/stat.h> // For mkfifo, mode_t
      // int mkfifo(const char *filename, mode_t mode);
      ```

      * `filename`: Đường dẫn đến Named Pipe sẽ được tạo.
      * `mode`: Quyền hạn của Named Pipe (tương tự như `mode` trong `open()`, bị ảnh hưởng bởi `umask`).
      * Trả về `0` nếu thành công, `-1` nếu thất bại (ví dụ: `EEXIST` nếu đã tồn tại).
  * **Xóa FIFO:** Bạn có thể xóa Named Pipe giống như một file thông thường bằng lệnh `rm` trong shell, hoặc bằng System Call `unlink()` trong chương trình.
  * **Truy cập FIFO (`open()`, `read()`, `write()`):**
    * Vì FIFO xuất hiện trong hệ thống file, bạn có thể sử dụng các System Call file chuẩn (`open()`, `read()`, `write()`, `close()`) để thao tác với nó.
    * **Hành vi chặn (Blocking Behavior):**
      * Khi một tiến trình `open()` một FIFO để  **đọc (`O_RDONLY`)** , nó sẽ **bị chặn** cho đến khi có một tiến trình khác `open()` cùng FIFO đó để  **ghi (`O_WRONLY`)** .
      * Tương tự, khi một tiến trình `open()` một FIFO để  **ghi (`O_WRONLY`)** , nó sẽ **bị chặn** cho đến khi có một tiến trình khác `open()` cùng FIFO đó để  **đọc (`O_RDONLY`)** .
      * Đây là một cơ chế đồng bộ hóa tự động, hiệu quả về CPU (tiến trình bị chặn không tiêu thụ CPU).
* **Liên hệ Embedded Linux:** Việc tạo và quản lý FIFOs là cần thiết khi bạn muốn các thành phần phần mềm khác nhau (ví dụ: một ứng dụng giám sát và một daemon điều khiển) giao tiếp với nhau mà không cần cấu hình phức tạp.
* **Ví dụ (C++): `fifo_create_access.cpp` - Tạo và Truy cập FIFO**
  **C++**

  ```CPP
  #include <iostream>
  #include <string>
  #include <fcntl.h>    // For open, O_RDONLY, O_WRONLY, O_CREAT
  #include <sys/stat.h> // For mkfifo, mode_t
  #include <unistd.h>   // For close, unlink, getpid, sleep
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

  #define FIFO_NAME "/tmp/my_fifo_example"

  int main(int argc, char *argv[]) {
      int res;
      int fifo_fd;
      pid_t my_pid = getpid();

      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": FIFO Demonstration.");

      // 1. Tạo FIFO (nếu chưa tồn tại)
      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Attempting to create FIFO: " + FIFO_NAME);
      res = mkfifo(FIFO_NAME, 0666); // Quyền rw-rw-rw-
      if (res == -1) {
          if (errno == EEXIST) {
              AppLogger::log(AppLogger::WARNING_L, "Process " + std::to_string(my_pid) + ": FIFO already exists. Continuing.");
          } else {
              AppLogger::log(AppLogger::CRITICAL_L, "Process " + std::to_string(my_pid) + ": Failed to create FIFO: " + strerror(errno));
              return EXIT_FAILURE;
          }
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": FIFO created successfully.");
      }

      // 2. Kiểm tra chế độ chạy (reader/writer)
      if (argc < 2) {
          std::cout << "Usage: " << argv[0] << " [reader|writer]" << std::endl;
          AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": To test, run one terminal as reader and another as writer.");
          return EXIT_FAILURE;
      }

      std::string mode = argv[1];

      if (mode == "reader") {
          // Reader: Mở FIFO để đọc (sẽ chặn cho đến khi writer mở)
          AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Opening FIFO in O_RDONLY mode. Will block until writer opens.");
          fifo_fd = open(FIFO_NAME, O_RDONLY);
          if (fifo_fd == -1) {
              AppLogger::log(AppLogger::CRITICAL_L, "Process " + std::to_string(my_pid) + ": Failed to open FIFO for reading: " + strerror(errno));
              unlink(FIFO_NAME); // Dọn dẹp nếu có lỗi nghiêm trọng
              return EXIT_FAILURE;
          }
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": FIFO opened for reading. FD: " + std::to_string(fifo_fd));

          char buffer[BUFSIZ + 1];
          ssize_t bytes_read;
          AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Reading from FIFO...");

          // Đọc dữ liệu cho đến khi EOF (writer đóng)
          while ((bytes_read = read(fifo_fd, buffer, BUFSIZ)) > 0) {
              buffer[bytes_read] = '\0';
              AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Read " + std::to_string(bytes_read) + " bytes: '" + std::string(buffer) + "'");
          }
          if (bytes_read == -1) {
              AppLogger::log(AppLogger::ERROR_L, "Process " + std::to_string(my_pid) + ": Read error: " + strerror(errno));
          } else if (bytes_read == 0) {
              AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": End of file detected (writer closed).");
          }

          close(fifo_fd);
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": FIFO closed.");
      } else if (mode == "writer") {
          // Writer: Mở FIFO để ghi (sẽ chặn cho đến khi reader mở)
          AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Opening FIFO in O_WRONLY mode. Will block until reader opens.");
          fifo_fd = open(FIFO_NAME, O_WRONLY);
          if (fifo_fd == -1) {
              AppLogger::log(AppLogger::CRITICAL_L, "Process " + std::to_string(my_pid) + ": Failed to open FIFO for writing: " + strerror(errno));
              unlink(FIFO_NAME);
              return EXIT_FAILURE;
          }
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": FIFO opened for writing. FD: " + std::to_string(fifo_fd));

          const char data_to_send[] = "Hello from Writer!";
          ssize_t bytes_written = write(fifo_fd, data_to_send, strlen(data_to_send));
          if (bytes_written == -1) {
              AppLogger::log(AppLogger::ERROR_L, "Process " + std::to_string(my_pid) + ": Write error: " + strerror(errno));
          } else {
              AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Wrote " + std::to_string(bytes_written) + " bytes: '" + std::string(data_to_send) + "'");
          }

          AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Sleeping for 2 seconds before closing FIFO.");
          sleep(2); // Giữ FIFO mở một lát
          close(fifo_fd);
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": FIFO closed.");
      } else {
          std::cout << "Usage: " << argv[0] << " [reader|writer]" << std::endl;
          return EXIT_FAILURE;
      }

      // Dọn dẹp FIFO (chỉ nên làm bởi một bên, hoặc khi chắc chắn không còn ai dùng)
      // unlink(FIFO_NAME); 
      // AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": FIFO " + FIFO_NAME + " unlinked.");

      return EXIT_SUCCESS;
  }
  ```

  * **Cách Biên dịch:**
    **Bash**

    ```
    g++ fifo_create_access.cpp -o fifo_create_access
    ```
  * **Cách Chạy và Kiểm tra:**

    1. **Mở hai terminal.**
    2. **Terminal 1 (Reader):** `./fifo_create_access reader` (Nó sẽ bị chặn).
    3. **Terminal 2 (Writer):** `./fifo_create_access writer` (Nó sẽ mở, ghi dữ liệu, và sau đó Reader sẽ nhận được).
    4. Quan sát output.

#### **3.3. Chế độ Không chặn (`O_NONBLOCK`) và `PIPE_BUF` 💨**

* **Lý thuyết:**

  * **`O_NONBLOCK`:** Khi bạn `open()` một FIFO với cờ `O_NONBLOCK` (trong `oflags`), các thao tác `open()`, `read()`, `write()` sẽ không bị chặn.
    * **`open()` với `O_NONBLOCK`:**
      * Nếu mở để **đọc (`O_RDONLY | O_NONBLOCK`)** và không có tiến trình nào mở để ghi: `open()` sẽ thất bại ngay lập tức với `errno = ENXIO`.
      * Nếu mở để **ghi (`O_WRONLY | O_NONBLOCK`)** và không có tiến trình nào mở để đọc: `open()` sẽ thất bại ngay lập tức với `errno = ENXIO`.
    * **`read()` trên FIFO không chặn:**
      * Nếu FIFO trống, `read()` sẽ trả về `-1` với `errno = EAGAIN` hoặc `EWOULDBLOCK` (thay vì chặn). Nếu đọc 0 byte, đó là EOF (writer đã đóng).
    * **`write()` trên FIFO không chặn:**
      * Nếu FIFO đầy, `write()` sẽ trả về `-1` với `errno = EAGAIN` hoặc `EWOULDBLOCK` (thay vì chặn).
      * Nếu yêu cầu ghi lớn hơn `PIPE_BUF` và FIFO không thể nhận tất cả, nó có thể ghi một phần hoặc trả về 0 byte.
  * **`PIPE_BUF`:**
    * Là một hằng số được định nghĩa trong `<limits.h>` (thường là 4096 bytes trên Linux).
    * Nó xác định kích thước buffer nội bộ của pipe/FIFO.
    * **Tính nguyên tử (Atomicity):** Hệ thống đảm bảo rằng các thao tác `write()` có kích thước **nhỏ hơn hoặc bằng `PIPE_BUF`** vào một FIFO được mở ở chế độ chặn (`O_WRONLY`, không `O_NONBLOCK`) sẽ là  **atomic** . Tức là, toàn bộ khối dữ liệu sẽ được ghi vào FIFO hoặc không ghi gì cả, không bị xen kẽ bởi các `write` từ tiến trình khác. Điều này rất quan trọng khi nhiều writer cùng ghi vào một FIFO.
* **Liên hệ Embedded Linux:**

  * `O_NONBLOCK` rất hữu ích khi bạn không muốn ứng dụng bị treo khi chờ dữ liệu từ FIFO, cho phép nó thực hiện các tác vụ khác.
  * Hiểu `PIPE_BUF` và tính nguyên tử là quan trọng khi thiết kế các hệ thống đa writer/single reader để đảm bảo tính toàn vẹn của các "tin nhắn" nhỏ.
* **Ví dụ (C++): `fifo_nonblock.cpp` - FIFO với `O_NONBLOCK`**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <fcntl.h>    // For open, O_RDONLY, O_WRONLY, O_NONBLOCK, O_CREAT
  #include <sys/stat.h> // For mkfifo
  #include <unistd.h>   // For close, unlink, getpid, sleep, write, read
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

  #define FIFO_NAME "/tmp/my_fifo_nonblock_example"

  int main(int argc, char *argv[]) {
      int res;
      int fifo_fd;
      pid_t my_pid = getpid();

      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": FIFO Non-Blocking Demonstration.");

      // Tạo FIFO (nếu chưa tồn tại)
      mkfifo(FIFO_NAME, 0666); // Bỏ qua lỗi EEXIST

      if (argc < 2) {
          std::cout << "Usage: " << argv[0] << " [reader|writer]" << std::endl;
          AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": To test, run one terminal as reader and another as writer.");
          return EXIT_FAILURE;
      }

      std::string mode = argv[1];

      if (mode == "reader") {
          // Reader: Mở FIFO để đọc ở chế độ KHÔNG CHẶN
          AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Opening FIFO in O_RDONLY | O_NONBLOCK mode.");
          fifo_fd = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
          if (fifo_fd == -1) {
              if (errno == ENXIO) {
                  AppLogger::log(AppLogger::WARNING_L, "Process " + std::to_string(my_pid) + ": No writer is present. open() failed with ENXIO (expected).");
              } else {
                  AppLogger::log(AppLogger::CRITICAL_L, "Process " + std::to_string(my_pid) + ": Failed to open FIFO for reading: " + strerror(errno));
              }
              unlink(FIFO_NAME);
              return EXIT_FAILURE;
          }
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": FIFO opened for reading. FD: " + std::to_string(fifo_fd));

          char buffer[BUFSIZ + 1];
          ssize_t bytes_read;
          int loop_count = 0;
          AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Reading from FIFO (non-blocking loop)...");

          while (loop_count < 10) { // Đọc 10 lần để minh họa non-blocking
              bytes_read = read(fifo_fd, buffer, BUFSIZ);
              if (bytes_read == -1) {
                  if (errno == EAGAIN || errno == EWOULDBLOCK) {
                      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": No data available. Doing other work (sleeping 1s)...");
                      sleep(1); // Giả vờ làm việc khác
                  } else {
                      AppLogger::log(AppLogger::ERROR_L, "Process " + std::to_string(my_pid) + ": Read error: " + strerror(errno));
                      break;
                  }
              } else if (bytes_read == 0) {
                  AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": End of file detected (writer closed). Exiting loop.");
                  break;
              } else {
                  buffer[bytes_read] = '\0';
                  AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Read " + std::to_string(bytes_read) + " bytes: '" + std::string(buffer) + "'");
              }
              loop_count++;
          }
          close(fifo_fd);
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": FIFO closed.");
      } else if (mode == "writer") {
          // Writer: Mở FIFO để ghi ở chế độ KHÔNG CHẶN
          AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Opening FIFO in O_WRONLY | O_NONBLOCK mode.");
          fifo_fd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK);
          if (fifo_fd == -1) {
              if (errno == ENXIO) {
                  AppLogger::log(AppLogger::WARNING_L, "Process " + std::to_string(my_pid) + ": No reader is present. open() failed with ENXIO (expected).");
              } else {
                  AppLogger::log(AppLogger::CRITICAL_L, "Process " + std::to_string(my_pid) + ": Failed to open FIFO for writing: " + strerror(errno));
              }
              unlink(FIFO_NAME);
              return EXIT_FAILURE;
          }
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": FIFO opened for writing. FD: " + std::to_string(fifo_fd));

          const char data_to_send[] = "Hello from Non-Blocking Writer!";
          ssize_t bytes_written = write(fifo_fd, data_to_send, strlen(data_to_send));
          if (bytes_written == -1) {
              if (errno == EAGAIN || errno == EWOULDBLOCK) {
                  AppLogger::log(AppLogger::WARNING_L, "Process " + std::to_string(my_pid) + ": FIFO full or no reader. Write failed with EAGAIN/EWOULDBLOCK (expected).");
              } else {
                  AppLogger::log(AppLogger::ERROR_L, "Process " + std::to_string(my_pid) + ": Write error: " + strerror(errno));
              }
          } else {
              AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": Wrote " + std::to_string(bytes_written) + " bytes: '" + std::string(data_to_send) + "'");
          }

          AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": Sleeping for 2 seconds before closing FIFO.");
          sleep(2);
          close(fifo_fd);
          AppLogger::log(AppLogger::SUCCESS_L, "Process " + std::to_string(my_pid) + ": FIFO closed.");
      } else {
          std::cout << "Usage: " << argv[0] << " [reader|writer]" << std::endl;
          return EXIT_FAILURE;
      }

      // Dọn dẹp FIFO (chỉ nên làm bởi một bên, hoặc khi chắc chắn không còn ai dùng)
      unlink(FIFO_NAME); 
      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(my_pid) + ": FIFO " + FIFO_NAME + " unlinked.");

      return EXIT_SUCCESS;
  }
  ```

  * **Cách Biên dịch:**
    **Bash**

    ```
    g++ fifo_nonblock.cpp -o fifo_nonblock
    ```
  * **Cách Chạy và Kiểm tra:**

    1. **Chạy Reader trước (sẽ lỗi ENXIO):** `./fifo_nonblock reader` (Output: "No writer is present. open() failed with ENXIO (expected).")
    2. **Chạy Writer trước (sẽ lỗi ENXIO):** `./fifo_nonblock writer` (Output: "No reader is present. open() failed with ENXIO (expected).")
    3. **Chạy Reader và Writer đồng thời (Reader sẽ thấy "No data available"):**
       * Terminal 1: `./fifo_nonblock reader`
       * Terminal 2: `./fifo_nonblock writer`
       * Quan sát output của Reader sẽ có các dòng "No data available. Doing other work..." trước khi nó đọc được dữ liệu.
    4. **Kiểm tra tính nguyên tử (Atomicity):** (Khó minh họa bằng ví dụ đơn giản này, cần nhiều writer đồng thời)
       * Viết hai chương trình writer, mỗi cái ghi một chuỗi nhỏ hơn `PIPE_BUF`.
       * Chạy chúng đồng thời vào cùng một FIFO.
       * Một chương trình reader đọc và kiểm tra xem dữ liệu có bị lẫn lộn không. Nếu các `write` nhỏ hơn `PIPE_BUF` và FIFO được mở blocking, dữ liệu sẽ không bị lẫn lộn.

---

### **Câu hỏi Tự đánh giá Module 3 🤔**

1. Named Pipe (FIFO) khác với unnamed pipe (được tạo bằng `pipe()`) ở những điểm nào?
2. Tại sao Named Pipe có thể được sử dụng để giao tiếp giữa các tiến trình không liên quan, trong khi unnamed pipe thì không?
3. Bạn sẽ sử dụng hàm nào để tạo một Named Pipe từ chương trình C++?
4. Giải thích hành vi chặn (blocking behavior) của `open()` khi một tiến trình cố gắng mở một FIFO để đọc mà không có writer, hoặc để ghi mà không có reader.
5. Cờ `O_NONBLOCK` ảnh hưởng đến hành vi của `open()`, `read()`, và `write()` trên FIFO như thế nào?
6. `PIPE_BUF` là gì? Tại sao việc đảm bảo các `write()` có kích thước nhỏ hơn hoặc bằng `PIPE_BUF` lại quan trọng khi nhiều tiến trình cùng ghi vào một FIFO?

---

### **Bài tập Thực hành Module 3 ✍️**

1. **Chương trình "FIFO Chat":**
   * Viết hai chương trình C++: `fifo_chat_sender.cpp` và `fifo_chat_receiver.cpp`.
   * Cả hai chương trình sẽ sử dụng cùng một Named Pipe (ví dụ: `/tmp/chat_fifo`).
   * **`fifo_chat_sender.cpp`:**
     * Mở `/tmp/chat_fifo` ở chế độ ghi (`O_WRONLY`).
     * Trong vòng lặp, đọc một dòng từ `stdin` của người dùng.
     * Ghi dòng đó vào FIFO.
     * Thoát khi người dùng nhập "exit".
   * **`fifo_chat_receiver.cpp`:**
     * Mở `/tmp/chat_fifo` ở chế độ đọc (`O_RDONLY`).
     * Trong vòng lặp, đọc một dòng từ FIFO.
     * In dòng đó ra `stdout`.
     * Thoát khi đọc được "exit" hoặc khi FIFO bị đóng.
   * **Mục tiêu:** Chạy `receiver` trong một terminal và `sender` trong terminal khác. Gõ tin nhắn vào `sender` và thấy nó xuất hiện ở `receiver`.
2. **Chương trình "FIFO Monitor":**
   * Viết một chương trình C++ (`fifo_monitor.cpp`) mà:
     * Tạo một Named Pipe (ví dụ: `/tmp/monitor_fifo`).
     * Mở FIFO đó ở chế độ  **đọc không chặn (`O_RDONLY | O_NONBLOCK`)** .
     * Trong một vòng lặp vô hạn, cứ mỗi 1 giây:
       * Cố gắng đọc dữ liệu từ FIFO.
       * Nếu có dữ liệu, in ra "Received: [data]".
       * Nếu không có dữ liệu (lỗi `EAGAIN`/`EWOULDBLOCK`), in ra "No new data...".
       * Nếu FIFO bị đóng (read 0 bytes), in ra "Writer disconnected. Exiting." và thoát.
     * Chương trình sẽ tự động xóa FIFO khi thoát.
   * **Thử thách:**
     * Chạy `fifo_monitor.cpp` trong một terminal.
     * Mở terminal khác và dùng lệnh `echo "Hello" > /tmp/monitor_fifo` hoặc `echo "World" > /tmp/monitor_fifo` để gửi dữ liệu.
     * Quan sát cách `fifo_monitor` phản ứng với dữ liệu và khi không có dữ liệu.

---

