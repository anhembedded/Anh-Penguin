# **Module 3: Quản lý Tiến trình Con (Child Process Management) 👨‍👧‍👦**

#### **3.1. Chờ một Tiến trình (Waiting for a Process) ⏱️**

* **Lý thuyết:** Khi tiến trình cha tạo một tiến trình con bằng `fork()`, tiến trình con đó chạy độc lập. Tuy nhiên, tiến trình cha thường cần biết khi nào tiến trình con kết thúc để:

  * Thu thập **trạng thái thoát (exit status)** của tiến trình con.
  * Ngăn chặn tiến trình con trở thành  **"zombie process"** .
  * Thực hiện các tác vụ tiếp theo chỉ khi tiến trình con đã hoàn thành.
* **Hàm `wait()`:**

  * **Syntax:**
    **C++**

    ```cpp
    #include <sys/wait.h> // For wait, WIFEXITED, WEXITSTATUS, etc.
    #include <sys/types.h> // For pid_t (optional, often included by sys/wait.h)
    // pid_t wait(int *stat_loc);
    ```
  * **`stat_loc`** : Con trỏ tới một biến `int`. Hàm `wait()` sẽ ghi thông tin về trạng thái kết thúc của tiến trình con vào biến này. Đây không phải là mã thoát trực tiếp, mà là một giá trị bitfield cần được phân tích bằng các macro.
  * **Cách hoạt động:** Tiến trình cha sẽ **bị chặn (block)** – tức là tạm dừng việc thực thi – cho đến khi **một trong các tiến trình con** của nó kết thúc hoặc dừng.
  * **Giá trị trả về:**

    * PID của tiến trình con đã kết thúc hoặc dừng nếu thành công.
    * `-1` nếu không có tiến trình con nào hoặc xảy ra lỗi (ví dụ: `ECHILD` nếu không có tiến trình con nào để chờ).
* **Các Macro Diễn giải Trạng thái (`<sys/wait.h>`) 🔬:** Để giải mã giá trị `stat_val` được trả về bởi `wait()` (hoặc `waitpid()`), bạn cần sử dụng các macro sau:

  * **`WIFEXITED(stat_val)`:** Trả về `true` (khác 0) nếu tiến trình con **kết thúc bình thường** (gọi `exit()` hoặc `return` từ `main()`).
  * **`WEXITSTATUS(stat_val)`:** Nếu `WIFEXITED` là `true`, macro này trả về **mã thoát (exit code)** của tiến trình con (giá trị truyền cho `exit()` hoặc trả về từ `main()`).
  * **`WIFSIGNALED(stat_val)`:** Trả về `true` nếu tiến trình con bị  **kết thúc do một tín hiệu không được bắt (uncaught signal)** .
  * **`WTERMSIG(stat_val)`:** Nếu `WIFSIGNALED` là `true`, macro này trả về **số hiệu tín hiệu** đã làm tiến trình con kết thúc.
  * `WIFSTOPPED(stat_val)`: Trả về `true` nếu tiến trình con bị **tạm dừng** bởi một tín hiệu điều khiển công việc.
  * `WSTOPSIG(stat_val)`: Nếu `WIFSTOPPED` là `true`, trả về số hiệu tín hiệu đã làm tiến trình con dừng.
* **Liên hệ Embedded Linux:** Rất quan trọng để:

  * Đảm bảo các tác vụ con đã hoàn thành trước khi tiến hành các bước tiếp theo.
  * Thu thập mã thoát của các dịch vụ con để biết chúng có thành công hay không.
  * **Ngăn chặn "Zombie Processes" (sẽ học sau).**
* **Ví dụ (C++): `wait_example.cpp` - Sử dụng `wait()`**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <cstdlib>  // For exit, EXIT_SUCCESS, EXIT_FAILURE
  #include <unistd.h> // For fork, sleep, getpid
  #include <sys/wait.h> // For wait, WIFEXITED, WEXITSTATUS

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
      int exit_code_child = 37; // Mã thoát của tiến trình con

      AppLogger::log(AppLogger::INFO_L, "Parent Process (PID: " + std::to_string(getpid()) + "): Starting wait example.");

      pid = fork(); // Tạo tiến trình con

      if (pid == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Fork failed: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      } else if (pid == 0) {
          // Mã của tiến trình con
          AppLogger::log(AppLogger::INFO_L, "Child Process (PID: " + std::to_string(getpid()) + ", PPID: " + std::to_string(getppid()) + "): I will work for 3 seconds and exit with code " + std::to_string(exit_code_child) + ".");
          sleep(3); // Giả vờ làm việc
          AppLogger::log(AppLogger::INFO_L, "Child Process (PID: " + std::to_string(getpid()) + "): Exiting.");
          exit(exit_code_child); // Tiến trình con thoát với mã
      } else {
          // Mã của tiến trình cha
          int stat_val; // Biến để lưu trạng thái thoát của con
          pid_t child_pid_returned;

          AppLogger::log(AppLogger::INFO_L, "Parent Process (PID: " + std::to_string(getpid()) + "): Child created with PID: " + std::to_string(pid) + ". Waiting for child...");

          // Tiến trình cha bị chặn tại đây cho đến khi con kết thúc
          child_pid_returned = wait(&stat_val); 

          AppLogger::log(AppLogger::SUCCESS_L, "Parent Process: Child (PID: " + std::to_string(child_pid_returned) + ") has finished.");

          // Phân tích trạng thái thoát của con
          if (WIFEXITED(stat_val)) { // Kiểm tra nếu con thoát bình thường
              AppLogger::log(AppLogger::SUCCESS_L, "Parent Process: Child exited normally with code: " + std::to_string(WEXITSTATUS(stat_val)));
          } else if (WIFSIGNALED(stat_val)) { // Kiểm tra nếu con bị kết thúc bởi tín hiệu
              AppLogger::log(AppLogger::WARNING_L, "Parent Process: Child terminated by uncaught signal: " + std::to_string(WTERMSIG(stat_val)));
          } else if (WIFSTOPPED(stat_val)) { // Kiểm tra nếu con bị dừng
              AppLogger::log(AppLogger::WARNING_L, "Parent Process: Child stopped by signal: " + std::to_string(WSTOPSIG(stat_val)));
          } else {
              AppLogger::log(AppLogger::WARNING_L, "Parent Process: Child terminated abnormally (unknown reason).");
          }
      }
      AppLogger::log(AppLogger::INFO_L, "Parent Process (PID: " + std::to_string(getpid()) + "): Exiting.");
      return EXIT_SUCCESS;
  }
  ```

#### **3.2. Zombie Processes (Tiến trình Zombie) 🧟**

* **Lý thuyết:** Một **tiến trình zombie (zombie process)** là một tiến trình con đã **kết thúc việc thực thi** của nó, nhưng mục nhập của nó trong bảng tiến trình của Kernel vẫn còn tồn tại (chưa được giải phóng). Điều này xảy ra khi tiến trình cha **chưa gọi `wait()` hoặc `waitpid()`** để thu thập trạng thái thoát của tiến trình con.

  * **Tại sao lại là Zombie?** Kernel cần giữ lại một lượng nhỏ thông tin về tiến trình con đã chết (như PID, trạng thái thoát) để tiến trình cha có thể thu thập nó sau này. Nếu tiến trình cha không thu thập, thông tin này sẽ "lơ lửng" trong bảng tiến trình.
  * **Vấn đề:** Mặc dù zombie không tiêu thụ tài nguyên CPU hoặc bộ nhớ RAM, nhưng mỗi zombie chiếm một  **mục trong bảng tiến trình** . Nếu quá nhiều zombie được tạo ra, bảng tiến trình có thể bị đầy, ngăn cản việc tạo các tiến trình mới và có thể dẫn đến sự cố hệ thống.
  * **Cách nhận biết:** Trong output của `ps -al`, bạn sẽ thấy trạng thái `Z` (hoặc `defunct`).
* **Giải pháp để tránh Zombie:**

  1. **Luôn gọi `wait()` hoặc `waitpid()`:** Đây là cách tốt nhất để tiến trình cha thu thập trạng thái của con và cho phép Kernel giải phóng mục nhập zombie.
  2. **Bỏ qua `SIGCHLD`:** Nếu tiến trình cha không quan tâm đến trạng thái thoát của con và không muốn `wait()` cho chúng, nó có thể cài đặt trình xử lý tín hiệu cho `SIGCHLD` (tín hiệu được gửi khi con kết thúc) thành `SIG_IGN` (Ignore - bỏ qua). Kernel sẽ tự động dọn dẹp các zombie khi `SIGCHLD` bị bỏ qua.
     * **Lưu ý:** Nếu dùng `SIG_IGN` cho `SIGCHLD`, bạn sẽ không thể lấy mã thoát của tiến trình con sau này.
  3. **Fork hai lần (Double Fork):** Một kỹ thuật phổ biến để tạo daemon. Tiến trình cha `fork()` lần đầu, sau đó tiến trình con đầu tiên `fork()` lần nữa và thoát ngay lập tức. Tiến trình con thứ hai (là cháu của tiến trình gốc) sẽ trở thành tiến trình mồ côi và được `init` (PID 1) nhận nuôi. `init` sẽ tự động `wait()` và dọn dẹp nó.
  4. **Tiến trình mồ côi (Orphan Processes):** Nếu tiến trình cha kết thúc trước khi tiến trình con, tiến trình con đó sẽ trở thành "tiến trình mồ côi". Các tiến trình mồ côi sẽ được Kernel tự động "nhận nuôi" bởi tiến trình `init` (hoặc `systemd`, PID 1). `init` (hoặc `systemd`) có nhiệm vụ đặc biệt là tự động `wait()` cho các tiến trình con mồ côi này, ngăn chặn chúng trở thành zombie vĩnh viễn.
* **Liên hệ Embedded Linux:** Cực kỳ quan trọng trong các ứng dụng daemon chạy liên tục. Nếu một daemon tạo ra nhiều tiến trình con mà không dọn dẹp chúng, hệ thống nhúng có thể bị đầy bảng tiến trình và không thể khởi chạy thêm chương trình nào khác.
* **Ví dụ (C++): `zombie_example.cpp` - Tạo và Tránh Zombie**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <cstdlib>  // For exit, EXIT_SUCCESS, EXIT_FAILURE
  #include <unistd.h> // For fork, sleep, getpid
  #include <sys/wait.h> // For wait, WIFEXITED, WEXITSTATUS
  #include <signal.h> // For signal, SIGCHLD, SIG_IGN

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

  int main(int argc, char* argv[]) {
      pid_t pid;

      AppLogger::log(AppLogger::INFO_L, "Parent Process (PID: " + std::to_string(getpid()) + "): Starting zombie demonstration.");

      // --- Trường hợp 1: Tạo Zombie (cha không chờ con) ---
      if (argc > 1 && std::string(argv[1]) == "zombie") {
          AppLogger::log(AppLogger::INFO_L, "Parent: Creating a child that will become a zombie.");
          pid = fork();
          if (pid == -1) {
              AppLogger::log(AppLogger::ERROR_L, "Fork failed: " + std::string(strerror(errno)));
              return EXIT_FAILURE;
          } else if (pid == 0) {
              // Child process
              AppLogger::log(AppLogger::INFO_L, "Child Process (PID: " + std::to_string(getpid()) + "): Exiting immediately (will become zombie).");
              exit(0); // Con thoát ngay lập tức
          } else {
              // Parent process
              AppLogger::log(AppLogger::INFO_L, "Parent Process: Child (PID: " + std::to_string(pid) + ") created. NOT waiting for it.");
              AppLogger::log(AppLogger::INFO_L, "Parent Process: Sleeping for 10 seconds. Check 'ps -al | grep Z' for zombie.");
              sleep(10); // Cha ngủ, không chờ con
              AppLogger::log(AppLogger::INFO_L, "Parent Process: Finished sleeping. Zombie should be gone now (init adopted).");
          }
      }
      // --- Trường hợp 2: Tránh Zombie bằng SIG_IGN cho SIGCHLD ---
      else if (argc > 1 && std::string(argv[1]) == "no-zombie-sigign") {
          AppLogger::log(AppLogger::INFO_L, "Parent: Setting SIGCHLD to SIG_IGN to avoid zombies.");
          // Cài đặt trình xử lý SIGCHLD thành SIG_IGN
          if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
              AppLogger::log(AppLogger::ERROR_L, "Failed to set SIGCHLD handler: " + std::string(strerror(errno)));
              return EXIT_FAILURE;
          }

          AppLogger::log(AppLogger::INFO_L, "Parent: Creating a child. It should not become a zombie.");
          pid = fork();
          if (pid == -1) {
              AppLogger::log(AppLogger::ERROR_L, "Fork failed: " + std::string(strerror(errno)));
              return EXIT_FAILURE;
          } else if (pid == 0) {
              // Child process
              AppLogger::log(AppLogger::INFO_L, "Child Process (PID: " + std::to_string(getpid()) + "): Exiting immediately.");
              exit(0);
          } else {
              // Parent process
              AppLogger::log(AppLogger::INFO_L, "Parent Process: Child (PID: " + std::to_string(pid) + ") created. Not waiting (SIG_IGN).");
              AppLogger::log(AppLogger::INFO_L, "Parent Process: Sleeping for 5 seconds. Zombie should NOT appear.");
              sleep(5);
          }
      }
      // --- Trường hợp 3: Tránh Zombie bằng waitpid (không chặn) ---
      else if (argc > 1 && std::string(argv[1]) == "no-zombie-waitpid") {
          AppLogger::log(AppLogger::INFO_L, "Parent: Creating a child and waiting with WNOHANG.");
          pid = fork();
          if (pid == -1) {
              AppLogger::log(AppLogger::ERROR_L, "Fork failed: " + std::string(strerror(errno)));
              return EXIT_FAILURE;
          } else if (pid == 0) {
              // Child process
              AppLogger::log(AppLogger::INFO_L, "Child Process (PID: " + std::to_string(getpid()) + "): Working for 2 seconds then exiting.");
              sleep(2);
              exit(0);
          } else {
              // Parent process
              int status;
              pid_t result_pid;
              AppLogger::log(AppLogger::INFO_L, "Parent Process: Child (PID: " + std::to_string(pid) + ") created.");
              AppLogger::log(AppLogger::INFO_L, "Parent Process: Polling for child status with WNOHANG.");

              do {
                  result_pid = waitpid(pid, &status, WNOHANG); // Kiểm tra không chặn
                  if (result_pid == 0) {
                      AppLogger::log(AppLogger::INFO_L, "Parent: Child not yet finished, doing other work (sleeping 1s)...");
                      sleep(1);
                  } else if (result_pid == -1) {
                      AppLogger::log(AppLogger::ERROR_L, "Parent: waitpid failed: " + std::string(strerror(errno)));
                      break;
                  }
              } while (result_pid == 0); // Lặp lại cho đến khi con kết thúc

              if (result_pid == pid) {
                  AppLogger::log(AppLogger::SUCCESS_L, "Parent: Child (PID: " + std::to_string(result_pid) + ") collected.");
              }
          }
      }
      else {
          std::cout << "Usage: " << argv[0] << " [zombie|no-zombie-sigign|no-zombie-waitpid]" << std::endl;
          return EXIT_FAILURE;
      }

      AppLogger::log(AppLogger::INFO_L, "Parent Process (PID: " + std::to_string(getpid()) + "): Exiting.");
      return EXIT_SUCCESS;
  }
  ```

  * **Cách biên dịch:**
    **Bash**

    ```
    g++ zombie_example.cpp -o zombie_example
    ```
  * **Cách chạy và kiểm tra:**

    1. **Tạo Zombie:**
       **Bash**

       ```
       ./zombie_example zombie & # Chạy nền
       ps -al | grep zombie_example # Quan sát tiến trình cha và con (trạng thái Z)
       # Sau 10 giây, zombie sẽ biến mất (được init dọn dẹp)
       ```
    2. **Tránh Zombie bằng SIG_IGN:**
       **Bash**

       ```
       ./zombie_example no-zombie-sigign & # Chạy nền
       ps -al | grep zombie_example # Quan sát, zombie không xuất hiện
       ```
    3. **Tránh Zombie bằng waitpid (không chặn):**
       **Bash**

       ```
       ./zombie_example no-zombie-waitpid # Chạy tiền cảnh
       # Quan sát thông báo "doing other work" và sau đó con được thu thập
       ```

#### **3.3. `waitpid()`: Chờ Tiến trình Cụ thể hoặc Không Chặn 🔍**

* **Lý thuyết:** Hàm `waitpid()` cung cấp khả năng kiểm soát chi tiết hơn so với `wait()`.
  * **Syntax:**
    **C++**

    ```
    #include <sys/wait.h> // For waitpid, WNOHANG
    #include <sys/types.h> // For pid_t
    // pid_t waitpid(pid_t pid, int *stat_loc, int options);
    ```
  * **`pid` (Đối tượng chờ):**

    * `pid > 0`: Chờ tiến trình con có PID cụ thể đó.
    * `pid == 0`: Chờ bất kỳ tiến trình con nào trong cùng nhóm tiến trình với tiến trình gọi.
    * `pid == -1`: Chờ bất kỳ tiến trình con nào (giống như `wait()`).
    * `pid < -1`: Chờ bất kỳ tiến trình con nào có nhóm tiến trình ID (PGID) bằng giá trị tuyệt đối của `pid`.
  * **`stat_loc`** : Tương tự như `wait()`, con trỏ tới `int` để lưu thông tin trạng thái. Dùng các macro `WIFEXITED()`, `WEXITSTATUS()` để diễn giải.
  * **`options` (Tùy chọn hành vi):**

    * **`WNOHANG`:** Đây là tùy chọn quan trọng nhất. Nếu không có tiến trình con nào đã kết thúc và sẵn sàng được chờ, `waitpid()` **sẽ trả về `0` ngay lập tức** thay vì bị chặn. Điều này cho phép tiến trình cha kiểm tra trạng thái con mà không bị treo, và có thể tiếp tục làm việc khác.
    * `WUNTRACED`: Trả về cả các con bị dừng bởi tín hiệu.
    * `WCONTINUED`: Trả về cả các con được tiếp tục.
  * **Giá trị trả về:**

    * PID của tiến trình con đã kết thúc/dừng nếu thành công.
    * `0` nếu `WNOHANG` được chỉ định và không có tiến trình con nào đã kết thúc.
    * `-1` nếu lỗi (ví dụ: `ECHILD` nếu không có con, `EINTR` nếu bị ngắt bởi tín hiệu).
* **Liên hệ Embedded Linux:** Cực kỳ quan trọng cho các daemon hoặc server cần quản lý nhiều tiến trình con đồng thời mà không bị chặn. Ví dụ, một server có thể dùng `waitpid(..., WNOHANG)` trong vòng lặp chính để định kỳ kiểm tra các tiến trình con đã hoàn thành mà không làm gián đoạn việc chấp nhận kết nối mới.

---

### **Câu hỏi Tự đánh giá Module 3 🤔**

1. Tiến trình zombie là gì? Tại sao chúng lại là một vấn đề trong hệ thống Linux?
2. Nêu ít nhất hai cách để ngăn chặn các tiến trình con trở thành zombie. Giải thích ưu và nhược điểm của mỗi cách.
3. Phân biệt `wait()` và `waitpid()`. Khi nào bạn sẽ dùng `waitpid()` thay vì `wait()`?
4. Giải thích ý nghĩa của cờ `WNOHANG` khi sử dụng `waitpid()`. Khi `waitpid()` với `WNOHANG` trả về `0`, điều đó có ý nghĩa gì?
5. Bạn muốn khởi chạy một tiến trình con để thực hiện một tác vụ dài, và tiến trình cha cần tiếp tục công việc của mình mà không bị chặn. Tuy nhiên, bạn vẫn muốn đảm bảo tiến trình con không trở thành zombie. Bạn sẽ làm thế nào?

---

### **Bài tập Thực hành Module 3 ✍️**

1. **Chương trình "Zombie Creator" (Kiểm soát):**
   * Viết một chương trình C++ (`controlled_zombie.cpp`) mà:
     * Tạo một tiến trình con.
     * Tiến trình con sẽ in ra PID của nó và thoát ngay lập tức với mã thoát 123.
     * Tiến trình cha sẽ in ra PID của nó và PID của con.
     * Tiến trình cha sẽ  **không gọi `wait()` hoặc `waitpid()` ngay lập tức** . Thay vào đó, nó sẽ:
       * Ngủ 5 giây.
       * Sau đó, in ra thông báo "Parent is about to wait for child."
       * Gọi `wait()` để thu thập zombie.
       * In ra thông báo "Child collected." và mã thoát của con.
   * **Mục tiêu:** Chạy chương trình này, và trong 5 giây ngủ của tiến trình cha, sử dụng `ps -al | grep controlled_zombie` để quan sát tiến trình con ở trạng thái `Z`. Sau khi cha `wait()`, zombie sẽ biến mất.
2. **Chương trình "Daemon Launcher" (Không Zombie):**
   * Viết một chương trình C++ (`daemon_launcher.cpp`) mà:
     * Tạo một tiến trình con để mô phỏng việc khởi chạy một daemon.
     * Tiến trình con sẽ:
       * In ra PID của nó và thông báo "Daemon child started."
       * Ngủ 10 giây.
       * In ra "Daemon child exiting." và thoát.
     * Tiến trình cha sẽ:
       * **Không gọi `wait()`** .
       * Thay vào đó, cài đặt trình xử lý tín hiệu cho `SIGCHLD` thành `SIG_IGN`.
       * In ra "Parent exiting (child should not be zombie)."
       * Thoát ngay lập tức.
   * **Mục tiêu:** Chạy chương trình này trong nền (`./daemon_launcher &`). Ngay lập tức dùng `ps -al | grep daemon_launcher` để kiểm tra. Quan sát xem tiến trình con có bao giờ xuất hiện ở trạng thái `Z` không.
3. **Chương trình "Multi-Child Manager" (Nâng cao):**
   * Viết một chương trình C++ (`multi_child_manager.cpp`) mà:
     * Tạo 5 tiến trình con.
     * Mỗi tiến trình con sẽ:
       * In ra PID của nó và một thông báo "Child [PID] working for [random_seconds] seconds."
       * Ngủ một khoảng thời gian ngẫu nhiên (ví dụ: từ 1 đến 5 giây).
       * Thoát với mã thoát là PID của nó.
     * Tiến trình cha sẽ:
       * **Không bị chặn** . Nó sẽ đi vào một vòng lặp.
       * Trong mỗi lần lặp (ví dụ: mỗi 1 giây), nó sẽ sử dụng `waitpid(-1, &status, WNOHANG)` để kiểm tra xem có bất kỳ tiến trình con nào đã kết thúc không.
       * Nếu `waitpid()` trả về PID của một con, in ra thông báo "Parent collected child [PID] with status [exit_code]."
       * Vòng lặp cha sẽ tiếp tục cho đến khi tất cả 5 tiến trình con đã được thu thập.
   * **Thử thách:** Đảm bảo rằng việc sử dụng `WNOHANG` cho phép tiến trình cha vẫn hoạt động (in thông báo "Parent is checking...") trong khi chờ các con.

---

Hãy dành thời gian để thực hành các bài tập này. Chúng sẽ giúp bạn làm chủ việc quản lý các tiến trình con, một kỹ năng cốt lõi cho lập trình hệ thống. Khi bạn đã sẵn sàng, hãy cho tôi biết để chuyển sang  **Module 4: Signals** !
