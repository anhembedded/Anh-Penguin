# **Module 4: Signals (Tín hiệu) 🚦**

#### **4.1. Signal là gì? (What is a Signal?) 🔔**

* **Lý thuyết:** Một **Signal (Tín hiệu)** là một **thông báo không đồng bộ (asynchronous notification)** được gửi đến một tiến trình để báo hiệu một sự kiện đã xảy ra.
  * Hãy hình dung nó như một "cú điện thoại" bất ngờ mà hệ điều hành hoặc một tiến trình khác gọi cho tiến trình của bạn.
  * Tín hiệu được **"gửi" (raise)** và được  **"nhận" (catch)** .
* **Nguồn gốc của tín hiệu:**
  * **Hệ điều hành/Kernel:** Do các lỗi nghiêm trọng (ví dụ: truy cập bộ nhớ bất hợp lệ - `SIGSEGV`, lỗi số học dấu phẩy động - `SIGFPE`, lệnh bất hợp pháp - `SIGILL`).
  * **Shell/Terminal:** Do người dùng nhập các tổ hợp phím đặc biệt (ví dụ: `Ctrl+C` gửi `SIGINT`, `Ctrl+\` gửi `SIGQUIT`, `Ctrl+Z` gửi `SIGTSTP`).
  * **Các tiến trình khác:** Một tiến trình có thể gửi tín hiệu đến một tiến trình khác (bao gồm cả chính nó) để yêu cầu nó thực hiện một hành động nào đó hoặc thông báo một sự kiện.

#### **4.2. Các Loại Tín hiệu Phổ biến và Hành vi Mặc định 📋**

Tất cả các tên tín hiệu bắt đầu bằng "SIG". Mỗi tín hiệu có một số hiệu duy nhất.

* **Tín hiệu gây kết thúc chương trình kèm Core Dump (lỗi nghiêm trọng) 💀:** Mặc định sẽ chấm dứt tiến trình và thường tạo ra `core dump` file (ảnh chụp bộ nhớ tiến trình để debug).
  * `SIGABRT` (6): Hủy bỏ tiến trình (do hàm `abort()` gọi).
  * `SIGFPE` (8): Lỗi số học dấu phẩy động (ví dụ: chia cho 0).
  * `SIGILL` (4): Lệnh bất hợp pháp.
  * `SIGQUIT` (3): Thoát từ terminal (Ctrl+).
  * `SIGSEGV` (11): Lỗi phân đoạn (truy cập bộ nhớ bất hợp pháp).
* **Tín hiệu yêu cầu kết thúc hoặc điều khiển (thường không tạo core dump) 🚪:**
  * `SIGHUP` (1): Treo máy (Hangup). Thường gửi khi terminal ngắt kết nối; được dùng để yêu cầu các daemon tải lại cấu hình mà không khởi động lại.
  * `SIGINT` (2): Ngắt từ terminal (Ctrl+C).
  * `SIGPIPE` (13): Ghi vào pipe/socket không có bộ đọc.
  * `SIGTERM` (15): Yêu cầu tiến trình kết thúc một cách duyên dáng. Đây là tín hiệu mặc định của lệnh `kill`.
  * `SIGUSR1` (10), `SIGUSR2` (12): Hai tín hiệu do người dùng định nghĩa, để các tiến trình giao tiếp với nhau theo mục đích riêng.
* **Tín hiệu dừng/tiếp tục tiến trình (Job Control) ⏸️▶️:**
  * `SIGSTOP` (19): Dừng tiến trình ngay lập tức (không thể bắt hoặc bỏ qua).
  * `SIGTSTP` (20): Dừng terminal (Ctrl+Z, có thể bắt).
  * `SIGCONT` (18): Tiếp tục tiến trình đã dừng (bị bỏ qua nếu tiến trình không dừng).
* **Tín hiệu quản lý tiến trình con 👶:**
  * `SIGCHLD` (17): Được gửi đến tiến trình cha khi một tiến trình con dừng hoặc kết thúc. Mặc định bị bỏ qua nhưng rất quan trọng để quản lý và "dọn dẹp" các tiến trình zombie.
* **Các tín hiệu không thể Bắt hoặc Bỏ qua 🔒:**
  * `SIGKILL` (9): Buộc tiến trình kết thúc ngay lập tức.
  * `SIGSTOP` (19): Buộc tiến trình dừng ngay lập tức.
  * Hai tín hiệu này không thể bị chặn (blocked), bắt (caught), hay bỏ qua (ignored) bởi một tiến trình; chúng là "lệnh tuyệt đối" từ Kernel.

#### **4.3. Gửi Tín hiệu ✉️**

* **Từ Shell:**
  * `kill PID`: Gửi `SIGTERM` (15) mặc định.
  * `kill -s SIGNAL_NAME PID` (ví dụ: `kill -s HUP 1234`).
  * `kill -SIGNAL_NUMBER PID` (ví dụ: `kill -9 1234`).
  * `killall PROGRAM_NAME`: Gửi tín hiệu đến tất cả tiến trình có tên `PROGRAM_NAME`.
* **Từ chương trình C/C++:**
  * `int kill(pid_t pid, int sig);`: Gửi tín hiệu `sig` tới tiến trình `pid`. Cần quyền thích hợp (thường là cùng UID hoặc root).
  * `int raise(int sig);`: Gửi tín hiệu `sig` tới chính tiến trình gọi.
  * `unsigned int alarm(unsigned int seconds);`: Đặt bộ hẹn giờ để gửi `SIGALRM` tới chính tiến trình sau `seconds` giây.

#### **4.4. Xử lý Tín hiệu (Signal Handling) 🛡️**

Một tiến trình có thể thay đổi hành vi mặc định khi nhận được một tín hiệu bằng cách cài đặt một **trình xử lý tín hiệu (signal handler)** – một hàm mà Kernel sẽ gọi khi tín hiệu đến.

* **Hàm `signal()` (Cũ và không khuyến nghị):**
  * `void (*signal(int sig, void (*func)(int)))(int);`
  * Là hàm truyền thống nhưng có hành vi không nhất quán và dễ gây ra điều kiện tranh chấp (race conditions). Trình xử lý thường tự động đặt lại về mặc định sau khi được gọi.
  * **Khuyến nghị:** **Không nên sử dụng `signal()` trong các chương trình mới.**
* **Hàm `sigaction()` (Hiện đại, Mạnh mẽ, và được Khuyến nghị) ✅:**
  * `int sigaction(int sig, const struct sigaction *act, struct sigaction *oact);`
  * Cung cấp kiểm soát chi tiết hơn thông qua cấu trúc `struct sigaction`.
  * **`struct sigaction`** :
  * `sa_handler`: Con trỏ tới hàm xử lý tín hiệu (hoặc `SIG_IGN` để bỏ qua, `SIG_DFL` để dùng hành vi mặc định).
  * `sa_mask` (`sigset_t`): Một **tập hợp các tín hiệu** sẽ bị **chặn (blocked)** khi `sa_handler` đang chạy. Điều này ngăn chặn các tín hiệu khác làm gián đoạn trình xử lý.
  * `sa_flags`: Các cờ để điều chỉnh hành vi:
    * `SA_RESTART`: Tự động khởi động lại các System Call bị gián đoạn bởi tín hiệu.
    * `SA_RESETHAND`: Đặt lại handler về mặc định sau khi được gọi một lần (giống `signal()`).
    * `SA_SIGINFO`: (Rất quan trọng) Cho phép handler nhận thêm thông tin chi tiết về tín hiệu và ngữ cảnh (thay đổi chữ ký handler thành `void handler(int sig, siginfo_t *info, void *ucontext)`).
    * `SA_NODEFER`: Không thêm tín hiệu hiện tại vào mask khi handler chạy (ít dùng, cẩn thận với re-entrancy).
* **Quy tắc Vàng cho Signal Handler ⚠️:**
  * Hàm xử lý tín hiệu **PHẢI re-entrant** (có thể bị gọi lại một cách an toàn khi nó chưa hoàn thành).
  * Chỉ được phép gọi một tập hợp rất hạn chế các hàm an toàn (async-signal-safe functions) bên trong signal handler (ví dụ: `write` với một file descriptor cố định, `_exit`, `kill`). **Tuyệt đối không dùng `printf`, `malloc`, `free` trực tiếp trong handler.**
  * **Thực hành tốt nhất:** Signal handler chỉ nên **thiết lập một cờ (flag) toàn cục** (kiểu `volatile sig_atomic_t`) và sau đó vòng lặp chính của chương trình sẽ kiểm tra cờ đó để thực hiện các tác vụ phức tạp hơn.

#### **4.5. Tập hợp Tín hiệu (Signal Sets) & Mặt nạ Tiến trình 🎭**

* **`sigset_t`** : Một kiểu dữ liệu để biểu diễn một tập hợp các tín hiệu.
* **Các hàm tiện ích (`<signal.h>`):**
  * `sigemptyset(&set)`: Khởi tạo `set` thành tập hợp rỗng.
  * `sigfillset(&set)`: Khởi tạo `set` chứa tất cả các tín hiệu.
  * `sigaddset(&set, signo)`: Thêm `signo` vào `set`.
  * `sigdelset(&set, signo)`: Xóa `signo` khỏi `set`.
  * `sigismember(&set, signo)`: Kiểm tra `signo` có trong `set` không.
* **`sigprocmask()`:**
  * `int sigprocmask(int how, const sigset_t *set, sigset_t *oset);`
  * Được dùng để  **thay đổi mặt nạ tín hiệu của tiến trình** . Mặt nạ này xác định những tín hiệu nào mà tiến trình hiện đang  **chặn (blocked)** . Tín hiệu bị chặn sẽ không được gửi ngay lập tức mà sẽ bị **treo (pending)** cho đến khi nó được bỏ chặn.
  * `how`: `SIG_BLOCK` (thêm vào mask), `SIG_UNBLOCK` (xóa khỏi mask), `SIG_SETMASK` (đặt mask mới).
* **`sigpending()`:**
  * `int sigpending(sigset_t *set);`
  * Kiểm tra các tín hiệu hiện đang bị chặn và đang chờ được gửi đến tiến trình.
* **`sigsuspend()`:**
  * `int sigsuspend(const sigset_t *sigmask);`
  * Tạm dừng tiến trình, thay thế mặt nạ tín hiệu hiện tại bằng `sigmask` được cung cấp. Tiến trình sẽ tiếp tục khi một tín hiệu không bị chặn được gửi đến và được xử lý. Đây là cách an toàn và mạnh mẽ hơn để chờ tín hiệu so với `pause()`.

#### **4.6. Signals và POSIX Threads 🤝**

* **Signals là của Tiến trình, nhưng được Phân phối cho một Luồng cụ thể.**
* **Mặt nạ tín hiệu (Signal Mask) là CỦA TỪNG LUỒNG.** Mỗi luồng có thể chặn các tín hiệu khác nhau bằng `pthread_sigmask()`.
* **Trình xử lý tín hiệu (Signal Handler) là CỦA TOÀN BỘ TIẾN TRÌNH.** Tất cả các luồng trong một tiến trình chia sẻ cùng một bảng trình xử lý tín hiệu.
* **Quy tắc phân phối tín hiệu:**

  * **Tín hiệu đồng bộ (`SIGSEGV`, `SIGFPE`):** Luôn được gửi đến  **luồng đã gây ra lỗi** .
  * **Tín hiệu không đồng bộ (`SIGINT`, `SIGTERM`):** Kernel sẽ gửi đến **một luồng bất kỳ** trong tiến trình đó mà **không chặn** tín hiệu đó. Nếu tất cả các luồng đều chặn, tín hiệu sẽ bị treo.
  * **`SIGKILL` và `SIGSTOP`:** Luôn ảnh hưởng đến  **toàn bộ tiến trình** , không thể bị chặn hoặc bắt bởi bất kỳ luồng nào.
* **Ví dụ (C++): `signal_example.cpp` - Sử dụng `sigaction()`**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <csignal>  // For sigaction, sigemptyset, SIG_IGN, SIG_DFL, SIGTERM, SIGINT, SIGHUP, SIGUSR1
  #include <unistd.h> // For sleep, getpid
  #include <cstdlib>  // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>  // For strerror
  #include <cerrno>   // For errno

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

  // Biến cờ để điều khiển vòng lặp chính từ signal handler
  volatile sig_atomic_t keep_running = 1; 

  // Trình xử lý tín hiệu (Signal Handler)
  // Lưu ý: Chỉ các hàm async-signal-safe mới được gọi an toàn ở đây.
  // std::cout/printf không phải là async-signal-safe, nhưng dùng để minh họa.
  void signal_handler(int sig) {
      if (sig == SIGINT) {
          AppLogger::log(AppLogger::WARNING_L, "Received SIGINT (Ctrl+C). Ignoring for now...");
      } else if (sig == SIGTERM) {
          AppLogger::log(AppLogger::INFO_L, "Received SIGTERM. Setting keep_running to 0.");
          keep_running = 0; // Yêu cầu vòng lặp chính thoát
      } else if (sig == SIGHUP) {
          AppLogger::log(AppLogger::INFO_L, "Received SIGHUP. Reloading configuration (simulated).");
      } else if (sig == SIGUSR1) {
          AppLogger::log(AppLogger::INFO_L, "Received SIGUSR1. User-defined action (simulated).");
      } else {
          AppLogger::log(AppLogger::WARNING_L, "Received unhandled signal: " + std::to_string(sig));
      }
  }

  int main() {
      struct sigaction sa;

      AppLogger::log(AppLogger::INFO_L, "Process " + std::to_string(getpid()) + " started. Press Ctrl+C, Ctrl+\\, or send signals.");
      AppLogger::log(AppLogger::INFO_L, "  - Ctrl+C (SIGINT) will be handled.");
      AppLogger::log(AppLogger::INFO_L, "  - kill " + std::to_string(getpid()) + " (SIGTERM) will cause graceful exit.");
      AppLogger::log(AppLogger::INFO_L, "  - kill -HUP " + std::to_string(getpid()) + " (SIGHUP) will simulate config reload.");
      AppLogger::log(AppLogger::INFO_L, "  - kill -USR1 " + std::to_string(getpid()) + " (SIGUSR1) will trigger custom action.");
      AppLogger::log(AppLogger::INFO_L, "  - Ctrl+\\ (SIGQUIT) or kill -9 (SIGKILL) will terminate immediately.");

      // --- Cấu hình trình xử lý tín hiệu cho SIGINT, SIGTERM, SIGHUP, SIGUSR1 ---
      sa.sa_handler = signal_handler; // Gán hàm xử lý
      sigemptyset(&sa.sa_mask);     // Xóa tất cả các tín hiệu khỏi mặt nạ (không chặn tín hiệu nào khác khi handler chạy)
      sa.sa_flags = 0;              // Không cờ đặc biệt (mặc định không SA_RESTART)

      // Đăng ký handler cho SIGINT
      if (sigaction(SIGINT, &sa, nullptr) == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Failed to register SIGINT handler: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      }
      // Đăng ký handler cho SIGTERM
      if (sigaction(SIGTERM, &sa, nullptr) == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Failed to register SIGTERM handler: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      }
      // Đăng ký handler cho SIGHUP
      if (sigaction(SIGHUP, &sa, nullptr) == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Failed to register SIGHUP handler: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      }
      // Đăng ký handler cho SIGUSR1
      if (sigaction(SIGUSR1, &sa, nullptr) == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Failed to register SIGUSR1 handler: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      }

      // --- Bỏ qua SIGCHLD để tránh zombie processes (thực hành tốt) ---
      struct sigaction sa_chld;
      sa_chld.sa_handler = SIG_IGN; // Bỏ qua tín hiệu
      sigemptyset(&sa_chld.sa_mask);
      sa_chld.sa_flags = SA_NOCLDWAIT; // SA_NOCLDWAIT cũng giúp tránh zombie
      if (sigaction(SIGCHLD, &sa_chld, nullptr) == -1) {
          AppLogger::log(AppLogger::CRITICAL_L, "Failed to ignore SIGCHLD: " + std::string(strerror(errno)));
          return EXIT_FAILURE;
      }

      // --- Vòng lặp chính của chương trình ---
      while (keep_running) {
          AppLogger::log(AppLogger::INFO_L, "Program is running... PID: " + std::to_string(getpid()));
          sleep(2); // Ngủ 2 giây
      }

      AppLogger::log(AppLogger::INFO_L, "Program exiting gracefully.");
      return EXIT_SUCCESS;
  }
  ```

  * **Cách biên dịch:**
    **Bash**

    ```
    g++ signal_example.cpp -o signal_example
    ```
  * **Cách chạy và kiểm tra:**

    1. Chạy chương trình: `./signal_example`
    2. Trong terminal đó, nhấn `Ctrl+C` nhiều lần. Quan sát output.
    3. Mở một terminal khác, lấy PID của chương trình (`ps -ax | grep signal_example`).
    4. Gửi tín hiệu:
       * `kill -HUP <PID>`: Quan sát thông báo "Reloading configuration".
       * `kill -USR1 <PID>`: Quan sát thông báo "User-defined action".
       * `kill <PID>` (gửi `SIGTERM`): Chương trình sẽ thoát duyên dáng.
       * `kill -QUIT <PID>` hoặc `kill -9 <PID>`: Chương trình sẽ chấm dứt ngay lập tức (không chạy code dọn dẹp).

---

### **Câu hỏi Tự đánh giá Module 4 🤔**

1. Giải thích khái niệm "tín hiệu không đồng bộ" (asynchronous signal).
2. Nêu ba nguồn gốc khác nhau của tín hiệu trong Linux.
3. Phân biệt `SIGINT`, `SIGTERM`, và `SIGKILL`. Tín hiệu nào không thể bị bắt hoặc bỏ qua và tại sao?
4. Khi nào bạn sẽ sử dụng `sigaction()` thay vì `signal()` để cài đặt trình xử lý tín hiệu? Nêu ít nhất hai ưu điểm của `sigaction()`.
5. Tại sao việc sử dụng `printf()` hoặc `malloc()` trực tiếp bên trong một trình xử lý tín hiệu lại được coi là không an toàn? Nêu một kỹ thuật lập trình an toàn hơn để xử lý logic phức tạp trong signal handler.
6. Giải thích vai trò của `sa_mask` trong cấu trúc `struct sigaction`.
7. Bạn muốn một System Call bị gián đoạn bởi tín hiệu sẽ tự động tiếp tục lại sau khi trình xử lý tín hiệu kết thúc. Bạn sẽ đặt cờ nào trong `sa_flags` của `struct sigaction`?

---

### **Bài tập Thực hành Module 4 ✍️**

1. **Chương trình "Alarm Clock" (Đồng hồ Báo thức):**
   * Viết một chương trình C++ (`alarm_clock.cpp`) mà:
     * Sử dụng `alarm()` để đặt một báo thức sau 5 giây.
     * Cài đặt trình xử lý tín hiệu cho `SIGALRM`.
     * Trình xử lý `SIGALRM` sẽ chỉ đặt một cờ `volatile sig_atomic_t alarm_fired = 1;`.
     * Trong `main()`, sau khi đặt báo thức, chương trình sẽ đi vào một vòng lặp `while(keep_running)` và cứ mỗi 1 giây in ra "Waiting for alarm..."
     * Khi `alarm_fired` là 1, in ra "Ding! Alarm fired!" và thoát chương trình.
     * **Thử thách:** Sử dụng `sigsuspend()` để tạm dừng chương trình một cách hiệu quả cho đến khi tín hiệu báo thức đến, thay vì vòng lặp `sleep()`.
2. **Chương trình "Process Control with Signals":**
   * Viết hai chương trình C++:
     * **`controller.cpp`:**
       * Nhận một PID từ dòng lệnh.
       * Gửi `SIGSTOP` đến PID đó.
       * Ngủ 5 giây.
       * Gửi `SIGCONT` đến PID đó.
       * Ngủ 5 giây.
       * Gửi `SIGTERM` đến PID đó.
       * Xử lý lỗi nếu không thể gửi tín hiệu (ví dụ: PID không tồn tại, không có quyền).
     * **`target_app.cpp`:**
       * In ra PID của nó.
       * Đi vào một vòng lặp vô hạn, cứ mỗi 1 giây in ra "I am running...".
       * Cài đặt trình xử lý tín hiệu cho `SIGTSTP` để khi nhận được, nó in ra "Received SIGTSTP. I am stopping." và sau đó gọi `raise(SIGSTOP)` để tự dừng.
       * Cài đặt trình xử lý cho `SIGCONT` để in ra "Received SIGCONT. I am resuming."
       * Cài đặt trình xử lý cho `SIGTERM` để thoát duyên dáng.
   * **Mục tiêu:** Chạy `target_app.cpp` trong một terminal. Trong terminal khác, chạy `controller.cpp` với PID của `target_app`. Quan sát cách `target_app` dừng, tiếp tục và cuối cùng kết thúc.
3. **Chương trình "Graceful Shutdown for Daemon":**
   * Sử dụng lại chương trình `minimal_daemon_logger.cpp` hoặc `resource_monitor_daemon.cpp` từ Module 6 của giáo trình "The Linux Environment".
   * Đảm bảo rằng trình xử lý `SIGTERM` của nó hoạt động đúng cách:
     * Đặt cờ `daemon_running = 0`.
     * Ghi một thông báo "Daemon shutting down gracefully." vào log.
     * **Thử thách:** Trong trình xử lý tín hiệu, đảm bảo rằng `closelog()` (nếu dùng syslog) hoặc `file_log_stream.close()` (nếu dùng file log) được gọi. **Lưu ý:** Việc gọi `close()` hoặc `fclose()` trong signal handler có thể không an toàn trong mọi trường hợp. Một cách an toàn hơn là đặt cờ và để vòng lặp chính thực hiện việc đóng.
