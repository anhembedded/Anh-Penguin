# **Module 4: Signals (Tín hiệu) 🚦**

---

### 🔔 **4.1. Signal là gì? (What is a Signal?)**

#### 🧠 **Lý thuyết chung**

Một **Signal (Tín hiệu)** là một dạng **thông báo không đồng bộ (asynchronous notification)** được gửi đến một tiến trình để báo hiệu rằng một sự kiện nào đó đã xảy ra.

* Hãy tưởng tượng nó như một "cuộc gọi bất ngờ" mà hệ điều hành hoặc một tiến trình khác thực hiện để **đánh thức** tiến trình của bạn.
* Tín hiệu sẽ được:
  * **Gửi (raise)** — phát sinh từ một tác nhân nào đó.
  * **Nhận (catch)** — tiến trình xử lý thông qua signal handler hoặc theo hành vi mặc định.

---

#### 📦 **Nguồn gốc phát sinh tín hiệu**

Tín hiệu có thể xuất hiện từ nhiều nguồn khác nhau:

* 🧩 **Hệ điều hành/Kernel**

  Do các lỗi nghiêm trọng trong quá trình thực thi:

  * Truy cập bộ nhớ sai (segmentation fault) → `SIGSEGV`
  * Lỗi toán học dấu phẩy động (float exception) → `SIGFPE`
  * Thi hành lệnh máy không hợp lệ → `SIGILL`
* ⌨️ **Shell/Terminal**

  Người dùng nhập các phím đặc biệt khi chạy chương trình:

  * `Ctrl+C` → gửi `SIGINT` (ngắt chương trình)
  * `Ctrl+\` → gửi `SIGQUIT` (thoát kèm core dump)
  * `Ctrl+Z` → gửi `SIGTSTP` (dừng tạm thời tiến trình)
* 🔁 **Tiến trình khác (Process-to-Process)**

  Một tiến trình có thể gửi tín hiệu đến:

  * **Tiến trình khác** , ví dụ: daemon giám sát gửi `SIGTERM` để yêu cầu thoát.
  * **Chính nó** , ví dụ: gọi `raise(SIGUSR1)` để tự xử lý nội bộ.

---

## 💀 **1. Tín hiệu gây Core Dump — Lỗi nghiêm trọng**

| Tín hiệu      | Mô tả                                              | Nguyên nhân thường gặp                         | Handler được không?                                        |
| --------------- | ---------------------------------------------------- | --------------------------------------------------- | -------------------------------------------------------------- |
| `SIGABRT`(6)  | Tự hủy tiến trình bằng `abort()`              | Vi phạm logic, assert fail                         | ✅ Có thể bắt bằng `sigaction()`                         |
| `SIGFPE`(8)   | Lỗi số học: chia cho 0, tràn số, chia sai kiểu | `int x = 1 / 0;`, float-to-int overflow           | ✅ Có thể bắt                                               |
| `SIGILL`(4)   | Lệnh bất hợp pháp                                | Gọi hàm không hợp lệ, corruption bộ nhớ mã  | ✅ Có thể bắt                                               |
| `SIGQUIT`(3)  | Ctrl + \ từ terminal → kết thúc có core dump    | Thoát tự nguyện có debug, thường không dùng | ✅ Có thể bắt                                               |
| `SIGSEGV`(11) | Lỗi phân đoạn: truy cập sai vùng bộ nhớ      | Dereference null pointer, buffer overflow           | ✅ Có thể bắt (thường để ghi log rồi `_exit()`luôn) |

📌 Nếu hệ thống bật core dump (`ulimit -c unlimited`) thì file dump sẽ nằm trong `/core`, giúp debug qua `gdb`.

---

## 🚪 **2. Tín hiệu điều khiển hoặc yêu cầu kết thúc**

| Tín hiệu           | Mô tả                                     | Ứng dụng thực tế                       | Có thể bắt không? |
| -------------------- | ------------------------------------------- | ------------------------------------------ | --------------------- |
| `SIGHUP`(1)        | Treo terminal, yêu cầu daemon reload      | `nginx`,`sshd`dùng SIGHUP để reload | ✅ Có thể bắt      |
| `SIGINT`(2)        | Ctrl + C từ terminal                       | Ngắt tiến trình foreground              | ✅ Có thể bắt      |
| `SIGPIPE`(13)      | Ghi vào pipe/socket không có đầu đọc | `write()`vào socket bị đóng          | ✅ Có thể bắt      |
| `SIGTERM`(15)      | Yêu cầu kết thúc êm đẹp              | `kill <pid>`gửi mặc định `SIGTERM` | ✅ Có thể bắt      |
| `SIGUSR1/2`(10/12) | Tín hiệu tùy người dùng định nghĩa | Giao tiếp giữa tiến trình              | ✅ Có thể bắt      |

💡 Daemon thường xử lý `SIGHUP`, `SIGTERM`, `SIGUSR1` để reload, shutdown hoặc trigger custom actions.

---

## ⏸️▶️ **3. Tín hiệu Job Control: Dừng & Tiếp tục**

| Tín hiệu      | Mô tả                           | Đặc điểm kỹ thuật            | Có thể bắt không? |
| --------------- | --------------------------------- | ---------------------------------- | --------------------- |
| `SIGSTOP`(19) | Dừng tiến trình tức thì      | ❌ Không thể bắt/chặn/bỏ qua  | ❌                    |
| `SIGTSTP`(20) | Ctrl + Z → dừng từ terminal    | Dùng trong shell                  | ✅ Có thể bắt      |
| `SIGCONT`(18) | Tiếp tục tiến trình bị dừng | Kích hoạt trở lại tiến trình | ✅ Có thể bắt      |

📌 `SIGSTOP` là “tuyệt đối” — giống như nút “Pause” của Kernel, không thương lượng 😅.

---

## 👶 **4. Tín hiệu quản lý tiến trình con**

| Tín hiệu      | Mô tả                        | Tác dụng thực tế                                   |
| --------------- | ------------------------------ | ------------------------------------------------------ |
| `SIGCHLD`(17) | Khi con thoát hoặc bị dừng | Giúp tiến trình cha gọi `wait()`để dọn zombie |

💡 Cực kỳ quan trọng trong hệ thống đa tiến trình. Nếu không xử lý `SIGCHLD`, sẽ tạo tiến trình  **zombie** !

---

## 🔒 **5. Tín hiệu tuyệt đối — Kernel Master Control**

| Tín hiệu      | Mô tả                                           | Không thể chặn, bắt hay bỏ qua |
| --------------- | ------------------------------------------------- | ----------------------------------- |
| `SIGKILL`(9)  | Kernel “ép chết” tiến trình ngay tức khắc | ✅ Absolute                         |
| `SIGSTOP`(19) | Kernel “đóng băng” tiến trình hoàn toàn  | ✅ Absolute                         |

⛔ Anh không thể dùng `sigaction()` để bắt hay ignore chúng → chỉ có Kernel hoặc `kill -9` mới gửi được.

---

## 🧠 Mẹo thực chiến

* Dùng `sigaction()` thay vì `signal()` để xử lý linh hoạt hơn.
* Nếu dùng `SIGUSR1/2`, nên tài liệu rõ trong team: mỗi tiến trình hiểu 2 tín hiệu theo cách riêng.
* Không nên “bắt sống” `SIGSEGV` rồi cố tiếp tục chạy — chỉ nên ghi log và thoát an toàn.

---

## ✉️ **4.3. Gửi Tín hiệu**

### 🔹 Từ Shell

* `kill PID`

  → Gửi `SIGTERM` (15) mặc định.
* `kill -s SIGNAL_NAME PID`

  → Ví dụ: `kill -s HUP 1234`.
* `kill -SIGNAL_NUMBER PID`

  → Ví dụ: `kill -9 1234`.
* `killall PROGRAM_NAME`

  → Gửi tín hiệu tới tất cả tiến trình có tên `PROGRAM_NAME`.

### 🔹 Từ chương trình C/C++

* `int kill(pid_t pid, int sig);`

  → Gửi tín hiệu `sig` tới tiến trình `pid`. Cần quyền hợp lệ (thường là cùng UID hoặc root).
* `int raise(int sig);`

  → Gửi tín hiệu `sig` tới chính tiến trình gọi.
* `unsigned int alarm(unsigned int seconds);`

  → Đặt hẹn giờ gửi `SIGALRM` tới chính tiến trình sau `seconds` giây.

---

## 🛡️ **4.4. Xử lý Tín hiệu (Signal Handling)**

Một tiến trình có thể thay đổi hành vi khi nhận tín hiệu bằng cách cài đặt **Signal Handler** — hàm mà Kernel gọi khi tín hiệu tới.

### ⚠️ `signal()` – Cũ, không khuyến nghị

```cpp
void (*signal(int sig, void (*func)(int)))(int);
```

* Dễ gây  **race condition** , hành vi không nhất quán.
* Handler thường bị reset về mặc định sau khi xử lý.
* 📛 Không khuyến khích dùng trong phần mềm mới.

### ✅ `sigaction()` – Chuẩn POSIX, mạnh mẽ

```cpp
int sigaction(int sig, const struct sigaction *act, struct sigaction *oact);
```

Cung cấp điều khiển chi tiết qua `struct sigaction`:

| Thành phần   | Ý nghĩa                                                                  |
| -------------- | -------------------------------------------------------------------------- |
| `sa_handler` | Hàm xử lý tín hiệu, hoặc `SIG_IGN`,`SIG_DFL`                     |
| `sa_mask`    | Các tín hiệu sẽ bị**chặn tạm thời**khi handler đang xử lý |
| `sa_flags`   | Các cờ kiểm soát hành vi đặc biệt:                                 |

* `SA_RESTART`: Tự restart lại các system call bị ngắt
* `SA_RESETHAND`: Handler sẽ bị reset sau lần gọi đầu tiên
* `SA_SIGINFO`: Cho phép nhận thêm thông tin (`siginfo_t`, `ucontext`)
* `SA_NODEFER`: Không chặn chính tín hiệu đang xử lý

### 🧠 Quy tắc VÀNG cho Signal Handler

* **Phải re-entrant!** → an toàn khi bị ngắt giữa chừng.
* Chỉ dùng hàm async-signal-safe: `write()`, `_exit()`, `kill()`
* ❌ Tuyệt đối tránh `printf()`, `malloc()`, `free()` trong handler!
* ✅ Thực hành tốt nhất: chỉ gán cờ kiểu `volatile sig_atomic_t`, xử lý logic ở vòng lặp chính.

---

## 🎭 **4.5. Tập hợp Tín hiệu & Mặt nạ Tiến trình**

### 🔹 Kiểu dữ liệu `sigset_t`: biểu diễn tập tín hiệu

#### 🧰 Các hàm tiện ích

```cpp
sigemptyset(&set);   // Rỗng
sigfillset(&set);    // Tất cả
sigaddset(&set, sig); // Thêm tín hiệu
sigdelset(&set, sig); // Gỡ tín hiệu
sigismember(&set, sig); // Kiểm tra có trong set không
```

#### 🔧 `sigprocmask()`

```cpp
int sigprocmask(int how, const sigset_t *set, sigset_t *oset);
```

Thay đổi mặt nạ tín hiệu của tiến trình:

| `how`         | Hành vi                       |
| --------------- | ------------------------------ |
| `SIG_BLOCK`   | Thêm vào mặt nạ (chặn)    |
| `SIG_UNBLOCK` | Gỡ khỏi mặt nạ (bỏ chặn) |
| `SIG_SETMASK` | Ghi đè mặt nạ hiện tại   |

→ Các tín hiệu bị chặn sẽ **không xử lý ngay** mà được  **treo (pending)** .

#### 🕸️ `sigpending()`

```cpp
int sigpending(sigset_t *set);
```

→ Kiểm tra tín hiệu nào đang treo.

#### 🛌 `sigsuspend()`

```cpp
int sigsuspend(const sigset_t *mask);
```

→ Tạm dừng tiến trình, thay mặt nạ, chờ tín hiệu → an toàn hơn `pause()`.

---

## 🤝 **4.6. Signals và POSIX Threads**

| Đặc tính                                                   | Ý nghĩa                                                    |
| ------------------------------------------------------------- | ------------------------------------------------------------ |
| 📌 Tín hiệu là của**toàn tiến trình**            | Nhưng được**phân phối cho một luồng cụ thể** |
| 🎭 Mặt nạ tín hiệu là**của từng luồng**         | Dùng `pthread_sigmask()`để chặn riêng từng luồng    |
| 🧠 Signal Handler là**dùng chung toàn tiến trình** | Một luồng gọi → cả tiến trình biết                   |

### 🧩 Quy tắc phân phối

| Loại tín hiệu                           | Gửi đến                                                          |
| ------------------------------------------ | ------------------------------------------------------------------- |
| Đồng bộ (`SIGSEGV`,`SIGFPE`)        | Gửi đến**luồng gây lỗi**                                |
| Không đồng bộ (`SIGINT`,`SIGTERM`) | Gửi đến**một luồng không chặn tín hiệu**             |
| Tuyệt đối (`SIGKILL`,`SIGSTOP`)     | Gửi đến**toàn bộ tiến trình**— không thể chặn/bắt |

---

Muốn mình format tiếp phần core dump, job control, hay tập tín hiệu người dùng (`SIGUSR1`, `SIGUSR2`) không Anh? Hay mình làm phần minh hoạ xử lý `SIGTERM` chuyên nghiệp như trong các daemon thực tế? 😄

* **Ví dụ (C++): `signal_example.cpp` - Sử dụng `sigaction()`**
  **C++**

  ```cpp
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

# Step by Step

---

## 🧩 **Bước 1: Khởi đầu — `sigaction()` là gì?**

`sigaction()` là hàm dùng để đăng ký **hàm xử lý tín hiệu (signal handler)** một cách chi tiết và kiểm soát mạnh mẽ hơn `signal()` truyền thống.

```cpp
int sigaction(int sig, const struct sigaction *act, struct sigaction *oact);
```

* `sig`: tín hiệu muốn xử lý (ví dụ: `SIGINT`, `SIGTERM`)
* `act`: con trỏ tới `struct sigaction` mô tả cách xử lý
* `oact`: nếu không `nullptr`, sẽ lưu lại handler cũ

---

## ⚙️ **Bước 2: Khai báo và cấu hình `struct sigaction`**

```cpp
#include <signal.h>

void my_handler(int sig) {
    write(STDOUT_FILENO, "Received SIGINT\n", 16);
}

int main() {
    struct sigaction sa {};
    sa.sa_handler = my_handler;             // Gán handler
    sigemptyset(&sa.sa_mask);               // Không chặn thêm tín hiệu nào khi handler chạy
    sa.sa_flags = SA_RESTART;               // Tự restart system call bị gián đoạn

    sigaction(SIGINT, &sa, nullptr);        // Đăng ký handler cho Ctrl+C
    while (true) pause();                   // Đợi tín hiệu
}
```

📌 `pause()` sẽ treo chương trình cho đến khi nhận một tín hiệu.

---

## 🔐 **Bước 3: Xử lý mặt nạ tín hiệu (Signal Mask)**

`sa_mask` là tập hợp tín hiệu sẽ **bị chặn tạm thời** khi handler đang xử lý.

```cpp
sigemptyset(&sa.sa_mask);           // Bắt đầu với tập rỗng
sigaddset(&sa.sa_mask, SIGTERM);   // Chặn SIGTERM khi xử lý SIGINT
```

→ Khi `SIGINT` xảy ra, handler chạy, và nếu `SIGTERM` cũng được gửi cùng lúc thì nó sẽ bị treo lại — tránh việc gián đoạn bên trong handler.

---

## 🧠 **Bước 4: Hiểu các `sa_flags` thường dùng**

| Flag             | Ý nghĩa                                                                    |
| ---------------- | ---------------------------------------------------------------------------- |
| `SA_RESTART`   | Tự restart system call nếu bị gián đoạn bởi tín hiệu                |
| `SA_SIGINFO`   | Dùng hàm handler chi tiết hơn (3 tham số: sig, info, ucontext)          |
| `SA_NODEFER`   | Không chặn tín hiệu hiện tại khi handler đang xử lý                 |
| `SA_RESETHAND` | Chỉ gọi handler một lần rồi quay về hành vi mặc định (`SIG_DFL`) |

📦 Với `SA_SIGINFO`, Anh sẽ đổi handler thành:

```cpp
void my_handler(int sig, siginfo_t *info, void *ctx) {
    // info->si_pid: PID tiến trình gửi tín hiệu
    // info->si_code: loại tín hiệu
}
```

→ Rất hữu ích để phân tích nguồn gốc tín hiệu (ai gửi, lý do gì...).

---

## 🎯 **Bước 5: Các quy tắc vàng khi viết signal handler**

* **Tránh dùng hàm không an toàn** : như `printf`, `malloc`, vì không re-entrant.
* **Chỉ nên gọi `write()`, `kill()`, `_exit()` trong handler**
* **Phương pháp tốt nhất** :

```cpp
  volatile sig_atomic_t got_signal = 0;
  void handler(int sig) { got_signal = 1; }
  // Trong main: kiểm tra cờ để xử lý thật
```

---

## 🎭 **Bước 6: Quản lý tín hiệu bằng `sigset_t`**

```cpp
sigset_t set;
sigemptyset(&set);
sigaddset(&set, SIGINT);
sigprocmask(SIG_BLOCK, &set, nullptr); // Chặn SIGINT
```

✅ Khi Anh chặn tín hiệu, nó không mất — nó sẽ bị “pending” và có thể kiểm tra bằng:

```cpp
sigpending(&set); // Kiểm tra các tín hiệu đang treo
```

---

## 🛑 **Bước 7: Dừng tiến trình tạm thời để chờ tín hiệu**

```cpp
sigset_t suspend_mask;
sigemptyset(&suspend_mask);
sigsuspend(&suspend_mask); // Tạm thời thay mặt nạ và treo tiến trình
```

⚠️ Khác với `pause()`: an toàn hơn và tránh race condition.

---

## 🧵 **Bước 8: Tín hiệu trong chương trình có nhiều luồng (POSIX Threads)**

| Đặc điểm                               | Ý nghĩa                             |
| ------------------------------------------ | ------------------------------------- |
| Tín hiệu thuộc toàn tiến trình       | Không phải riêng từng luồng      |
| Mặt nạ tín hiệu là của từng luồng  | Mỗi luồng có thể chặn khác nhau |
| Handler là dùng chung toàn tiến trình | Các luồng chia sẻ handler          |

📌 Dùng `pthread_sigmask()` để chặn tín hiệu trong từng luồng:

```cpp
sigset_t set;
sigemptyset(&set);
sigaddset(&set, SIGINT);
pthread_sigmask(SIG_BLOCK, &set, nullptr); // Chặn trong luồng hiện tại
```

---

## 🧠 Vậy sau khi chặn thì chuyện gì xảy ra?

* Nếu một tín hiệu bị chặn và được gửi đến tiến trình:
  * Nó **không được xử lý ngay** (handler không chạy)
  * Kernel sẽ **lưu trạng thái tín hiệu đó trong hàng chờ (pending)**
* Khi tín hiệu đó được **gỡ khỏi mặt nạ** → Kernel **kích hoạt handler ngay lập tức** nếu vẫn còn chờ

⏱️ Gần giống hàng đợi — nhưng tín hiệu thì không xếp hàng nhiều lần: mỗi loại chỉ pending  **một lần duy nhất** !

---

## 🧪 Có thể kiểm tra bằng `sigpending()`

Ví dụ thực chiến:

```cpp
#include <signal.h>
#include <unistd.h>
#include <iostream>

int main() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    // Chặn SIGINT
    sigprocmask(SIG_BLOCK, &mask, nullptr);

    std::cout << "SIGINT đã bị chặn. Gửi Ctrl+C bây giờ...\n";
    sleep(5); // Thời gian để Anh Ctrl+C

    // Kiểm tra tín hiệu nào đang pending
    sigset_t pending;
    sigpending(&pending);

    if (sigismember(&pending, SIGINT))
        std::cout << "SIGINT đang pending!\n";

    // Bỏ chặn → xử lý ngay
    sigprocmask(SIG_UNBLOCK, &mask, nullptr);

    pause(); // đợi handler chạy
}
```

📌 Khi Ctrl+C trong lúc `sleep(5)` → `SIGINT` không chạy ngay → nhưng sẽ được xử lý sau khi `UNBLOCK`.

---

## 📦 Kết luận gọn gàng

| Tình huống                  | Kết quả                                                     |
| ----------------------------- | ------------------------------------------------------------- |
| Tín hiệu bị chặn          | Được treo lại, không bị bỏ qua                         |
| Gỡ chặn tín hiệu          | Tín hiệu pending được xử lý ngay                       |
| `sigpending()`              | Kiểm tra những tín hiệu đang chờ                        |
| Hành vi giống queue không? | Gần giống, nhưng mỗi loại tín hiệu chỉ pending 1 lần |

---
