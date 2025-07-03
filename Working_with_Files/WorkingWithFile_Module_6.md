### **Module 6: Xử lý Lỗi và Hệ thống File Ảo `/proc` 🐞🔍**

#### **6.1. Xử lý Lỗi trong Lập trình File (Error Handling) ❌**

Khi làm việc với các System Calls hoặc hàm thư viện, lỗi là điều không thể tránh khỏi. Việc xử lý lỗi đúng cách là yếu tố then chốt để xây dựng các ứng dụng mạnh mẽ và ổn định.

* **Lý thuyết:**
  * Hầu hết các System Calls và hàm thư viện C/POSIX khi thất bại sẽ trả về một giá trị đặc biệt (thường là `-1` cho System Calls, `NULL` hoặc `EOF` cho `stdio`) và thiết lập một biến toàn cục có tên **`errno`** để chỉ ra lý do thất bại.
  * **`errno`** :
  * Là một biến toàn cục kiểu `int` (khai báo trong `<errno.h>`).
  * Nó được thiết lập bởi các hàm thư viện khi có lỗi.
  * **QUAN TRỌNG:** Giá trị của `errno` chỉ có ý nghĩa  **ngay sau khi một hàm báo lỗi** . Các lệnh gọi hàm tiếp theo (thậm chí là `printf` hoặc các hàm khác) có thể ghi đè lên `errno`, làm mất đi thông tin lỗi gốc. Vì vậy, luôn sao chép giá trị của `errno` vào một biến cục bộ ngay lập tức sau khi phát hiện lỗi.
  * **Các mã lỗi phổ biến (`<errno.h>`):**
    * `EPERM`: Operation not permitted (Không có quyền).
    * `ENOENT`: No such file or directory (File hoặc thư mục không tồn tại).
    * `EINTR`: Interrupted system call (System call bị gián đoạn bởi tín hiệu).
    * `EIO`: I/O Error (Lỗi I/O cấp thấp).
    * `EBUSY`: Device or resource busy (Thiết bị/tài nguyên bận).
    * `EEXIST`: File exists (File đã tồn tại khi cố gắng tạo mới độc quyền).
    * `EINVAL`: Invalid argument (Đối số không hợp lệ).
    * `EMFILE`: Too many open files (Quá nhiều File Descriptor đã mở cho tiến trình).
    * `ENODEV`: No such device (Không có thiết bị như vậy).
    * `EISDIR`: Is a directory (Là một thư mục khi mong đợi file).
    * `ENOTDIR`: Isn’t a directory (Không phải thư mục khi mong đợi).

#### **6.1.1. Hàm `strerror()` 💬**

* **Lý thuyết:** Hàm `strerror()` chuyển đổi một mã lỗi (`errno` value) thành một **chuỗi mô tả lỗi dễ đọc** bởi con người.
  * **Syntax:**
    **C++**

    ```
    #include <string.h> // For strerror
    char *strerror(int errnum);
    ```
  * **`errnum`** : Mã lỗi (thường là giá trị của `errno`).
  * **Giá trị trả về:** Con trỏ tới một chuỗi (lưu ý: chuỗi này thường nằm trong bộ nhớ tĩnh, không nên sửa đổi và có thể bị ghi đè bởi các lệnh gọi `strerror` tiếp theo).
* **Liên hệ Embedded Linux:** Rất hữu ích khi ghi log lỗi vào file hoặc gửi qua mạng, nơi cần thông báo lỗi rõ ràng và dễ hiểu.

#### **6.1.2. Hàm `perror()` 🗣️**

* **Lý thuyết:** Hàm `perror()` in ra một thông báo lỗi đầy đủ lên luồng lỗi chuẩn (`stderr`). Thông báo bao gồm một chuỗi tiền tố tùy chọn mà bạn cung cấp, tiếp theo là dấu hai chấm, một khoảng trắng và chuỗi mô tả lỗi tương ứng với giá trị hiện tại của `errno`.

  * **Syntax:**
    **C++**

    ```
    #include <stdio.h> // For perror
    void perror(const char *s);
    ```
  * **`s`** : Chuỗi tiền tố (thường là tên chương trình hoặc tên hàm gây lỗi). Nếu `NULL`, chỉ in ra thông báo lỗi từ `errno`.
* **Liên hệ Embedded Linux:** Là công cụ gỡ lỗi nhanh chóng, giúp bạn thấy ngay lỗi xảy ra ở đâu trong mã của mình khi chạy chương trình trên console.
* **Ví dụ (C++): Xử lý Lỗi với `errno`, `strerror()`, `perror()`**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <stdio.h>    // For perror
  #include <stdlib.h>   // For EXIT_FAILURE, EXIT_SUCCESS
  #include <fcntl.h>    // For open, O_RDONLY, O_EXCL, O_CREAT
  #include <unistd.h>   // For close, unlink
  #include <errno.h>    // For errno
  #include <string.h>   // For strerror

  int main() {
      const char *non_existent_file = "non_existent_file.txt";
      const char *directory_as_file = "/tmp";
      const char *existing_file_for_excl = "existing_exclusive_file.txt";
      int fd;
      int saved_errno;

      // --- Trường hợp 1: File không tồn tại ---
      std::cout << "INFO    : Attempting to open a non-existent file: " << non_existent_file << std::endl;
      fd = open(non_existent_file, O_RDONLY);
      if (fd == -1) {
          saved_errno = errno; // Lưu errno ngay lập tức
          std::cerr << "ERROR   : open failed for " << non_existent_file << "." << std::endl;
          std::cerr << "          Error code: " << saved_errno << std::endl;
          std::cerr << "          Error message (strerror): " << strerror(saved_errno) << std::endl;
          perror("          Error message (perror)"); // perror cũng đọc errno hiện tại
      } else {
          std::cout << "SUCCESS : Unexpectedly opened " << non_existent_file << std::endl;
          close(fd);
      }

      // --- Trường hợp 2: Cố gắng mở thư mục như file ---
      std::cout << "\nINFO    : Attempting to open a directory as a file: " << directory_as_file << std::endl;
      fd = open(directory_as_file, O_RDONLY);
      if (fd == -1) {
          saved_errno = errno; // Lưu errno ngay lập tức
          std::cerr << "ERROR   : open failed for " << directory_as_file << "." << std::endl;
          std::cerr << "          Error code: " << saved_errno << std::endl;
          std::cerr << "          Error message (strerror): " << strerror(saved_errno) << std::endl;
          perror("          Error message (perror)");
      } else {
          std::cout << "SUCCESS : Unexpectedly opened " << directory_as_file << std::endl;
          close(fd);
      }

      // --- Trường hợp 3: Sử dụng O_EXCL trên file đã tồn tại ---
      // Tạo file trước
      FILE *fp = fopen(existing_file_for_excl, "w");
      if (fp == NULL) {
          std::cerr << "ERROR   : Failed to create temporary file: " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      fclose(fp);
      std::cout << "\nINFO    : Created temporary file: " << existing_file_for_excl << std::endl;

      std::cout << "INFO    : Attempting to open " << existing_file_for_excl << " with O_CREAT | O_EXCL." << std::endl;
      fd = open(existing_file_for_excl, O_RDWR | O_CREAT | O_EXCL, 0644);
      if (fd == -1) {
          saved_errno = errno; // Lưu errno ngay lập tức
          std::cerr << "ERROR   : open failed for " << existing_file_for_excl << "." << std::endl;
          std::cerr << "          Error code: " << saved_errno << std::endl;
          std::cerr << "          Error message (strerror): " << strerror(saved_errno) << std::endl;
          perror("          Error message (perror)");
      } else {
          std::cout << "SUCCESS : Unexpectedly opened " << existing_file_for_excl << std::endl;
          close(fd);
      }

      // Dọn dẹp file tạm
      unlink(existing_file_for_excl);

      return EXIT_SUCCESS;
  }
  ```

---

#### **6.2. Hệ thống File Ảo `/proc` (The `/proc` File System) 🧠**

* **Lý thuyết:**

  * `/proc` là một **hệ thống file ảo (virtual filesystem)** được Kernel Linux tạo ra trong bộ nhớ. Nó không chứa các file vật lý trên đĩa cứng mà cung cấp một giao diện file để truy cập  **thông tin về trạng thái của Kernel và các tiến trình đang chạy** .
  * Nó là một cách rất chuẩn và nhất quán để ứng dụng của bạn có thể đọc thông tin hệ thống và thậm chí thay đổi một số tham số Kernel (nếu có quyền).
  * Các file trong `/proc` thường là  **chỉ đọc (read-only)** , nhưng một số có thể **ghi được (writable)** để thay đổi cấu hình Kernel.
* **Các thư mục và file quan trọng trong `/proc`:**

  * **`/proc/cpuinfo`** : Thông tin chi tiết về CPU (kiến trúc, tốc độ, bộ nhớ cache).
  * **`/proc/meminfo`** : Thông tin về việc sử dụng bộ nhớ RAM và swap.
  * **`/proc/version`** : Phiên bản Kernel Linux.
  * **`/proc/devices`** : Danh sách các thiết bị driver được cấu hình.
  * **`/proc/mounts`** : Danh sách các filesystem đang được mount.
  * **`/proc/sys/`** : Chứa các file để đọc/ghi các tham số Kernel (sysctl parameters).
  * Ví dụ: `/proc/sys/fs/file-max` (số lượng file descriptors tối đa mà hệ thống có thể mở). Bạn có thể đọc và ghi vào file này (cần quyền root) để thay đổi giới hạn.
  * **`/proc/<PID>/`** : Mỗi thư mục con có tên là một số (PID của tiến trình) chứa thông tin chi tiết về tiến trình đó.
  * **`/proc/<PID>/cmdline`** : Chuỗi lệnh dùng để khởi chạy tiến trình.
  * **`/proc/<PID>/environ`** : Các biến môi trường của tiến trình.
  * **`/proc/<PID>/exe`** : Một symbolic link trỏ đến file thực thi của tiến trình.
  * **`/proc/<PID>/cwd`** : Một symbolic link trỏ đến thư mục làm việc hiện tại của tiến trình.
  * **`/proc/<PID>/fd/`** : Một thư mục chứa các symbolic link đến tất cả các file descriptor đang mở của tiến trình (ví dụ: `0`, `1`, `2`, `3`, ...). Rất hữu ích để debug xem một tiến trình đang mở những file nào.
  * **`/proc/<PID>/stat`, `/proc/<PID>/status`** : Thông tin về trạng thái của tiến trình (chạy, ngủ, mức sử dụng CPU, bộ nhớ, v.v.).
* **Cách truy cập:** Bạn có thể dùng `cat` trong shell để xem nội dung, hoặc dùng các hàm `open()`, `read()`, `write()`, `close()` trong chương trình C/C++ như với file thông thường.
* **Liên hệ Embedded Linux:** `/proc` là công cụ **vô giá** trong môi trường nhúng.

  * **Giám sát Hệ thống:** Theo dõi tài nguyên (CPU, RAM, số FD) của thiết bị.
  * **Gỡ lỗi Tiến trình:** Kiểm tra trạng thái, môi trường, file đang mở của các daemon hoặc ứng dụng bị lỗi.
  * **Cấu hình thời gian thực (Runtime Configuration):** Thay đổi các tham số Kernel động (ví dụ: giới hạn bộ nhớ, network parameters) mà không cần khởi động lại.
  * **Phát hiện và Xử lý lỗi:** Ví dụ, kiểm tra `/proc/<PID>/fd` để xem tiến trình có đang rò rỉ file descriptor không.
* **Ví dụ (C++): Đọc thông tin từ `/proc`**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <fstream>  // For std::ifstream (C++ stream for file I/O)
  #include <vector>
  #include <sstream>  // For std::istringstream
  #include <unistd.h> // For getpid
  #include <sys/stat.h> // For stat, S_ISLNK
  #include <limits.h> // For PATH_MAX

  // Hàm đọc nội dung file từ /proc và in ra
  void read_and_print_proc_file(const std::string& path) {
      std::ifstream file(path);
      if (file.is_open()) {
          std::cout << "INFO    : --- Content of " << path << " ---" << std::endl;
          std::string line;
          while (std::getline(file, line)) {
              std::cout << line << std::endl;
          }
          std::cout << "INFO    : ----------------------------------" << std::endl;
          file.close();
          std::cout << "SUCCESS : Successfully read " << path << std::endl;
      } else {
          std::cerr << "ERROR   : Failed to open " << path << ": " << strerror(errno) << std::endl;
      }
  }

  int main() {
      // --- 1. Đọc thông tin hệ thống chung ---
      read_and_print_proc_file("/proc/cpuinfo");
      read_and_print_proc_file("/proc/meminfo");
      read_and_print_proc_file("/proc/version");
      read_and_print_proc_file("/proc/sys/fs/file-max"); // Giới hạn số file toàn hệ thống

      // --- 2. Đọc thông tin về tiến trình hiện tại ---
      pid_t my_pid = getpid();
      std::string proc_dir = "/proc/" + std::to_string(my_pid);

      std::cout << "\nINFO    : --- Information for current process (PID: " << my_pid << ") ---" << std::endl;
      read_and_print_proc_file(proc_dir + "/cmdline"); // Lệnh khởi chạy
      read_and_print_proc_file(proc_dir + "/environ"); // Biến môi trường (có thể chứa ký tự null)

      // Đọc symbolic link `exe` trỏ tới file thực thi
      char exe_path[PATH_MAX];
      ssize_t len = readlink((proc_dir + "/exe").c_str(), exe_path, sizeof(exe_path) - 1);
      if (len != -1) {
          exe_path[len] = '\0';
          std::cout << "INFO    : Executable path: " << exe_path << std::endl;
      } else {
          std::cerr << "ERROR   : Failed to read exe symlink: " << strerror(errno) << std::endl;
      }

      // Liệt kê các file descriptor đang mở
      std::string fd_dir = proc_dir + "/fd";
      std::cout << "INFO    : Listing open file descriptors in " << fd_dir << ":" << std::endl;
      // Mở thư mục /proc/<PID>/fd (dùng opendir/readdir từ Module 5)
      DIR *dp;
      struct dirent *entry;
      if ((dp = opendir(fd_dir.c_str())) == NULL) {
          std::cerr << "ERROR   : Cannot open FD directory: " << strerror(errno) << std::endl;
      } else {
          while ((entry = readdir(dp)) != NULL) {
              if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) continue;
              // Mỗi entry là một symlink trỏ tới file thực sự mà FD đang mở
              std::string fd_symlink_path = fd_dir + "/" + entry->d_name;
              char target_path[PATH_MAX];
              ssize_t link_len = readlink(fd_symlink_path.c_str(), target_path, sizeof(target_path) - 1);
              if (link_len != -1) {
                  target_path[link_len] = '\0';
                  std::cout << "INFO    :   FD " << entry->d_name << " -> " << target_path << std::endl;
              } else {
                  std::cerr << "WARNING :   FD " << entry->d_name << " (error: " << strerror(errno) << ")" << std::endl;
              }
          }
          closedir(dp);
      }
      return EXIT_SUCCESS;
  }
  ```

---

### **Câu hỏi Tự đánh giá Module 6 🤔**

1. Tại sao việc kiểm tra giá trị của `errno` ngay lập tức sau khi một hàm báo lỗi lại quan trọng? Nêu một ví dụ về tình huống có thể xảy ra nếu bạn không làm như vậy.
2. Phân biệt giữa `strerror()` và `perror()`. Khi nào bạn sẽ dùng `strerror()` thay vì `perror()`?
3. Giải thích triết lý đằng sau `/proc` filesystem. Nó khác với các filesystem "thông thường" như `/home` hay `/usr` như thế nào?
4. Bạn muốn biết phiên bản Kernel Linux đang chạy và mức sử dụng bộ nhớ RAM hiện tại. Bạn sẽ đọc những file nào trong `/proc` để lấy thông tin này?
5. Giải thích cách bạn có thể tìm ra tất cả các file mà một tiến trình cụ thể (ví dụ: với PID là 1234) đang mở bằng cách sử dụng `/proc` filesystem.

---

### **Bài tập Thực hành Module 6 ✍️**

1. **Chương trình Thử lỗi và Báo cáo:**
   * Viết một chương trình C++ (`error_reporter.cpp`) có một hàm `attempt_open(const char* filename, int flags, mode_t mode)` mà:
     * Cố gắng mở file `filename` với `flags` và `mode` được cung cấp.
     * Nếu `open()` thành công, in ra "Successfully opened [filename] with FD: [fd_number]". Sau đó đóng file.
     * Nếu `open()` thất bại, in ra "Failed to open [filename]: [mã lỗi errno] - [thông báo lỗi từ strerror]". Sau đó gọi `perror()` với tiền tố là tên file.
   * Trong hàm `main()`, gọi `attempt_open` với các trường hợp sau:
     * Mở một file không tồn tại (chỉ đọc).
     * Mở một thư mục như thể nó là một file (chỉ đọc).
     * Tạo một file mới với `O_CREAT | O_EXCL` khi file đó đã tồn tại.
     * Thử các trường hợp lỗi khác mà bạn nghĩ ra.
2. **Chương trình Giám sát Tài nguyên Tiến trình:**
   * Viết một chương trình C++ (`proc_monitor.cpp`) nhận một tham số dòng lệnh là PID của một tiến trình.
   * Chương trình sẽ định kỳ (ví dụ: mỗi 2 giây) đọc và in ra:
     * Tên lệnh (từ `cmdline`).
     * Trạng thái tiến trình (Running, Sleeping, v.v. - từ `stat` hoặc `status`).
     * Mức sử dụng bộ nhớ ảo (VSZ) và vật lý (RSS) (từ `statm` hoặc `status`).
     * Số lượng File Descriptor đang mở (bằng cách đếm các mục trong `/proc/<PID>/fd`).
   * Sử dụng `std::ifstream` để đọc các file trong `/proc`.
   * Chương trình sẽ dừng khi bạn nhấn `Ctrl+C` hoặc khi tiến trình được giám sát không còn tồn tại.
   * **Thử thách:**
     * Sử dụng `sigaction()` để bắt `SIGINT` và thoát chương trình một cách duyên dáng.
     * Nếu tiến trình được giám sát không tồn tại, báo lỗi và thoát.
3. **Chương trình Thay đổi Giới hạn File Hệ thống (Nâng cao - Cần Root):**
   * Viết một chương trình C++ (`set_file_limit.cpp`) nhận một tham số dòng lệnh là số nguyên, đại diện cho giới hạn file tối đa mới toàn hệ thống.
   * Chương trình sẽ:
     * Đọc giá trị hiện tại của `/proc/sys/fs/file-max` và in ra.
     * Thử ghi giá trị mới vào `/proc/sys/fs/file-max`.
     * Đọc lại `/proc/sys/fs/file-max` và in ra để xác nhận.
   * **Lưu ý:** Chương trình này sẽ cần **quyền root** để thay đổi giá trị. Bạn có thể cần biên dịch và chạy nó với `sudo`. Xử lý lỗi `EACCES` nếu không có quyền.
   * **Cảnh báo:** Thận trọng khi thay đổi các giá trị trong `/proc/sys/`, việc thiết lập các giá trị không hợp lý có thể ảnh hưởng đến sự ổn định của hệ thống.

---

Hãy dành thời gian để thực hành các bài tập này. Chúng sẽ cung cấp cho bạn kinh nghiệm thực tế quý giá trong việc gỡ lỗi và quản lý hệ thống Linux ở cấp độ sâu. Khi bạn đã sẵn sàng, hãy cho tôi biết để chuyển sang Module 7!
