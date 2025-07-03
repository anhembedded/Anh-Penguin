

### **Module 8: Tổng hợp và Ứng dụng 🧩**

#### **8.1. Ôn tập và Kết nối Kiến thức 🔗**

Trước khi đi vào các câu hỏi và bài tập, hãy cùng điểm lại các chủ đề chính và mối liên hệ giữa chúng:

* **Module 1: Tham số Chương trình (Program Arguments)**
  * Cách chương trình nhận input từ dòng lệnh (`argc`, `argv`, `getopt`, `getopt_long`).
  * **Kết nối:** Đây là cách đầu tiên và trực tiếp nhất để người dùng cấu hình chương trình của bạn.
* **Module 2: Biến Môi trường (Environment Variables)**
  * Cách chương trình truy cập và thay đổi các biến môi trường (`getenv`, `putenv`, `setenv`, `unsetenv`, `environ`).
  * **Kết nối:** Cung cấp một lớp cấu hình linh hoạt hơn, được kế thừa bởi các tiến trình con, hữu ích cho các cài đặt hệ thống hoặc thư viện.
* **Module 3: Thời gian và Ngày tháng (Time and Date)**
  * Làm việc với thời gian Epoch (`time_t`, `time`, `difftime`).
  * Chuyển đổi sang cấu trúc `tm` (`gmtime`, `localtime`, `mktime`).
  * Định dạng và phân tích chuỗi thời gian (`strftime`, `strptime`).
  * **Kết nối:** Cung cấp dấu thời gian cho log, hẹn giờ, đo lường hiệu suất.
* **Module 4: File Tạm thời (Temporary Files)**
  * Tạo và quản lý file tạm thời an toàn (`tmpfile`, `mkstemp`).
  * **Kết nối:** Đảm bảo sử dụng không gian lưu trữ hiệu quả và an toàn, tránh xung đột tên file.
* **Module 5: Thông tin Người dùng và Máy chủ (User and Host Information)**
  * Lấy thông tin về người dùng (`getuid`, `geteuid`, `getlogin`, `getpwuid`, `getpwnam`).
  * Lấy thông tin về máy chủ (`gethostname`, `uname`, `gethostid`).
  * **Kết nối:** Giúp chương trình tự nhận biết môi trường, tùy chỉnh hành vi dựa trên người dùng hoặc phần cứng.
* **Module 6: Ghi Log (Logging)**
  * Sử dụng hệ thống Syslog (`syslog`, `openlog`, `closelog`, `setlogmask`) để ghi thông báo.
  * **Kết nối:** Công cụ không thể thiếu để gỡ lỗi và giám sát các ứng dụng chạy nền hoặc không có giao diện.
* **Module 7: Tài nguyên và Giới hạn (Resources and Limits)**
  * Kiểm tra và đặt giới hạn tài nguyên (`getrlimit`, `setrlimit`).
  * Quản lý độ ưu tiên tiến trình (`getpriority`, `setpriority`).
  * Đo lường mức sử dụng CPU (`getrusage`).
  * **Kết nối:** Đảm bảo ứng dụng hoạt động ổn định, hiệu quả, và không gây quá tải hệ thống, đặc biệt quan trọng trong môi trường nhúng.

---

#### **8.2. Câu hỏi Tổng hợp và Tình huống ❓**

1. Tình huống (Cấu hình ứng dụng nhúng):
   Bạn đang phát triển một daemon (ứng dụng chạy nền) cho một thiết bị nhúng. Daemon này cần:

   * Đọc một file cấu hình tùy chỉnh (đường dẫn được cung cấp qua dòng lệnh hoặc biến môi trường).
   * Nếu không có file cấu hình, sử dụng các giá trị mặc định.
   * Ghi log các sự kiện quan trọng vào syslog.
   * Đảm bảo rằng nó không tiêu thụ quá nhiều bộ nhớ hoặc mở quá nhiều file.
   * Khi nhận tín hiệu `SIGHUP`, nó cần tải lại cấu hình mà không cần khởi động lại.
   * Khi nhận `SIGTERM`, nó cần thoát một cách duyên dáng.

   Hãy mô tả các hàm và khái niệm bạn sẽ sử dụng từ các Module khác nhau để giải quyết từng yêu cầu trên.
2. **Phân biệt và Ứng dụng:**

   * Khi nào bạn sẽ ưu tiên dùng `getopt_long()` thay vì chỉ đọc trực tiếp `argv[]`?
   * Nêu một trường hợp bạn sẽ dùng `setenv()` để cấu hình chương trình con thay vì truyền tham số dòng lệnh.
   * Bạn cần ghi log các sự kiện với dấu thời gian chính xác đến mili giây. Bạn sẽ dùng hàm nào từ `<time.h>` và tại sao?
   * Nếu ứng dụng của bạn cần tạo một file tạm thời để lưu trữ dữ liệu nhạy cảm trong thời gian ngắn, bạn sẽ dùng hàm nào (`tmpfile()` hay `mkstemp()`) và tại sao? Làm thế nào để đảm bảo file đó được xóa ngay cả khi chương trình crash?
   * Bạn muốn biết chương trình của mình đang chạy trên kiến trúc phần cứng nào (ví dụ: `arm`, `x86_64`). Bạn sẽ dùng hàm nào?
   * Khi nào bạn sẽ dùng `syslog()` với `LOG_DEBUG` và khi nào với `LOG_CRIT`?
3. **Tối ưu và Gỡ lỗi:**

   * Một ứng dụng nhúng của bạn thỉnh thoảng bị treo và bạn nghi ngờ có lỗi rò rỉ bộ nhớ hoặc file descriptor. Bạn sẽ sử dụng những công cụ và kiến thức nào từ giáo trình này để chẩn đoán vấn đề?
   * Bạn có một tác vụ tính toán nặng cần chạy nền mà không làm ảnh hưởng đến các tác vụ quan trọng khác. Bạn sẽ sử dụng những khái niệm nào để quản lý độ ưu tiên của tác vụ này?
   * Giải thích cách `umask` ảnh hưởng đến quyền hạn của file mới được tạo bởi `open()` hoặc `mkdir()`.

---

#### **8.3. Bài tập Thực hành Tổng hợp ✍️**

Bài tập này sẽ yêu cầu bạn kết hợp kiến thức từ nhiều Module để xây dựng một ứng dụng thực tế hơn.

**Bài tập: Daemon Giám sát Tài nguyên Đơn giản**

Viết một chương trình C++ (`resource_monitor_daemon.cpp`) hoạt động như một daemon giám sát tài nguyên.

**Yêu cầu chức năng:**

1. **Chuyển đổi thành Daemon:**
   * Sử dụng kỹ thuật `fork()` hai lần để chương trình trở thành một daemon chạy nền, không có terminal điều khiển.
   * Đảm bảo đóng tất cả các file descriptor chuẩn (`stdin`, `stdout`, `stderr`) sau khi trở thành daemon.
2. **Xử lý Tham số Dòng lệnh:**
   * Sử dụng `getopt_long()` để xử lý các tùy chọn sau:
     * `-i <interval>` hoặc `--interval <interval>`: Bắt buộc đối số (số nguyên). Chỉ định khoảng thời gian (giây) giữa các lần ghi log giám sát. Mặc định là 5 giây.
     * `-l <logfile>` hoặc `--log-file <logfile>`: Bắt buộc đối số (chuỗi). Chỉ định đường dẫn đến file log riêng của daemon (thay vì syslog). Nếu không có tùy chọn này, daemon sẽ ghi log vào syslog.
     * `-p <pid_file>` hoặc `--pid-file <pid_file>`: Bắt buộc đối số (chuỗi). Chỉ định đường dẫn đến file PID. Daemon sẽ ghi PID của nó vào file này khi khởi động và xóa khi thoát.
     * `-h` hoặc `--help`: In ra hướng dẫn sử dụng và thoát.
3. **Ghi Log:**
   * Nếu tùy chọn `--log-file` được cung cấp:
     * Mở file log đó ở chế độ append (`O_WRONLY | O_CREAT | O_APPEND`).
     * Ghi tất cả các thông báo log vào file này.
     * Đảm bảo dữ liệu được flush xuống đĩa ngay lập tức sau mỗi lần ghi.
     * Sử dụng `fprintf()` hoặc `write()` để ghi log.
   * Nếu tùy chọn `--log-file` không được cung cấp:
     * Sử dụng hệ thống Syslog (`syslog()`, `openlog()`, `closelog()`).
     * Sử dụng định danh là "resource_monitor".
     * Thêm `LOG_PID` vào `openlog()`.
4. **Giám sát Tài nguyên:**
   * Trong vòng lặp chính của daemon, cứ mỗi `<interval>` giây, thực hiện các bước sau:
     * Lấy thời gian hiện tại và định dạng nó thành chuỗi `[YYYY-MM-DD HH:MM:SS]`.
     * Đọc thông tin về mức sử dụng bộ nhớ RAM (tổng, đã dùng, trống) từ `/proc/meminfo`.
     * Đọc thông tin về mức sử dụng CPU (user time, system time) từ `/proc/stat` (hoặc `getrusage()` cho tiến trình daemon đó, nhưng `/proc/stat` cung cấp thông tin toàn hệ thống).
     * Đọc số lượng file descriptor đang mở của chính daemon từ `/proc/self/fd/`.
     * Ghi tất cả thông tin này vào file log (hoặc syslog) với mức độ `LOG_INFO`.
5. **Xử lý Tín hiệu:**
   * Cài đặt trình xử lý tín hiệu cho `SIGTERM` và `SIGHUP` bằng `sigaction()`.
   * Khi nhận `SIGTERM`: Daemon phải thoát một cách duyên dáng (ghi thông báo cuối cùng, đóng file log/syslog, xóa file PID).
   * Khi nhận `SIGHUP`: Daemon phải tải lại cấu hình (nếu có) và ghi một thông báo vào log. (Ở đây, bạn có thể chỉ cần ghi một thông báo "Reloading configuration" vào log).
   * Bỏ qua tín hiệu `SIGCHLD` để tránh zombie processes.
6. **Quản lý File PID:**
   * Khi khởi động, daemon ghi PID của nó vào file được chỉ định bởi `--pid-file`.
   * Khi thoát (do `SIGTERM`), daemon xóa file PID này.

**Cấu trúc Code gợi ý:**

**C++**

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>      // For std::ofstream (file log)
#include <chrono>       // For std::chrono::seconds, std::this_thread::sleep_for
#include <thread>       // For std::this_thread
#include <ctime>        // For time, localtime, strftime
#include <cstdio>       // For sprintf
#include <cstdlib>      // For EXIT_SUCCESS, EXIT_FAILURE, getenv
#include <cstring>      // For strlen, strcpy, strcmp, strerror
#include <unistd.h>     // For fork, setsid, chdir, close, getpid, readlink, unlink, sleep
#include <fcntl.h>      // For open, O_WRONLY, O_CREAT, O_APPEND
#include <sys/stat.h>   // For stat (for /proc/self/fd), umask
#include <syslog.h>     // For syslog, openlog, closelog, LOG_*, setlogmask
#include <signal.h>     // For sigaction, sigemptyset, SIGTERM, SIGHUP, SIGCHLD, SIG_IGN
#include <getopt.h>     // For getopt_long, struct option
#include <errno.h>      // For errno
#include <dirent.h>     // For opendir, readdir, closedir (for counting fds)
#include <limits.h>     // For PATH_MAX (for readlink)

// --- Global variables for daemon control and logging ---
static volatile sig_atomic_t daemon_running = 1; // Dùng volatile và sig_atomic_t cho biến cờ tín hiệu
static std::string log_file_path = "";
static std::ofstream file_log_stream;
static std::string pid_file_path = "";
static int monitor_interval_seconds = 5; // Mặc định 5 giây

// --- Logger function (unified interface) ---
void daemon_log(int level, const std::string& message) {
    if (!log_file_path.empty()) {
        // Ghi vào file log
        if (!file_log_stream.is_open()) { // Mở lại nếu bị đóng (ví dụ sau SIGHUP)
            file_log_stream.open(log_file_path, std::ios_base::app);
        }
        if (file_log_stream.is_open()) {
            time_t rawtime;
            struct tm *timeinfo;
            char time_buffer[80];
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(time_buffer, sizeof(time_buffer), "[%Y-%m-%d %H:%M:%S]", timeinfo);

            file_log_stream << time_buffer << " " << message << std::endl;
            file_log_stream.flush(); // Flush ngay lập tức
        } else {
            // Fallback to syslog if file log fails after re-opening
            syslog(LOG_ERR, "Failed to write to log file %s: %m", log_file_path.c_str());
        }
    } else {
        // Ghi vào syslog
        syslog(level, "%s", message.c_str());
    }
}

// --- Signal handler ---
void signal_handler(int sig) {
    switch (sig) {
        case SIGTERM:
            daemon_running = 0; // Yêu cầu daemon thoát
            daemon_log(LOG_INFO, "Received SIGTERM. Shutting down gracefully.");
            break;
        case SIGHUP:
            // Tải lại cấu hình (ở đây chỉ là thông báo)
            daemon_log(LOG_INFO, "Received SIGHUP. Reloading configuration (simulated).");
            // Trong thực tế, bạn sẽ gọi một hàm để đọc lại config
            break;
        case SIGCHLD:
            // Bỏ qua SIGCHLD để tránh zombie processes (đã được set SIG_IGN)
            break;
        default:
            daemon_log(LOG_WARNING, "Received unhandled signal: " + std::to_string(sig));
            break;
    }
}

// --- Hàm chuyển đổi thành daemon ---
void daemonize() {
    pid_t pid;

    // 1. Fork lần 1: Thoát tiến trình cha, để tiến trình con chạy nền
    pid = fork();
    if (pid < 0) {
        std::cerr << "ERROR   : Fork failed (daemonize step 1): " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); // Tiến trình cha thoát
    }

    // 2. Tạo phiên mới và trở thành leader của phiên
    if (setsid() < 0) {
        std::cerr << "ERROR   : setsid failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // 3. Fork lần 2: Đảm bảo không có terminal điều khiển
    pid = fork();
    if (pid < 0) {
        std::cerr << "ERROR   : Fork failed (daemonize step 2): " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); // Tiến trình cha thứ hai thoát
    }

    // 4. Đặt thư mục làm việc hiện tại là thư mục gốc
    if (chdir("/") < 0) {
        std::cerr << "ERROR   : chdir failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // 5. Đặt umask để kiểm soát quyền hạn file
    umask(0); // Cho phép đầy đủ quyền khi tạo file (sau đó sẽ chmod nếu cần)

    // 6. Đóng tất cả các file descriptor chuẩn
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Mở lại /dev/null cho stdin/stdout/stderr để tránh lỗi
    // int fd_null = open("/dev/null", O_RDWR);
    // dup2(fd_null, STDIN_FILENO);
    // dup2(fd_null, STDOUT_FILENO);
    // dup2(fd_null, STDERR_FILENO);
    // if (fd_null > STDERR_FILENO) {
    //     close(fd_null);
    // }
}

// --- Hàm đọc thông tin tài nguyên ---
void collect_resource_info() {
    std::string info_message;
    char buffer[256];

    // Đọc thông tin bộ nhớ từ /proc/meminfo
    std::ifstream mem_file("/proc/meminfo");
    if (mem_file.is_open()) {
        long total_mem = 0, free_mem = 0, available_mem = 0;
        std::string line;
        while (std::getline(mem_file, line)) {
            if (line.rfind("MemTotal:", 0) == 0) {
                sscanf(line.c_str(), "MemTotal: %ld kB", &total_mem);
            } else if (line.rfind("MemFree:", 0) == 0) {
                sscanf(line.c_str(), "MemFree: %ld kB", &free_mem);
            } else if (line.rfind("MemAvailable:", 0) == 0) {
                sscanf(line.c_str(), "MemAvailable: %ld kB", &available_mem);
            }
        }
        mem_file.close();
        info_message += "Mem: " + std::to_string(total_mem) + "KB Total, " + std::to_string(free_mem) + "KB Free, " + std::to_string(available_mem) + "KB Avail. ";
    } else {
        info_message += "Mem: N/A. ";
    }

    // Đọc thông tin CPU usage từ /proc/stat
    std::ifstream stat_file("/proc/stat");
    if (stat_file.is_open()) {
        std::string cpu_line;
        std::getline(stat_file, cpu_line); // Đọc dòng cpu tổng
        long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
        sscanf(cpu_line.c_str(), "cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
        stat_file.close();
        long total_cpu_time = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
        long idle_time = idle;
        info_message += "CPU: " + std::to_string(user) + "us, " + std::to_string(system) + "sy, " + std::to_string(idle) + "id. ";
    } else {
        info_message += "CPU: N/A. ";
    }

    // Đếm số lượng file descriptor đang mở
    int open_fds = 0;
    std::string fd_path = "/proc/self/fd";
    DIR *dp;
    struct dirent *entry;
    if ((dp = opendir(fd_path.c_str())) != NULL) {
        while ((entry = readdir(dp)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                open_fds++;
            }
        }
        closedir(dp);
        info_message += "FDs: " + std::to_string(open_fds) + ".";
    } else {
        info_message += "FDs: N/A. ";
    }
  
    daemon_log(LOG_INFO, "Resource Status: " + info_message);
}

// --- Hàm chính của Daemon ---
int main(int argc, char *argv[]) {
    // --- 1. Xử lý tham số dòng lệnh ---
    static struct option long_options[] = {
        {"interval", required_argument, nullptr, 'i'},
        {"log-file", required_argument, nullptr, 'l'},
        {"pid-file", required_argument, nullptr, 'p'},
        {"help",     no_argument,       nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };

    int opt;
    int long_index = 0;
    while ((opt = getopt_long(argc, argv, "i:l:p:h", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'i':
                monitor_interval_seconds = std::stoi(optarg);
                if (monitor_interval_seconds <= 0) {
                    std::cerr << "ERROR   : Interval must be positive." << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 'l':
                log_file_path = optarg;
                break;
            case 'p':
                pid_file_path = optarg;
                break;
            case 'h':
                std::cout << "Usage: " << argv[0] << " [OPTIONS]" << std::endl;
                std::cout << "  -i, --interval <seconds>  Monitoring interval in seconds (default: 5)" << std::endl;
                std::cout << "  -l, --log-file <path>     Path to custom log file (default: syslog)" << std::endl;
                std::cout << "  -p, --pid-file <path>     Path to PID file" << std::endl;
                std::cout << "  -h, --help                Show this help message" << std::endl;
                exit(EXIT_SUCCESS);
            case '?':
                std::cerr << "ERROR   : Unknown option or missing argument." << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    // --- 2. Chuyển đổi thành Daemon ---
    daemonize();

    // --- 3. Cấu hình Logging ---
    if (!log_file_path.empty()) {
        file_log_stream.open(log_file_path, std::ios_base::app);
        if (!file_log_stream.is_open()) {
            // Fallback to stderr if custom log file cannot be opened
            std::cerr << "ERROR   : Failed to open custom log file " << log_file_path << ": " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        daemon_log(LOG_INFO, "Daemon started. Logging to custom file: " + log_file_path);
    } else {
        openlog("resource_monitor", LOG_PID | LOG_CONS, LOG_DAEMON); // LOG_DAEMON là phù hợp cho daemon
        daemon_log(LOG_INFO, "Daemon started. Logging to syslog.");
    }

    // --- 4. Ghi PID vào file ---
    if (!pid_file_path.empty()) {
        std::ofstream pid_file(pid_file_path);
        if (pid_file.is_open()) {
            pid_file << getpid() << std::endl;
            pid_file.close();
            daemon_log(LOG_INFO, "PID written to file: " + pid_file_path);
        } else {
            daemon_log(LOG_ERR, "Failed to write PID to file " + pid_file_path + ": " + strerror(errno));
        }
    }

    // --- 5. Cấu hình Signal Handlers ---
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // Không SA_RESTART để các sleep() có thể bị ngắt
  
    // Bắt SIGTERM để thoát duyên dáng
    if (sigaction(SIGTERM, &sa, nullptr) == -1) {
        daemon_log(LOG_CRIT, "Failed to register SIGTERM handler: " + std::string(strerror(errno)));
        exit(EXIT_FAILURE);
    }
    // Bắt SIGHUP để tải lại cấu hình
    if (sigaction(SIGHUP, &sa, nullptr) == -1) {
        daemon_log(LOG_CRIT, "Failed to register SIGHUP handler: " + std::string(strerror(errno)));
        exit(EXIT_FAILURE);
    }
    // Bỏ qua SIGCHLD để tránh zombie processes
    struct sigaction sa_chld;
    sa_chld.sa_handler = SIG_IGN;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_NOCLDWAIT; // SA_NOCLDWAIT cũng giúp tránh zombie
    if (sigaction(SIGCHLD, &sa_chld, nullptr) == -1) {
        daemon_log(LOG_CRIT, "Failed to ignore SIGCHLD: " + std::string(strerror(errno)));
        exit(EXIT_FAILURE);
    }

    daemon_log(LOG_INFO, "Daemon running. Monitoring every " + std::to_string(monitor_interval_seconds) + " seconds.");

    // --- 6. Vòng lặp chính của Daemon ---
    while (daemon_running) {
        collect_resource_info();
        // Ngủ trong khoảng thời gian xác định, có thể bị ngắt bởi tín hiệu
        std::this_thread::sleep_for(std::chrono::seconds(monitor_interval_seconds));
    }

    // --- 7. Dọn dẹp trước khi thoát ---
    daemon_log(LOG_INFO, "Daemon shutting down.");
    if (!pid_file_path.empty()) {
        if (unlink(pid_file_path.c_str()) == -1) {
            daemon_log(LOG_ERR, "Failed to remove PID file " + pid_file_path + ": " + strerror(errno));
        } else {
            daemon_log(LOG_INFO, "PID file " + pid_file_path + " removed.");
        }
    }
    if (!log_file_path.empty() && file_log_stream.is_open()) {
        file_log_stream.close();
    } else {
        closelog();
    }

    return EXIT_SUCCESS;
}
```

**Cách Biên dịch:**

**Bash**

```
g++ resource_monitor_daemon.cpp -o resource_monitor_daemon -D_GNU_SOURCE -lstdc++ # -D_GNU_SOURCE nếu dùng getopt_long, -lstdc++ cho C++ features
```

**Cách Chạy và Kiểm tra:**

1. **Chạy với log file tùy chỉnh và PID file:**
   **Bash**

   ```
   ./resource_monitor_daemon -i 3 -l /tmp/my_daemon.log -p /tmp/resource_monitor.pid &
   ```

   * Kiểm tra log: `tail -f /tmp/my_daemon.log`
   * Kiểm tra PID file: `cat /tmp/resource_monitor.pid`
   * Kiểm tra tiến trình: `ps -ax | grep resource_monitor_daemon`
2. **Chạy với Syslog (mặc định) và PID file:**
   **Bash**

   ```
   ./resource_monitor_daemon -i 2 -p /tmp/resource_monitor.pid &
   ```

   * Kiểm tra log: `journalctl -f -t resource_monitor` (nếu dùng systemd) hoặc `tail -f /var/log/syslog`
   * Kiểm tra PID file: `cat /tmp/resource_monitor.pid`
3. **Dừng Daemon:**

   * Lấy PID từ file: `PID=$(cat /tmp/resource_monitor.pid)`
   * Gửi tín hiệu `SIGTERM`: `kill $PID`
   * Quan sát log để thấy daemon báo cáo tắt máy và file PID bị xóa.
4. **Tải lại cấu hình (SIGHUP):**

   * Trong khi daemon đang chạy, gửi `SIGHUP`: `kill -HUP $PID`
   * Quan sát log để thấy thông báo "Reloading configuration".

---

Đây là một bài tập khá phức tạp, kết hợp nhiều kiến thức đã học. Hãy dành thời gian để hiểu từng phần, biên dịch, chạy và kiểm tra kỹ lưỡng. Nó sẽ giúp bạn củng cố rất nhiều kỹ năng lập trình Linux và nhúng của mình!
