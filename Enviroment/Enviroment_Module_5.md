

### **Module 5: Thông tin Người dùng và Máy chủ (User and Host Information) 🧑‍💻🖥️**

#### **5.1. Thông tin Người dùng (User Information) 🧑‍💻**

Các chương trình thường cần biết ai đang chạy chúng để cấu hình đường dẫn, quyền hạn, hoặc các cài đặt cá nhân.

* **Lý thuyết:**

  * Mỗi người dùng trong Linux có một **User ID (UID)** duy nhất và một  **Group ID (GID)** . Các chương trình chạy dưới danh nghĩa của một UID/GID cụ thể.
  * **`getuid()`** : Lấy User ID (UID) thực tế của tiến trình. Đây là UID của người dùng đã khởi chạy chương trình.
    **C++**

  ```cpp
  #include <unistd.h>  // For getuid
  #include <sys/types.h> // For uid_t
  uid_t getuid(void);
  ```

  * **`geteuid()`** : Lấy Effective User ID (EUID) của tiến trình. EUID có thể khác UID thực tế nếu chương trình là một chương trình `setuid` (ví dụ: `sudo` hoặc `passwd`). EUID xác định quyền hạn thực tế của tiến trình.
    **C++**

  ```
  #include <unistd.h>  // For geteuid
  #include <sys/types.h> // For uid_t
  uid_t geteuid(void);
  ```

  * **`getlogin()`** : Trả về tên đăng nhập (login name) của người dùng điều khiển terminal mà chương trình đang chạy.
    **C++**

  ```cpp
  #include <unistd.h>  // For getlogin
  char *getlogin(void);
  ```

  * **Lưu ý:** Hàm này có thể trả về `NULL` nếu tiến trình không có terminal điều khiển (ví dụ: daemon chạy nền).
  * **Cơ sở dữ liệu người dùng (`/etc/passwd`)** : Thông tin chi tiết về người dùng (tên đăng nhập, UID, GID, thư mục home, shell mặc định) được lưu trữ trong file `/etc/passwd` (hoặc các cơ chế bảo mật hơn như shadow files).
  * **`getpwuid()` và `getpwnam()`** : Các hàm này truy vấn cơ sở dữ liệu người dùng để lấy thông tin chi tiết hơn về một người dùng, trả về một con trỏ tới cấu trúc `struct passwd`.
    **C++**

  ```cpp
  #include <pwd.h>     // For struct passwd, getpwuid, getpwnam
  #include <sys/types.h> // For uid_t

  struct passwd *getpwuid(uid_t uid);       // Lấy thông tin bằng UID
  struct passwd *getpwnam(const char *name); // Lấy thông tin bằng tên đăng nhập
  ```

  * **`struct passwd`** : Chứa các thành viên như `pw_name` (tên đăng nhập), `pw_uid` (UID), `pw_gid` (GID), `pw_dir` (thư mục home), `pw_shell` (shell mặc định).
  * **Lưu ý:** Cấu trúc `passwd` trả về là con trỏ tới bộ nhớ tĩnh, có thể bị ghi đè bởi các lệnh gọi tiếp theo.
* **Liên hệ Embedded Linux:**

  * Xác định người dùng đang chạy một ứng dụng hoặc script trên thiết bị nhúng.
  * Tùy chỉnh đường dẫn file cấu hình hoặc thư mục làm việc dựa trên thư mục home của người dùng.
  * Kiểm tra quyền hạn của tiến trình (dựa vào UID/EUID) để thực hiện các thao tác nhạy cảm.
* **Ví dụ (C++): `user_info.cpp` - Lấy thông tin người dùng**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <unistd.h>    // For getuid, geteuid, getlogin
  #include <sys/types.h> // For uid_t, gid_t
  #include <pwd.h>       // For struct passwd, getpwuid, getpwnam
  #include <grp.h>       // For getgrnam (optional, for group info)
  #include <cstdlib>     // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>     // For strerror
  #include <errno.h>     // For errno

  int main() {
      uid_t real_uid = getuid();
      uid_t effective_uid = geteuid();
      gid_t real_gid = getgid();
      gid_t effective_gid = getegid(); // getegid for effective group ID

      std::cout << "INFO    : Real User ID (UID): " << real_uid << std::endl;
      std::cout << "INFO    : Effective User ID (EUID): " << effective_uid << std::endl;
      std::cout << "INFO    : Real Group ID (GID): " << real_gid << std::endl;
      std::cout << "INFO    : Effective Group ID (EGID): " << effective_gid << std::endl;

      // Lấy tên đăng nhập
      char *login_name = getlogin();
      if (login_name) {
          std::cout << "INFO    : Login Name: " << login_name << std::endl;
      } else {
          std::cerr << "WARNING : Could not get login name (e.g., not connected to a terminal): " << strerror(errno) << std::endl;
      }

      // Lấy thông tin chi tiết từ cơ sở dữ liệu người dùng bằng UID
      std::cout << "\nINFO    : --- Detailed User Info (from getpwuid) ---" << std::endl;
      struct passwd *pw_entry = getpwuid(real_uid);
      if (pw_entry) {
          std::cout << "SUCCESS : Username: " << pw_entry->pw_name << std::endl;
          std::cout << "SUCCESS : UID: " << pw_entry->pw_uid << std::endl;
          std::cout << "SUCCESS : GID: " << pw_entry->pw_gid << std::endl;
          std::cout << "SUCCESS : Home Directory: " << pw_entry->pw_dir << std::endl;
          std::cout << "SUCCESS : Default Shell: " << pw_entry->pw_shell << std::endl;
          // pw_gecos có thể không có trên mọi hệ thống hoặc có tên khác
          // std::cout << "SUCCESS : Full Name (GECOS): " << pw_entry->pw_gecos << std::endl;
      } else {
          std::cerr << "ERROR   : Failed to get user info for UID " << real_uid << ": " << strerror(errno) << std::endl;
      }

      // Lấy thông tin chi tiết từ cơ sở dữ liệu người dùng bằng tên
      std::cout << "\nINFO    : --- Detailed User Info (from getpwnam for 'root') ---" << std::endl;
      pw_entry = getpwnam("root");
      if (pw_entry) {
          std::cout << "SUCCESS : Username: " << pw_entry->pw_name << std::endl;
          std::cout << "SUCCESS : UID: " << pw_entry->pw_uid << std::endl;
          std::cout << "SUCCESS : GID: " << pw_entry->pw_gid << std::endl;
      } else {
          std::cerr << "ERROR   : Failed to get user info for 'root': " << strerror(errno) << std::endl;
      }

      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ user_info.cpp -o user_info
    ./user_info
    ```
  * **Phân tích:** Quan sát UID/EUID và GID/EGID. Nếu bạn chạy chương trình này với `sudo ./user_info`, bạn sẽ thấy EUID trở thành 0 (root), trong khi UID vẫn là của bạn.

#### **5.2. Thông tin Máy chủ (Host Information) 🖥️**

Các chương trình đôi khi cần xác định máy chủ mà chúng đang chạy để cấu hình mạng, ghi log, hoặc cung cấp thông tin định danh.

* **Lý thuyết:**

  * **`gethostname()`** : Lấy tên máy chủ (hostname) của máy hiện tại.
    **C++**

  ```cpp
  #include <unistd.h>  // For gethostname
  int gethostname(char *name, size_t namelen);
  ```

  * `name`: Buffer để lưu tên máy chủ.
  * `namelen`: Kích thước của buffer. Tên sẽ được cắt bớt nếu quá dài.
  * Trả về `0` nếu thành công, `-1` nếu thất bại.
  * **`uname()`** : Lấy thông tin chi tiết hơn về hệ thống và phần cứng.
    **C++**

  ```cpp
  #include <sys/utsname.h> // For uname, struct utsname
  int uname(struct utsname *name);
  ```

  * `name`: Con trỏ tới cấu trúc `struct utsname` để lưu thông tin.
  * **`struct utsname`** : Chứa các thành viên (mảng `char[]`) như:
    * `sysname`: Tên hệ điều hành (ví dụ: "Linux").
    * `nodename`: Tên máy chủ (giống `gethostname()`).
    * `release`: Phiên bản phát hành của Kernel (ví dụ: "5.15.0-76-generic").
    * `version`: Phiên bản chi tiết của Kernel (bao gồm ngày biên dịch).
    * `machine`: Loại kiến trúc phần cứng (ví dụ: "x86_64", "aarch64").
  * Trả về số không âm nếu thành công, `-1` nếu thất bại.
  * **`gethostid()`** : Cố gắng trả về một ID duy nhất cho máy chủ.
    **C++**

  ```cpp
  #include <unistd.h> // For gethostid
  long gethostid(void);
  ```

  * **Lưu ý:** Trên Linux, giá trị này thường dựa trên địa chỉ IP và không đủ an toàn hoặc duy nhất cho mục đích cấp phép phần mềm.
* **Liên hệ Embedded Linux:**

  * Xác định phiên bản Kernel và kiến trúc phần cứng để tải module hoặc cấu hình ứng dụng phù hợp.
  * Sử dụng hostname để định danh thiết bị trong mạng hoặc trong các file log.
  * Tùy chỉnh hành vi ứng dụng dựa trên loại phần cứng cụ thể (ví dụ: tối ưu hóa cho CPU ARM).
* **Ví dụ (C++): `host_info.cpp` - Lấy thông tin máy chủ**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <unistd.h>    // For gethostname, gethostid
  #include <sys/utsname.h> // For uname, struct utsname
  #include <cstdlib>     // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>     // For strerror
  #include <errno.h>     // For errno

  int main() {
      char hostname_buffer[256];
      struct utsname uts_info;

      // --- Lấy tên máy chủ ---
      std::cout << "INFO    : Attempting to get hostname." << std::endl;
      if (gethostname(hostname_buffer, sizeof(hostname_buffer) - 1) == -1) {
          std::cerr << "ERROR   : Failed to get hostname: " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      hostname_buffer[sizeof(hostname_buffer) - 1] = '\0'; // Đảm bảo null-terminated
      std::cout << "SUCCESS : Computer hostname is: " << hostname_buffer << std::endl;

      // --- Lấy thông tin hệ thống chi tiết ---
      std::cout << "\nINFO    : Attempting to get detailed system information (uname)." << std::endl;
      if (uname(&uts_info) == -1) {
          std::cerr << "ERROR   : Failed to get uname info: " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : System Name (sysname): " << uts_info.sysname << std::endl;
      std::cout << "SUCCESS : Node Name (nodename): " << uts_info.nodename << std::endl;
      std::cout << "SUCCESS : Kernel Release (release): " << uts_info.release << std::endl;
      std::cout << "SUCCESS : Kernel Version (version): " << uts_info.version << std::endl;
      std::cout << "SUCCESS : Hardware Type (machine): " << uts_info.machine << std::endl;

      // --- Lấy Host ID (Lưu ý về tính duy nhất) ---
      std::cout << "\nINFO    : Attempting to get host ID." << std::endl;
      long host_id = gethostid();
      std::cout << "INFO    : Host ID: " << host_id << std::endl;
      std::cout << "WARNING : Note: Host ID on Linux is often based on IP address and may not be truly unique for licensing purposes." << std::endl;

      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ host_info.cpp -o host_info
    ./host_info
    ```
  * **Phân tích:** Quan sát các thông tin chi tiết về hệ điều hành và phần cứng của máy bạn.

---

### **Câu hỏi Tự đánh giá Module 5 🤔**

1. Phân biệt giữa Real User ID (UID) và Effective User ID (EUID). Tại sao một chương trình có thể có UID và EUID khác nhau?
2. Khi nào thì hàm `getlogin()` có thể trả về `NULL`?
3. Bạn muốn lấy thư mục home của người dùng hiện tại trong chương trình C++. Nêu hai cách để làm điều này (một cách dùng `getenv()`, một cách dùng `getpwuid()`). Cách nào đáng tin cậy hơn và tại sao?
4. Giải thích sự khác biệt giữa thông tin trả về bởi `gethostname()` và `uname()`.
5. Tại sao `gethostid()` không được khuyến nghị để sử dụng cho mục đích quản lý bản quyền phần mềm trên Linux?

---

### **Bài tập Thực hành Module 5 ✍️**

1. **Chương trình Kiểm tra Quyền hạn Người dùng:**
   * Viết một chương trình C++ (`check_user_privileges.cpp`) mà:
     * In ra UID, EUID, GID, EGID của tiến trình hiện tại.
     * Kiểm tra xem tiến trình có đang chạy với quyền root (EUID == 0) hay không. In ra thông báo "Running as root" hoặc "Running as normal user".
     * Nếu không phải root, thử tạo một file trong thư mục `/root/test_file.txt` (sẽ thất bại). In ra lỗi bằng `perror()`.
     * Nếu là root, thử tạo file đó (sẽ thành công).
   * **Thử thách:**
     * Chạy chương trình với quyền người dùng bình thường.
     * Chạy chương trình với `sudo ./check_user_privileges`. Quan sát sự khác biệt về EUID và khả năng tạo file.
2. **Chương trình Thông tin Hệ thống Chi tiết:**
   * Viết một chương trình C++ (`system_details.cpp`) mà:
     * Lấy và in ra tên máy chủ, loại hệ điều hành, phiên bản kernel, và kiến trúc phần cứng.
     * Thêm thông tin về tổng bộ nhớ RAM và bộ nhớ trống (từ `/proc/meminfo`).
     * Thêm thông tin về loại CPU và tốc độ (từ `/proc/cpuinfo`).
   * **Lưu ý:** Bạn sẽ cần sử dụng các kỹ thuật đọc file từ `/proc` mà chúng ta đã học trong Module 6 của giáo trình "Working with Files".
3. **Chương trình "Who Am I" nâng cao:**
   * Viết một chương trình C++ (`who_am_i.cpp`) mà:
     * In ra tên đăng nhập của người dùng.
     * In ra thư mục home của người dùng.
     * In ra shell mặc định của người dùng.
     * In ra tên đầy đủ của người dùng (nếu có thể lấy được từ `pw_gecos` trong `struct passwd`).
     * **Thử thách:** Nếu chương trình được chạy bởi một người dùng không tồn tại trong `/etc/passwd` (ví dụ: một người dùng tạm thời trong một số môi trường), xử lý lỗi và báo cáo.

---

Hãy dành thời gian để thực hành các bài tập này. Chúng sẽ giúp bạn làm quen với việc lấy thông tin về môi trường chạy của ứng dụng, rất quan trọng cho các ứng dụng thực tế. Khi bạn đã sẵn sàng, hãy cho tôi biết để chuyển sang  **Module 6: Ghi Log** !
