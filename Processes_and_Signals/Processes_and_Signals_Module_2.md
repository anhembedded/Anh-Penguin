# **Module 2: Khởi động Tiến trình Mới (Starting New Processes) 🚀**

#### **2.1. `system()`: Chạy Lệnh qua Shell 🐚**

* **Lý thuyết:** Hàm **`system()`** là một hàm thư viện chuẩn C/C++ đơn giản nhất để chạy một lệnh bên ngoài.

  * **Syntax:**
    **C++**

    ```
    #include <cstdlib> // For system
    // int system(const char *command);
    ```
  * **`command`** : Một chuỗi ký tự chứa lệnh shell bạn muốn thực thi (ví dụ: `"ls -l /tmp"`, `"ps -ax &"`).
  * **Cách hoạt động:**

    1. Hàm `system()` sẽ  **tạo một tiến trình con mới** .
    2. Tiến trình con này là một **shell** (thường là `/bin/sh` hoặc `/bin/bash`).
    3. Shell con đó sẽ thực thi `command` mà bạn cung cấp.
    4. Chương trình gốc của bạn (tiến trình cha) sẽ **bị tạm dừng** và **chờ** cho đến khi shell con và lệnh của nó hoàn thành.
    5. Khi lệnh và shell con kết thúc, hàm `system()` **trả về** cho chương trình gốc.
  * **Giá trị trả về:**

    * `127`: Nếu không thể khởi động shell.
    * `-1`: Nếu có lỗi khác.
    * Khác: Mã thoát (exit code) của lệnh đã thực thi.
  * **Ưu điểm:** Rất đơn giản và tiện lợi khi bạn chỉ cần chạy một lệnh shell nhanh chóng hoặc một script. Nó tự động xử lý việc phân tích lệnh và tìm kiếm trong `PATH`.
  * **Hạn chế:**

    * **Kém hiệu quả (Inefficient):** Tốn overhead vì phải khởi tạo một tiến trình shell con riêng biệt cho mỗi lần gọi.
    * **Ít kiểm soát:** Bạn không có kiểm soát trực tiếp đối với tiến trình con (ví dụ: chuyển hướng I/O, quản lý tín hiệu).
    * **Rủi ro bảo mật:** Nếu chuỗi `command` đến từ input của người dùng và không được làm sạch (sanitized) cẩn thận, nó có thể dẫn đến các lỗ hổng bảo mật (command injection).
* **Liên hệ Embedded Linux:** Dùng cho các tác vụ quản trị đơn giản hoặc khi gọi các script hệ thống. Ví dụ: `system("reboot");` hoặc `system("echo 'hello' > /sys/class/gpio/gpio1/value");`. Tuy nhiên, trong các ứng dụng quan trọng, thường ưu tiên các phương pháp cấp thấp hơn để tránh overhead và tăng kiểm soát.
* **Ví dụ (C++): `system_example.cpp` - Sử dụng `system()`**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <cstdlib> // For system, EXIT_SUCCESS, EXIT_FAILURE
  #include <unistd.h> // For sleep (optional)

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
      int result;

      AppLogger::log(AppLogger::INFO_L, "--- Demonstrating system() ---");

      // Ví dụ 1: Chạy lệnh 'ps -ax' và chờ nó hoàn thành
      AppLogger::log(AppLogger::INFO_L, "Calling system(\"ps -ax\") - will wait for it to finish.");
      result = system("ps -ax");
      if (result == -1) {
          AppLogger::log(AppLogger::ERROR_L, "system() failed: " + std::string(strerror(errno)));
      } else if (result == 127) {
          AppLogger::log(AppLogger::ERROR_L, "system() failed: Shell could not be started.");
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "system(\"ps -ax\") finished with exit code: " + std::to_string(result));
      }
      AppLogger::log(AppLogger::INFO_L, "Program continues after ps -ax finished.");

      // Ví dụ 2: Chạy lệnh 'sleep 3' ở chế độ nền và không chờ
      AppLogger::log(AppLogger::INFO_L, "\nCalling system(\"sleep 3 &\") - will NOT wait for it to finish.");
      result = system("sleep 3 &"); // Dấu '&' đưa lệnh vào nền
      if (result == -1) {
          AppLogger::log(AppLogger::ERROR_L, "system() failed: " + std::string(strerror(errno)));
      } else if (result == 127) {
          AppLogger::log(AppLogger::ERROR_L, "system() failed: Shell could not be started.");
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "system(\"sleep 3 &\") finished with exit code: " + std::to_string(result));
      }
      AppLogger::log(AppLogger::INFO_L, "Program continues immediately after sleep 3 & is launched.");
      AppLogger::log(AppLogger::INFO_L, "You may see the shell prompt before 'sleep 3' finishes.");
      sleep(4); // Cho phép sleep 3 có thời gian chạy và kết thúc

      AppLogger::log(AppLogger::INFO_L, "--- system() Demonstration Finished ---");

      return EXIT_SUCCESS;
  }
  ```

#### **2.2. `exec` Family: Thay thế Ảnh Tiến trình (Replacing a Process Image) ♻️**

* **Lý thuyết:** Họ các hàm **`exec`** (ví dụ: `execlp`, `execvp`, `execve`) được dùng để **thay thế hoàn toàn** mã lệnh và dữ liệu của tiến trình hiện tại bằng một chương trình mới.

  * **Quan trọng:** Hàm `exec`  **không tạo ra một tiến trình mới** . Nó "biến" tiến trình đang chạy thành một tiến trình mới chạy chương trình khác, giữ nguyên PID của tiến trình gốc.
  * Nếu `exec` thành công, nó **không bao giờ trả về** tới dòng code tiếp theo trong chương trình gọi nó. Nếu nó trả về (với giá trị `-1`), điều đó có nghĩa là có lỗi xảy ra.
  * **Các hàm trong họ `exec`:**
    * **`execl(path, arg0, ..., (char *)0)`** : `l` cho "list" (đối số dạng danh sách), `path` là đường dẫn tuyệt đối hoặc tương đối đến chương trình.
    * **`execlp(file, arg0, ..., (char *)0)`** : `l` cho "list", `p` cho "PATH". `file` là tên chương trình, sẽ được tìm kiếm trong biến môi trường `PATH`.
    * **`execle(path, arg0, ..., (char *)0, envp[])`** : `l` cho "list", `e` cho "environment". Cho phép bạn chỉ định một môi trường mới cho chương trình con.
    * **`execv(path, argv[])`** : `v` cho "vector" (đối số dạng mảng `char *const argv[]`), `path` là đường dẫn.
    * **`execvp(file, argv[])`** : `v` cho "vector", `p` cho "PATH". `file` được tìm kiếm trong `PATH`.
    * **`execve(path, argv[], envp[])`** : `v` cho "vector", `e` cho "environment". Mạnh mẽ nhất, cho phép chỉ định cả đối số dạng mảng và môi trường mới.
  * **Kế thừa:** Tiến trình mới được `exec` khởi chạy kế thừa nhiều thuộc tính từ tiến trình gốc, quan trọng nhất là các **file descriptor đang mở** (trừ khi cờ `FD_CLOEXEC` được đặt).
* **Liên hệ Embedded Linux:** Cực kỳ quan trọng khi xây dựng các daemon, dịch vụ nền, hoặc các công cụ hệ thống. Ví dụ, một chương trình `init` tùy chỉnh sẽ `exec` các dịch vụ khác.
* **Ví dụ (C++): `exec_example.cpp` - Sử dụng `execlp()`**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <cstdlib> // For exit, EXIT_SUCCESS, EXIT_FAILURE
  #include <unistd.h> // For execlp, getpid
  #include <map>
  #include <cstring>

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
      AppLogger::log(AppLogger::INFO_L, "Main program (PID: " + std::to_string(getpid()) + ") is about to execute a new program.");
      AppLogger::log(AppLogger::INFO_L, "It will replace itself with 'ls -l /tmp'.");

      // execlp(file, arg0, arg1, ..., (char *)0)
      // file: "ls" (sẽ tìm trong PATH)
      // arg0: "ls" (tên chương trình cho argv[0] của ls)
      // arg1: "-l"
      // arg2: "/tmp"
      // (char *)0: Dấu hiệu kết thúc danh sách đối số
      execlp("ls", "ls", "-l", "/tmp", (char *)0);

      // Dòng này chỉ được chạy nếu execlp thất bại (ví dụ: không tìm thấy 'ls')
      AppLogger::log(AppLogger::ERROR_L, "execlp failed: " + std::string(strerror(errno)));
      return EXIT_FAILURE;
  }

  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ exec_example.cpp -o exec_example
    ./exec_example
    ```
  * **Phân tích:** Bạn sẽ thấy output của lệnh `ls -l /tmp`. Điều quan trọng là dòng "Main program (PID: ...)" sẽ được in, nhưng dòng "execlp failed..." **sẽ không bao giờ được in** nếu `ls` được tìm thấy và chạy thành công. Chương trình `exec_example` đã bị thay thế hoàn toàn bởi `ls`.

#### **2.3. `fork()`: Sao chép Ảnh Tiến trình (Duplicating a Process Image) 🧬**

* **Lý thuyết:** Hàm **`fork()`** là System Call cơ bản để **tạo một tiến trình con mới** bằng cách nhân bản tiến trình hiện tại (tiến trình cha).

  * **Syntax:**
    **C++**

    ```cpp
    #include <unistd.h> // For fork, getpid, getppid
    // pid_t fork(void);
    ```
  * **Cách hoạt động:**

    1. Khi `fork()` được gọi, Kernel tạo một bản sao gần như y hệt của tiến trình cha.
    2. Cả tiến trình cha và tiến trình con đều tiếp tục thực thi từ **ngay sau** lệnh `fork()`.
    3. **Giá trị trả về:** Đây là điểm phân biệt giữa cha và con:
       * Trong  **tiến trình con** , `fork()` trả về  **`0`** .
       * Trong  **tiến trình cha** , `fork()` trả về **PID của tiến trình con** vừa được tạo.
       * Nếu `fork()` thất bại, nó trả về `-1` (và `errno` được thiết lập, ví dụ: `EAGAIN` nếu vượt quá giới hạn tiến trình con, `ENOMEM` nếu hết bộ nhớ ảo).
  * **Tài nguyên:** Tiến trình con nhận được bản sao riêng của không gian dữ liệu, stack, heap, và môi trường. Tuy nhiên, chúng **chia sẻ cùng mã lệnh** (read-only) và các **file descriptor đang mở** (khi `fork()` được gọi, tất cả các FD đang mở của cha đều được sao chép sang con).
* **Mô hình `fork()` + `exec` (The Standard Way):** Đây là cách phổ biến nhất để tạo một tiến trình mới để chạy một chương trình khác.

  1. Tiến trình cha gọi `fork()` để tạo một tiến trình con.
  2. Trong nhánh của tiến trình con (khi `fork()` trả về `0`), tiến trình con gọi một trong các hàm `exec()` để nạp và chạy chương trình mới.
  3. Trong nhánh của tiến trình cha (khi `fork()` trả về PID của con), tiến trình cha có thể tiếp tục công việc của mình hoặc chờ tiến trình con kết thúc bằng `wait()`/`waitpid()`.
* **Liên hệ Embedded Linux:** Cực kỳ quan trọng để:

  * Khởi chạy các daemon hoặc dịch vụ nền.
  * Phân chia công việc thành các tiến trình độc lập.
  * Tạo ra các "wrapper" hoặc "launcher" cho các ứng dụng khác.

  📌 **Hàm `wait()` trong C/C++ (trên hệ điều hành Unix/Linux)** dùng để:

  * **Chặn tiến trình cha** cho đến khi  **một tiến trình con kết thúc** .
  * **Thu thập mã thoát (exit status)** của tiến trình con để tránh tạo ra tiến trình zombie.

  ---

  ### 🧪 Cách dùng cơ bản:


  ```cpp
  #include <sys/wait.h>  // wait()
  #include <unistd.h>    // fork()
  #include <iostream>

  int main() {
      pid_t pid = fork();

      if (pid == 0) {
          // Tiến trình con
          std::cout << "Child PID: " << getpid() << std::endl;
          return 42; // Mã thoát
      } else {
          // Tiến trình cha
          int status;
          wait(&status); // Chờ con kết thúc

          if (WIFEXITED(status)) {
              std::cout << "Child exited with code: " << WEXITSTATUS(status) << std::endl;
          }
      }

      return 0;
  }
  ```

  ---

  ### 🧠 Một số macro hữu ích:

  | Macro              | Ý nghĩa                                                                                  |
  | ------------------ | ------------------------------------------------------------------------------------------ |
  | `WIFEXITED(s)`   | Trả về true nếu tiến trình con kết thúc bình thường (`exit()`hoặc `return`) |
  | `WEXITSTATUS(s)` | Lấy mã thoát (exit code) nếu `WIFEXITED(s)`là true                                  |
  | `WIFSIGNALED(s)` | Trả về true nếu tiến trình con bị kết thúc bởi tín hiệu (signal)                |
  | `WTERMSIG(s)`    | Lấy số hiệu tín hiệu đã kết thúc tiến trình con                                 |

  ---

  ### 🧩 Ghi nhớ:

  * Nếu có  **nhiều tiến trình con** , `wait()` sẽ chờ **bất kỳ** tiến trình nào kết thúc.
  * Nếu muốn chờ  **một tiến trình cụ thể** , dùng `waitpid(pid, &status, 0);`.
* **Ví dụ (C++): `fork_example.cpp` - Sử dụng `fork()`**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <cstdlib>  // For exit, EXIT_SUCCESS, EXIT_FAILURE
  #include <unistd.h> // For fork, getpid, getppid, sleep
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
      pid_t pid;
      char *message;
      int n_loops;

      AppLogger::log(AppLogger::INFO_L, "Fork program starting. PID: " + std::to_string(getpid()));

      pid = fork(); // Tạo tiến trình con

      if (pid == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Fork failed: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      } else if (pid == 0) {
          // Đây là mã của tiến trình con
          message = (char*)"This is the CHILD process";
          n_loops = 5;
          AppLogger::log(AppLogger::INFO_L, "Child Process (PID: " + std::to_string(getpid()) + ", PPID: " + std::to_string(getppid()) + "): Started.");
      } else {
          // Đây là mã của tiến trình cha
          message = (char*)"This is the PARENT process";
          n_loops = 3;
          AppLogger::log(AppLogger::INFO_L, "Parent Process (PID: " + std::to_string(getpid()) + "): Created child with PID: " + std::to_string(pid));
      }

      // Cả cha và con đều thực thi đoạn code này
      for (int i = 0; i < n_loops; ++i) {
          AppLogger::log(AppLogger::INFO_L, std::string(message) + ": Iteration " + std::to_string(i + 1));
          sleep(1); // Giả vờ làm việc
      }

      // Nếu là tiến trình cha, chờ con kết thúc (để tránh output lộn xộn và zombie)
      if (pid > 0) {
          int status;
          AppLogger::log(AppLogger::INFO_L, "Parent Process (PID: " + std::to_string(getpid()) + "): Waiting for child to finish...");
          wait(&status); // Chờ bất kỳ tiến trình con nào
          AppLogger::log(AppLogger::SUCCESS_L, "Parent Process: Child finished with status: " + std::to_string(WEXITSTATUS(status)));
      }

      AppLogger::log(AppLogger::INFO_L, std::string(message) + ": Exiting.");
      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ fork_example.cpp -o fork_example
    ./fork_example
    ```
  * **Phân tích:** Bạn sẽ thấy output từ cả tiến trình cha và con xen kẽ nhau. Tiến trình cha sẽ chờ con kết thúc trước khi in thông báo cuối cùng.

#### **2.4. Chuyển hướng Đầu vào/Đầu ra (Input and Output Redirection) ➡️🔀⬅️**

* **Lý thuyết:** Khi bạn `fork()` và `exec()` một chương trình con, bạn có thể thay đổi các **file descriptor chuẩn** (`stdin` - 0, `stdout` - 1, `stderr` - 2) của tiến trình con để chúng trỏ đến các file hoặc pipes khác. Điều này cho phép chương trình con đọc/ghi từ/vào một nguồn khác thay vì terminal.

  * **Các hàm chính:**
    * `close(int fd)`: Đóng một file descriptor.
    * `dup(int oldfd)`: Tạo một bản sao của `oldfd` và gán cho nó số hiệu file descriptor  **thấp nhất có sẵn** .
    * `dup2(int oldfd, int newfd)`: Sao chép `oldfd` sang `newfd`. Nếu `newfd` đã mở, nó sẽ được đóng trước.
  * **Kỹ thuật:**
    1. Trong tiến trình con (sau `fork()`, trước `exec()`):
    2. `close(STDIN_FILENO)` (nếu muốn chuyển hướng input) hoặc `close(STDOUT_FILENO)` (nếu muốn chuyển hướng output).
    3. Gọi `dup(pipe_read_end)` hoặc `dup2(file_fd, STDOUT_FILENO)` để gán file descriptor của pipe/file vào vị trí của `stdin`/`stdout`.
    4. Đóng các file descriptor gốc của pipe/file trong tiến trình con (vì chúng đã được sao chép).
    5. Gọi `exec()` chương trình con. Chương trình con sẽ đọc/ghi từ `stdin`/`stdout` như bình thường, nhưng thực tế là đang tương tác với pipe/file đã được chuyển hướng.
* **Liên hệ Embedded Linux:** Rất quan trọng khi xây dựng các daemon (ghi output vào file log thay vì terminal), hoặc khi tạo các pipeline xử lý dữ liệu giữa các chương trình (ví dụ: output của một chương trình là input của chương trình khác).
* **Ví dụ (C++): `redirect_output.cpp` - Chuyển hướng `stdout` của tiến trình con**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <cstdlib>  // For exit, EXIT_SUCCESS, EXIT_FAILURE
  #include <unistd.h> // For fork, execlp, close, dup2, getpid
  #include <fcntl.h>  // For open, O_WRONLY, O_CREAT, O_TRUNC
  #include <sys/wait.h> // For wait

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
      pid_t pid;
      const char *output_file = "child_output.txt";
      int file_fd;

      AppLogger::log(AppLogger::INFO_L, "Parent Process (PID: " + std::to_string(getpid()) + "): Starting. Child's output will be redirected to " + std::string(output_file));

      // 1. Mở file đích cho output
      file_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (file_fd == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Parent: Failed to open output file: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Parent: Opened " + std::string(output_file) + " with FD: " + std::to_string(file_fd));

      // 2. Fork để tạo tiến trình con
      pid = fork();
      if (pid == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Parent: Fork failed: " + std::string(strerror(errno)));
          close(file_fd);
          return EXIT_FAILURE;
      } else if (pid == 0) {
          // 3. Đây là mã của tiến trình con
          AppLogger::log(AppLogger::INFO_L, "Child Process (PID: " + std::to_string(getpid()) + "): Redirecting STDOUT.");

          // Đóng STDOUT hiện tại của child (FD 1)
          close(STDOUT_FILENO); 
          // Sao chép file_fd (FD của file) vào vị trí của STDOUT_FILENO (FD 1)
          // Bây giờ, mọi thứ ghi vào STDOUT_FILENO sẽ đi vào file_fd
          if (dup2(file_fd, STDOUT_FILENO) == -1) {
              AppLogger::log(AppLogger::ERROR_L, "Child: dup2 failed: " + std::string(strerror(errno)));
              close(file_fd); // Đóng file_fd gốc (nếu dup2 thất bại)
              _exit(EXIT_FAILURE); // Thoát con
          }
          close(file_fd); // Đóng file_fd gốc (nó đã được sao chép vào STDOUT_FILENO)

          // 4. Chương trình con thay thế chính nó bằng 'echo'
          AppLogger::log(AppLogger::INFO_L, "Child: Executing 'echo' command (output goes to file).");
          execlp("echo", "echo", "Hello from the child process! This output is redirected.", (char *)0);

          // Nếu execvp trả về, có lỗi
          AppLogger::log(AppLogger::ERROR_L, "Child: execlp failed: " + std::string(strerror(errno)));
          _exit(EXIT_FAILURE); // _exit để tránh flush buffer 2 lần
      } else {
          // 5. Đây là mã của tiến trình cha
          int status;
          AppLogger::log(AppLogger::INFO_L, "Parent: Child (PID: " + std::to_string(pid) + ") created. Waiting for child...");

          close(file_fd); // Đóng file_fd trong tiến trình cha (con đã có bản sao của nó)

          wait(&status); // Chờ tiến trình con kết thúc
          AppLogger::log(AppLogger::SUCCESS_L, "Parent: Child finished with status: " + std::to_string(WEXITSTATUS(status)));
          AppLogger::log(AppLogger::INFO_L, "Parent: Check file '" + std::string(output_file) + "' for child's output.");
      }

      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ redirect_output.cpp -o redirect_output
    ./redirect_output
    cat child_output.txt # Kiểm tra nội dung file
    ```
  * **Phân tích:** Bạn sẽ thấy output của `echo` **không xuất hiện trên terminal** mà thay vào đó nằm trong file `child_output.txt`.

---

### **Câu hỏi Tự đánh giá Module 2 🤔**

1. Phân biệt sự khác biệt cơ bản về cách hoạt động giữa hàm `system()` và họ hàm `exec` (ví dụ: `execlp()`). Khi nào bạn sẽ ưu tiên dùng `fork()` + `exec()` thay vì `system()`?
2. Hàm `exec()` có tạo ra một tiến trình mới không? Giải thích điều gì xảy ra với PID của tiến trình sau khi gọi `exec()` thành công.
3. Nêu hai sự khác biệt giữa `execlp()` và `execv()` (hoặc `execvp()`).
4. Giải thích ý nghĩa của giá trị trả về của `fork()` trong tiến trình cha và tiến trình con.
5. Bạn muốn một chương trình con được khởi chạy bằng `exec()` ghi tất cả output của nó vào một file log thay vì màn hình terminal. Bạn sẽ sử dụng những hàm nào và theo thứ tự nào để chuyển hướng `stdout` của nó?

---

### **Bài tập Thực hành Module 2 ✍️**

1. **Chương trình "Simple Shell":**
   * Viết một chương trình C++ (`simple_shell.cpp`) mô phỏng một shell rất đơn giản.
   * Chương trình sẽ:
     * In ra một dấu nhắc lệnh (prompt) (ví dụ: `my_shell> `).
     * Đọc một dòng lệnh từ `stdin` (sử dụng `fgets()`).
     * Nếu lệnh là `exit`, thoát chương trình.
     * Nếu không, phân tích dòng lệnh thành các đối số (ví dụ: dùng `strtok` hoặc `std::istringstream`).
     * Sử dụng `fork()` để tạo một tiến trình con.
     * Trong tiến trình con, sử dụng `execvp()` để chạy lệnh đã nhập.
     * Trong tiến trình cha, `wait()` cho tiến trình con kết thúc.
     * Xử lý lỗi cơ bản (ví dụ: `fork` thất bại, `execvp` thất bại).
   * **Thử thách:**
     * Hỗ trợ các lệnh đơn giản như `ls -l`, `pwd`, `echo Hello`.
     * Thêm khả năng chạy lệnh nền (background) bằng cách kiểm tra dấu `&` ở cuối lệnh. Nếu có, tiến trình cha không `wait()` mà chỉ in ra PID của tiến trình con.
2. **Chương trình "Filter":**
   * Viết hai chương trình C++:
     * **`uppercase_filter.cpp`:**
       * Đọc từng ký tự từ `stdin`.
       * Chuyển đổi ký tự đó thành chữ hoa.
       * Ghi ký tự đã chuyển đổi ra `stdout`.
       * Dừng khi đọc được `EOF`.
     * **`file_processor.cpp`:**
       * Nhận một tham số dòng lệnh: `<input_file>`.
       * Mở `<input_file>` để đọc.
       * Sử dụng `fork()` và `dup2()` để chuyển hướng `stdin` của tiến trình con sang `<input_file>`.
       * Trong tiến trình con, `execvp()` chương trình `uppercase_filter`.
       * Trong tiến trình cha, `wait()` cho tiến trình con.
       * Dọn dẹp file đã mở.
   * **Mục tiêu:** Khi chạy `./file_processor my_text.txt`, nó sẽ in ra nội dung của `my_text.txt` đã được chuyển đổi thành chữ hoa.
