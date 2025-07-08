## **Module 2: System Call `pipe()` và `dup()`/`dup2()` ⚙️💧**

#### **2.1. System Call `pipe()`: Tạo Unnamed Pipe (Pipe không tên) 🔗**

Bạn đã học về `popen()`, một hàm thư viện cấp cao tiện lợi. Bây giờ, chúng ta sẽ đi xuống cấp độ thấp hơn với  **`pipe()`** , một **System Call** (cuộc gọi hệ thống) cho phép bạn trực tiếp yêu cầu Kernel tạo ra một kênh giao tiếp.

---

### **1. `pipe()` là gì? 💧**

`pipe()` tạo ra một  **"pipe không tên" (unnamed pipe)** . Anh có thể hình dung nó như một ống dẫn dữ liệu tạm thời, chỉ tồn tại trong bộ nhớ Kernel và không có tên trên hệ thống file (khác với Named Pipes mà ta sẽ học sau).

Mục đích chính của nó là tạo một kênh giao tiếp **một chiều (unidirectional)** giữa các  **tiến trình có quan hệ (related processes)** , thường là giữa tiến trình cha và tiến trình con được tạo ra từ `fork()`.

---

### **2. Cú pháp và Cách hoạt động ⚙️**

**C++**

```cpp
#include <unistd.h> // Cần thiết cho hàm pipe(), read(), write()

// Cú pháp hàm:
// int pipe(int file_descriptor[2]);
```

Khi anh gọi `pipe(pipe_fds)` (với `pipe_fds` là một mảng `int` có 2 phần tử):

* Kernel sẽ tạo ra một **pipe mới** trong bộ nhớ.
* Nó sẽ tạo ra **hai File Descriptor (FD)** mới và điền chúng vào mảng `pipe_fds` của anh:
  * **`pipe_fds[0]`** : Đây là **đầu đọc (read end)** của pipe. Bất kỳ dữ liệu nào được ghi vào pipe sẽ được đọc từ FD này.
  * **`pipe_fds[1]`** : Đây là **đầu ghi (write end)** của pipe. Anh sẽ ghi dữ liệu vào pipe thông qua FD này.
* **Nguyên tắc FIFO (First-In, First-Out):** Dữ liệu được ghi vào `pipe_fds[1]` sẽ được đọc từ `pipe_fds[0]` theo đúng thứ tự đã ghi. Tức là, byte đầu tiên được ghi sẽ là byte đầu tiên được đọc.
* **Giá trị trả về:**
  * `0`: Nếu pipe được tạo thành công.
  * `-1`: Nếu thất bại (ví dụ: `errno` sẽ được đặt thành `EMFILE` nếu tiến trình đã mở quá nhiều File Descriptor, hoặc `ENFILE` nếu bảng File Descriptor của hệ thống đã đầy).

---

### **3. Luồng dữ liệu và Phạm vi sử dụng 🔄**

* **Luồng dữ liệu:**
  **Code snippet**

  ```
  graph TD
      Writer_Process[Tiến trình Ghi] -->|Ghi vào pipefd[1]| Pipe_Buffer[Bộ đệm Pipe (FIFO)]
      Pipe_Buffer -->|Đọc từ pipefd[0]| Reader_Process[Tiến trình Đọc]
  ```

  Trong sơ đồ trên:

  * `Writer_Process` (Tiến trình Ghi) sẽ sử dụng `write(pipe_fds[1], ...)` để đẩy dữ liệu vào pipe.
  * `Reader_Process` (Tiến trình Đọc) sẽ sử dụng `read(pipe_fds[0], ...)` để kéo dữ liệu từ pipe.
* **Đặc điểm quan trọng:** Pipe không tên (unnamed pipe) chỉ có thể được sử dụng giữa các tiến trình có  **chung nguồn gốc (related processes)** . Điều này có nghĩa là chúng thường được tạo bởi một tiến trình cha và sau đó được các tiến trình con (tạo ra từ `fork()`) sử dụng, vì File Descriptor được kế thừa qua `fork()`.
* **Lưu ý về `read()` và `write()`:** Vì `pipe()` trả về File Descriptor (`int`), anh **phải sử dụng các System Call cấp thấp** như **`read()`** và **`write()`** để trao đổi dữ liệu qua pipe này. Anh **không thể sử dụng** các hàm I/O cấp cao từ `stdio.h` như `fread()` hay `fwrite()` trực tiếp với các File Descriptor này (trừ khi anh chuyển đổi FD thành `FILE*` bằng `fdopen()`, nhưng điều đó lại thêm một lớp phức tạp khác).

---

### **4. Liên hệ với Embedded Linux 🤖**

* `pipe()` là một cơ chế IPC cơ bản và rất hiệu quả để tạo các đường ống dữ liệu nội bộ trong các ứng dụng đa tiến trình trên thiết bị nhúng.
* Ví dụ: một tiến trình con chuyên thu thập dữ liệu từ cảm biến có thể ghi dữ liệu thô vào một pipe, và tiến trình cha (hoặc một tiến trình con khác) có thể đọc từ pipe đó để xử lý hoặc lưu trữ dữ liệu. Điều này giúp tách biệt các chức năng và tối ưu hóa tài nguyên.

---

### **5. Ví dụ (C++): `pipe_simple.cpp` - Tạo và dùng Pipe trong một tiến trình**

Ví dụ này minh họa cách tạo một pipe và sử dụng nó để ghi dữ liệu từ một "đầu" và đọc từ "đầu" còn lại,  **tất cả trong cùng một tiến trình** . Mặc dù điều này không phải là mục đích chính của pipe (vì nó chủ yếu dùng giữa các tiến trình khác nhau), nó giúp anh hiểu cơ chế cơ bản của việc ghi và đọc qua pipe.

**C++**

```cpp
#include <iostream>   // For std::cout, std::cerr
#include <string>     // For std::string, std::to_string
#include <unistd.h>   // For pipe, read, write, close, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO
#include <cstdlib>    // For EXIT_SUCCESS, EXIT_FAILURE
#include <cstring>    // For memset, strlen, strerror (used by std::perror implicitly)
#include <map>        // For std::map (just to define AppLogger style)
#include <errno.h>    // For errno

// Không sử dụng AppLogger::log nữa, thay bằng std::cout trực tiếp
// namespace AppLogger { ... }

int main() {
    int pipe_fds[2]; // Mảng 2 File Descriptor cho pipe
    const char some_data[] = "Hello Pipe World!";
    char buffer[BUFSIZ + 1]; // BUFSIZ thường là 8192, định nghĩa trong <cstdio>
    ssize_t data_processed;

    std::memset(buffer, '\0', sizeof(buffer)); // Khởi tạo buffer

    std::cout << "INFO    : --- Demonstrating simple pipe() ---" << std::endl;

    // Tạo pipe
    // Gọi pipe() với pipe_fds. Nếu thành công, pipe_fds[0] là đầu đọc, pipe_fds[1] là đầu ghi.
    if (pipe(pipe_fds) == -1) {
        std::cerr << "CRITICAL: Failed to create pipe: " << std::strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "SUCCESS : Pipe created successfully." << std::endl;
    std::cout << "INFO    : Read end FD: " << pipe_fds[0] << ", Write end FD: " << pipe_fds[1] << std::endl;

    // Ghi dữ liệu vào đầu ghi của pipe (pipe_fds[1])
    std::cout << "INFO    : Writing data to pipe's write end (FD " << pipe_fds[1] << "): '" << some_data << "'" << std::endl;
    data_processed = write(pipe_fds[1], some_data, std::strlen(some_data));
    if (data_processed == -1) {
        std::cerr << "ERROR   : Write to pipe failed: " << std::strerror(errno) << std::endl;
        // Đóng các FD nếu có lỗi để tránh rò rỉ
        close(pipe_fds[0]); close(pipe_fds[1]);
        return EXIT_FAILURE;
    }
    std::cout << "SUCCESS : Wrote " << data_processed << " bytes to pipe." << std::endl;

    // Đọc dữ liệu từ đầu đọc của pipe (pipe_fds[0])
    std::cout << "INFO    : Reading data from pipe's read end (FD " << pipe_fds[0] << ")..." << std::endl;
    data_processed = read(pipe_fds[0], buffer, BUFSIZ);
    if (data_processed == -1) {
        std::cerr << "ERROR   : Read from pipe failed: " << std::strerror(errno) << std::endl;
        // Đóng các FD nếu có lỗi
        close(pipe_fds[0]); close(pipe_fds[1]);
        return EXIT_FAILURE;
    } else if (data_processed == 0) {
        // Đọc 0 bytes thường có nghĩa là EOF. Trong pipe, điều này xảy ra khi tất cả các đầu ghi đã đóng.
        std::cout << "WARNING : Read 0 bytes (EOF from pipe). This means the write end was closed before reading." << std::endl;
    } else {
        buffer[data_processed] = '\0'; // Null-terminate chuỗi để in an toàn
        std::cout << "SUCCESS : Read " << data_processed << " bytes: '" << std::string(buffer) << "'" << std::endl;
    }

    // Đóng các File Descriptor của pipe
    std::cout << "INFO    : Closing pipe File Descriptors." << std::endl;
    close(pipe_fds[0]);
    close(pipe_fds[1]);
    std::cout << "SUCCESS : Pipe FDs closed." << std::endl;

    std::cout << "INFO    : --- pipe() Demonstration Finished ---" << std::endl;

    return EXIT_SUCCESS;
}
```

---

### **Cách Biên dịch và Chạy:**

**Bash**

```
g++ pipe_simple.cpp -o pipe_simple
./pipe_simple
```

**2.2. `pipe()` qua `fork()`: Giao tiếp giữa Cha và Con 👨‍👧‍👦**

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

---

## 🔧 Mục tiêu

Xây dựng **2 tiến trình riêng biệt** (gọi là `writer` và `reader`) giao tiếp với nhau bằng **pipe()**, không dùng `popen()`, không truyền dữ liệu cho chính mình. Cụ thể:

- `writer` tạo pipe → ghi dữ liệu vào pipe
- `reader` nhận dữ liệu đó từ `stdin` (nhờ `dup2()` redirect)
- `writer` dùng `fork()` + `exec()` để gọi `reader` như một binary riêng biệt

---

## 🧠 Tổng quan pipeline

```text
[writer.cpp] 
   |
   |--- pipe() tạo mảng 2 fd: [read_fd, write_fd]
   |
   |--- fork()
         |-- Parent: ghi vào write_fd
         |-- Child:
             |-- dup2(read_fd → STDIN_FILENO)
             |-- exec("./reader")
```

---

## 🧩 writer.cpp – Tạo pipe và gửi dữ liệu

```cpp
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // 👉 Child process → chạy reader
        close(pipefd[1]); // Đóng đầu ghi
        dup2(pipefd[0], STDIN_FILENO); // Chuyển read-end thành stdin
        close(pipefd[0]);

        execl("./reader", "reader", NULL);
        perror("exec");
        exit(1);
    } else {
        // 👉 Parent process → ghi dữ liệu vào pipe
        close(pipefd[0]); // Đóng đầu đọc
        const char* msg = "🔔 Hello from writer!\n";
        write(pipefd[1], msg, strlen(msg)); // Gửi dữ liệu
        close(pipefd[1]); // Đóng đầu ghi khi xong
        wait(nullptr);   // Chờ child kết thúc
    }

    return 0;
}
```

---

## 🧾 reader.cpp – Nhận dữ liệu từ stdin (đã redirect từ pipe)

```cpp
#include <iostream>
#include <string>

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        std::cout << "📥 Reader nhận được: " << line << std::endl;
    }
    return 0;
}
```

---

## 🔍 Phân tích từng bước hoạt động

| Thành phần         | Hành vi                                                    |
|--------------------|------------------------------------------------------------|
| `pipe(pipefd)`     | Tạo mảng gồm 2 file descriptor: `pipefd[0]` đọc, `pipefd[1]` ghi |
| `fork()`           | Tạo child process độc lập                                   |
| `dup2(pipefd[0], 0)` | Child chuyển hướng stdin thành `read-end` của pipe         |
| `execl()`          | Child thực thi tiến trình reader mới                       |
| `write()`          | Parent gửi dữ liệu sang pipe                               |
| `std::getline()`   | Reader đọc từ stdin — thực tế là nhận dữ liệu từ pipe      |

---

## 📦 Dữ liệu đi như sau:

```text
[writer] write(pipefd[1], ...)  →  [kernel] pipe buffer  →  [reader] std::getline(stdin)
```

✅ Đây là **IPC thực thụ** giữa 2 tiến trình — kernel quản lý buffer pipe nằm giữa.

---

## 🛡️ Xử lý lỗi nên có

- Kiểm tra `pipe()`, `fork()`, `dup2()`, `execl()` đều có thể lỗi → dùng `perror()` + `exit()` để đảm bảo dễ debug
- Dọn dẹp file descriptor không dùng ở mỗi process để tránh bị treo do pipe không đóng (EOF không tới)

---

## 💬 Mở rộng nâng cao

| Ý tưởng mở rộng       | Mô tả                                                      |
|-----------------------|-------------------------------------------------------------|
| Dùng 2 pipe           | Để giao tiếp 2 chiều → writer ↔ reader (bi-directional)     |
| Dùng FIFO (named pipe)| Pipe tồn tại trên hệ thống, cho phép tiến trình độc lập hoàn toàn |
| Dùng `poll()` / `select()` | Giao tiếp phi blocking, nhiều tiến trình cùng đọc pipe   |
| Biến `reader` thành Python | Dễ viết script reader → ghép các ngôn ngữ khác nhau       |

---

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

  ```cpp
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
