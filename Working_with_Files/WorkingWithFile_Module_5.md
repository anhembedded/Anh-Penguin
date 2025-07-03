### **Module 5: Quét Thư mục (Directory Scanning) 📂🔍**

Trong Module này, bạn sẽ học cách chương trình C/C++ của mình có thể "đọc" nội dung của một thư mục, liệt kê các file và thư mục con bên trong nó.

#### **5.1. Khái niệm và Mục đích 🗺️**

* **Lý thuyết:** Quét thư mục là quá trình duyệt qua các mục nhập (entries) trong một thư mục để xác định những file và thư mục con nào đang tồn tại trong đó.
* **Tại sao không dùng `open()` và `read()` thông thường?** Mặc dù thư mục về bản chất cũng là một loại file, nhưng cấu trúc nội bộ của chúng trên đĩa có thể khác nhau tùy theo loại hệ thống file (ví dụ: ext4, XFS, FAT32). Việc đọc trực tiếp nội dung thư mục bằng `read()` là **không di động (non-portable)** và không được khuyến nghị.
* **Giải pháp:** POSIX cung cấp một bộ hàm thư viện chuẩn (trong `<dirent.h>`) được thiết kế đặc biệt để quét thư mục một cách di động và an toàn.
* **Liên hệ Embedded Linux:** Cực kỳ hữu ích khi bạn cần:
  * Liệt kê các file log trong một thư mục.
  * Tìm kiếm các file cấu hình.
  * Duyệt qua các thiết bị trong `/dev` hoặc các node trong `/sys`.
  * Xây dựng các công cụ quản lý file đơn giản trên thiết bị.

#### **5.2. Các Hàm Quét Thư mục Chính (Core Functions) 🔑**

Bạn sẽ làm việc với cấu trúc `DIR *` (gọi là "directory stream") để đại diện cho một thư mục đang mở, tương tự như `FILE *` cho các file thông thường. Các mục nhập thư mục được trả về dưới dạng `struct dirent`.

* **`opendir()`: Mở Thư mục 🚪**
  * **Mục đích:** Mở một thư mục và thiết lập một "directory stream" để bạn có thể đọc các mục nhập của nó.
  * **Syntax:**
    **C++**

    ```cpp
    #include <sys/types.h> // For DIR type
    #include <dirent.h>    // For opendir, DIR structure
    DIR *opendir(const char *name);
    ```
  * **`name`** : Đường dẫn đến thư mục cần mở.
  * **Giá trị trả về:** Con trỏ `DIR *` nếu thành công, `NULL` nếu thất bại (ví dụ: thư mục không tồn tại, không có quyền).
  * **Lưu ý:** Giống như `open()`, `opendir()` có thể thất bại nếu số lượng file descriptors mở vượt quá giới hạn.
* **`readdir()`: Đọc Mục nhập Thư mục 📖**
  * **Mục đích:** Đọc mục nhập tiếp theo từ một directory stream.
  * **Syntax:**
    **C++**

    ```cpp
    #include <sys/types.h> // For struct dirent type
    #include <dirent.h>    // For readdir, struct dirent
    struct dirent *readdir(DIR *dirp);
    ```
  * **`dirp`** : Con trỏ `DIR *` được trả về bởi `opendir()`.
  * **Giá trị trả về:**

    * Con trỏ tới một `struct dirent` nếu đọc thành công. Cấu trúc này chứa thông tin về mục nhập thư mục (inode number, tên file).
    * `NULL` khi đạt đến cuối thư mục hoặc khi có lỗi. Bạn phải dùng `errno` để phân biệt EOF và lỗi.
  * **`struct dirent`:** Cấu trúc này chứa ít nhất hai thành viên quan trọng:

    * `ino_t d_ino;`: Inode number của file/thư mục.
    * `char d_name[];`: Tên của file hoặc thư mục (là một chuỗi kết thúc bằng null).
  * **Lưu ý:** `readdir()` không đảm bảo liệt kê tất cả các file trong thư mục nếu có các tiến trình khác đang tạo hoặc xóa file đồng thời. Ngoài ra, thông tin trong `struct dirent` là hạn chế (chỉ có tên và inode). Để biết thêm chi tiết (kích thước, quyền, thời gian), bạn cần sử dụng `stat()` hoặc `lstat()` với `d_name`.
* **`closedir()`: Đóng Thư mục 🚪**
  * **Mục đích:** Đóng một directory stream và giải phóng các tài nguyên liên quan.
  * **Syntax:**
    **C++**

    ```cpp
    #include <sys/types.h>
    #include <dirent.h>
    int closedir(DIR *dirp);
    ```
  * **`dirp`** : Con trỏ `DIR *` cần đóng.
  * **Giá trị trả về:** `0` nếu thành công, `-1` nếu có lỗi.
  * **Quan trọng:** Luôn đóng directory stream sau khi sử dụng để tránh rò rỉ tài nguyên.

#### **5.3. Điều hướng trong Directory Stream 🧭**

Các hàm này ít được sử dụng hơn nhưng hữu ích cho các trường hợp cụ thể:

* **`telldir()`: Lấy vị trí hiện tại 📍**
  * **Mục đích:** Trả về một giá trị `long int` ghi lại vị trí hiện tại của con trỏ trong directory stream.
  * **Syntax:** `long int telldir(DIR *dirp);`
  * **Hữu ích:** Dùng để lưu lại vị trí trong quá trình quét và sau đó quay lại vị trí đó bằng `seekdir()`.
* **`seekdir()`: Đặt vị trí 🎯**
  * **Mục đích:** Đặt con trỏ trong directory stream về một vị trí đã lưu trước đó bằng `telldir()`.
  * **Syntax:** `void seekdir(DIR *dirp, long int loc);`
  * **`loc`** : Giá trị được trả về bởi một lần gọi `telldir()` trước đó.

#### **5.4. Ví dụ `printdir.cpp`: Chương trình Quét Thư mục Đơn giản 📜**

Ví dụ này minh họa cách kết hợp các hàm `opendir()`, `readdir()`, `closedir()`, `lstat()`, và `chdir()` để tạo một chương trình quét thư mục đệ quy (recursive directory scanner).

* Nó in ra các file và thư mục con với thụt lề để thể hiện cấu trúc cây.
* **Điểm quan trọng:**
  * Kiểm tra `.` và `..` để tránh vòng lặp vô hạn trong đệ quy.
  * Sử dụng `chdir(dir)` để di chuyển vào thư mục con trước khi đệ quy, và `chdir("..")` để quay lại sau khi xử lý xong thư mục con đó. Điều này giúp các tên file/thư mục con luôn là "usable names".
  * Dùng `lstat()` để lấy thông tin về mục nhập thư mục (kể cả symbolic link) và `S_ISDIR()` để kiểm tra xem đó có phải thư mục hay không.

**C++**

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>  // For chdir
#include <stdio.h>   // For fprintf, perror
#include <stdlib.h>  // For EXIT_FAILURE, EXIT_SUCCESS
#include <dirent.h>  // For opendir, readdir, closedir, DIR, dirent
#include <string.h>  // For strcmp, strerror
#include <sys/stat.h> // For lstat, statbuf, S_ISDIR
#include <errno.h>   // For errno

// Simple logger for demonstration, mimicking loguru style
// In a real project, you'd use a proper logging library or your custom one.
namespace AppLogger {
    enum LogLevel { TRACE_L, DEBUG_L, INFO_L, SUCCESS_L, WARNING_L, ERROR_L, CRITICAL_L };

    static const std::map<LogLevel, std::string> LogLevelNames = {
        {TRACE_L,    "TRACE   "},
        {DEBUG_L,    "DEBUG   "},
        {INFO_L,     "INFO    "},
        {SUCCESS_L,  "SUCCESS "},
        {WARNING_L,  "WARNING "},
        {ERROR_L,    "ERROR   "},
        {CRITICAL_L, "CRITICAL"}
    };

    void log(LogLevel level, const std::string& message) {
        std::cout << LogLevelNames.at(level) << ": " << message << std::endl;
    }
}

// Hàm đệ quy để in cấu trúc thư mục
void printdir(const char *dir, int depth) {
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    // Mở thư mục
    AppLogger::log(AppLogger::DEBUG_L, std::string(depth, ' ') + "Opening directory: " + dir);
    if ((dp = opendir(dir)) == NULL) {
        AppLogger::log(AppLogger::ERROR_L, std::string(depth, ' ') + "Cannot open directory: " + dir + " - " + strerror(errno));
        return;
    }

    // Thay đổi thư mục làm việc hiện tại vào thư mục này
    // Điều này giúp chúng ta dùng tên file/thư mục con trực tiếp
    AppLogger::log(AppLogger::TRACE_L, std::string(depth, ' ') + "Changing directory to: " + dir);
    if (chdir(dir) == -1) {
        AppLogger::log(AppLogger::ERROR_L, std::string(depth, ' ') + "Failed to change directory to " + dir + ": " + strerror(errno));
        closedir(dp);
        return;
    }

    // Đọc từng mục nhập trong thư mục
    while ((entry = readdir(dp)) != NULL) {
        // Lấy thông tin stat của mục nhập
        // Dùng lstat để không theo dõi symbolic links
        if (lstat(entry->d_name, &statbuf) == -1) {
            AppLogger::log(AppLogger::WARNING_L, std::string(depth + 4, ' ') + "Failed to get stat for " + entry->d_name + ": " + strerror(errno));
            continue; // Bỏ qua mục này và thử mục tiếp theo
        }

        // Kiểm tra nếu là thư mục
        if (S_ISDIR(statbuf.st_mode)) {
            // Bỏ qua "." (thư mục hiện tại) và ".." (thư mục cha) để tránh vòng lặp vô hạn
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) {
                continue;
            }
            // In tên thư mục con với thụt lề và dấu "/"
            std::cout << std::string(depth, ' ') << entry->d_name << "/" << std::endl;
            // Đệ quy vào thư mục con với độ sâu tăng thêm
            printdir(entry->d_name, depth + 4);
        } else {
            // Nếu không phải thư mục, in tên file với thụt lề
            std::cout << std::string(depth, ' ') << entry->d_name << std::endl;
        }
    }

    // Sau khi quét xong thư mục hiện tại, quay lại thư mục cha
    AppLogger::log(AppLogger::TRACE_L, std::string(depth, ' ') + "Changing directory back to '..'");
    if (chdir("..") == -1) {
        AppLogger::log(AppLogger::ERROR_L, std::string(depth, ' ') + "Failed to change directory back to '..': " + strerror(errno));
        // Có thể cần xử lý lỗi nghiêm trọng hơn nếu không thể quay lại thư mục cha
    }

    // Đóng directory stream
    if (closedir(dp) == -1) {
        AppLogger::log(AppLogger::ERROR_L, std::string(depth, ' ') + "Failed to close directory: " + strerror(errno));
    }
}

int main(int argc, char* argv[]) {
    const char* topdir = "."; // Mặc định quét thư mục hiện tại

    if (argc >= 2) {
        topdir = argv[1]; // Nếu có đối số, dùng nó làm thư mục bắt đầu
    }

    std::cout << "INFO    : Starting directory scan of " << topdir << ":" << std::endl;
    printdir(topdir, 0); // Bắt đầu quét từ độ sâu 0
    std::cout << "INFO    : Directory scan done." << std::endl;

    return EXIT_SUCCESS;
}
```

* **Cách biên dịch:**
  **Bash**

  ```
  g++ -o printdir printdir.cpp -lstdc++ # Sử dụng g++ vì có std::cout/std::string
  ```
* **Cách chạy:**
  **Bash**

  ```
  ./printdir /home/anhlinux # Quét thư mục home của bạn
  ./printdir . # Quét thư mục hiện tại
  ```
* **Phân tích output:** Bạn sẽ thấy output hiển thị cấu trúc cây của thư mục. Nếu output quá dài, bạn có thể pipe nó qua `more` hoặc `less`: `./printdir /usr | less`.

---

### **Câu hỏi Tự đánh giá Module 5 🤔**

1. Tại sao việc sử dụng `open()` và `read()` thông thường để quét thư mục lại không được khuyến nghị, thay vào đó nên dùng các hàm từ `<dirent.h>`?
2. Phân biệt ý nghĩa của các giá trị trả về `NULL` từ `opendir()` và `readdir()`. Làm thế nào để bạn xác định đó là lỗi hay cuối thư mục?
3. Khi nào bạn sẽ sử dụng `lstat()` thay vì `stat()` khi đang quét thư mục và kiểm tra từng mục nhập?
4. Giải thích vai trò của `chdir(dir)` và `chdir("..")` trong hàm `printdir()` đệ quy. Nếu thiếu một trong hai, chương trình có thể gặp vấn đề gì?
5. Nêu một hạn chế của `readdir()` khi có các tiến trình khác cùng thao tác trên thư mục đó.

---

### **Bài tập Thực hành Module 5 ✍️**

1. **Chương trình Tìm File theo Tên:**
   * Viết một chương trình C++ (`find_file.cpp`) nhận hai tham số dòng lệnh: `<starting_directory>` và `<filename_to_find>`.
   * Chương trình sẽ quét đệ quy từ `<starting_directory>` và in ra đường dẫn đầy đủ của bất kỳ file hoặc thư mục nào có tên khớp với `<filename_to_find>`.
   * Sử dụng `opendir()`, `readdir()`, `closedir()`, `chdir()`, `getcwd()` và `strcmp()`.
   * Xử lý lỗi cơ bản.
   * **Thử thách:** Thêm tùy chọn `-type f` (chỉ tìm file) và `-type d` (chỉ tìm thư mục).
2. **Chương trình Liệt kê File Lớn:**
   * Viết một chương trình C++ (`list_large_files.cpp`) nhận hai tham số dòng lệnh: `<starting_directory>` và `<min_size_kb>`.
   * Chương trình sẽ quét đệ quy từ `<starting_directory>` và in ra đường dẫn đầy đủ, tên, và kích thước (bytes) của tất cả các file thông thường có kích thước lớn hơn hoặc bằng `<min_size_kb>`.
   * Sử dụng `stat()` để lấy kích thước file.
   * **Thử thách:** Sắp xếp các file tìm thấy theo kích thước giảm dần trước khi in ra.
3. **Chương trình Xóa Thư mục không rỗng (Nâng cao):**
   * Dựa trên kiến thức Module 4 và 5, viết một hàm đệ quy `remove_directory_recursive(const char *path)` trong C++ để xóa một thư mục và tất cả nội dung của nó (bao gồm file và thư mục con không rỗng).
   * Hàm này sẽ:
     * Mở thư mục.
     * Đọc từng mục nhập.
     * Nếu là file: dùng `unlink()`.
     * Nếu là thư mục con (không phải `.` hay `..`): gọi đệ quy chính nó.
     * Sau khi tất cả nội dung đã được xóa: dùng `rmdir()`.
     * Sử dụng `chdir()` và `getcwd()` nếu cần để quản lý đường dẫn.
   * Tạo một `main()` đơn giản để kiểm tra hàm này trên một thư mục mẫu chứa file và thư mục con.

---

Hãy dành thời gian để hiểu rõ các hàm và thực hành các bài tập này. Kỹ năng quét thư mục là vô cùng cần thiết cho các ứng dụng thực tế. Khi bạn đã sẵn sàng, hãy cho tôi biết để chuyển sang Module 6!
