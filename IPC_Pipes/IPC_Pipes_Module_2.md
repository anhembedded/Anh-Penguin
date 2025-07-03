
# **Module 2: System Call `pipe()` và `dup()`/`dup2()` ⚙️💧**

#### **2.1. System Call `pipe()`: Tạo Unnamed Pipe (Pipe không tên) 🔗**

* **Lý thuyết:** Khác với `popen()` là một hàm thư viện cấp cao, `pipe()` là một **System Call** cấp thấp. Nó tạo ra một "pipe không tên" (unnamed pipe) – một kênh giao tiếp một chiều giữa các tiến trình liên quan (thường là cha và con).

  * **Syntax:**
    **C++**

    ```
    #include <unistd.h> // For pipe, read, write
    // int pipe(int file_descriptor[2]);
    ```
  * **`file_descriptor[2]`** : Đây là một mảng hai số nguyên File Descriptor mà hàm `pipe()` sẽ điền vào:
  * `file_descriptor[0]` (hay `pipefd[0]`): Là đầu **đọc** của pipe (read end).
  * `file_descriptor[1]` (hay `pipefd[1]`): Là đầu **ghi** của pipe (write end).
  * **Nguyên tắc FIFO (First-In, First-Out):** Dữ liệu được ghi vào đầu ghi (`pipefd[1]`) sẽ được đọc từ đầu đọc (`pipefd[0]`) theo đúng thứ tự đã ghi.
  * **Giá trị trả về:** `0` nếu thành công, `-1` nếu thất bại (và `errno` được thiết lập, ví dụ: `EMFILE` - quá nhiều FD đang mở, `ENFILE` - bảng file hệ thống đầy).
  * **Đặc điểm:** Pipe không tên chỉ có thể được sử dụng giữa các tiến trình có chung nguồn gốc (ví dụ: cha và con sau `fork()`), vì các file descriptor của pipe được kế thừa qua `fork()`.
  * **Lưu ý quan trọng:** Vì `pipe()` trả về File Descriptor (`int`), bạn phải dùng các System Call cấp thấp như **`read()`** và **`write()`** để trao đổi dữ liệu qua pipe này, không phải các hàm `stdio` như `fread()`/`fwrite()`.
* **Minh họa (Pipe concept):**
  **Code snippet**

  ```
  graph TD
      P1(Process A) -->|write(pipefd[1])| Pipe[Pipe Buffer (FIFO)]
      Pipe -->|read(pipefd[0])| P2(Process B)
  ```
* **Liên hệ Embedded Linux:** `pipe()` là cơ chế cơ bản để tạo các đường ống dữ liệu nội bộ giữa các phần khác nhau của ứng dụng đa tiến trình của bạn, ví dụ: một tiến trình thu thập dữ liệu và một tiến trình khác xử lý hoặc lưu trữ nó.
* **Ví dụ (C++): `pipe_simple.cpp` - Tạo và dùng Pipe trong một tiến trình**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <unistd.h>   // For pipe, read, write
  #include <cstdlib>    // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>    // For memset, strlen, strerror
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
      int pipe_fds[2]; // Mảng 2 File Descriptor cho pipe
      const char some_data[] = "Hello Pipe World!";
      char buffer[BUFSIZ + 1]; // BUFSIZ from stdio.h, but used here for generic buffer size
      ssize_t data_processed;

      memset(buffer, '\0', sizeof(buffer)); // Khởi tạo buffer

      AppLogger::log(AppLogger::INFO_L, "--- Demonstrating simple pipe() ---");

      // Tạo pipe
      if (pipe(pipe_fds) == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Failed to create pipe: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Pipe created successfully.");
      AppLogger::log(AppLogger::INFO_L, "Read end FD: " + std::to_string(pipe_fds[0]) + ", Write end FD: " + std::to_string(pipe_fds[1]));

      // Ghi dữ liệu vào đầu ghi của pipe
      AppLogger::log(AppLogger::INFO_L, "Writing data to pipe's write end (FD " + std::to_string(pipe_fds[1]) + "): '" + some_data + "'");
      data_processed = write(pipe_fds[1], some_data, strlen(some_data));
      if (data_processed == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Write to pipe failed: " + std::string(strerror(errno)));
          close(pipe_fds[0]); close(pipe_fds[1]);
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Wrote " + std::to_string(data_processed) + " bytes to pipe.");

      // Đọc dữ liệu từ đầu đọc của pipe
      AppLogger::log(AppLogger::INFO_L, "Reading data from pipe's read end (FD " + std::to_string(pipe_fds[0]) + ")...");
      data_processed = read(pipe_fds[0], buffer, BUFSIZ);
      if (data_processed == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Read from pipe failed: " + std::string(strerror(errno)));
          close(pipe_fds[0]); close(pipe_fds[1]);
          return EXIT_FAILURE;
      } else if (data_processed == 0) {
          AppLogger::log(AppLogger::WARNING_L, "Read 0 bytes (EOF).");
      } else {
          buffer[data_processed] = '\0'; // Null-terminate the string
          AppLogger::log(AppLogger::SUCCESS_L, "Read " + std::to_string(data_processed) + " bytes: '" + std::string(buffer) + "'");
      }

      // Đóng các File Descriptor của pipe
      AppLogger::log(AppLogger::INFO_L, "Closing pipe File Descriptors.");
      close(pipe_fds[0]);
      close(pipe_fds[1]);
      AppLogger::log(AppLogger::SUCCESS_L, "Pipe FDs closed.");

      AppLogger::log(AppLogger::INFO_L, "--- pipe() Demonstration Finished ---");

      return EXIT_SUCCESS;
  }
  ```

#### **2.2. `pipe()` qua `fork()`: Giao tiếp giữa Cha và Con 👨‍👧‍👦**

* **Lý thuyết:** Lợi thế thực sự của `pipe()` xuất hiện khi bạn kết hợp nó với `fork()`.

  1. **Tiến trình cha tạo pipe:** Tiến trình cha gọi `pipe()` để tạo một pipe, nhận hai File Descriptor.
  2. **`fork()`:** Khi `fork()` được gọi, tiến trình con sẽ nhận được **bản sao** của tất cả các File Descriptor đang mở của tiến trình cha, bao gồm cả hai đầu của pipe.
  3. **Đóng các đầu không dùng:**
     * Nếu tiến trình cha sẽ **ghi** vào pipe và tiến trình con sẽ  **đọc** , thì:
       * Tiến trình cha đóng đầu đọc của pipe (`pipefd[0]`).
       * Tiến trình con đóng đầu ghi của pipe (`pipefd[1]`).
     * Nếu tiến trình cha sẽ **đọc** từ pipe và tiến trình con sẽ  **ghi** , thì ngược lại.
  4. **Trao đổi dữ liệu:** Sau khi đóng các đầu không cần thiết, cha và con có thể giao tiếp một chiều qua pipe.
* **Minh họa (Pipe qua `fork()`):**
  **Code snippet**

  ```
  graph TD
      P[Parent Process] -- pipefd[1] (write) --> Pipe[Pipe Buffer]
      Pipe -- pipefd[0] (read) --> C[Child Process]
  ```
* **Ví dụ (C++): `pipe_fork.cpp` - Pipe qua `fork()`**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <unistd.h>   // For pipe, read, write, fork, getpid
  #include <cstdlib>    // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>    // For memset, strlen, strerror
  #include <errno.h>    // For errno
  #include <sys/wait.h> // For wait (optional, for parent to wait)

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
      int pipe_fds[2];
      const char parent_data[] = "Hello from Parent!";
      char child_buffer[BUFSIZ + 1];
      pid_t fork_result;
      ssize_t data_processed;

      memset(child_buffer, '\0', sizeof(child_buffer));

      AppLogger::log(AppLogger::INFO_L, "--- Demonstrating pipe() across fork() ---");

      if (pipe(pipe_fds) == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Failed to create pipe: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Pipe created. Read FD: " + std::to_string(pipe_fds[0]) + ", Write FD: " + std::to_string(pipe_fds[1]));

      fork_result = fork(); // Tạo tiến trình con

      if (fork_result == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Fork failure: " + std::string(strerror(errno)));
          close(pipe_fds[0]); close(pipe_fds[1]);
          return EXIT_FAILURE;
      }

      if (fork_result == 0) {
          // Đây là mã của tiến trình con
          AppLogger::log(AppLogger::INFO_L, "Child Process (PID: " + std::to_string(getpid()) + "): Started.");
          close(pipe_fds[1]); // Con không ghi, đóng đầu ghi của pipe

          AppLogger::log(AppLogger::INFO_L, "Child Process: Reading data from pipe's read end (FD " + std::to_string(pipe_fds[0]) + ")...");
          data_processed = read(pipe_fds[0], child_buffer, BUFSIZ);
          if (data_processed == -1) {
              AppLogger::log(AppLogger::ERROR_L, "Child: Read failed: " + std::string(strerror(errno)));
          } else if (data_processed == 0) {
              AppLogger::log(AppLogger::WARNING_L, "Child: Read 0 bytes (EOF).");
          } else {
              child_buffer[data_processed] = '\0';
              AppLogger::log(AppLogger::SUCCESS_L, "Child: Read " + std::to_string(data_processed) + " bytes: '" + std::string(child_buffer) + "'");
          }
          close(pipe_fds[0]); // Đóng đầu đọc của pipe
          AppLogger::log(AppLogger::INFO_L, "Child Process: Exiting.");
          exit(EXIT_SUCCESS);
      } else {
          // Đây là mã của tiến trình cha
          AppLogger::log(AppLogger::INFO_L, "Parent Process (PID: " + std::to_string(getpid()) + "): Child created with PID: " + std::to_string(fork_result) + ".");
          close(pipe_fds[0]); // Cha không đọc, đóng đầu đọc của pipe

          AppLogger::log(AppLogger::INFO_L, "Parent Process: Writing data to pipe's write end (FD " + std::to_string(pipe_fds[1]) + "): '" + parent_data + "'");
          data_processed = write(pipe_fds[1], parent_data, strlen(parent_data));
          if (data_processed == -1) {
              AppLogger::log(AppLogger::ERROR_L, "Parent: Write failed: " + std::string(strerror(errno)));
          } else {
              AppLogger::log(AppLogger::SUCCESS_L, "Parent: Wrote " + std::to_string(data_processed) + " bytes to pipe.");
          }
          close(pipe_fds[1]); // Đóng đầu ghi của pipe

          // Cha chờ con hoàn thành
          AppLogger::log(AppLogger::INFO_L, "Parent Process: Waiting for child to finish...");
          wait(nullptr); 
          AppLogger::log(AppLogger::INFO_L, "Parent Process: Child finished. Exiting.");
          exit(EXIT_SUCCESS);
      }
  }
  ```
* **Cách Biên dịch:**
  **Bash**

  ```
  g++ pipe_fork.cpp -o pipe_fork
  ```
* **Cách Chạy:**
  **Bash**

  ```
  ./pipe_fork
  ```
* **Phân tích Output:** Bạn sẽ thấy tiến trình cha ghi dữ liệu, và tiến trình con đọc được dữ liệu đó từ pipe.

#### **2.3. `dup()` và `dup2()`: Chuyển hướng Standard I/O qua Pipe ➡️🔀⬅️**

* **Lý thuyết:** Khi sử dụng `pipe()` với `fork()` và `exec()`, chương trình con được `exec` khởi chạy cần biết File Descriptor nào để đọc/ghi từ pipe. Hàm `dup()` và `dup2()` giúp giải quyết vấn đề này bằng cách chuyển hướng các File Descriptor chuẩn (0: `stdin`, 1: `stdout`, 2: `stderr`).

  * **`dup(int oldfd)`:**
    * Tạo một bản sao của `oldfd`.
    * Bản sao này sẽ được gán cho số hiệu File Descriptor  **thấp nhất chưa được sử dụng** .
    * Trả về File Descriptor mới nếu thành công, `-1` nếu thất bại.
  * **`dup2(int oldfd, int newfd)`:**
    * Sao chép `oldfd` sang `newfd`.
    * Nếu `newfd` đã được mở, nó sẽ tự động bị **đóng trước** khi sao chép.
    * Nếu `oldfd` và `newfd` giống nhau, hàm không làm gì và trả về `newfd`.
    * Trả về `newfd` nếu thành công, `-1` nếu thất bại.
  * **Kỹ thuật chuyển hướng I/O cho tiến trình con (trước `exec()`):**
    1. **Trong tiến trình con (sau `fork()`):**
    2. **Đóng** `STDIN_FILENO` (0) hoặc `STDOUT_FILENO` (1) hoặc `STDERR_FILENO` (2) mặc định.
    3. Sử dụng `dup2(pipe_read_end, STDIN_FILENO)` để làm cho đầu đọc của pipe trở thành `stdin` mới của tiến trình con. Hoặc `dup2(pipe_write_end, STDOUT_FILENO)` để làm cho đầu ghi của pipe trở thành `stdout` mới.
    4. **Đóng các File Descriptor gốc** của pipe trong tiến trình con (ví dụ: `pipe_read_end`, `pipe_write_end`), vì chúng đã được sao chép vào các FD chuẩn.
    5. Gọi `exec()` chương trình con. Chương trình con sẽ đọc/ghi từ `stdin`/`stdout` như bình thường, nhưng thực tế là đang tương tác với pipe đã được chuyển hướng.
* **Minh họa (Pipe and `dup2`):**
  **Code snippet**

  ```
  graph TD
      P[Parent Process] -- pipefd[1] (write) --> Pipe[Pipe Buffer]
      Pipe -- pipefd[0] (read) --> C[Child Process]
      C -- Standard Input (FD 0) <== redirected from pipefd[0] --- Pipe
      C --- Standard Output (FD 1) --> Screen[Terminal Screen]
      style C fill:#f9f,stroke:#333,stroke-width:2px;
      style Pipe fill:#f0f0f0,stroke:#333,stroke-width:2px;
  ```

  * Trong hình trên, `Child Process` đọc từ `Standard Input (FD 0)` của nó, nhưng `FD 0` này đã được `dup2()` từ `pipefd[0]` của Pipe.
* **Liên hệ Embedded Linux:** Đây là kỹ thuật cực kỳ quan trọng để xây dựng các pipeline xử lý dữ liệu phức tạp giữa các ứng dụng trên thiết bị nhúng (ví dụ: một ứng dụng A ghi dữ liệu ra `stdout`, ứng dụng B đọc từ `stdin`, bạn dùng pipe để kết nối chúng). Nó cũng là cơ sở để tạo các daemon ghi log vào file thay vì terminal.
* **Ví dụ (C++): `pipe_dup2.cpp` - Chuyển hướng `stdout` của tiến trình con**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <unistd.h>   // For pipe, read, write, fork, execlp, close, dup2, getpid
  #include <fcntl.h>    // For open, O_WRONLY, O_CREAT, O_TRUNC
  #include <cstdlib>    // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>    // For memset, strlen, strerror
  #include <errno.h>    // For errno
  #include <sys/wait.h> // For wait (optional)

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
      int pipe_fds[2];
      pid_t pid;
      const char *output_filename = "child_redirected_output.txt";
      int file_fd;

      AppLogger::log(AppLogger::INFO_L, "Parent Process (PID: " + std::to_string(getpid()) + "): Starting I/O redirection example.");

      // 1. Tạo pipe
      if (pipe(pipe_fds) == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Parent: Failed to create pipe: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Parent: Pipe created. Read FD: " + std::to_string(pipe_fds[0]) + ", Write FD: " + std::to_string(pipe_fds[1]));

      // 2. Mở file đích cho output của Parent
      file_fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (file_fd == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Parent: Failed to open output file for self: " + std::string(strerror(errno)));
          close(pipe_fds[0]); close(pipe_fds[1]);
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Parent: Opened " + std::string(output_filename) + " with FD: " + std::to_string(file_fd));

      // 3. Fork để tạo tiến trình con
      pid = fork();
      if (pid == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Parent: Fork failed: " + std::string(strerror(errno)));
          close(pipe_fds[0]); close(pipe_fds[1]); close(file_fd);
          return EXIT_FAILURE;
      } else if (pid == 0) {
          // 4. Đây là mã của tiến trình con
          AppLogger::log(AppLogger::INFO_L, "Child Process (PID: " + std::to_string(getpid()) + "): Redirecting STDOUT to pipe.");

          // Đóng đầu đọc của pipe trong con (vì con sẽ chỉ ghi vào pipe)
          close(pipe_fds[0]);
          // Chuyển hướng STDOUT_FILENO (FD 1) sang đầu ghi của pipe (FD pipe_fds[1])
          if (dup2(pipe_fds[1], STDOUT_FILENO) == -1) {
              AppLogger::log(AppLogger::ERROR_L, "Child: dup2 failed for STDOUT: " + std::string(strerror(errno)));
              close(pipe_fds[1]);
              _exit(EXIT_FAILURE);
          }
          close(pipe_fds[1]); // Đóng FD gốc của đầu ghi pipe (nó đã được dup2)

          // Chương trình con thay thế chính nó bằng lệnh 'ls -l'
          AppLogger::log(AppLogger::INFO_L, "Child: Executing 'ls -l /'. Output will go to pipe.");
          execlp("ls", "ls", "-l", "/", (char *)0);

          // Nếu execlp trả về, có lỗi
          AppLogger::log(AppLogger::ERROR_L, "Child: execlp failed: " + std::string(strerror(errno)));
          _exit(EXIT_FAILURE);
      } else {
          // 5. Đây là mã của tiến trình cha
          AppLogger::log(AppLogger::INFO_L, "Parent Process (PID: " + std::to_string(getpid()) + "): Child (PID: " + std::to_string(pid) + ") created. Reading from pipe...");

          close(pipe_fds[1]); // Đóng đầu ghi của pipe trong cha (vì cha sẽ chỉ đọc từ pipe)

          char parent_read_buffer[BUFSIZ];
          ssize_t bytes_read;
          AppLogger::log(AppLogger::INFO_L, "Parent: Reading child's output from pipe (FD " + std::to_string(pipe_fds[0]) + ").");

          // Đọc output của con từ pipe và ghi vào file của cha
          while ((bytes_read = read(pipe_fds[0], parent_read_buffer, sizeof(parent_read_buffer))) > 0) {
              AppLogger::log(AppLogger::TRACE_L, "Parent: Read " + std::to_string(bytes_read) + " bytes from pipe. Writing to file.");
              write(file_fd, parent_read_buffer, bytes_read); // Ghi vào file của cha
          }
          if (bytes_read == -1) {
              AppLogger::log(AppLogger::ERROR_L, "Parent: Read from pipe failed: " + std::string(strerror(errno)));
          } else if (bytes_read == 0) {
              AppLogger::log(AppLogger::SUCCESS_L, "Parent: End of pipe detected.");
          }

          close(pipe_fds[0]); // Đóng đầu đọc của pipe
          close(file_fd); // Đóng file output của cha

          AppLogger::log(AppLogger::INFO_L, "Parent: Waiting for child to finish...");
          wait(nullptr); 
          AppLogger::log(AppLogger::INFO_L, "Parent: Child finished. Check '" + std::string(output_filename) + "' for redirected output.");
      }

      AppLogger::log(AppLogger::INFO_L, "--- dup2() Demonstration Finished ---");
      return EXIT_SUCCESS;
  }
  ```

  * **Cách Biên dịch:**
    **Bash**

    ```
    g++ pipe_dup2.cpp -o pipe_dup2
    ```
  * **Cách Chạy:**
    **Bash**

    ```
    ./pipe_dup2
    cat child_redirected_output.txt # Kiểm tra nội dung file
    ```
  * **Phân tích Output:** Bạn sẽ thấy output của lệnh `ls -l /` đã được chuyển hướng vào file `child_redirected_output.txt` thông qua pipe.

#### **2.4. Liên hệ với Windows và RTOS 🤝**

* **Windows:**
  * Windows có các hàm tương đương cho `dup()` và `dup2()`: `_dup()` và `_dup2()`.
  * `CreatePipe()` tạo ra các Anonymous Pipes tương tự `pipe()`. Bạn cần xử lý các `HANDLE` thay vì File Descriptor (`int`).
  * Việc chuyển hướng standard I/O cho tiến trình con trong Windows phức tạp hơn một chút, yêu cầu thiết lập cấu trúc `STARTUPINFO` và cờ `STARTF_USESTDHANDLES` khi gọi `CreateProcess()`.
* **RTOS (Real-Time Operating Systems) như FreeRTOS:**
  * RTOS không có khái niệm `pipe()` hoặc `dup2()` theo kiểu Unix/Linux.
  * Giao tiếp giữa các task trong RTOS thường không sử dụng File Descriptor mà thông qua các cơ chế nội bộ của RTOS như  **Message Queues (Hàng đợi tin nhắn)** .
  * Nếu bạn cần truyền dữ liệu giữa các task hoặc module, bạn sẽ sử dụng Message Queues để gửi các gói dữ liệu có cấu trúc. Không có khái niệm chuyển hướng standard I/O cho một "task" theo cách Unix.
* **Độc lập Kiến thức:** Các kiến thức về pipe, `dup()`/`dup2()` là **độc lập** với các Module khác về Processes, Signals, Threads. Mặc dù chúng thường được sử dụng cùng nhau (ví dụ: `fork()` + `pipe()` + `dup2()` + `exec()`), nhưng mỗi phần có API và mục đích riêng.

---

### **Câu hỏi Tự đánh giá Module 2 🤔**

1. System Call `pipe()` trả về những gì? Vai trò của `file_descriptor[0]` và `file_descriptor[1]` là gì?
2. Tại sao `pipe()` không thể được sử dụng để giao tiếp giữa hai tiến trình không liên quan?
3. Giải thích sự khác biệt giữa `dup()` và `dup2()`. Khi nào bạn sẽ dùng `dup2()` thay vì `dup()`?
4. Bạn muốn tạo một pipeline xử lý dữ liệu nơi output của `program_A` sẽ trở thành input của `program_B`. Hãy mô tả các bước chính và các hàm (`fork()`, `pipe()`, `dup2()`, `exec()`) mà bạn sẽ sử dụng.
5. Điều gì sẽ xảy ra nếu bạn không đóng các File Descriptor của pipe mà tiến trình cha và con không sử dụng sau `fork()`?
6. Nêu một hạn chế của `pipe()` (về hướng giao tiếp) và cách khắc phục nó nếu cần giao tiếp hai chiều.

---

### **Bài tập Thực hành Module 2 ✍️**

1. **Chương trình "Parent-Child Communication":**
   * Viết một chương trình C++ (`parent_child_comm.cpp`) mà:
     * Tạo một pipe.
     * `fork()` một tiến trình con.
     * Tiến trình cha sẽ ghi một chuỗi "Hello from Parent!" vào pipe.
     * Tiến trình con sẽ đọc dữ liệu từ pipe và in ra.
     * Cả hai tiến trình đều đóng các đầu pipe mà chúng không sử dụng.
   * **Thử thách:** Làm cho giao tiếp là hai chiều: cha gửi dữ liệu cho con, con xử lý và gửi lại cho cha, cha in ra kết quả cuối cùng. (Sẽ cần hai pipe).
2. **Chương trình "Simple `grep` Filter":**
   * Viết hai chương trình C++:
     * **`producer.cpp`:**
       * Nhận một tham số dòng lệnh là tên file input.
       * Mở file đó và đọc từng dòng.
       * Ghi từng dòng ra `stdout`.
     * **`consumer.cpp`:**
       * Nhận một tham số dòng lệnh là chuỗi cần tìm (keyword).
       * Đọc từng dòng từ `stdin`.
       * Nếu dòng đó chứa `keyword`, in dòng đó ra `stdout`.
   * Viết một script shell `run_grep.sh` hoặc một chương trình C++ `pipeline_runner.cpp` để:
     * Chạy `producer.cpp <file_input>`
     * Chuyển hướng `stdout` của `producer` vào `stdin` của `consumer.cpp <keyword>` bằng cách sử dụng `pipe()`, `fork()`, `dup2()`, và `execvp()`.
   * **Mục tiêu:** Mô phỏng lệnh shell `cat <file_input> | grep "keyword"`.
3. **Chương trình "Daemon với Pipe Feedback" (Nâng cao):**
   * Viết một chương trình C++ (`feedback_daemon.cpp`) mà:
     * Thực hiện `daemonize()` (như đã học) để chạy nền.
     * Mở một pipe (unnamed pipe).
     * `fork()` một tiến trình con.
     * Trong tiến trình con:
       * Chuyển hướng `stdout` của nó vào đầu ghi của pipe.
       * `execvp()` một lệnh hệ thống (ví dụ: `df -h`).
     * Trong tiến trình daemon (cha):
       * Đóng đầu ghi của pipe.
       * Đọc output từ đầu đọc của pipe.
       * Ghi output này vào file log của daemon (hoặc syslog).
       * Sau khi đọc hết, `wait()` cho tiến trình con.
   * **Mục tiêu:** Tạo ra một daemon chạy lệnh hệ thống và lưu output của lệnh đó vào log của daemon.
