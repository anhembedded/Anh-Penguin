
---

### **1. Signal là gì? 🔔**

**Signal** là một **thông báo không đồng bộ (asynchronous notification)** được gửi đến một tiến trình (process) để báo hiệu một sự kiện đã xảy ra. Nó giống như một "cú điện thoại" bất ngờ mà hệ điều hành hoặc một tiến trình khác gọi cho tiến trình của bạn.

* Khi một tín hiệu được gửi đi, ta gọi là **"raise"** (tạo tín hiệu).
* Khi một tiến trình nhận tín hiệu, ta gọi là **"catch"** (bắt tín hiệu).

**Nguồn gốc của tín hiệu:**
* **Hệ điều hành/Kernel:** Do các lỗi nghiêm trọng (ví dụ: truy cập bộ nhớ bất hợp lệ).
* **Terminal/Shell:** Do người dùng nhập các tổ hợp phím (ví dụ: `Ctrl+C`).
* **Các tiến trình khác:** Một tiến trình có thể gửi tín hiệu đến tiến trình khác để yêu cầu hành động.

---

### **2. Các Loại Tín hiệu Phổ biến và Hành vi Mặc định của chúng 📋**

Mỗi tín hiệu có một tên duy nhất bắt đầu bằng "SIG".

* **Tín hiệu gây kết thúc chương trình kèm Core Dump (lỗi nghiêm trọng) 💀:**
    * `SIGSEGV` (Segmentation Fault): Truy cập bộ nhớ bất hợp pháp.
    * `SIGFPE` (Floating-Point Exception): Lỗi số học dấu phẩy động (ví dụ: chia cho 0).
    * `SIGILL` (Illegal Instruction): Lệnh CPU không hợp lệ.
    * `SIGABRT` (Abort): Bị hủy bỏ (do hàm `abort()` gọi).
    * `SIGQUIT` (Terminal Quit): Thoát từ terminal (Ctrl+\).
    * **Ý nghĩa:** Tạo ra file `core` (core dump), một "ảnh chụp" bộ nhớ của tiến trình tại thời điểm lỗi, cực kỳ hữu ích cho việc gỡ lỗi (debugging).

* **Tín hiệu yêu cầu kết thúc hoặc điều khiển (thường không tạo core dump) 🚪:**
    * `SIGINT` (Terminal Interrupt): Ngắt từ terminal (Ctrl+C).
    * `SIGTERM` (Termination): Yêu cầu tiến trình kết thúc một cách duyên dáng. Đây là tín hiệu mặc định của lệnh `kill`.
    * `SIGHUP` (Hangup): Thường gửi khi terminal ngắt kết nối; được dùng để yêu cầu các daemon tải lại cấu hình mà không khởi động lại.
    * `SIGPIPE` (Broken Pipe): Ghi vào một pipe/socket mà không có bên nhận.
    * `SIGUSR1`, `SIGUSR2` (User-defined Signals): Hai tín hiệu có thể được sử dụng tự do cho mục đích giao tiếp giữa các tiến trình của bạn.

* **Tín hiệu dừng/tiếp tục tiến trình (Job Control) ⏸️▶️:**
    * `SIGSTOP` (Stop): Dừng tiến trình ngay lập tức (không thể bắt/bỏ qua).
    * `SIGTSTP` (Terminal Stop): Dừng tiến trình từ terminal (Ctrl+Z, có thể bắt).
    * `SIGCONT` (Continue): Tiếp tục một tiến trình đã dừng.
    * `SIGTTIN`, `SIGTTOU`: Liên quan đến việc các tiến trình nền cố gắng đọc/ghi từ terminal.

* **Tín hiệu quản lý tiến trình con 👶:**
    * `SIGCHLD` (Child Stop/Exit): Được gửi đến tiến trình cha khi một tiến trình con dừng hoặc kết thúc. Mặc định bị bỏ qua nhưng rất quan trọng để quản lý và "dọn dẹp" các tiến trình zombie.

* **Các tín hiệu không thể Bắt hoặc Bỏ qua 🔒:**
    * `SIGKILL` (Kill): Buộc tiến trình kết thúc ngay lập tức.
    * `SIGSTOP` (Stop): Buộc tiến trình dừng ngay lập tức.
    * Hai tín hiệu này không thể bị chặn, bắt, hay bỏ qua; chúng là "lệnh tuyệt đối" từ Kernel.

---

### **3. Gửi Tín hiệu ✉️**

* **Từ Shell:**
    * `kill PID`: Gửi `SIGTERM` mặc định.
    * `kill -s SIGNAL_NAME PID` (ví dụ: `kill -s HUP 1234`).
    * `kill -SIGNAL_NUMBER PID` (ví dụ: `kill -9 1234`).
    * `killall PROGRAM_NAME`: Gửi tín hiệu đến tất cả tiến trình có tên `PROGRAM_NAME`.
* **Từ chương trình C/C++:**
    * `kill(pid_t pid, int sig);`: Gửi tín hiệu `sig` tới tiến trình `pid`.
    * `raise(int sig);`: Gửi tín hiệu `sig` tới chính tiến trình gọi.
    * `alarm(unsigned int seconds);`: Đặt hẹn giờ để gửi `SIGALRM` tới chính tiến trình sau `seconds` giây.

---

### **4. Xử lý Tín hiệu: `sigaction()` là Lựa chọn Tốt nhất! 🛡️**

Khi một tiến trình nhận được tín hiệu, nó có thể phản ứng theo 3 cách: hành vi mặc định, bỏ qua, hoặc bắt và xử lý bằng một hàm tùy chỉnh.

* **Hàm `signal()` (Cũ và không khuyến nghị):**
    * Là hàm truyền thống nhưng có hành vi không nhất quán và dễ gây ra điều kiện tranh chấp (race conditions). Trình xử lý thường tự động đặt lại về mặc định sau khi được gọi.

* **Hàm `sigaction()` (Hiện đại, Mạnh mẽ, và được Khuyến nghị) ✅:**
    * Cung cấp kiểm soát chi tiết hơn thông qua cấu trúc `struct sigaction`.
    * **`sa_handler`**: Con trỏ tới hàm xử lý của bạn, hoặc `SIG_IGN` (bỏ qua), `SIG_DFL` (hành vi mặc định).
    * **`sa_mask`**: Một **tập hợp các tín hiệu** sẽ bị **chặn** trong suốt thời gian hàm `sa_handler` đang chạy. Điều này ngăn chặn các tín hiệu khác làm gián đoạn trình xử lý, giúp tránh lỗi.
    * **`sa_flags`**: Các cờ để điều chỉnh hành vi:
        * `SA_RESTART`: Khi một System Call bị gián đoạn bởi tín hiệu, nó sẽ tự động khởi động lại sau khi handler hoàn tất.
        * `SA_RESETHAND`: Đặt lại handler về mặc định sau khi được gọi một lần (giống `signal()`).
        * `SA_SIGINFO`: (Rất quan trọng) Cho phép handler nhận thêm thông tin chi tiết về tín hiệu và ngữ cảnh.
        * `SA_NODEFER`: Không thêm tín hiệu hiện tại vào mask khi handler chạy (ít dùng, cẩn thận với re-entrancy).

* **Quy tắc Vàng cho Signal Handler ⚠️:**
    * Hàm xử lý tín hiệu **PHẢI re-entrant** (có thể bị gọi lại một cách an toàn khi nó chưa hoàn thành).
    * Chỉ được phép gọi một tập hợp rất hạn chế các hàm an toàn (async-signal-safe functions) bên trong signal handler (ví dụ: `write` với một file descriptor cố định, `_exit`, `kill`).
    * **Thực hành tốt nhất:** Signal handler chỉ nên **thiết lập một cờ (flag) toàn cục** và sau đó vòng lặp chính của chương trình sẽ kiểm tra cờ đó để thực hiện các tác vụ phức tạp hơn.

---

### **5. Tập hợp Tín hiệu (Signal Sets) & Mặt nạ Tiến trình 🎭**

* **`sigset_t`**: Một kiểu dữ liệu để biểu diễn một tập hợp các tín hiệu.
* **Các hàm tiện ích**: `sigemptyset()` (khởi tạo rỗng), `sigaddset()` (thêm), `sigdelset()` (xóa), `sigfillset()` (chứa tất cả).
* **`sigprocmask()`**: Được dùng để **thay đổi mặt nạ tín hiệu của tiến trình**. Mặt nạ này xác định những tín hiệu nào mà tiến trình hiện đang chặn (không muốn nhận). Tín hiệu bị chặn sẽ bị treo (pending) cho đến khi nó được bỏ chặn.
* **`sigsuspend()`**: Tạm dừng tiến trình cho đến khi một tín hiệu không bị chặn được gửi đến. Đây là cách an toàn hơn để chờ tín hiệu so với `pause()`.

---

### **6. Signals và POSIX Threads 🤝**

Đây là một điểm phức tạp nhưng then chốt:

* **Signals là của Tiến trình, nhưng được Phân phối cho một Luồng cụ thể.**
* **Mặt nạ tín hiệu (Signal Mask) là CỦA TỪNG LUỒNG.** Mỗi luồng có thể chặn các tín hiệu khác nhau.
* **Trình xử lý tín hiệu (Signal Handler) là CỦA TOÀN BỘ TIẾN TRÌNH.** Tất cả các luồng trong một tiến trình chia sẻ cùng một bảng trình xử lý tín hiệu.
* **Quy tắc phân phối tín hiệu:**
    * **Tín hiệu đồng bộ (`SIGSEGV`, `SIGFPE`):** Luôn được gửi đến **luồng đã gây ra lỗi**.
    * **Tín hiệu không đồng bộ (`SIGINT`, `SIGTERM`):** Kernel sẽ gửi đến **một luồng bất kỳ** trong tiến trình đó mà **không chặn** tín hiệu đó. Nếu tất cả các luồng đều chặn, tín hiệu sẽ bị treo.
    * **`SIGKILL` và `SIGSTOP`:** Luôn ảnh hưởng đến **toàn bộ tiến trình**, không thể bị chặn hoặc bắt bởi bất kỳ luồng nào.

---

### **7. Liên hệ với FreeRTOS, Windows và Embedded Systems 🌍**

* **Embedded Linux (Rất quan trọng cho bạn!) 🤖:**
    * **Tắt máy duyên dáng (Graceful Shutdown):** Bắt `SIGTERM`, `SIGHUP` để lưu dữ liệu, đóng file, giải phóng tài nguyên phần cứng trước khi ứng dụng kết thúc.
    * **Xử lý Lỗi và Phục hồi:** Bắt `SIGSEGV`, `SIGFPE` để ghi log lỗi, tạo core dump, và cố gắng phục hồi.
    * **Giao tiếp liên tiến trình (IPC):** Sử dụng `SIGUSR1`/`SIGUSR2` để báo hiệu sự kiện giữa các thành phần phần mềm.
    * **Quản lý Tiến trình Con:** Xử lý `SIGCHLD` để dọn dẹp các zombie và theo dõi trạng thái của các tiến trình con.
    * **Hiệu quả Tài nguyên:** Dùng `sigsuspend()` để chờ sự kiện thay vì "busy-waiting", giúp tiết kiệm CPU và năng lượng trên thiết bị nhúng.

* **FreeRTOS (và các RTOS khác) ⏱️:**
    * **Không dùng Signals theo kiểu Unix:** RTOS tập trung vào tính xác định thời gian thực.
    * **Cơ chế thay thế:** Sử dụng **ngắt (interrupts)**, **ISR (Interrupt Service Routines)**, **Task Notifications**, **Queues**, **Semaphores**, và **Mutexes** để xử lý các sự kiện không đồng bộ và đồng bộ hóa giữa các task.

* **Windows 🪟:**
    * **Không dùng Signals theo kiểu Unix:** Windows có mô hình khác.
    * **Cơ chế thay thế:** Dùng **Events**, **APCs (Asynchronous Procedure Calls)**, **Message Queues (hàng đợi tin nhắn)**, và **Structured Exception Handling (SEH)** để xử lý các sự kiện và lỗi tương tự.

---

### **Lời khuyên quan trọng cho bạn (Embedded Engineer) 🛠️**

* **Luôn sử dụng `sigaction()`** thay vì `signal()` trong các chương trình mới.
* **Hiểu rõ `sa_mask` và `sa_flags`** để kiểm soát hành vi tín hiệu một cách chính xác.
* **Thiết kế Signal Handler cực kỳ cẩn thận:** Chỉ làm những việc tối thiểu, sử dụng cờ và xử lý logic chính trong vòng lặp chính.
* **Biết cách `kill` và `killall`** để kiểm soát các daemon và dịch vụ trên thiết bị nhúng của bạn.
* **Nắm vững cách tín hiệu tương tác với Threads** để tránh các lỗi khó lường và xây dựng ứng dụng đa luồng bền vững.

Hiểu về Signals là một trong những cột mốc quan trọng để bạn thực sự làm chủ Linux programming, đặc biệt là khi bạn xây dựng các hệ thống nhúng có yêu cầu cao về độ tin cậy và khả năng quản lý lỗi.

Bạn có muốn đi sâu vào một khía cạnh cụ thể nào trong phần tóm tắt này nữa không, hay chúng ta sẽ tiếp tục khám phá các chủ đề khác? 🤔