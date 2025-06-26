
-----

### **1. The Linux Environment: Tổng quan về môi trường chương trình 🌍**

Trong Linux, "môi trường" của một chương trình đề cập đến tập hợp các tài nguyên, thông tin và cấu hình mà một tiến trình (process) có thể truy cập khi nó đang chạy. Điều này bao gồm mọi thứ từ các tham số dòng lệnh (command-line arguments) được truyền vào, các biến môi trường (environment variables) được kế thừa, cho đến các giới hạn tài nguyên (resource limits) mà Kernel áp đặt.

Việc hiểu rõ môi trường giúp bạn:

* **Cấu hình linh hoạt:** Thay đổi hành vi của chương trình mà không cần biên dịch lại.
* **Tương tác hệ thống:** Truy cập thông tin hệ thống như thời gian, thông tin người dùng, tên máy chủ.
* **Quản lý tài nguyên:** Đảm bảo chương trình hoạt động hiệu quả và không gây quá tải hệ thống, đặc biệt quan trọng trong các hệ thống nhúng với tài nguyên hạn chế.

-----

### **2. Program Arguments (Tham số Chương trình) 🚦**

**Tham số chương trình** là các chuỗi giá trị được truyền vào một chương trình khi bạn khởi chạy nó từ dòng lệnh (command line). Chúng cho phép người dùng hoặc các script tùy chỉnh hành vi của chương trình tại thời điểm thực thi.

Trong C/C++, bạn truy cập các tham số này thông qua hàm `main()`:

```c
int main(int argc, char *argv[]) {
    // argc: số lượng tham số (bao gồm cả tên chương trình)
    // argv: một mảng các chuỗi (char*) chứa từng tham số
    // argv[0] là tên chương trình
    // argv[1] là tham số đầu tiên, v.v.
    return 0;
}
```

#### **2.1. `getopt` (Get Option Characters from Command Line Argument List) 📜**

Khi chương trình của bạn có nhiều tùy chọn (options) hoặc cờ (flags), việc phân tích cú pháp `argv` một cách thủ công trở nên phức tạp. Hàm `getopt` được thiết kế để đơn giản hóa việc này cho các **tùy chọn ngắn** (short options), thường là một dấu gạch ngang (`-`) theo sau là một ký tự duy nhất, ví dụ: `-v` (verbose), `-o output.txt` (output file).

* **Cách hoạt động:** `getopt` xử lý `argv` lần lượt, trả về ký tự tùy chọn được tìm thấy.
* **Các biến toàn cục quan trọng:**
    * `extern char *optarg;`: Chứa giá trị của tham số tùy chọn (nếu có, ví dụ: "output.txt" trong `-o output.txt`).
    * `extern int optind;`: Chỉ số của tham số `argv` tiếp theo sẽ được xử lý.
    * `extern int opterr;`: Nếu khác 0, `getopt` sẽ in thông báo lỗi không hợp lệ.
    * `extern int optopt;`: Chứa ký tự tùy chọn không xác định hoặc thiếu tham số.
* **`optstring`:** Một chuỗi các ký tự đại diện cho các tùy chọn hợp lệ.
    * Nếu một ký tự tùy chọn được theo sau bởi dấu hai chấm (`:`), nó yêu cầu một tham số (ví dụ: `"o:"` cho `-o output.txt`).
    * Nếu theo sau bởi hai dấu hai chấm (`::`), nó có tham số tùy chọn.

**Ví dụ (C): Sử dụng `getopt`**

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For getopt

int main(int argc, char *argv[]) {
    int opt;
    int verbose_flag = 0;
    char *output_file = NULL;

    // "vo:" means:
    //   'v' is a flag (no argument)
    //   'o' requires an argument
    while ((opt = getopt(argc, argv, "vo:")) != -1) {
        switch (opt) {
            case 'v':
                verbose_flag = 1;
                printf("Verbose mode enabled.\n");
                break;
            case 'o':
                output_file = optarg;
                printf("Output file: %s\n", output_file);
                break;
            case '?': // Unknown option or missing argument
                fprintf(stderr, "Usage: %s [-v] [-o <output_file>]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Process non-option arguments
    for (; optind < argc; optind++) {
        printf("Non-option argument: %s\n", argv[optind]);
    }

    if (verbose_flag) {
        printf("Application running in verbose mode.\n");
    }
    if (output_file) {
        printf("Writing results to: %s\n", output_file);
    }

    return EXIT_SUCCESS;
}
```

**Cách biên dịch và chạy:**

```bash
gcc -o myapp myapp.c
./myapp -v -o output.txt arg1 arg2
./myapp -o output.txt -v
./myapp -x # Lỗi
```

#### **2.2. `getopt_long` (Get Long Options from Command Line Argument List) 📏**

`getopt_long` là phiên bản mạnh mẽ hơn của `getopt`, cho phép bạn xử lý **tùy chọn dài** (long options) có tên đầy đủ, bắt đầu bằng hai dấu gạch ngang (`--`), ví dụ: `--verbose`, `--output=file.txt`. Điều này giúp các chương trình thân thiện với người dùng hơn vì các tùy chọn dễ đọc và dễ nhớ.

* **Cấu trúc:** Sử dụng một mảng `struct option` để định nghĩa các tùy chọn dài.
  ```c
  struct option {
      const char *name;      // Tên tùy chọn dài (ví dụ: "verbose")
      int         has_arg;   // Yêu cầu tham số: no_argument, required_argument, optional_argument
      int        *flag;      // Con trỏ tới cờ để set (hoặc NULL)
      int         val;       // Giá trị trả về khi tùy chọn được tìm thấy (hoặc được set vào *flag)
  };
  ```
* **`has_arg`:**
    * `no_argument`: Không có tham số (ví dụ: `--verbose`).
    * `required_argument`: Yêu cầu tham số (ví dụ: `--output=file.txt` hoặc `--output file.txt`).
    * `optional_argument`: Tham số tùy chọn (ví dụ: `--level` hoặc `--level=debug`).

**Ví dụ (C): Sử dụng `getopt_long`**

```c
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h> // For getopt_long

int main(int argc, char *argv[]) {
    int opt;
    int verbose_flag = 0;
    char *output_file = NULL;

    // Định nghĩa các tùy chọn dài
    static struct option long_options[] = {
        {"verbose", no_argument,       &verbose_flag, 1}, // Set verbose_flag = 1
        {"output",  required_argument, 0,             'o'}, // Returns 'o'
        {0, 0, 0, 0} // Sentinel to mark the end of the array
    };

    int long_index = 0;
    // Chuỗi tùy chọn ngắn "vo:" vẫn áp dụng cho các tùy chọn ngắn
    while ((opt = getopt_long(argc, argv, "vo:", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'v':
                verbose_flag = 1;
                printf("Short option -v: Verbose mode enabled.\n");
                break;
            case 'o':
                output_file = optarg;
                printf("Short option -o: Output file: %s\n", output_file);
                break;
            case 0: // Tùy chọn dài đã set một cờ (flag)
                if (long_options[long_index].flag != 0) {
                    printf("Long option --%s was set.\n", long_options[long_index].name);
                }
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-v|--verbose] [-o <file>|--output=<file>] [args...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Process non-option arguments
    for (; optind < argc; optind++) {
        printf("Non-option argument: %s\n", argv[optind]);
    }

    if (verbose_flag) {
        printf("Application running with verbose details.\n");
    }
    if (output_file) {
        printf("Final output destination: %s\n", output_file);
    }

    return EXIT_SUCCESS;
}
```

**Cách biên dịch và chạy:**

```bash
gcc -o myapp_long myapp_long.c
./myapp_long --verbose --output=report.log arg1
./myapp_long -v -o results.txt
./myapp_long --output results.txt --verbose
```

-----

### **3. Environment Variables (Biến Môi trường) 🌐**

**Biến môi trường** là các cặp tên-giá trị được lưu trữ trong môi trường của một tiến trình. Chúng cung cấp một cách để cấu hình hành vi của chương trình hoặc hệ thống mà không cần sửa đổi mã nguồn hoặc các file cấu hình tĩnh. Không giống như tham số chương trình chỉ được truyền khi khởi động, biến môi trường được các tiến trình con kế thừa từ tiến trình cha của chúng.

#### **3.1. Use of Environment Variables (Sử dụng Biến Môi trường) 🛠️**

Biến môi trường được sử dụng rộng rãi cho nhiều mục đích:

* **Đường dẫn thực thi (PATH):** Liệt kê các thư mục mà shell sẽ tìm kiếm các lệnh thực thi.
* **Thư mục Home (HOME):** Đường dẫn đến thư mục cá nhân của người dùng hiện tại.
* **Ngôn ngữ (LANG/LC\_ALL):** Cấu hình ngôn ngữ và locale cho các ứng dụng.
* **Thư viện động (LD\_LIBRARY\_PATH):** Quan trọng trong embedded Linux, biến này chỉ định các thư mục bổ sung để tìm kiếm các thư viện động (shared libraries) khi khởi chạy chương trình.
* **Cấu hình ứng dụng:** Nhiều ứng dụng sử dụng biến môi trường để tùy chỉnh cài đặt của chúng (ví dụ: `DEBUG=true` để bật chế độ debug).

**Cách truy cập trong Shell:**

* Hiển thị giá trị: `echo $VARIABLE_NAME`
* Thiết lập: `export VARIABLE_NAME=VALUE` (chỉ có hiệu lực trong shell hiện tại và các tiến trình con của nó).
* Liệt kê tất cả: `env` hoặc `printenv`

**Truy cập trong C/C++:** Hàm `getenv()`

```c
#include <stdio.h>
#include <stdlib.h> // For getenv

int main() {
    char *path_env = getenv("PATH");
    char *home_env = getenv("HOME");
    char *user_env = getenv("USER");

    if (path_env) {
        printf("PATH: %s\n", path_env);
    } else {
        printf("PATH environment variable not set.\n");
    }

    if (home_env) {
        printf("HOME: %s\n", home_env);
    } else {
        printf("HOME environment variable not set.\n");
    }

    if (user_env) {
        printf("USER: %s\n", user_env);
    } else {
        printf("USER environment variable not set.\n");
    }

    return 0;
}
```

**Truy cập trong Python:** Module `os.environ`

```python
import os
import logging

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

def demonstrate_env_vars():
    logger.info("Demonstrating Environment Variables access:")

    # Accessing specific environment variables
    path_env = os.environ.get("PATH")
    home_env = os.environ.get("HOME")
    user_env = os.environ.get("USER")
    ld_library_path_env = os.environ.get("LD_LIBRARY_PATH") # Useful for embedded

    if path_env:
        logger.info(f"PATH: {path_env[:50]}...") # Print first 50 chars for brevity
    else:
        logger.warning("PATH environment variable not set.")

    if home_env:
        logger.info(f"HOME: {home_env}")
    else:
        logger.warning("HOME environment variable not set.")

    if user_env:
        logger.info(f"USER: {user_env}")
    else:
        logger.warning("USER environment variable not set.")

    if ld_library_path_env:
        logger.info(f"LD_LIBRARY_PATH: {ld_library_path_env}")
    else:
        logger.debug("LD_LIBRARY_PATH environment variable not set (common).")

    # Setting a temporary environment variable
    os.environ["MY_CUSTOM_VAR"] = "Hello, Embedded World!"
    logger.success("Set MY_CUSTOM_VAR temporarily.")
    logger.info(f"MY_CUSTOM_VAR: {os.environ.get('MY_CUSTOM_VAR')}")

    # Deleting an environment variable
    if "MY_CUSTOM_VAR" in os.environ:
        del os.environ["MY_CUSTOM_VAR"]
        logger.success("Deleted MY_CUSTOM_VAR.")

    # Iterate through all environment variables (caution: can be very long)
    # logger.info("All Environment Variables (first 5):")
    # for i, (key, value) in enumerate(os.environ.items()):
    #     if i >= 5:
    #         break
    #     logger.debug(f"{key}={value}")

if __name__ == "__main__":
    demonstrate_env_vars()
```

#### **3.2. The `environ` Variable (Biến `environ`) 📚**

Trong C/C++, ngoài `getenv()`, bạn có thể truy cập toàn bộ môi trường thông qua một biến toàn cục đặc biệt tên là `environ`. Đây là một con trỏ tới một mảng các con trỏ `char`, kết thúc bằng một `NULL` pointer. Mỗi chuỗi trong mảng có định dạng "NAME=VALUE".

```c
#include <stdio.h>

// environ is declared in unistd.h, but often available directly
extern char **environ; // Khai báo extern để sử dụng biến toàn cục

int main() {
    printf("Listing all environment variables using 'environ':\n");
    for (char **env = environ; *env != NULL; env++) {
        printf("%s\n", *env);
    }
    return 0;
}
```

**Lưu ý:** Mặc dù bạn có thể truy cập `environ` trực tiếp, việc sử dụng các hàm như `getenv()` và `putenv()`/`setenv()` được khuyến khích hơn vì chúng an toàn và dễ quản lý hơn, đặc biệt khi sửa đổi môi trường.

#### **3.3. Time and Date (Thời gian và Ngày tháng) ⏰**

Lập trình viên thường cần truy cập thông tin thời gian và ngày tháng cho các tác vụ như ghi log (timestamping), lên lịch sự kiện, hoặc đo lường hiệu suất.

* **System Calls/C Library Functions:**
    * `time_t time(time_t *tloc);`: Trả về thời gian hiện tại dưới dạng số giây kể từ Epoch (00:00:00 UTC, 1/1/1970).
    * `int gettimeofday(struct timeval *tv, struct timezone *tz);`: Cung cấp độ chính xác cao hơn (micro giây - µs). Rất hữu ích cho việc đo lường hiệu suất trong embedded.
    * `int clock_gettime(clockid_t clk_id, struct timespec *tp);`: Cung cấp độ chính xác nano giây (ns) và nhiều loại đồng hồ khác nhau (ví dụ: CLOCK\_REALTIME, CLOCK\_MONOTONIC - quan trọng cho việc đo thời gian chạy không bị ảnh hưởng bởi thay đổi đồng hồ hệ thống).
    * `struct tm *localtime(const time_t *timer);`: Chuyển đổi `time_t` sang cấu trúc `tm` (năm, tháng, ngày, giờ, phút, giây) dựa trên múi giờ cục bộ.
    * `size_t strftime(char *s, size_t max, const char *format, const struct tm *tm);`: Định dạng thời gian/ngày tháng thành chuỗi theo định dạng tùy chỉnh.

**Ví dụ (C): Lấy thời gian hiện tại**

```c
#include <stdio.h>
#include <time.h>    // For time, localtime, strftime
#include <sys/time.h> // For gettimeofday

int main() {
    time_t current_time;
    struct tm *local_time_info;
    char time_buffer[80];

    // Using time() and strftime()
    time(&current_time);
    local_time_info = localtime(&current_time);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", local_time_info);
    printf("Current local time (time_t): %s\n", time_buffer);

    // Using gettimeofday() for microsecond precision
    struct timeval tv;
    gettimeofday(&tv, NULL);
    printf("Current time (gettimeofday): %ld seconds, %06ld microseconds\n",
           tv.tv_sec, tv.tv_usec);

    // Using clock_gettime() for nanosecond precision (more robust for benchmarking)
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        printf("Monotonic time (clock_gettime): %ld seconds, %09ld nanoseconds\n",
               ts.tv_sec, ts.tv_nsec);
    } else {
        perror("clock_gettime");
    }

    return 0;
}
```

#### **3.4. Temporary Files (File Tạm thời) ⏳**

Các chương trình thường cần tạo các file tạm thời để lưu trữ dữ liệu trong thời gian ngắn, ví dụ: file trung gian cho các phép tính, buffer dữ liệu. Điều quan trọng là phải tạo các file này một cách an toàn để tránh xung đột tên hoặc lỗ hổng bảo mật.

* **Vị trí:** Thường là trong thư mục `/tmp` hoặc `/var/tmp`. `/tmp` thường bị xóa khi khởi động lại hệ thống, còn `/var/tmp` có thể tồn tại qua các lần khởi động.
* **Hàm an toàn trong C:**
    * `int mkstemp(char *template);`: Tạo một file tạm thời duy nhất và trả về một file descriptor. Bạn phải cung cấp một chuỗi mẫu (ví dụ: `/tmp/myapp_XXXXXX`, `XXXXXX` sẽ được thay thế bằng các ký tự ngẫu nhiên). **Đây là phương pháp ưu tiên.**
    * `FILE *tmpfile(void);`: Tạo một file tạm thời duy nhất, mở nó ở chế độ nhị phân (`wb+`), và tự động xóa nó khi đóng hoặc khi chương trình kết thúc.
    * **Tránh dùng:** `tmpnam()`, `mktemp()` vì chúng không an toàn (có thể bị tấn công bằng cách đoán tên file).

**Ví dụ (C): Tạo file tạm thời với `mkstemp`**

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For close
#include <string.h> // For strcpy

int main() {
    char template[] = "/tmp/myprog_temp_XXXXXX"; // Template for the temporary file
    int fd;

    // mkstemp modifies the template string with the unique filename
    fd = mkstemp(template);
    if (fd == -1) {
        perror("mkstemp");
        return EXIT_FAILURE;
    }

    printf("Temporary file created: %s (fd: %d)\n", template, fd);

    // Write some data to the temporary file
    const char *data = "This is temporary data.";
    if (write(fd, data, strlen(data)) == -1) {
        perror("write");
        close(fd);
        unlink(template); // Clean up on error
        return EXIT_FAILURE;
    }

    close(fd); // Close the file descriptor

    // File still exists; you can unlink it immediately after opening
    // to ensure it's removed when the process exits or crashes.
    unlink(template); // This deletes the directory entry, but the file content
                      // remains accessible via 'fd' until 'fd' is closed.
                      // After close(fd), the file is actually deleted.

    printf("Temporary file unlinked (will be deleted when closed or program exits).\n");

    return EXIT_SUCCESS;
}
```

**Ví dụ (Python): Tạo file tạm thời với `tempfile` module**

```python
import tempfile
import os
import logging

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

def demonstrate_temp_files():
    logger.info("Demonstrating Temporary Files:")

    # Using NamedTemporaryFile
    # It creates a file, and by default, it will be deleted when closed
    with tempfile.NamedTemporaryFile(mode='w+', delete=True, encoding='utf-8') as temp_file:
        logger.info(f"Created named temporary file: {temp_file.name}")
        temp_file.write("Hello from a temporary file!\n")
        temp_file.flush() # Ensure data is written to disk
        temp_file.seek(0) # Rewind to beginning to read
        content = temp_file.read()
        logger.info(f"Content of temp file: {content.strip()}")
        logger.success("Named temporary file used and will be deleted automatically.")

    # Using TemporaryDirectory (useful for temporary groups of files)
    with tempfile.TemporaryDirectory() as temp_dir:
        logger.info(f"Created temporary directory: {temp_dir}")
        temp_file_path = os.path.join(temp_dir, "my_temp_data.txt")
        with open(temp_file_path, "w") as f:
            f.write("Data inside temp directory.")
        logger.info(f"File created inside temp directory: {temp_file_path}")
        logger.success("Temporary directory used and will be deleted automatically.")

if __name__ == "__main__":
    demonstrate_temp_files()
```

#### **3.5. User Information (Thông tin Người dùng) 🧑‍💻**

Các chương trình đôi khi cần biết thông tin về người dùng đang chạy chúng để cấu hình đường dẫn, quyền hạn, hoặc các cài đặt cá nhân.

* **Hàm trong C:**
    * `uid_t getuid(void);`: Trả về User ID (UID) thực tế của tiến trình.
    * `uid_t geteuid(void);`: Trả về Effective User ID (EUID) của tiến trình (quan trọng cho các chương trình `setuid`).
    * `char *getlogin(void);`: Trả về tên đăng nhập của người dùng điều khiển terminal.
    * `struct passwd *getpwnam(const char *name);`: Lấy thông tin người dùng từ `/etc/passwd` bằng tên đăng nhập.
    * `struct passwd *getpwuid(uid_t uid);`: Lấy thông tin người dùng từ `/etc/passwd` bằng UID.

**Ví dụ (C): Lấy thông tin người dùng**

```c
#include <stdio.h>
#include <unistd.h>  // For getuid, geteuid
#include <pwd.h>     // For getpwuid, getpwnam
#include <sys/types.h> // For uid_t, gid_t

int main() {
    uid_t uid = getuid();
    uid_t euid = geteuid();
    struct passwd *pw;

    printf("Real User ID (UID): %d\n", uid);
    printf("Effective User ID (EUID): %d\n", euid);

    pw = getpwuid(uid);
    if (pw) {
        printf("Username: %s\n", pw->pw_name);
        printf("Home directory: %s\n", pw->pw_dir);
        printf("Shell: %s\n", pw->pw_shell);
    } else {
        perror("getpwuid");
    }

    return 0;
}
```

**Ví dụ (Python): Lấy thông tin người dùng**

```python
import os
import pwd # Python specific module for user database
import logging

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

def demonstrate_user_info():
    logger.info("Demonstrating User Information access:")

    # Get UID and EUID
    real_uid = os.getuid()
    effective_uid = os.geteuid()
    logger.info(f"Real UID: {real_uid}")
    logger.info(f"Effective UID: {effective_uid}")

    # Get login name (from current terminal)
    try:
        login_name = os.getlogin()
        logger.info(f"Login Name: {login_name}")
    except OSError as e:
        logger.warning(f"Could not get login name (e.g., not connected to a terminal): {e}")

    # Get user info from UID
    try:
        user_info = pwd.getpwuid(real_uid)
        logger.info(f"Username (from UID): {user_info.pw_name}")
        logger.info(f"Home Directory: {user_info.pw_dir}")
        logger.info(f"User Shell: {user_info.pw_shell}")
    except KeyError:
        logger.error(f"User with UID {real_uid} not found in system database.")
    except Exception as e:
        logger.error(f"An error occurred getting user info: {e}")

if __name__ == "__main__":
    demonstrate_user_info()
```

#### **3.6. Host Information (Thông tin Máy chủ) 🖥️**

Trong môi trường mạng hoặc hệ thống phân tán, các chương trình cần xác định máy chủ mà chúng đang chạy để cấu hình mạng, ghi log, hoặc cung cấp thông tin định danh.

* **Hàm trong C:**
    * `int gethostname(char *name, size_t len);`: Lấy tên máy chủ của hệ thống.
    * `int uname(struct utsname *buf);`: Cung cấp thông tin chi tiết hơn về hệ thống (tên OS, tên máy chủ, phiên bản kernel, kiến trúc phần cứng).

**Ví dụ (C): Lấy thông tin máy chủ**

```c
#include <stdio.h>
#include <unistd.h> // For gethostname
#include <sys/utsname.h> // For uname
#include <string.h> // For strlen

int main() {
    char hostname[256];
    struct utsname os_info;

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("Hostname: %s\n", hostname);
    } else {
        perror("gethostname");
    }

    if (uname(&os_info) == 0) {
        printf("OS Name: %s\n", os_info.sysname);
        printf("Node Name (Hostname): %s\n", os_info.nodename);
        printf("Kernel Release: %s\n", os_info.release);
        printf("Kernel Version: %s\n", os_info.version);
        printf("Machine Architecture: %s\n", os_info.machine);
    } else {
        perror("uname");
    }

    return 0;
}
```

**Ví dụ (Python): Lấy thông tin máy chủ**

```python
import socket # For gethostname
import platform # For uname
import logging

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

def demonstrate_host_info():
    logger.info("Demonstrating Host Information access:")

    # Get hostname
    try:
        hostname = socket.gethostname()
        logger.info(f"Hostname: {hostname}")
    except Exception as e:
        logger.error(f"Error getting hostname: {e}")

    # Get more detailed OS info (like uname)
    uname_info = platform.uname()
    logger.info(f"OS Name: {uname_info.system}")
    logger.info(f"Node Name (Hostname): {uname_info.node}")
    logger.info(f"Kernel Release: {uname_info.release}")
    logger.info(f"Kernel Version: {uname_info.version}")
    logger.info(f"Machine Architecture: {uname_info.machine}")

if __name__ == "__main__":
    demonstrate_host_info()
```

#### **3.7. Logging (Ghi Log) 📜**

Ghi log là một kỹ thuật thiết yếu để gỡ lỗi (debugging), giám sát (monitoring), và phân tích hoạt động của chương trình. Trong môi trường nhúng, log là nguồn thông tin chính khi bạn không có giao diện đồ họa.

* **Các cấp độ Log (Log Levels):** Hầu hết các hệ thống log đều hỗ trợ các cấp độ để phân loại mức độ nghiêm trọng của thông báo:
    * **DEBUG:** Thông tin chi tiết để gỡ lỗi.
    * **INFO:** Thông tin chung về hoạt động bình thường.
    * **WARNING:** Cảnh báo về các vấn đề tiềm ẩn nhưng chương trình vẫn có thể tiếp tục.
    * **ERROR:** Lỗi nghiêm trọng khiến một phần chức năng thất bại.
    * **CRITICAL:** Lỗi nghiêm trọng có thể khiến chương trình dừng hoạt động.
* **Phương pháp ghi log:**
    * **`printf`/`fprintf`:** Ghi ra `stdout` (output chuẩn) hoặc `stderr` (output lỗi chuẩn). Đơn giản nhưng không có cấu trúc.
    * **Ghi vào file:** Bạn có thể mở một file và ghi log trực tiếp vào đó.
    * **Syslog:** Hệ thống log tiêu chuẩn của Unix/Linux. Các thông báo từ nhiều ứng dụng và dịch vụ khác nhau được tập trung vào một nơi (thường là `/var/log`).
        * **Trong C:** Sử dụng các hàm `openlog()`, `syslog()`, `closelog()`.
        * **Trong Python:** Module `logging` có thể được cấu hình để gửi log đến syslog.

**Ví dụ (C): Sử dụng `syslog`**

```c
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h> // For syslog functions

int main() {
    // openlog(ident, option, facility)
    // ident: Tiền tố cho mỗi thông báo log (tên chương trình)
    // option: LOG_PID (thêm PID vào log), LOG_CONS (ghi ra console nếu có lỗi gửi syslog)
    // facility: LOG_USER (mặc định), LOG_DAEMON, LOG_LOCAL0-7, v.v.
    openlog("my_linux_app", LOG_PID | LOG_CONS, LOG_USER);

    syslog(LOG_INFO, "Application started successfully.");
    syslog(LOG_WARNING, "Configuration file not found, using default settings.");
    syslog(LOG_ERR, "Failed to connect to hardware device: %s", "Device not ready");

    // Simulate an error requiring critical attention
    int critical_error_code = 101;
    syslog(LOG_CRIT, "Critical error %d: System shutting down.", critical_error_code);

    closelog(); // Close syslog connection

    printf("Log messages sent to syslog. Check /var/log/syslog or journalctl.\n");

    return EXIT_SUCCESS;
}
```

Để xem log này, bạn có thể dùng `journalctl -f -t my_linux_app` hoặc `tail -f /var/log/syslog` (tùy thuộc vào cấu hình hệ thống của bạn).

**Ví dụ (Python): Sử dụng `logging` module (theo yêu cầu Loguru style)**

```python
# Giả định đã import và cấu hình logger, ví dụ: from loguru import logger
# (Trong thực tế, bạn sẽ cấu hình logger cụ thể nếu không dùng loguru)
# Ở đây ta mô phỏng output tương tự loguru với logging chuẩn
import logging

# Cấu hình logging để output ra console với định dạng tương tự loguru
# Trong môi trường sản phẩm, bạn có thể cấu hình gửi đến syslog, file, v.v.
logging.basicConfig(
    level=logging.DEBUG, # Cấp độ thấp nhất để hiển thị tất cả
    format='%(levelname)s: %(message)s'
)

# Tạo một logger instance
logger = logging.getLogger(__name__)

# Hàm mô phỏng các cấp độ log của loguru
def loguru_style_print(level, message):
    if level == 'TRACE':
        logger.debug(message) # logging.DEBUG is the closest standard level
    elif level == 'DEBUG':
        logger.debug(message)
    elif level == 'INFO':
        logger.info(message)
    elif level == 'SUCCESS':
        logger.info(f"SUCCESS: {message}") # Add SUCCESS prefix for distinction
    elif level == 'WARNING':
        logger.warning(message)
    elif level == 'ERROR':
        logger.error(message)
    elif level == 'CRITICAL':
        logger.critical(message)

# Sử dụng các cấp độ log
loguru_style_print('TRACE', 'A trace message for deep debugging.')
loguru_style_print('DEBUG', 'Some debug message for internal state.')
loguru_style_print('INFO', 'Info message: Application is running.')
loguru_style_print('SUCCESS', 'Some successful operation completed successfully.')
loguru_style_print('WARNING', 'Something requires attention: Low disk space.')
loguru_style_print('ERROR', 'Something errored out: Failed to open file.')
loguru_style_print('CRITICAL', 'Critical error occurred! Application is shutting down.')

# Example of a function that logs
def perform_task(value):
    try:
        loguru_style_print('INFO', f"Starting task with value: {value}")
        if value < 0:
            raise ValueError("Value cannot be negative.")
        result = value * 2
        loguru_style_print('SUCCESS', f"Task completed, result: {result}")
        return result
    except ValueError as e:
        loguru_style_print('ERROR', f"Task failed due to input error: {e}")
        return None
    except Exception as e:
        loguru_style_print('CRITICAL', f"An unexpected critical error during task: {e}")
        return None

if __name__ == "__main__":
    loguru_style_print('INFO', "--- Starting Logging Demonstration ---")
    perform_task(10)
    perform_task(-5)
    loguru_style_print('INFO', "--- Logging Demonstration Finished ---")
```

#### **3.8. Resources and Limits (Tài nguyên và Giới hạn) 📈**

Kernel Linux cung cấp cơ chế để đặt **giới hạn tài nguyên** (resource limits) cho các tiến trình. Điều này rất quan trọng để ngăn chặn một tiến trình độc hại hoặc bị lỗi chiếm dụng quá nhiều tài nguyên hệ thống (CPU, bộ nhớ, file descriptor), gây ảnh hưởng đến sự ổn định hoặc hiệu suất của các tiến trình khác.

* **Các giới hạn phổ biến:**

    * `RLIMIT_CORE`: Kích thước tối đa của core file.
    * `RLIMIT_CPU`: Thời gian CPU tối đa mà tiến trình có thể sử dụng (giây).
    * `RLIMIT_DATA`: Kích thước tối đa của phân đoạn dữ liệu.
    * `RLIMIT_FSIZE`: Kích thước tối đa của file có thể tạo.
    * `RLIMIT_NOFILE`: Số lượng file descriptor tối đa mà tiến trình có thể mở đồng thời.
    * `RLIMIT_AS`: Tổng không gian địa chỉ ảo tối đa (kích thước bộ nhớ ảo).
    * `RLIMIT_NPROC`: Số lượng tiến trình con tối đa mà một người dùng có thể tạo.

* **Kiểm tra và đặt giới hạn:**

    * **Trong Shell:** Lệnh `ulimit -a` để xem tất cả giới hạn. `ulimit -n 1024` để đặt giới hạn file descriptor.
    * **Trong C:** Hàm `int getrlimit(int resource, struct rlimit *rlim);` để lấy giới hạn hiện tại và `int setrlimit(int resource, const struct rlimit *rlim);` để đặt giới hạn.
        * `struct rlimit` có hai trường: `rlim_cur` (giới hạn mềm - soft limit) và `rlim_max` (giới hạn cứng - hard limit). Tiến trình có thể tự tăng `rlim_cur` lên tới `rlim_max`. Chỉ root mới có thể tăng `rlim_max`.

**Ví dụ (C): Kiểm tra và thử thay đổi giới hạn file descriptor**

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h> // For getrlimit, setrlimit

int main() {
    struct rlimit rlim;

    // Get current file descriptor limits
    if (getrlimit(RLIMIT_NOFILE, &rlim) == 0) {
        printf("Current RLIMIT_NOFILE (max open files):\n");
        printf("  Soft limit: %ld\n", (long)rlim.rlim_cur);
        printf("  Hard limit: %ld\n", (long)rlim.rlim_max);
    } else {
        perror("getrlimit RLIMIT_NOFILE");
        return EXIT_FAILURE;
    }

    // Try to increase soft limit for file descriptors (to current hard limit)
    rlim.rlim_cur = rlim.rlim_max; // Attempt to set soft limit to hard limit
    if (setrlimit(RLIMIT_NOFILE, &rlim) == 0) {
        printf("\nSuccessfully increased soft limit for RLIMIT_NOFILE to %ld\n", (long)rlim.rlim_cur);
    } else {
        perror("setrlimit RLIMIT_NOFILE");
        printf("Could not increase soft limit. Maybe already at max or requires root privileges.\n");
    }

    // Get limits again to confirm change
    if (getrlimit(RLIMIT_NOFILE, &rlim) == 0) {
        printf("New RLIMIT_NOFILE:\n");
        printf("  Soft limit: %ld\n", (long)rlim.rlim_cur);
        printf("  Hard limit: %ld\n", (long)rlim.rlim_max);
    }

    // Example of trying to set a value beyond hard limit (will fail unless root)
    rlim.rlim_cur = rlim.rlim_max + 100; // Try to set above hard limit
    if (setrlimit(RLIMIT_NOFILE, &rlim) == 0) {
        printf("\nERROR: Unexpectedly set soft limit beyond hard limit.\n");
    } else {
        printf("\nFailed to set soft limit beyond hard limit, as expected (need root).\n");
        perror("setrlimit (attempt to exceed hard limit)");
    }

    return EXIT_SUCCESS;
}
```

**Tầm quan trọng trong Embedded Systems:**
Trong các hệ thống nhúng, tài nguyên rất hạn chế. Việc hiểu và quản lý giới hạn tài nguyên là cực kỳ quan trọng để:

* **Ngăn chặn tràn bộ nhớ:** Đảm bảo ứng dụng không yêu cầu quá nhiều RAM hoặc không gian địa chỉ ảo.
* **Tránh "resource exhaustion":** Đảm bảo chương trình không mở quá nhiều file hoặc tạo quá nhiều tiến trình, làm cạn kiệt tài nguyên của hệ thống nhỏ.
* **Đảm bảo độ tin cậy:** Giúp hệ thống hoạt động ổn định và dự đoán được trong các điều kiện tải nặng.

-----

### **Kết luận 🏁**

Việc hiểu rõ về **tham số chương trình**, **biến môi trường**, và cách truy cập các thông tin **thời gian, file tạm, thông tin người dùng/máy chủ**, cùng với việc nắm bắt các **giới hạn tài nguyên** là nền tảng vững chắc cho bất kỳ ai lập trình trên Linux, đặc biệt là trong lĩnh vực phần mềm nhúng. Bạn có thể xây dựng các ứng dụng linh hoạt, mạnh mẽ, và hiệu quả hơn.

Bạn có muốn đi sâu hơn vào một trong những hàm hoặc khái niệm này, hoặc bạn muốn chuyển sang một chủ đề mới trong lập trình Linux không? 🤔