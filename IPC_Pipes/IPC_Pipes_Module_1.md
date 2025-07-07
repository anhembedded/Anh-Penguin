# **Giáo trình: Giao tiếp Liên Tiến trình: Pipes 💧**

**Mục tiêu của Giáo trình 🎯**

Sau khi hoàn thành giáo trình này, bạn sẽ có thể:

* Hiểu định nghĩa và các loại Pipe (unnamed pipes, named pipes/FIFOs).
* Sử dụng các hàm cấp cao (`popen`, `pclose`) để giao tiếp với các lệnh shell.
* Sử dụng System Call cấp thấp (`pipe`) để tạo và quản lý pipe giữa các tiến trình liên quan.
* Áp dụng `dup()` và `dup2()` để chuyển hướng I/O chuẩn qua pipes.
* Nắm vững khái niệm Named Pipes (FIFOs) để giao tiếp giữa các tiến trình không liên quan.
* Hiểu các vấn đề về chặn/không chặn (blocking/non-blocking) và tính nguyên tử (atomicity) khi dùng FIFOs.
* Xây dựng các ứng dụng Client/Server đơn giản sử dụng Named Pipes.
* Liên hệ các cơ chế Pipe với các hệ điều hành khác như Windows và môi trường RTOS.

---

### **Cấu trúc Giáo trình 📚**

Giáo trình này sẽ được chia thành các Modules nhỏ để dễ dàng theo dõi và tiếp thu:

* **Module 1: Pipe là gì? và `popen()`/`pclose()`**
* **Module 2: System Call `pipe()` và `dup()`/`dup2()`**
* **Module 3: Named Pipes (FIFOs)**
* **Module 4: Ứng dụng Client/Server với FIFOs**
* **Module 5: Luyện tập Tổng hợp & Ứng dụng**

Mỗi Module sẽ bao gồm:

* **Lý thuyết chi tiết:** Giải thích các khái niệm, hàm, và cơ chế.
* **Ví dụ Code (C++):** Minh họa cách sử dụng các hàm.
* **Liên hệ với Embedded Linux, Windows, RTOS:** Giải thích tầm quan trọng và ứng dụng trong các môi trường khác nhau.
* **Hình ảnh/Biểu đồ:** Minh họa để dễ hiểu hơn.
* **Câu hỏi Tự đánh giá:** Giúp bạn kiểm tra mức độ hiểu bài.
* **Bài tập Thực hành:** Các bài tập coding để bạn áp dụng kiến thức.

Hãy bắt đầu với Module đầu tiên!

---

### **Module 1: Pipe là gì? và `popen()`/`pclose()` 💧**

Module này sẽ giới thiệu khái niệm cơ bản về pipe và cách sử dụng các hàm cấp cao `popen()` và `pclose()` để tạo các kênh giao tiếp đơn giản.

#### **1.1. Pipe là gì? (What Is a Pipe?)**

* **Lý thuyết:** Pipe (ống dẫn) là một cơ chế giao tiếp liên tiến trình (IPC) cho phép luồng dữ liệu chảy **một chiều (unidirectional)** từ đầu ra của một tiến trình sang đầu vào của một tiến trình khác.
  * Hãy hình dung nó như một ống nước chỉ cho phép nước chảy theo một hướng nhất định.
  * Nó không phải là một file vật lý trên đĩa cứng mà là một vùng nhớ đệm (buffer) do Kernel quản lý.
* **Ví dụ quen thuộc trong Shell:** Hầu hết người dùng Linux đều quen thuộc với việc sử dụng dấu pipe `|` trong shell để nối các lệnh: `cmd1 | cmd2`.
  * Shell sẽ sắp xếp để đầu ra chuẩn (`stdout`) của `cmd1` được chuyển thành đầu vào chuẩn (`stdin`) của `cmd2`).
  * **Minh họa:**
    **Code snippet**

    ```plantuml
    @startuml
    title 🎯 Pipe Data Flow: cmd1 | cmd2

    actor User as U
    rectangle "Cmd 1\n(Std Input)" as C1
    rectangle "Cmd 1\n(Std Output)" as C2
    rectangle "Pipe" as P
    rectangle "Cmd 2\n(Std Input)" as C3
    rectangle "Cmd 2\n(Std Output)" as C4
    actor Terminal as T

    U --> C1
    C1 --> C2
    C2 --> P
    P --> C3
    C3 --> C4
    C4 --> T

    @enduml
    ```

    * Ở đây, `stdout` từ `Cmd 1` được "đẩy" vào `Pipe`, và `Pipe` này lại "đẩy" dữ liệu vào `stdin` của `Cmd 2`.
* **Mục đích:** Cho phép các chương trình làm việc cùng nhau một cách tuần tự, xử lý dữ liệu theo từng bước (ví dụ: lọc, chuyển đổi, tổng hợp).
* **Liên hệ Embedded Linux:** Pipes hữu ích cho việc xây dựng các pipeline xử lý dữ liệu đơn giản trên thiết bị nhúng, nơi một chương trình thu thập dữ liệu thô, sau đó "pipe" cho chương trình khác để xử lý hoặc lưu trữ.

#### **1.2. `popen()` và `pclose()`: Ống dẫn Tiến trình Cấp cao 🚀**

## 🔧 `popen()` và `pclose()` — Kênh giao tiếp cấp cao với Shell

Hai hàm này là cách **giao tiếp một chiều** giữa chương trình C/C++ và một lệnh shell bên dưới.

### 🧠 Tưởng tượng thế này:

* Anh có một chương trình C là "người gọi lệnh"
* Anh cần gửi input cho lệnh đó (ghi) hoặc nhận output từ nó (đọc)
* `popen()` tạo kết nối giống như một cái "ống dẫn" (pipe) giữa hai bên

---

### 🧨 1. `popen()` – Gọi lệnh shell và mở pipe

```cpp
#include <cstdio> // popen, FILE

FILE* popen(const char* command, const char* mode);
```

* **`command`** : chuỗi shell cần thực thi → ví dụ `"ls -l"`, `"uname -a"`, `"grep Hello"`
* **`mode`** :
* `"r"` → đọc output của lệnh đó (stdout → pipe → mình đọc)
* `"w"` → ghi input vào lệnh đó (mình ghi → pipe → stdin của lệnh)

📦  **Kết quả** : trả về con trỏ `FILE*` để thao tác như đọc/ghi file bình thường

---

### 🌀 Cách hoạt động bên trong:

| Bước                             | Giải thích                             |
| ---------------------------------- | ---------------------------------------- |
| `popen()`gọi `fork()`         | tạo tiến trình con để chạy lệnh   |
| Tiến trình con `exec()`lệnh   | thực thi `sh -c command`như shell    |
| Tiến trình cha được `FILE*` | dùng `fgets()`,`fprintf()`với pipe |

🛑 Pipe chỉ **một chiều** – nếu muốn hai chiều thì phải tạo 2 pipe thủ công!

---

### 🛑 2. `pclose()` – Đóng pipe, lấy exit code

```cpp
#include <cstdio> // pclose
int pclose(FILE* stream);
```

* Đóng stream nhận được từ `popen()`
* Đợi tiến trình con hoàn tất
* Trả về mã thoát (`exit code`) của lệnh → có thể kiểm tra thành công/thất bại

---

## 🔬 Ví dụ dùng thực tế: Đọc hostname từ shell

```cpp
FILE* fp = popen("hostname", "r");
if (!fp) perror("popen");

char buffer[128];
while (fgets(buffer, sizeof(buffer), fp)) {
    printf("Host: %s", buffer);
}
int status = pclose(fp);
```

---

## 🔗 Liên hệ với Embedded Linux

| Tình huống                                   | Cách dùng `popen()`                      |
| ---------------------------------------------- | -------------------------------------------- |
| Lấy thông tin hệ thống (`uname`,`top`) | `popen("uname -a", "r")`đọc kết quả    |
| Gửi dữ liệu cho lệnh shell xử lý         | `popen("grep pattern", "w")`ghi vào grep  |
| Chạy script thu thập sensor                  | `popen("./sensor.sh", "r")`đọc giá trị |

🎯 Dễ dùng hơn so với `fork()` + `pipe()` + `exec()` → đặc biệt khi xử lý đơn giản.

---

## 🔁 So sánh với Windows và RTOS

### 🪟 Trên Windows:

* Không có `popen()` gốc kiểu POSIX
* Có `_popen()` và `_pclose()` tương đương (Visual Studio)
* IPC dùng `CreatePipe()`, `CreateNamedPipe()`

### ⚙️ Trên RTOS (như FreeRTOS):

| POSIX `pipe()`equivalent | RTOS cơ chế thay thế                     |
| -------------------------- | ------------------------------------------- |
| Gửi data qua pipe         | **Message Queue**(`xQueueSend`)     |
| Đồng bộ ghi/đọc       | **Semaphore** ,**Event Groups** |
| Gửi tín hiệu cực nhẹ  | **Direct Task Notification**          |

📌 RTOS không có tiến trình → không dùng được kiểu "gọi shell" như Linux. IPC của RTOS được tối ưu để chạy nhanh, nhẹ, chính xác.

---

### **1.4. Ví dụ (C++): `popen_example.cpp` - Đọc/Ghi qua `popen()`**

**C++**

```cpp
#include <iostream>
#include <string>
#include <cstdio>   // For popen, pclose, FILE, BUFSIZ, sprintf, fread, fwrite
#include <cstdlib>  // For EXIT_SUCCESS, EXIT_FAILURE
#include <cstring>  // For memset, strlen
#include <errno.h>  // For errno

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
    FILE *pipe_fp;
    char buffer[BUFSIZ + 1];
    int chars_read;

    AppLogger::log(AppLogger::INFO_L, "--- Demonstrating popen() ---");

    // --- Ví dụ 1: Đọc output từ một lệnh ngoại vi ("r" mode) ---
    const char *command_read = "uname -a";
    AppLogger::log(AppLogger::INFO_L, "Calling popen(\"" + std::string(command_read) + "\", \"r\")...");
  
    memset(buffer, '\0', sizeof(buffer)); // Khởi tạo buffer

    pipe_fp = popen(command_read, "r");
    if (pipe_fp == NULL) {
        AppLogger::log(AppLogger::CRITICAL_L, "popen failed for reading: " + std::string(strerror(errno)));
        return EXIT_FAILURE;
    }
    AppLogger::log(AppLogger::SUCCESS_L, "popen succeeded for reading. Reading output...");

    chars_read = fread(buffer, sizeof(char), BUFSIZ, pipe_fp);
    if (chars_read > 0) {
        // Đảm bảo chuỗi kết thúc bằng null
        buffer[chars_read] = '\0'; // BUFSIZ+1 cho '\0'
        AppLogger::log(AppLogger::INFO_L, "Output from '" + std::string(command_read) + "':\n" + buffer);
    } else {
        AppLogger::log(AppLogger::WARNING_L, "No output read from '" + std::string(command_read) + "'.");
    }

    if (pclose(pipe_fp) == -1) {
        AppLogger::log(AppLogger::ERROR_L, "pclose failed for reading pipe: " + std::string(strerror(errno)));
    } else {
        AppLogger::log(AppLogger::SUCCESS_L, "pclose succeeded for reading pipe. Command exited normally.");
    }

    // --- Ví dụ 2: Ghi input cho một lệnh ngoại vi ("w" mode) ---
    const char *command_write = "od -c"; // Lệnh octal dump, đọc từ stdin
    const char *data_to_pipe = "Once upon a time, there was...\n";
    AppLogger::log(AppLogger::INFO_L, "\nCalling popen(\"" + std::string(command_write) + "\", \"w\")...");

    pipe_fp = popen(command_write, "w");
    if (pipe_fp == NULL) {
        AppLogger::log(AppLogger::CRITICAL_L, "popen failed for writing: " + std::string(strerror(errno)));
        return EXIT_FAILURE;
    }
    AppLogger::log(AppLogger::SUCCESS_L, "popen succeeded for writing. Writing data to pipe...");

    size_t bytes_written = fwrite(data_to_pipe, sizeof(char), strlen(data_to_pipe), pipe_fp);
    if (bytes_written != strlen(data_to_pipe)) {
        AppLogger::log(AppLogger::ERROR_L, "fwrite did not write all data to pipe. Bytes written: " + std::to_string(bytes_written));
    } else {
        AppLogger::log(AppLogger::INFO_L, "Wrote " + std::to_string(bytes_written) + " bytes to pipe.");
    }

    if (pclose(pipe_fp) == -1) {
        AppLogger::log(AppLogger::ERROR_L, "pclose failed for writing pipe: " + std::string(strerror(errno)));
    } else {
        AppLogger::log(AppLogger::SUCCESS_L, "pclose succeeded for writing pipe. Command exited normally.");
    }
    AppLogger::log(AppLogger::INFO_L, "--- popen() Demonstration Finished ---");

    return EXIT_SUCCESS;
}
```

* **Cách Biên dịch:**
  **Bash**

  ```
  g++ popen_example.cpp -o popen_example
  ```
* **Cách Chạy:**
  **Bash**

  ```
  ./popen_example
  ```
* **Phân tích Output:** Bạn sẽ thấy output của lệnh `uname -a` và `od -c` xuất hiện trực tiếp trên console, chứng minh `popen()` đã thành công trong việc chuyển hướng I/O.

---

### **Câu hỏi Tự đánh giá Module 1 🤔**

1. Giải thích khái niệm "pipe" trong Linux. Nó khác gì với một file thông thường trên đĩa?
2. Khi bạn chạy lệnh `cmd1 | cmd2` trong shell, điều gì xảy ra với standard input và output của `cmd1` và `cmd2`?
3. Hàm `popen()` được dùng để làm gì? Nêu hai chế độ mở (`open_mode`) phổ biến của nó.
4. Tại sao `popen()` chỉ cho phép giao tiếp một chiều tại một thời điểm?
5. Giải thích overhead của việc sử dụng `popen()` so với các phương pháp IPC cấp thấp hơn như `pipe()` System Call.
6. `pclose()` có vai trò gì? Điều gì xảy ra nếu bạn không gọi `pclose()` sau khi dùng `popen()`?
7. Nêu một cơ chế IPC tương đương với pipe trong môi trường Windows và một cơ chế trong môi trường RTOS như FreeRTOS. Chúng có điểm khác biệt cơ bản nào về triết lý so với pipe của Linux?

---

### **Bài tập Thực hành Module 1 ✍️**

1. **Chương trình "Filter with `popen`":**
   * Viết một chương trình C++ (`my_filter.cpp`) mà:
     * Nhận một tham số dòng lệnh là tên lệnh shell (ví dụ: `"sort -r"`, `"wc -l"`).
     * Đọc dữ liệu từ `stdin` của chương trình của bạn.
     * Sử dụng `popen()` ở chế độ ghi (`"w"`) để gửi dữ liệu đọc được cho lệnh shell đó.
     * Đóng pipe và in ra mã thoát của lệnh shell.
   * **Thử thách:**
     * Tạo một file `numbers.txt` với các số không theo thứ tự.
     * Chạy: `cat numbers.txt | ./my_filter "sort -n"` và quan sát output.
     * Chạy: `echo -e "Hello\nWorld\nHello" | ./my_filter "wc -l"`
     * Chạy: `./my_filter "od -c"` và gõ input trực tiếp.
2. **Chương trình "Ghi Log Lệnh":**
   * Viết một chương trình C++ (`command_logger.cpp`) mà:
     * Nhận một tham số dòng lệnh là tên lệnh shell (ví dụ: `"ls -l"`, `"df -h"`).
     * Sử dụng `popen()` ở chế độ đọc (`"r"`) để chạy lệnh đó.
     * Đọc toàn bộ output của lệnh vào một buffer.
     * Ghi output này vào một file log (`command_output.log`) với dấu thời gian hiện tại.
     * In ra thông báo thành công/thất bại và mã thoát của lệnh.
   * **Thử thách:** Xử lý các lỗi khi `popen()` thất bại hoặc khi ghi log file.


---

### 🔧 `FILE *popen(const char *command, const char *type);`

| Tham số       | Ý nghĩa                                                                               |
| -------------- | --------------------------------------------------------------------------------------- |
| `command`    | Chuỗi lệnh shell cần thực thi, ví dụ `"ls -l"` hoặc `"grep hello"`           |
| `type`       | Kiểu luồng mở:`"r"` để đọc (output từ tiến trình), `"w"` để ghi (input) |
| Trả về       | Con trỏ `FILE*` để dùng như file stream                                          |
| Sử dụng với | `fgets()`, `fread()`, `fwrite()`, v.v.                                            |

🟢 Ví dụ: đọc kết quả từ lệnh `ls`:

```c
FILE *fp = popen("ls", "r");
```

---

### 📦 `size_t fread(void *ptr, size_t size, size_t count, FILE *stream);`

| Tham số   | Ý nghĩa                                                                 |
| ---------- | ------------------------------------------------------------------------- |
| `ptr`    | Bộ đệm (buffer) nơi dữ liệu được lưu sau khi đọc              |
| `size`   | Kích thước mỗi phần tử cần đọc (đơn vị byte)                  |
| `count`  | Số lượng phần tử cần đọc                                          |
| `stream` | Luồng đầu vào (`FILE*`), có thể từ file, stdin hoặc `popen()` |
| Trả về   | Tổng số phần tử thực sự đã đọc                                  |

🟢 Ví dụ:

```c
char buffer[128];
size_t n = fread(buffer, 1, sizeof(buffer), fp); // đọc raw data
```

---

### 📄 `char *fgets(char *str, int n, FILE *stream);`

| Tham số   | Ý nghĩa                                                                     |
| ---------- | ----------------------------------------------------------------------------- |
| `str`    | Con trỏ đến buffer để lưu chuỗi đã đọc                             |
| `n`      | Kích thước tối đa của chuỗi (`n-1` ký tự + dấu kết thúc `\0`) |
| `stream` | Luồng đầu vào (`FILE*`)                                                 |
| Trả về   | Con trỏ tới `str` nếu thành công, hoặc `NULL` nếu lỗi/EOF         |

🟢 Ví dụ:

```c
char line[256];
if (fgets(line, sizeof(line), fp)) {
    printf(">> %s", line);
}
```

---

### 🔗 Mối liên hệ giữa các hàm

- `popen()` cung cấp một `FILE*` giống như một file, giúp bạn đọc output của một tiến trình.
- `fgets()` và `fread()` là cách đọc dữ liệu từ `FILE*`:
  - `fgets()` dùng cho chuỗi (text).
  - `fread()` dùng cho dữ liệu nhị phân (binary).
- Có thể dùng `fwrite()` hoặc `fprintf()` nếu bạn dùng `popen()` ở chế độ `"w"` để ghi vào tiến trình.

---
