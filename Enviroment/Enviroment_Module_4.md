


### **Module 4: File Tạm thời (Temporary Files) ⏳**

Các chương trình thường cần sử dụng các file tạm thời để lưu trữ dữ liệu trong thời gian ngắn. Điều này có thể là kết quả trung gian của một phép tính, các bản sao lưu trước khi thực hiện các thao tác quan trọng, hoặc buffer dữ liệu lớn.

#### **4.1. Tại sao cần File Tạm thời? 💾**

* **Lý thuyết:**
  * **Lưu trữ trung gian:** Dùng để lưu các kết quả tạm thời mà không cần giữ trong RAM.
  * **Bộ đệm lớn:** Khi dữ liệu quá lớn để nằm hoàn toàn trong bộ nhớ.
  * **Backup:** Tạo bản sao tạm thời của file trước khi sửa đổi.
  * **IPC (không chính thức):** Đôi khi dùng để trao đổi dữ liệu giữa các tiến trình (mặc dù có các cơ chế IPC tốt hơn).
* **Vị trí:** Các file tạm thời thường được tạo trong thư mục `/tmp` hoặc `/var/tmp`.
  * `/tmp`: Nội dung thường bị xóa khi hệ thống khởi động lại (reboot).
  * `/var/tmp`: Nội dung có thể tồn tại qua các lần khởi động lại.
* **Thách thức quan trọng: Đảm bảo tên file duy nhất (Unique Filenames) ⚠️:**
  * Trong môi trường đa nhiệm như Linux, nhiều chương trình có thể chạy cùng lúc. Nếu không cẩn thận, hai chương trình có thể chọn cùng một tên file tạm thời, dẫn đến việc ghi đè dữ liệu của nhau hoặc tạo ra lỗ hổng bảo mật.
  * Do đó, việc tạo các tên file tạm thời **duy nhất và an toàn** là tối quan trọng.

#### **4.2. Các Hàm Tạo File Tạm thời An toàn trong C/C++ 🔑**

Thư viện chuẩn C và POSIX cung cấp các hàm để tạo file tạm thời một cách an toàn.

* **`tmpnam()` (Không khuyến nghị dùng!) 🚫:**
  * `char *tmpnam(char *s);`
  * Tạo một **tên file duy nhất** không trùng với bất kỳ file hiện có nào.
  * **Hạn chế:** Chỉ trả về  **tên file** , không mở file. Có  **điều kiện tranh chấp (race condition)** . Một chương trình khác có thể tạo file với tên đó giữa lúc `tmpnam()` trả về và bạn gọi `open()`.
  * **Khuyến nghị:** **TUYỆT ĐỐI TRÁNH SỬ DỤNG `tmpnam()` trong các chương trình mới.**
* **`tmpfile()` (An toàn và Đơn giản) ✅:**
  * `FILE *tmpfile(void);`
  * Tạo và mở một file tạm thời **duy nhất** (sử dụng chế độ `w+b` hoặc `w+`).
  * Trả về một con trỏ `FILE *` (stream) tới file tạm thời đó.
  * **Điểm mạnh:** File này sẽ **tự động bị xóa** khi bạn gọi `fclose()` cho stream đó hoặc khi chương trình kết thúc. Điều này giải quyết vấn đề race condition của `tmpnam()` và đảm bảo dọn dẹp tài nguyên.
  * **Hạn chế:** Không cung cấp quyền truy cập file descriptor cấp thấp.
* **`mkstemp()` (An toàn và Linh hoạt) ✨:**
  * `int mkstemp(char *template);`
  * Tạo và mở một file tạm thời  **duy nhất** , và trả về một  **file descriptor cấp thấp (`int`)** .
  * **Cách dùng:**
    1. Bạn cung cấp một **chuỗi mẫu (template string)** có dạng `"/path/to/my_temp_XXXXXX"`, trong đó `XXXXXX` là 6 ký tự cuối cùng.
    2. `mkstemp()` sẽ thay thế `XXXXXX` bằng một chuỗi ký tự ngẫu nhiên duy nhất để tạo tên file.
    3. Nó sẽ tạo và mở file đó, trả về File Descriptor.
  * **Điểm mạnh:**
    * An toàn (không có race condition).
    * Trả về File Descriptor, cho phép bạn sử dụng các System Calls cấp thấp (`read()`, `write()`, `lseek()`).
    * Bạn có thể kiểm soát thư mục chứa file tạm thời (trong template).
  * **Tạo "Transient File":** Ngay sau khi gọi `mkstemp()` thành công, bạn có thể gọi `unlink(template)` để xóa tên file khỏi thư mục. File đó sẽ vẫn tồn tại và có thể được truy cập thông qua File Descriptor `fd` của bạn cho đến khi `fd` đó được đóng. Điều này đảm bảo file sẽ tự động bị xóa ngay cả khi chương trình của bạn crash.
* **`mktemp()` (Không khuyến nghị dùng!) 🚫:**
  * `char *mktemp(char *template);`
  * Tương tự `tmpnam()`, nó chỉ trả về  **tên file** , không mở file. Cũng có **race condition** tương tự `tmpnam()`. **TUYỆT ĐỐI TRÁNH DÙNG!**
* **Liên hệ Embedded Linux:**
  * Trong các hệ thống nhúng, việc quản lý không gian lưu trữ và file tạm thời là cực kỳ quan trọng. `tmpfile()` và `mkstemp()` giúp bạn tạo các file tạm thời một cách an toàn, hiệu quả, và đảm bảo chúng được dọn dẹp tự động, tránh lãng phí tài nguyên.
  * Việc tạo "Transient File" bằng `mkstemp()` + `unlink()` rất hữu ích khi bạn cần một vùng bộ nhớ tạm thời có thể ghi ra đĩa nhưng không muốn nó tồn tại lâu dài trên filesystem, ngay cả khi ứng dụng gặp sự cố.

#### **4.3. Ví dụ (C++): `temp_files.cpp` - Sử dụng `tmpnam`, `tmpfile`, `mkstemp`**

**C++**

```cpp
#include <iostream>
#include <string>
#include <cstdio>   // For tmpnam, tmpfile, L_tmpnam
#include <cstdlib>  // For EXIT_SUCCESS, EXIT_FAILURE
#include <cstring>  // For strlen, strcpy
#include <unistd.h> // For close, unlink, sleep
#include <fcntl.h>  // For open, O_RDWR, O_CREAT, O_EXCL
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
    char tmp_name_buffer[L_tmpnam]; // Buffer cho tên file tạm của tmpnam
    char *filename;
    FILE *tmp_fp;
    int tmp_fd;
    const char *mkstemp_template = "/tmp/my_mkstemp_XXXXXX"; // Template cho mkstemp

    // --- Ví dụ 1: Sử dụng tmpnam() (KHÔNG NÊN DÙNG TRONG THỰC TẾ) ---
    AppLogger::log(AppLogger::WARNING_L, "--- Demonstrating tmpnam() (Avoid in production code!) ---");
    // Lấy tên file tạm
    filename = tmpnam(tmp_name_buffer);
    if (filename == nullptr) {
        AppLogger::log(AppLogger::ERROR_L, "tmpnam failed: " + std::string(strerror(errno)));
    } else {
        AppLogger::log(AppLogger::INFO_L, "Temporary file name (from tmpnam): " + std::string(filename));
        // Để chứng minh race condition, chúng ta không mở file ngay lập tức ở đây.
        // Một chương trình khác có thể tạo file này giữa lúc này.
        // Nếu không unlink, nó sẽ để lại file rác.
        // unlink(filename); // Unlink nếu không muốn để lại file rác
    }

    // --- Ví dụ 2: Sử dụng tmpfile() (An toàn và Dễ dùng) ---
    AppLogger::log(AppLogger::INFO_L, "\n--- Demonstrating tmpfile() ---");
    // Tạo và mở file tạm thời. Nó sẽ tự động bị xóa khi đóng hoặc khi chương trình thoát.
    tmp_fp = tmpfile();
    if (tmp_fp == nullptr) {
        AppLogger::log(AppLogger::ERROR_L, "tmpfile failed: " + std::string(strerror(errno)));
    } else {
        AppLogger::log(AppLogger::SUCCESS_L, "Opened a temporary file using tmpfile() OK.");
        fprintf(tmp_fp, "Data written to tmpfile.\n");
        AppLogger::log(AppLogger::INFO_L, "Wrote data to tmpfile. It will be deleted automatically.");
        // Đóng file. File sẽ bị xóa.
        fclose(tmp_fp); 
        AppLogger::log(AppLogger::SUCCESS_L, "tmpfile closed.");
    }

    // --- Ví dụ 3: Sử dụng mkstemp() (An toàn và Linh hoạt) ---
    AppLogger::log(AppLogger::INFO_L, "\n--- Demonstrating mkstemp() ---");
    // Tạo template (lưu ý: mkstemp sẽ sửa đổi chuỗi template này)
    char template_buffer[256];
    strcpy(template_buffer, mkstemp_template); 

    // Tạo và mở file tạm thời, trả về FD
    tmp_fd = mkstemp(template_buffer);
    if (tmp_fd == -1) {
        AppLogger::log(AppLogger::ERROR_L, "mkstemp failed: " + std::string(strerror(errno)));
    } else {
        AppLogger::log(AppLogger::SUCCESS_L, "Created temporary file (from mkstemp): " + std::string(template_buffer) + " with FD " + std::to_string(tmp_fd));
      
        // Tạo "Transient File": xóa tên file khỏi thư mục ngay lập tức
        if (unlink(template_buffer) == -1) {
            AppLogger::log(AppLogger::WARNING_L, "Failed to unlink mkstemp file: " + std::string(strerror(errno)));
        } else {
            AppLogger::log(AppLogger::INFO_L, "File '" + std::string(template_buffer) + "' unlinked from directory. It remains accessible via FD " + std::to_string(tmp_fd) + " and will be deleted on close.");
        }

        // Ghi dữ liệu vào file thông qua FD
        const char *data = "Hello from mkstemp transient file!\n";
        write(tmp_fd, data, strlen(data));
        AppLogger::log(AppLogger::INFO_L, "Wrote data to mkstemp file via its FD.");
        sleep(1); // Để bạn có thể kiểm tra nhanh bằng `ls` nếu muốn

        // Đóng file. File sẽ bị xóa hoàn toàn.
        close(tmp_fd);
        AppLogger::log(AppLogger::SUCCESS_L, "mkstemp file closed. It should now be fully deleted.");
    }

    return EXIT_SUCCESS;
}
```

---

### **Câu hỏi Tự đánh giá Module 4 🤔**

1. Tại sao việc tạo tên file tạm thời một cách duy nhất và an toàn lại quan trọng trong môi trường đa nhiệm như Linux? Nêu một rủi ro nếu không làm vậy.
2. Phân biệt giữa `tmpnam()` và `tmpfile()`. Hàm nào được khuyến nghị sử dụng và tại sao?
3. Giải thích cơ chế tạo "Transient File" bằng cách kết hợp `mkstemp()` và `unlink()`. Lợi ích của phương pháp này là gì so với việc chỉ `mkstemp()` rồi `close()` khi kết thúc chương trình?
4. Khi sử dụng `mkstemp()`, chuỗi template phải tuân theo quy tắc nào?

---

### **Bài tập Thực hành Module 4 ✍️**

1. **Chương trình Tạo và Ghi log vào File Tạm thời:**
   * Viết một chương trình C++ (`log_to_temp.cpp`) mà:
     * Tạo một file tạm thời an toàn bằng `mkstemp()`.
     * Lấy tên file tạm thời và in ra console.
     * Ghi 5 dòng log vào file tạm thời đó, mỗi dòng có nội dung "Log entry [số thứ tự] at [thời gian hiện tại]". Sử dụng `write()` System Call và các hàm thời gian từ Module 3.
     * Đóng file tạm thời.
     * **Thử thách:** Sau khi đóng file, thử mở lại file tạm thời đó (sử dụng tên bạn đã lấy được) ở chế độ đọc và in ra nội dung để xác nhận. Sau đó, xóa file.
2. **Chương trình Xử lý Dữ liệu tạm thời lớn:**
   * Viết một chương trình C++ (`large_temp_data.cpp`) mà:
     * Tạo một file tạm thời an toàn bằng `tmpfile()`.
     * Ghi 1MB dữ liệu ngẫu nhiên (ví dụ: các ký tự 'A' đến 'Z' lặp lại) vào file tạm thời đó.
     * Di chuyển con trỏ file về đầu file bằng `fseek()`.
     * Đọc 512KB dữ liệu từ file tạm thời đó vào một buffer.
     * In ra một phần nhỏ của buffer (ví dụ: 100 ký tự đầu tiên).
     * Chương trình sẽ tự động dọn dẹp file tạm thời khi kết thúc.
   * **Thử thách:** Thực hiện các thao tác đọc/ghi bằng `fread()`/`fwrite()` để tận dụng buffering của `stdio`.
3. **Chương trình Kiểm tra Race Condition của `tmpnam()` (Cảnh báo: Chỉ để học tập, không dùng trong thực tế!):**
   * Viết một chương trình C++ (`tmpnam_race.cpp`) mà:
     * Gọi `tmpnam()` để lấy một tên file.
     * **Tạm dừng** trong vài giây (ví dụ: `sleep(5)`).
     * Trong lúc chương trình tạm dừng, hãy mở một terminal khác và cố gắng tạo một file có cùng tên mà `tmpnam()` đã tạo (bạn sẽ cần biết tên đó).
     * Sau khi `sleep` kết thúc, chương trình của bạn sẽ cố gắng mở file đó bằng `open(filename, O_CREAT | O_EXCL, 0644)`.
     * Quan sát xem `open()` có thất bại với lỗi `EEXIST` hay không, chứng tỏ race condition đã xảy ra.
   * **Mục đích:** Chỉ để minh họa rủi ro của `tmpnam()`. Không bao giờ thực hiện trong mã sản xuất!

---
