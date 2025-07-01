
-----

### **Module 2: Tương tác File Cấp thấp (System Calls) ⚙️**

Trong Module này, chúng ta sẽ đi sâu vào các **System Calls** – những hàm cấp thấp nhất để tương tác với file và thiết bị trong Linux.

#### **2.1. System Calls (Cuộc gọi Hệ thống): Giao diện với Kernel 📞**

  * **Lý thuyết:**
      * **System Call** là cách duy nhất để một chương trình user-space (như chương trình C của bạn) yêu cầu các dịch vụ trực tiếp từ **Kernel hệ điều hành**. Đây là "cầu nối" giữa ứng dụng của bạn và phần cứng/tài nguyên hệ thống.
      * Khi một System Call được gọi, CPU sẽ chuyển từ chế độ người dùng (user mode) sang chế độ Kernel (kernel mode). Quá trình này tốn một lượng nhỏ thời gian (overhead).
      * Các System Call được thiết kế để cung cấp giao diện nhất quán cho nhiều loại tài nguyên khác nhau (file, thiết bị, tiến trình).
  * **Overhead và Hiệu quả:**
      * Việc chuyển đổi ngữ cảnh (context switch) giữa user mode và kernel mode có một chi phí hiệu suất nhất định (performance penalty).
      * Vì vậy, khi làm việc với System Calls, một thực hành tốt là **giảm thiểu số lượng cuộc gọi System Call** và cố gắng làm cho **mỗi cuộc gọi thực hiện càng nhiều công việc càng tốt** (ví dụ: đọc/ghi các khối dữ liệu lớn thay vì từng byte một).

#### **2.2. File Descriptors (Mô tả File) 🔢**

  * **Lý thuyết:**
      * Khi bạn mở một file hoặc thiết bị trong Linux bằng các System Call cấp thấp, Kernel sẽ trả về một **File Descriptor (FD)**. Đây là một **số nguyên không âm nhỏ** mà tiến trình của bạn sử dụng để tham chiếu đến file hoặc thiết bị đang mở đó.
      * Mỗi tiến trình có một bảng riêng các File Descriptor.
      * **File Descriptor Chuẩn:** Khi một chương trình bắt đầu, nó thường có ba File Descriptor mặc định đã được mở sẵn:
          * **`0` (STDIN\_FILENO):** Standard Input (đầu vào chuẩn - thường là bàn phím).
          * **`1` (STDOUT\_FILENO):** Standard Output (đầu ra chuẩn - thường là màn hình terminal).
          * **`2` (STDERR\_FILENO):** Standard Error (đầu ra lỗi chuẩn - cũng thường là màn hình terminal).
  * **Liên hệ Embedded Linux:** Trong nhúng, bạn sẽ thường xuyên làm việc trực tiếp với File Descriptor khi tương tác với các thiết bị thông qua các file trong `/dev` hoặc các interface tùy chỉnh.

#### **2.3. Mở File: `open()` 🔓**

  * **Lý thuyết:** Hàm `open()` là System Call dùng để **tạo một File Descriptor mới** bằng cách mở một file hoặc thiết bị.

      * **Syntax:**
        
        ```c
        #include <fcntl.h>   // Cho open, các cờ O_
        #include <sys/types.h> // Cho mode_t (tùy chọn nhưng nên có)
        #include <sys/stat.h>  // Cho các cờ S_ (khi tạo file mới)

        int open(const char *path, int oflags);
                int open(const char *path, int oflags, mode_t mode); // Khi O_CREAT được dùng
        ```
      * **`path`**: Đường dẫn đến file hoặc thiết bị cần mở.
      * **`oflags` (Cờ Mở):**
          * Là sự kết hợp (sử dụng toán tử bitwise OR `|`) của một **chế độ truy cập bắt buộc** và các **chế độ tùy chọn** khác.
          * **Chế độ truy cập bắt buộc (chọn một):**
              * `O_RDONLY`: Mở chỉ để đọc.
              * `O_WRONLY`: Mở chỉ để ghi.
              * `O_RDWR`: Mở để đọc và ghi.
          * **Các chế độ tùy chọn phổ biến:**
              * `O_APPEND`: Ghi dữ liệu vào cuối file.
              * `O_TRUNC`: Cắt bớt file về kích thước 0 (xóa nội dung cũ) nếu file đã tồn tại và mở để ghi.
              * `O_CREAT`: **Tạo file** nếu nó chưa tồn tại. Khi dùng cờ này, bạn **phải cung cấp tham số `mode` thứ ba** để xác định quyền hạn của file mới.
              * `O_EXCL`: Sử dụng **kết hợp với `O_CREAT`**. Đảm bảo rằng chỉ tiến trình gọi `open()` mới có thể tạo file. Nếu file đã tồn tại, `open()` sẽ thất bại (atomic file creation).
              * `O_NONBLOCK`: Mở file ở chế độ không chặn (non-blocking). Các thao tác đọc/ghi sẽ trả về ngay lập tức nếu không có dữ liệu sẵn sàng hoặc buffer đầy, thay vì bị chặn. Rất quan trọng cho I/O không đồng bộ.
      * **`mode` (Quyền hạn - khi `O_CREAT`):**
          * Là sự kết hợp bitwise OR của các cờ quyền hạn (permissions) được định nghĩa trong `<sys/stat.h>`.
          * Ví dụ: `S_IRUSR | S_IWUSR` (chủ sở hữu có quyền đọc/ghi), `S_IRGRP` (nhóm có quyền đọc), `S_IXOTH` (người khác có quyền thực thi).
          * Các quyền này sẽ bị ảnh hưởng bởi `umask` của người dùng (sẽ xem sau).
      * **Giá trị trả về:**
          * Thành công: Trả về một **File Descriptor mới** (một số nguyên không âm, luôn là số nhỏ nhất chưa được sử dụng).
          * Thất bại: Trả về `-1` và thiết lập biến toàn cục `errno` để chỉ ra lỗi.

  * **Liên hệ Embedded Linux:** `open()` là hàm cơ bản nhất để giao tiếp với bất kỳ thiết bị nào được ánh xạ trong `/dev` (ví dụ: `open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NONBLOCK)` cho cổng serial) hoặc file hệ thống. Việc hiểu rõ các cờ `oflags` là tối quan trọng để điều khiển hành vi của thiết bị.

  * **Ví dụ (C): Mở và Tạo File**

    ```c
    #include <stdio.h>
    #include <stdlib.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <string.h> // For strerror
    #include <errno.h>  // For errno

    int main() {
        int fd;
        const char *filename_read = "example_read.txt";
        const char *filename_write = "example_write.txt";

        // Tạo một file mới để ghi với quyền 0644 (rw-r--r--)
        // S_IRUSR (Read by User), S_IWUSR (Write by User)
        // S_IRGRP (Read by Group), S_IROTH (Read by Others)
        AppLogger::log(AppLogger::INFO_L, "Attempting to create and open " + std::string(filename_write));
        fd = open(filename_write, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Failed to open " + std::string(filename_write) + ": " + std::string(strerror(errno)));
            return EXIT_FAILURE;
        }
        AppLogger::log(AppLogger::SUCCESS_L, "Successfully opened " + std::string(filename_write) + " with FD: " + std::to_string(fd));
        
        // Ghi vài dữ liệu vào file
        const char *data_to_write = "Hello from Linux File Programming!\n";
        ssize_t bytes_written = write(fd, data_to_write, strlen(data_to_write));
        if (bytes_written == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Failed to write to " + std::string(filename_write) + ": " + std::string(strerror(errno)));
            close(fd);
            return EXIT_FAILURE;
        }
        AppLogger::log(AppLogger::INFO_L, "Wrote " + std::to_string(bytes_written) + " bytes to " + std::string(filename_write));
        close(fd); // Đóng file

        // Mở một file chỉ để đọc
        AppLogger::log(AppLogger::INFO_L, "Attempting to open " + std::string(filename_read) + " for reading.");
        fd = open(filename_read, O_RDONLY);
        if (fd == -1) {
            if (errno == ENOENT) {
                AppLogger::log(AppLogger::WARNING_L, "File " + std::string(filename_read) + " does not exist. Please create it first.");
            } else {
                AppLogger::log(AppLogger::ERROR_L, "Failed to open " + std::string(filename_read) + ": " + std::string(strerror(errno)));
            }
            return EXIT_FAILURE;
        }
        AppLogger::log(AppLogger::SUCCESS_L, "Successfully opened " + std::string(filename_read) + " with FD: " + std::to_string(fd));
        
        close(fd); // Đóng file

        return EXIT_SUCCESS;
    }
    ```

#### **2.4. Ghi File: `write()` ✍️**

  * **Lý thuyết:** Hàm `write()` là System Call dùng để ghi dữ liệu từ bộ đệm vào một File Descriptor đã mở.

      * **Syntax:**
        ```c
        #include <unistd.h> // For write
        ssize_t write(int fildes, const void *buf, size_t nbytes);
        ```
      * **`fildes`**: File Descriptor của file/thiết bị đã mở để ghi.
      * **`buf`**: Con trỏ tới bộ đệm chứa dữ liệu cần ghi.
      * **`nbytes`**: Số byte tối đa cần ghi.
      * **Giá trị trả về:**
          * Thành công: Trả về **số byte thực tế đã ghi**. Có thể nhỏ hơn `nbytes` nếu có lỗi hoặc giới hạn của thiết bị (ví dụ: pipe đầy).
          * Thất bại: Trả về `-1` và thiết lập `errno`.
          * `0`: Có thể xảy ra nếu `nbytes` là 0, hoặc nếu bạn cố gắng ghi vào một File Descriptor hợp lệ nhưng không thể ghi thêm dữ liệu (ví dụ: ghi vào đầu đọc của pipe, hoặc ghi vào một ổ đĩa đầy và đã đạt đến giới hạn `PIPE_BUF` mà không thể ghi thêm).

  * **Liên hệ Embedded Linux:** `write()` là hàm cơ bản để xuất dữ liệu ra các thiết bị (ví dụ: ghi byte ra cổng serial, điều khiển GPIO bằng cách ghi vào file sysfs của nó).

  * **Ví dụ (C): Ghi ra Standard Output và Standard Error**

    ```c
    #include <unistd.h> // For write
    #include <stdlib.h> // For exit
    #include <string.h> // For strlen
    #include <stdio.h>  // For perror (optional)

    int main() {
        const char *data_to_stdout = "This goes to standard output.\n";
        const char *error_to_stderr = "This is an error message.\n";

        // Ghi ra standard output (FD 1)
        AppLogger::log(AppLogger::INFO_L, "Writing to STDOUT_FILENO (FD 1).");
        ssize_t bytes_written_out = write(STDOUT_FILENO, data_to_stdout, strlen(data_to_stdout));
        if (bytes_written_out == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Write to STDOUT_FILENO failed: " + std::string(strerror(errno)));
        } else {
            AppLogger::log(AppLogger::SUCCESS_L, "Wrote " + std::to_string(bytes_written_out) + " bytes to STDOUT.");
        }

        // Ghi ra standard error (FD 2)
        AppLogger::log(AppLogger::INFO_L, "Writing to STDERR_FILENO (FD 2).");
        ssize_t bytes_written_err = write(STDERR_FILENO, error_to_stderr, strlen(error_to_stderr));
        if (bytes_written_err == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Write to STDERR_FILENO failed: " + std::string(strerror(errno)));
        } else {
            AppLogger::log(AppLogger::SUCCESS_L, "Wrote " + std::to_string(bytes_written_err) + " bytes to STDERR.");
        }

        return EXIT_SUCCESS;
    }
    ```

#### **2.5. Đọc File: `read()` 📖**

  * **Lý thuyết:** Hàm `read()` là System Call dùng để đọc dữ liệu từ một File Descriptor đã mở vào bộ đệm.

      * **Syntax:**
        ```c
        #include <unistd.h> // For read
        ssize_t read(int fildes, void *buf, size_t nbytes);
        ```
      * **`fildes`**: File Descriptor của file/thiết bị đã mở để đọc.
      * **`buf`**: Con trỏ tới bộ đệm nơi dữ liệu đọc được sẽ được lưu trữ.
      * **`nbytes`**: Số byte tối đa cần đọc.
      * **Giá trị trả về:**
          * Thành công: Trả về **số byte thực tế đã đọc**. Có thể nhỏ hơn `nbytes` nếu đã đạt đến cuối file (EOF), hoặc dữ liệu chưa đủ, hoặc bị gián đoạn.
          * `0`: Đã đạt đến cuối file (End-Of-File - EOF) và không còn gì để đọc.
          * Thất bại: Trả về `-1` và thiết lập `errno`.

  * **Liên hệ Embedded Linux:** `read()` là hàm cơ bản để lấy dữ liệu từ các cảm biến, thiết bị ngoại vi thông qua file thiết bị, hoặc đọc dữ liệu từ một socket/pipe.

  * **Ví dụ (C): Đọc từ Standard Input và sao chép**

    ```c
    #include <unistd.h> // For read, write
    #include <stdlib.h> // For exit
    #include <string.h> // For memset
    #include <stdio.h>  // For BUFSIZ

    int main() {
        char buffer[BUFSIZ]; // BUFSIZ thường là 8192 bytes
        ssize_t nread;

        AppLogger::log(AppLogger::INFO_L, "Reading from STDIN_FILENO (FD 0). Type something and press Enter.");
        AppLogger::log(AppLogger::INFO_L, "Press Ctrl+D to signal End-Of-File.");

        // Đọc từ standard input (FD 0)
        while ((nread = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
            AppLogger::log(AppLogger::INFO_L, "Read " + std::to_string(nread) + " bytes. Echoing to STDOUT.");
            // Ghi lại ra standard output (FD 1)
            ssize_t bytes_written_out = write(STDOUT_FILENO, buffer, nread);
            if (bytes_written_out == -1) {
                AppLogger::log(AppLogger::ERROR_L, "Write to STDOUT_FILENO failed: " + std::string(strerror(errno)));
                return EXIT_FAILURE;
            }
        }

        if (nread == -1) { // Lỗi
            AppLogger::log(AppLogger::ERROR_L, "Read from STDIN_FILENO failed: " + std::string(strerror(errno)));
            return EXIT_FAILURE;
        } else if (nread == 0) { // EOF
            AppLogger::log(AppLogger::SUCCESS_L, "End-Of-File detected on STDIN.");
        }

        return EXIT_SUCCESS;
    }
    ```

#### **2.6. Đóng File: `close()` 🛑**

  * **Lý thuyết:** Hàm `close()` là System Call dùng để chấm dứt sự liên kết giữa một File Descriptor và file/thiết bị mà nó đang tham chiếu. File Descriptor đó sau đó sẽ trở nên trống và có thể được tái sử dụng bởi Kernel.

      * **Syntax:**
        ```c
        #include <unistd.h> // For close
        int close(int fildes);
        ```
      * **`fildes`**: File Descriptor của file/thiết bị cần đóng.
      * **Giá trị trả về:**
          * Thành công: Trả về `0`.
          * Thất bại: Trả về `-1` và thiết lập `errno`.
      * **Quan trọng:**
          * Việc kiểm tra giá trị trả về của `close()` là quan trọng, đặc biệt với các file system mạng, vì các lỗi ghi có thể chỉ được báo cáo khi file được đóng.
          * Số lượng File Descriptor mà một tiến trình có thể mở cùng lúc bị giới hạn (thường là `OPEN_MAX`, ít nhất 16 theo POSIX). Đóng các FD không dùng nữa là cần thiết để tránh vượt quá giới hạn này và rò rỉ tài nguyên.
          * Khi một chương trình thoát một cách bình thường (gọi `exit()`), tất cả các File Descriptor đã mở sẽ tự động được đóng bởi Kernel. Tuy nhiên, việc đóng file một cách rõ ràng là thực hành tốt để quản lý tài nguyên.

  * **Liên hệ Embedded Linux:** Quản lý File Descriptor là tối quan trọng trong các hệ thống nhúng, nơi tài nguyên bị giới hạn nghiêm ngặt. Việc mở quá nhiều file/thiết bị mà không đóng chúng có thể dẫn đến lỗi "Too many open files" và làm treo hệ thống.

#### **2.7. Thay đổi vị trí con trỏ File: `lseek()` ➡️**

  * **Lý thuyết:** Hàm `lseek()` là System Call dùng để thay đổi vị trí hiện tại của con trỏ đọc/ghi trong một file. Điều này cho phép bạn truy cập ngẫu nhiên đến các vị trí khác nhau trong file.

      * **Syntax:**
        ```c
        #include <unistd.h>  // For lseek
        #include <sys/types.h> // For off_t
        off_t lseek(int fildes, off_t offset, int whence);
        ```
      * **`fildes`**: File Descriptor của file đã mở.
      * **`offset`**: Giá trị offset tính bằng byte.
      * **`whence`**: Xác định cách `offset` được tính:
          * `SEEK_SET`: `offset` tính từ đầu file.
          * `SEEK_CUR`: `offset` tính từ vị trí hiện tại của con trỏ file.
          * `SEEK_END`: `offset` tính từ cuối file.
      * **Giá trị trả về:**
          * Thành công: Trả về vị trí con trỏ file mới (tính từ đầu file).
          * Thất bại: Trả về `-1` và thiết lập `errno`.

  * **Liên hệ Embedded Linux:** Hữu ích khi làm việc với các file chứa dữ liệu có cấu trúc (ví dụ: các bản ghi trong cơ sở dữ liệu nhúng) và bạn cần truy cập một bản ghi cụ thể mà không cần đọc toàn bộ file.

  * **Ví dụ (C): Sử dụng `lseek` để đọc một phần file**

    ```c
    #include <stdio.h>
    #include <stdlib.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <string.h>
    #include <errno.h>

    int main() {
        int fd;
        const char *filename = "lseek_test.txt";
        char buffer[20];
        ssize_t bytes_read;

        // Tạo một file ví dụ
        AppLogger::log(AppLogger::INFO_L, "Creating file " + std::string(filename));
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Failed to create file: " + std::string(strerror(errno)));
            return EXIT_FAILURE;
        }
        write(fd, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36);
        close(fd);

        // Mở file để đọc
        AppLogger::log(AppLogger::INFO_L, "Opening file " + std::string(filename) + " for reading.");
        fd = open(filename, O_RDONLY);
        if (fd == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Failed to open file for reading: " + std::string(strerror(errno)));
            return EXIT_FAILURE;
        }

        // Di chuyển con trỏ file đến byte thứ 10 (từ đầu)
        off_t current_pos = lseek(fd, 10, SEEK_SET);
        if (current_pos == -1) {
            AppLogger::log(AppLogger::ERROR_L, "lseek failed: " + std::string(strerror(errno)));
            close(fd);
            return EXIT_FAILURE;
        }
        AppLogger::log(AppLogger::INFO_L, "Moved file pointer to position: " + std::to_string(current_pos));

        // Đọc 5 bytes từ vị trí hiện tại
        bytes_read = read(fd, buffer, 5);
        if (bytes_read == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Read failed: " + std::string(strerror(errno)));
        } else if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // Null-terminate the string
            AppLogger::log(AppLogger::SUCCESS_L, "Read " + std::to_string(bytes_read) + " bytes: '" + std::string(buffer) + "'");
        } else {
            AppLogger::log(AppLogger::WARNING_L, "No bytes read.");
        }

        close(fd);
        unlink(filename); // Xóa file ví dụ
        return EXIT_SUCCESS;
    }
    ```

#### **2.8. Lấy thông tin File: `fstat()`, `stat()`, `lstat()` ℹ️**

  * **Lý thuyết:** Các System Call này dùng để lấy thông tin chi tiết (metadata) về một file hoặc thư mục. Thông tin được trả về trong một cấu trúc `struct stat`.

      * **Syntax:**
        ```c
        #include <unistd.h>  // For fstat, stat, lstat
        #include <sys/stat.h>  // For struct stat and mode flags
        #include <sys/types.h> // For types like ino_t, dev_t, uid_t, gid_t (optional but good practice)

        int fstat(int fildes, struct stat *buf);    // Lấy thông tin từ File Descriptor
        int stat(const char *path, struct stat *buf); // Lấy thông tin từ đường dẫn file
        int lstat(const char *path, struct stat *buf); // Lấy thông tin từ đường dẫn file (xử lý symlink khác stat)
        ```
      * **`fstat()`**: Dùng với một **File Descriptor đã mở**.
      * **`stat()`**: Dùng với một **đường dẫn file**. Nếu `path` là một symbolic link, `stat()` sẽ theo dõi link và trả về thông tin của file/thư mục mà link đó trỏ tới.
      * **`lstat()`**: Dùng với một **đường dẫn file**. Nếu `path` là một symbolic link, `lstat()` sẽ trả về thông tin của **chính symbolic link đó**, không theo dõi link.
      * **`struct stat`**: Chứa các trường thông tin như:
          * `st_mode`: Chế độ file (quyền hạn và loại file).
          * `st_ino`: Inode number.
          * `st_dev`: ID của thiết bị chứa file.
          * `st_uid`, `st_gid`: User ID và Group ID của chủ sở hữu.
          * `st_size`: Kích thước file (bytes).
          * `st_atime`: Thời gian truy cập lần cuối.
          * `st_mtime`: Thời gian sửa đổi nội dung lần cuối.
          * `st_ctime`: Thời gian thay đổi trạng thái (permissions, owner, group) lần cuối.
          * `st_nlink`: Số lượng hard links trỏ tới file.
      * **Macro phân tích `st_mode`:** Rất nhiều macro để kiểm tra loại file hoặc quyền hạn (ví dụ: `S_ISDIR(st_mode)` kiểm tra có phải thư mục không, `S_ISREG(st_mode)` kiểm tra có phải file thông thường không, `S_IRUSR`, `S_IWOTH` cho quyền hạn).

  * **Liên hệ Embedded Linux:** `stat()` là công cụ vô giá để kiểm tra sự tồn tại của file/thiết bị, loại file (char/block device, directory), quyền hạn của chúng trước khi cố gắng thao tác. Điều này rất quan trọng cho các chương trình cần tương tác với các file sysfs/devfs.

  * **Ví dụ (C): Lấy thông tin file/thư mục**

    ```c
    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/stat.h> // For stat, S_ISDIR, S_ISREG etc.
    #include <unistd.h>   // For stat, lstat
    #include <string.h>   // For strerror
    #include <errno.h>    // For errno

    int main() {
        const char *test_path_file = "my_test_file.txt";
        const char *test_path_dir = "my_test_dir";
        const char *test_path_symlink = "my_symlink_to_file";
        struct stat file_stat;

        // Tạo file và thư mục để test
        system("touch my_test_file.txt && mkdir my_test_dir");
        system("ln -s my_test_file.txt my_symlink_to_file");
        
        AppLogger::log(AppLogger::INFO_L, "--- Checking stat() for files and directories ---");

        // Kiểm tra file
        AppLogger::log(AppLogger::INFO_L, "Getting info for: " + std::string(test_path_file));
        if (stat(test_path_file, &file_stat) == -1) {
            AppLogger::log(AppLogger::ERROR_L, "stat failed for file: " + std::string(strerror(errno)));
        } else {
            if (S_ISREG(file_stat.st_mode)) {
                AppLogger::log(AppLogger::SUCCESS_L, "  Is a regular file.");
            }
            AppLogger::log(AppLogger::INFO_L, "  Size: " + std::to_string(file_stat.st_size) + " bytes");
            AppLogger::log(AppLogger::INFO_L, "  Owner UID: " + std::to_string(file_stat.st_uid));
        }

        // Kiểm tra thư mục
        AppLogger::log(AppLogger::INFO_L, "Getting info for: " + std::string(test_path_dir));
        if (stat(test_path_dir, &file_stat) == -1) {
            AppLogger::log(AppLogger::ERROR_L, "stat failed for dir: " + std::string(strerror(errno)));
        } else {
            if (S_ISDIR(file_stat.st_mode)) {
                AppLogger::log(AppLogger::SUCCESS_L, "  Is a directory.");
            }
            AppLogger::log(AppLogger::INFO_L, "  Permissions (octal): " + std::to_string(file_stat.st_mode & 0777));
        }

        // Kiểm tra symlink với stat() (theo dõi link)
        AppLogger::log(AppLogger::INFO_L, "Getting info for symlink with stat(): " + std::string(test_path_symlink));
        if (stat(test_path_symlink, &file_stat) == -1) {
            AppLogger::log(AppLogger::ERROR_L, "stat failed for symlink: " + std::string(strerror(errno)));
        } else {
            if (S_ISREG(file_stat.st_mode)) { // Sẽ là REG vì theo dõi link đến file
                AppLogger::log(AppLogger::SUCCESS_L, "  Is a regular file (via symlink).");
            }
            AppLogger::log(AppLogger::INFO_L, "  Size: " + std::to_string(file_stat.st_size) + " bytes");
        }

        // Kiểm tra symlink với lstat() (không theo dõi link)
        AppLogger::log(AppLogger::INFO_L, "Getting info for symlink with lstat(): " + std::string(test_path_symlink));
        if (lstat(test_path_symlink, &file_stat) == -1) {
            AppLogger::log(AppLogger::ERROR_L, "lstat failed for symlink: " + std::string(strerror(errno)));
        } else {
            if (S_ISLNK(file_stat.st_mode)) { // Sẽ là LNK vì không theo dõi link
                AppLogger::log(AppLogger::SUCCESS_L, "  Is a symbolic link.");
            }
            AppLogger::log(AppLogger::INFO_L, "  Size of symlink itself: " + std::to_string(file_stat.st_size) + " bytes");
        }
        
        // Dọn dẹp
        system("rm -rf my_test_file.txt my_test_dir my_symlink_to_file");
        return EXIT_SUCCESS;
    }
    ```

#### **2.9. Nhân bản File Descriptor: `dup()` và `dup2()` 👯**

  * **Lý thuyết:** Các System Call này cho phép bạn tạo một bản sao của một File Descriptor đã tồn tại. Hai File Descriptor khác nhau sẽ cùng trỏ đến cùng một file/thiết bị và chia sẻ trạng thái của con trỏ file.

      * **Syntax:**
        ```c
        #include <unistd.h> // For dup, dup2
        int dup(int fildes);
        int dup2(int oldfd, int newfd);
        ```
      * **`dup(fildes)`**:
          * Tạo một File Descriptor mới có số hiệu **thấp nhất chưa được sử dụng**.
          * File Descriptor mới này là một bản sao của `fildes`.
      * **`dup2(oldfd, newfd)`**:
          * Sao chép `oldfd` sang `newfd`.
          * Nếu `newfd` đã được mở, nó sẽ được đóng trước khi sao chép.
          * Nếu `oldfd` và `newfd` giống nhau, hàm không làm gì và trả về `newfd`.
          * Hữu ích để chuyển hướng các File Descriptor chuẩn (0, 1, 2) sang các file/pipes khác.

  * **Liên hệ Embedded Linux:** Cực kỳ quan trọng khi làm việc với Pipes và Socket để chuyển hướng `stdin`/`stdout`/`stderr` của tiến trình con sang các kênh giao tiếp khác.

  * **Ví dụ (C): Chuyển hướng Standard Output với `dup2`**

    ```c
    #include <stdio.h>
    #include <stdlib.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <string.h>
    #include <errno.h>

    int main() {
        int original_stdout_fd = dup(STDOUT_FILENO); // Sao lưu STDOUT gốc
        if (original_stdout_fd == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Failed to duplicate STDOUT: " + std::string(strerror(errno)));
            return EXIT_FAILURE;
        }
        
        const char *output_filename = "redirected_output.txt";
        int file_fd;

        AppLogger::log(AppLogger::INFO_L, "Main program: Output will be redirected to " + std::string(output_filename));

        // Mở file để ghi
        file_fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (file_fd == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Failed to open file for redirection: " + std::string(strerror(errno)));
            close(original_stdout_fd);
            return EXIT_FAILURE;
        }

        // Chuyển hướng STDOUT (FD 1) sang file_fd
        if (dup2(file_fd, STDOUT_FILENO) == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Failed to redirect STDOUT: " + std::string(strerror(errno)));
            close(file_fd);
            close(original_stdout_fd);
            return EXIT_FAILURE;
        }
        close(file_fd); // Đóng file_fd gốc (STDOUT_FILENO đã trỏ tới nó)

        // Bây giờ, mọi printf() hoặc write() ra STDOUT sẽ ghi vào file
        AppLogger::log(AppLogger::SUCCESS_L, "STDOUT is now redirected. Messages below go to file.");
        printf("This text goes into the file 'redirected_output.txt'.\n");
        printf("So does this line.\n");

        // Chuyển hướng STDOUT trở lại terminal (sử dụng bản sao lưu)
        if (dup2(original_stdout_fd, STDOUT_FILENO) == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Failed to restore STDOUT: " + std::string(strerror(errno)));
            close(original_stdout_fd);
            return EXIT_FAILURE;
        }
        close(original_stdout_fd); // Đóng bản sao lưu

        AppLogger::log(AppLogger::SUCCESS_L, "STDOUT is restored to console.");
        printf("This text goes back to the console.\n");

        // Dọn dẹp
        // unlink(output_filename); // Không unlink để bạn có thể kiểm tra nội dung file
        return EXIT_SUCCESS;
    }
    ```

#### **2.10. Điều khiển Thiết bị: `ioctl()` 🕹️**

  * **Lý thuyết:** Hàm `ioctl()` (Input/Output Control) là một System Call "đa năng" dùng để thực hiện các thao tác điều khiển cấp thấp, **cụ thể cho từng thiết bị** (device-specific control), mà không thể thực hiện được bằng các hàm `read()`/`write()` thông thường.

      * **Syntax:**
        ```c
        #include <unistd.h> // For ioctl
        int ioctl(int fildes, int cmd, ... /* arg */);
        ```
      * **`fildes`**: File Descriptor của thiết bị cần điều khiển.
      * **`cmd`**: Một mã lệnh (integer command code) cụ thể cho thiết bị/driver đó, xác định hành động cần thực hiện.
      * **`... (arg)`**: Một đối số tùy chọn (có thể là con trỏ tới một cấu trúc dữ liệu) mà `cmd` có thể yêu cầu.
      * **Giá trị trả về:** `0` nếu thành công, `-1` nếu thất bại (`errno` được thiết lập).

  * **Hạn chế:** `ioctl()` không di động (non-portable) giữa các loại thiết bị hoặc hệ thống khác nhau, vì mỗi driver định nghĩa các lệnh `ioctl` riêng của nó. Bạn cần tra cứu manual pages cụ thể cho thiết bị (`man 4 device_name`) hoặc source code của driver để biết các lệnh `ioctl` được hỗ trợ.

  * **Liên hệ Embedded Linux:** Cực kỳ quan trọng trong lập trình nhúng khi bạn cần điều khiển các tính năng đặc biệt của phần cứng mà không có API chuẩn (ví dụ: điều khiển tốc độ baud của UART, cấu hình chế độ của sensor thông qua driver).

  * **Ví dụ (C): `ioctl` (Mô phỏng - thực tế cần device thật)**

    ```c
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/ioctl.h> // For ioctl
    #include <string.h>
    #include <errno.h>

    // Các định nghĩa ioctl thực tế sẽ có trong các header file của driver hoặc hệ thống
    // Ví dụ: #include <linux/serial.h> cho cổng serial
    // Ở đây ta dùng một define giả để minh họa
    #define MY_CUSTOM_IOCTL_COMMAND_SET_BAUD_RATE 0x1234
    #define MY_CUSTOM_IOCTL_COMMAND_GET_STATUS  0x1235

    int main() {
        // Trong thực tế, đây sẽ là File Descriptor của một thiết bị thực sự, ví dụ: /dev/ttyS0
        // Để minh họa, chúng ta sẽ mở /dev/null
        int device_fd = open("/dev/null", O_RDWR); 
        if (device_fd == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Failed to open device: " + std::string(strerror(errno)));
            return EXIT_FAILURE;
        }
        AppLogger::log(AppLogger::SUCCESS_L, "Opened /dev/null with FD: " + std::to_string(device_fd));

        // --- Ví dụ 1: Gửi lệnh SET_BAUD_RATE ---
        int baud_rate = 115200;
        AppLogger::log(AppLogger::INFO_L, "Attempting to send custom IOCTL command SET_BAUD_RATE with value " + std::to_string(baud_rate));
        int result = ioctl(device_fd, MY_CUSTOM_IOCTL_COMMAND_SET_BAUD_RATE, &baud_rate);
        if (result == -1) {
            // Rất có thể sẽ lỗi vì /dev/null không hỗ trợ lệnh này
            AppLogger::log(AppLogger::WARNING_L, "IOCTL command SET_BAUD_RATE failed: " + std::string(strerror(errno)) + " (Expected if /dev/null)");
        } else {
            AppLogger::log(AppLogger::SUCCESS_L, "IOCTL command SET_BAUD_RATE successful.");
        }

        // --- Ví dụ 2: Gửi lệnh GET_STATUS ---
        int device_status = 0;
        AppLogger::log(AppLogger::INFO_L, "Attempting to send custom IOCTL command GET_STATUS.");
        result = ioctl(device_fd, MY_CUSTOM_IOCTL_COMMAND_GET_STATUS, &device_status);
        if (result == -1) {
            AppLogger::log(AppLogger::WARNING_L, "IOCTL command GET_STATUS failed: " + std::string(strerror(errno)) + " (Expected if /dev/null)");
        } else {
            AppLogger::log(AppLogger::SUCCESS_L, "IOCTL command GET_STATUS successful. Status: " + std::to_string(device_status));
        }

        close(device_fd);
        AppLogger::log(AppLogger::INFO_L, "Device closed.");
        return EXIT_SUCCESS;
    }
    ```

-----

### **Câu hỏi Tự đánh giá Module 2 🤔**

1.  Tại sao System Calls lại "đắt đỏ" hơn các hàm thư viện thông thường? Nêu một chiến lược để tối ưu hóa việc sử dụng System Calls.
2.  File Descriptor là gì? Nêu tên và số hiệu của 3 File Descriptor tiêu chuẩn được mở khi một chương trình bắt đầu.
3.  Khi gọi `open()` với cờ `O_CREAT`, bạn phải cung cấp thêm tham số `mode`. Tham số này có ý nghĩa gì và nó khác với `oflags` như thế nào?
4.  Bạn muốn tạo một file `my_log.txt` chỉ để ghi, và nếu nó đã tồn tại thì phải cắt bỏ nội dung cũ. Nếu file chưa tồn tại thì tạo mới với quyền chủ sở hữu đọc/ghi. Viết lệnh `open()` với các cờ `oflags` và `mode` phù hợp.
5.  Phân biệt giá trị trả về `0` và `-1` của hàm `read()`. Khi nào `read()` có thể trả về một số dương nhỏ hơn số byte yêu cầu?
6.  Giải thích tầm quan trọng của `lseek()` trong việc truy cập dữ liệu trong các file cấu trúc lớn.
7.  `stat()` và `lstat()` khác nhau như thế nào khi xử lý symbolic links? Nêu một trường hợp cụ thể bạn sẽ sử dụng `lstat()` thay vì `stat()`.
8.  Tại sao `ioctl()` lại không portable? Nêu một trường hợp sử dụng `ioctl()` trong lập trình nhúng.

-----

### **Bài tập Thực hành Module 2 ✍️**

1.  **Chương trình Sao chép File Cấp thấp:**

      * Viết một chương trình C (`my_cp_low_level.c`) sử dụng các System Call `open()`, `read()`, `write()`, `close()` để sao chép nội dung của một file nguồn sang một file đích.
      * Chương trình nhận hai tham số dòng lệnh: `<file_nguon>` và `<file_dich>`.
      * Sử dụng một buffer có kích thước 4KB (hoặc `BUFSIZ`) để đọc và ghi, thay vì đọc/ghi từng byte.
      * File đích cần được tạo nếu chưa tồn tại, và nếu có thì ghi đè nội dung cũ, với quyền `rw-r--r--`.
      * Xử lý lỗi cơ bản (kiểm tra giá trị trả về của các hàm, dùng `perror()`).
      * **Thử thách:** Đo thời gian chạy của chương trình bạn với một file lớn (ví dụ: 10MB) và so sánh với lệnh `cp` của Linux.

2.  **Kiểm tra loại File và Quyền hạn:**

      * Viết một chương trình C (`file_inspector.c`) nhận một tham số dòng lệnh là đường dẫn đến một file hoặc thư mục.
      * Sử dụng hàm `lstat()` để lấy thông tin về đường dẫn đó.
      * In ra các thông tin sau:
          * Tên file/thư mục.
          * Nó có phải là file thông thường, thư mục, symbolic link, character device hay block device không? (Sử dụng các macro `S_IS*()`).
          * Kích thước file (nếu là file thông thường).
          * Quyền hạn của file dưới dạng octal (ví dụ: `0644`).
          * UID và GID của chủ sở hữu.
      * Xử lý lỗi nếu đường dẫn không tồn tại hoặc không thể truy cập.

3.  **Chuyển hướng Standard Error:**

      * Viết một chương trình C (`redirect_stderr.c`) mà:
          * Mở một file `error_log.txt` để ghi (tạo mới nếu cần, ghi đè nếu tồn tại, quyền `0644`).
          * Sử dụng `dup2()` để chuyển hướng `STDERR_FILENO` (FD 2) của tiến trình sang file `error_log.txt`.
          * Sau đó, thực hiện một thao tác gây lỗi (ví dụ: cố gắng mở một file không tồn tại, hoặc gọi một hàm `perror()` nào đó).
          * Sau khi gây lỗi, chuyển hướng `STDERR_FILENO` trở lại terminal (bằng cách sao lưu FD 2 gốc trước khi chuyển hướng).
          * Kiểm tra nội dung của `error_log.txt` sau khi chạy chương trình.

-----

