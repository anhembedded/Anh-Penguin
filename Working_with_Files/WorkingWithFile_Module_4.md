
### **Module 4: Quản lý File và Thư mục 📁**

Trong Module này, chúng ta sẽ học cách chương trình của bạn có thể tạo, xóa, thay đổi quyền hạn, chủ sở hữu, và điều hướng trong cấu trúc thư mục của Linux.

#### **4.1. Thay đổi Quyền hạn File: `chmod()` 🔒**

* **Lý thuyết:** Hàm **`chmod()`** (change mode) là một System Call dùng để thay đổi **quyền hạn truy cập (permissions)** của một file hoặc thư mục.

  * **Syntax:**
    **C++**

    ```cpp
    #include <sys/stat.h> // For chmod, mode_t
    int chmod(const char *path, mode_t mode);
    ```

  * **`path`** : Đường dẫn đến file hoặc thư mục.
  * **`mode`** : Giá trị số nguyên (thường là số octal) biểu diễn các quyền hạn mới. Giá trị này được tạo ra bằng cách kết hợp (bitwise OR `|`) các cờ quyền hạn được định nghĩa trong `<sys/stat.h>`:
  * **Quyền của Owner (USR):**

    * `S_IRUSR`: Read (Đọc)
    * `S_IWUSR`: Write (Ghi)
    * `S_IXUSR`: Execute (Thực thi)
  * **Quyền của Group (GRP):**

    * `S_IRGRP`: Read
    * `S_IWGRP`: Write
    * `S_IXGRP`: Execute
  * **Quyền của Others (OTH):**

    * `S_IROTH`: Read
    * `S_IWOTH`: Write
    * `S_IXOTH`: Execute
  * **Ví dụ Octal:** `0755` = `(S_IRUSR|S_IWUSR|S_IXUSR)` (7) `| (S_IRGRP|S_IXGRP)` (5) `| (S_IROTH|S_IXOTH)` (5)
  * **Lưu ý:**

    * Chỉ chủ sở hữu của file hoặc superuser (root) mới có quyền thay đổi quyền hạn.
    * Quyền hạn thực tế được áp dụng cũng bị ảnh hưởng bởi `umask` của người dùng (sẽ xem sau).
  * **Giá trị trả về:** `0` nếu thành công, `-1` nếu thất bại (và `errno` được thiết lập).
* **Liên hệ Embedded Linux:** Cực kỳ quan trọng để thiết lập quyền hạn cho các file cấu hình, log, hoặc các chương trình thực thi trên thiết bị nhúng. Ví dụ, đảm bảo một file log chỉ có thể được ghi bởi một daemon cụ thể, hoặc một script chỉ có thể được thực thi bởi root.
* **Ví dụ (C++): `chmod()`**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <sys/stat.h> // For chmod, S_IRUSR, etc.
  #include <stdio.h>    // For fopen, fclose
  #include <stdlib.h>   // For EXIT_FAILURE, EXIT_SUCCESS
  #include <errno.h>    // For errno
  #include <string.h>   // For strerror
  #include <unistd.h>   // For unlink

  int main() {
      const char *filename = "my_file_to_chmod.txt";

      // Tạo file trước
      FILE *fp = fopen(filename, "w");
      if (fp == NULL) {
          std::cerr << "ERROR   : Failed to create file: " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      fclose(fp);
      std::cout << "INFO    : Created file: " << filename << std::endl;

      // Thay đổi quyền hạn thành 0600 (rw-------)
      mode_t new_permissions = S_IRUSR | S_IWUSR; // Owner: read, write; Group, Others: no access
      std::cout << "INFO    : Changing permissions of " << filename << " to 0600 (rw-------)." << std::endl;
      if (chmod(filename, new_permissions) == -1) {
          std::cerr << "ERROR   : Failed to chmod " << filename << ": " << strerror(errno) << std::endl;
          unlink(filename);
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : Permissions changed. Use 'ls -l " << filename << "' to verify." << std::endl;

      // Thay đổi quyền hạn thành 0755 (rwxr-xr-x) - Thường dùng cho thư mục/executable
      new_permissions = S_IRUSR | S_IWUSR | S_IXUSR | // Owner: rwx
                        S_IRGRP | S_IXGRP |             // Group: r-x
                        S_IROTH | S_IXOTH;              // Others: r-x
      std::cout << "INFO    : Changing permissions of " << filename << " to 0755 (rwxr-xr-x)." << std::endl;
      if (chmod(filename, new_permissions) == -1) {
          std::cerr << "ERROR   : Failed to chmod " << filename << ": " << strerror(errno) << std::endl;
          unlink(filename);
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : Permissions changed. Use 'ls -l " << filename << "' to verify." << std::endl;

      // Dọn dẹp
      unlink(filename);
      return EXIT_SUCCESS;
  }
  ```

#### **4.2. Thay đổi Chủ sở hữu File: `chown()` 🧑‍🤝‍🧑**

* **Lý thuyết:** Hàm **`chown()`** (change owner) là một System Call dùng để thay đổi **chủ sở hữu (owner)** và/hoặc **nhóm (group)** của một file hoặc thư mục.

  * **Syntax:**
    **C++**

    ```
    #include <unistd.h>  // For chown
    #include <sys/types.h> // For uid_t, gid_t
    int chown(const char *path, uid_t owner, gid_t group);
    ```

  * **`path`** : Đường dẫn đến file hoặc thư mục.
  * **`owner`** : User ID (UID) mới của chủ sở hữu. Nếu bạn không muốn thay đổi chủ sở hữu, dùng `(uid_t)-1`.
  * **`group`** : Group ID (GID) mới của nhóm. Nếu bạn không muốn thay đổi nhóm, dùng `(gid_t)-1`.
  * **Lưu ý:**

    * Thông thường, chỉ **superuser (root)** mới có quyền thay đổi chủ sở hữu của một file.
    * Một người dùng bình thường có thể thay đổi nhóm của file sang một nhóm mà họ là thành viên (nếu họ là chủ sở hữu file).
  * **Giá trị trả về:** `0` nếu thành công, `-1` nếu thất bại (và `errno` được thiết lập).
* **Liên hệ Embedded Linux:** Rất quan trọng trong các hệ thống nhúng để thiết lập quyền sở hữu chính xác cho các file cấu hình, dữ liệu, hoặc các thiết bị ảo mà các daemon hoặc người dùng cụ thể cần truy cập.
* **Ví dụ (C++): `chown()` (Cần quyền root để chạy hiệu quả)**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <unistd.h>  // For chown, getuid, getgid
  #include <sys/types.h> // For uid_t, gid_t
  #include <stdio.h>   // For fopen, fclose
  #include <stdlib.h>  // For EXIT_FAILURE, EXIT_SUCCESS
  #include <errno.h>   // For errno
  #include <string.h>  // For strerror
  #include <pwd.h>     // For getpwnam (to get UID/GID by name)
  #include <grp.h>     // For getgrnam (to get GID by name)

  int main() {
      const char *filename = "my_file_to_chown.txt";

      // Tạo file trước
      FILE *fp = fopen(filename, "w");
      if (fp == NULL) {
          std::cerr << "ERROR   : Failed to create file: " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      fclose(fp);
      std::cout << "INFO    : Created file: " << filename << std::endl;

      // Thử thay đổi chủ sở hữu và nhóm thành người dùng hiện tại (hoặc root nếu chạy sudo)
      // Để thay đổi sang người dùng khác, bạn cần chạy chương trình với sudo
      uid_t target_uid = getuid(); // UID của người dùng hiện tại
      gid_t target_gid = getgid(); // GID của nhóm hiện tại

      // Nếu bạn muốn thử thay đổi sang một người dùng/nhóm cụ thể (ví dụ: "nobody", "nogroup")
      // struct passwd *pw = getpwnam("nobody");
      // if (pw) target_uid = pw->pw_uid;
      // struct group *gr = getgrnam("nogroup");
      // if (gr) target_gid = gr->gr_gid;

      std::cout << "INFO    : Attempting to chown " << filename << " to UID " << target_uid << ", GID " << target_gid << "." << std::endl;
      if (chown(filename, target_uid, target_gid) == -1) {
          std::cerr << "ERROR   : Failed to chown " << filename << ": " << strerror(errno) << " (You might need root privileges for this)." << std::endl;
      } else {
          std::cout << "SUCCESS : Ownership changed. Use 'ls -l " << filename << "' to verify." << std::endl;
      }

      // Dọn dẹp
      unlink(filename);
      return EXIT_SUCCESS;
  }
  ```

#### **4.3. Xóa File: `unlink()` 🗑️**

* **Lý thuyết:** Hàm **`unlink()`** là System Call dùng để **xóa một mục nhập (directory entry)** của file khỏi một thư mục.

  * **Syntax:**
    **C++**

    ```
    #include <unistd.h> // For unlink
    int unlink(const char *path);
    ```

  * **`path`** : Đường dẫn đến file cần xóa.
  * **Cách hoạt động:**

    1. `unlink()` xóa tên file khỏi thư mục.
    2. Nó giảm **số lượng hard link** trỏ đến inode của file đó đi 1.
    3. File thực sự bị xóa khỏi đĩa và không gian của nó được giải phóng **chỉ khi** số lượng hard link về 0 **VÀ** không còn tiến trình nào đang mở file đó.
  * **Lưu ý:** Chương trình `rm` của shell sử dụng `unlink()` để xóa file.
  * **Mẹo (Transient Files):** Bạn có thể tạo một file bằng `open()` và sau đó gọi `unlink()` ngay lập tức. File đó vẫn sẽ tồn tại và có thể được truy cập miễn là tiến trình của bạn giữ File Descriptor của nó mở. Khi tiến trình đóng File Descriptor hoặc kết thúc, file sẽ tự động bị xóa khỏi đĩa. Hữu ích cho các file tạm thời.
  * **Giá trị trả về:** `0` nếu thành công, `-1` nếu thất bại (và `errno` được thiết lập).
* **Liên hệ Embedded Linux:** Quan trọng để quản lý các file tạm thời, log, hoặc các file cấu hình cũ trên các thiết bị nhúng có không gian lưu trữ hạn chế.
* **Ví dụ (C++): `unlink()` và Transient File**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <unistd.h> // For unlink, sleep
  #include <fcntl.h>  // For open
  #include <stdio.h>  // For fopen, fclose
  #include <stdlib.h> // For EXIT_FAILURE, EXIT_SUCCESS
  #include <errno.h>  // For errno
  #include <string.h> // For strerror

  int main() {
      const char *filename_to_delete = "file_to_delete.txt";
      const char *transient_filename = "transient_file.tmp";
      int fd;

      // --- Ví dụ 1: Xóa file thông thường ---
      // Tạo file
      FILE *fp = fopen(filename_to_delete, "w");
      if (fp == NULL) {
          std::cerr << "ERROR   : Failed to create " << filename_to_delete << ": " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      fprintf(fp, "This file will be deleted.\n");
      fclose(fp);
      std::cout << "INFO    : Created file: " << filename_to_delete << std::endl;
      system(("ls -l " + std::string(filename_to_delete)).c_str());

      // Xóa file
      std::cout << "INFO    : Attempting to unlink " << filename_to_delete << "." << std::endl;
      if (unlink(filename_to_delete) == -1) {
          std::cerr << "ERROR   : Failed to unlink " << filename_to_delete << ": " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : File " << filename_to_delete << " unlinked. Use 'ls -l' to verify it's gone." << std::endl;

      // --- Ví dụ 2: Tạo Transient File ---
      // Mở file (O_CREAT)
      fd = open(transient_filename, O_RDWR | O_CREAT | O_EXCL, 0600);
      if (fd == -1) {
          std::cerr << "ERROR   : Failed to open transient file: " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "INFO    : Created transient file: " << transient_filename << " with FD: " << fd << std::endl;
      system(("ls -l " + std::string(transient_filename)).c_str());

      // Unlink ngay lập tức. File vẫn tồn tại vì FD đang mở.
      std::cout << "INFO    : Unlinking transient file immediately. It will disappear when FD is closed." << std::endl;
      if (unlink(transient_filename) == -1) {
          std::cerr << "ERROR   : Failed to unlink transient file: " << strerror(errno) << std::endl;
          close(fd);
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : Transient file unlinked. Use 'ls -l " << transient_filename << "' to verify it's gone from directory." << std::endl;
      // Ghi dữ liệu vào file (vẫn có thể ghi vì FD vẫn mở)
      write(fd, "This data is in a transient file.\n", 34);
      std::cout << "INFO    : Wrote data to the unlinked file via its FD." << std::endl;
      sleep(2); // Cho bạn thời gian kiểm tra bằng ls

      // Đóng FD. File sẽ bị xóa hoàn toàn.
      close(fd);
      std::cout << "INFO    : Closed FD " << fd << ". Transient file should now be fully deleted." << std::endl;

      return EXIT_SUCCESS;
  }
  ```

#### **4.4. Tạo Liên kết: `link()` và `symlink()` 🔗**

* **Lý thuyết:**

  * **`link()`:** Tạo một **hard link** (liên kết cứng) mới đến một file hiện có.
    **C++**

    ```
    #include <unistd.h> // For link
    int link(const char *path1, const char *path2);
    ```

    * `path1`: Đường dẫn đến file gốc.
    * `path2`: Đường dẫn cho hard link mới.
    * **Lưu ý:** Không thể tạo hard link cho thư mục hoặc trên các hệ thống file khác nhau.
  * **`symlink()`:** Tạo một **symbolic link** (liên kết mềm) mới đến một file hoặc thư mục.
    **C++**

    ```
    #include <unistd.h> // For symlink
    int symlink(const char *path1, const char *path2);
    ```

    * `path1`: Đường dẫn đến file/thư mục gốc (target).
    * `path2`: Đường dẫn cho symbolic link mới.
    * **Lưu ý:** Có thể tạo symlink cho thư mục và trên các hệ thống file khác nhau. Symlink vẫn tồn tại ngay cả khi target bị xóa.
  * **Giá trị trả về:** Cả hai hàm đều trả về `0` nếu thành công, `-1` nếu thất bại.
* **Liên hệ Embedded Linux:** Dùng để tạo các đường dẫn tắt cho các file cấu hình hoặc thư viện, hoặc để tạo các alias cho các thiết bị trong `/dev`.
* **Ví dụ (C++): `link()` và `symlink()`**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <unistd.h> // For link, symlink, unlink
  #include <stdio.h>  // For fopen, fclose
  #include <stdlib.h> // For EXIT_FAILURE, EXIT_SUCCESS
  #include <errno.h>  // For errno
  #include <string.h> // For strerror

  int main() {
      const char *original_file = "original.txt";
      const char *hard_link_name = "hardlink.txt";
      const char *sym_link_name = "symlink.txt";

      // Tạo file gốc
      FILE *fp = fopen(original_file, "w");
      if (fp == NULL) {
          std::cerr << "ERROR   : Failed to create " << original_file << ": " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      fprintf(fp, "This is the original content.\n");
      fclose(fp);
      std::cout << "INFO    : Created original file: " << original_file << std::endl;
      system(("ls -li " + std::string(original_file)).c_str());

      // Tạo hard link
      std::cout << "INFO    : Creating hard link " << hard_link_name << " to " << original_file << std::endl;
      if (link(original_file, hard_link_name) == -1) {
          std::cerr << "ERROR   : Failed to create hard link: " << strerror(errno) << std::endl;
      } else {
          std::cout << "SUCCESS : Hard link created. Check inode numbers with 'ls -li'." << std::endl;
          system(("ls -li " + std::string(hard_link_name)).c_str());
      }

      // Tạo symbolic link
      std::cout << "INFO    : Creating symbolic link " << sym_link_name << " to " << original_file << std::endl;
      if (symlink(original_file, sym_link_name) == -1) {
          std::cerr << "ERROR   : Failed to create symbolic link: " << strerror(errno) << std::endl;
      } else {
          std::cout << "SUCCESS : Symbolic link created. Check file type with 'ls -l'." << std::endl;
          system(("ls -l " + std::string(sym_link_name)).c_str());
      }

      // Dọn dẹp
      std::cout << "INFO    : Cleaning up files." << std::endl;
      unlink(original_file);
      unlink(hard_link_name);
      unlink(sym_link_name);
      return EXIT_SUCCESS;
  }
  ```

#### **4.5. Tạo và Xóa Thư mục: `mkdir()` và `rmdir()` 📂**

* **Lý thuyết:**

  * **`mkdir()`:** Tạo một thư mục mới.
    **C++**

    ```
    #include <sys/stat.h> // For mkdir
    int mkdir(const char *path, mode_t mode);
    ```

    * `path`: Đường dẫn cho thư mục mới.
    * `mode`: Quyền hạn cho thư mục mới (giống như `open()` với `O_CREAT`).
  * **`rmdir()`:** Xóa một thư mục.
    **C++**

    ```
    #include <unistd.h> // For rmdir
    int rmdir(const char *path);
    ```

    * `path`: Đường dẫn đến thư mục cần xóa.
    * **Lưu ý:** `rmdir()` chỉ có thể xóa các thư mục  **rỗng** . Để xóa thư mục không rỗng, bạn cần xóa tất cả nội dung bên trong trước hoặc sử dụng lệnh `rm -r` của shell (mà bên dưới sẽ gọi `unlink()` cho từng file và `rmdir()` cho từng thư mục con rỗng).
  * **Giá trị trả về:** Cả hai hàm đều trả về `0` nếu thành công, `-1` nếu thất bại.
* **Liên hệ Embedded Linux:** Dùng để tổ chức dữ liệu, tạo các thư mục log, hoặc quản lý cấu trúc file tạm thời trên thiết bị.
* **Ví dụ (C++): `mkdir()` và `rmdir()`**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <sys/stat.h> // For mkdir
  #include <unistd.h>   // For rmdir, sleep
  #include <stdio.h>    // For fopen, fclose // Used for potential system() call setup, not direct file ops
  #include <stdlib.h>   // For EXIT_FAILURE, EXIT_SUCCESS
  #include <errno.h>    // For errno
  #include <string.h>   // For strerror

  int main() {
      const char *dirname = "my_new_directory";
      const char *subdirname = "my_new_directory/subdir";

      // --- Tạo thư mục ---
      std::cout << "INFO    : Attempting to create directory: " << dirname << std::endl;
      // Quyền 0755 (rwxr-xr-x)
      if (mkdir(dirname, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1) {
          std::cerr << "ERROR   : Failed to create directory " << dirname << ": " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : Directory " << dirname << " created. Use 'ls -ld " << dirname << "' to verify." << std::endl;

      std::cout << "INFO    : Attempting to create subdirectory: " << subdirname << std::endl;
      if (mkdir(subdirname, 0755) == -1) {
          std::cerr << "ERROR   : Failed to create subdirectory " << subdirname << ": " << strerror(errno) << std::endl;
          rmdir(dirname); // Dọn dẹp
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : Subdirectory " << subdirname << " created." << std::endl;

      // --- Xóa thư mục ---
      // Thử xóa thư mục không rỗng (sẽ lỗi)
      std::cout << "INFO    : Attempting to remove non-empty directory " << dirname << " (should fail)." << std::endl;
      if (rmdir(dirname) == -1) {
          std::cerr << "WARNING : Failed to remove " << dirname << ": " << strerror(errno) << " (Expected: Directory not empty)." << std::endl;
      }

      // Xóa thư mục con rỗng trước
      std::cout << "INFO    : Attempting to remove empty subdirectory: " << subdirname << std::endl;
      if (rmdir(subdirname) == -1) {
          std::cerr << "ERROR   : Failed to remove subdirectory " << subdirname << ": " << strerror(errno) << std::endl;
          rmdir(dirname); // Dọn dẹp
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : Subdirectory " << subdirname << " removed." << std::endl;

      // Xóa thư mục cha (giờ đã rỗng)
      std::cout << "INFO    : Attempting to remove now-empty directory: " << dirname << std::endl;
      if (rmdir(dirname) == -1) {
          std::cerr << "ERROR   : Failed to remove directory " << dirname << ": " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : Directory " << dirname << " removed." << std::endl;

      return EXIT_SUCCESS;
  }
  ```

#### **4.6. Thay đổi Thư mục Hiện tại: `chdir()` và `getcwd()` 🗺️**

* **Lý thuyết:**

  * **`chdir()`:** Thay đổi thư mục làm việc hiện tại (Current Working Directory - CWD) của tiến trình.
    **C++**

    ```
    #include <unistd.h> // For chdir
    int chdir(const char *path);
    ```

    * `path`: Đường dẫn đến thư mục mới.
  * **`getcwd()`:** Lấy đường dẫn tuyệt đối của thư mục làm việc hiện tại.
    **C++**

    ```
    #include <unistd.h> // For getcwd
    char *getcwd(char *buf, size_t size);
    ```

    * `buf`: Con trỏ tới bộ đệm để lưu đường dẫn.
    * `size`: Kích thước của bộ đệm `buf`.
    * **Lưu ý:** Bạn phải đảm bảo `buf` đủ lớn để chứa đường dẫn. Nếu `buf` là `NULL` và `size` là 0, `getcwd` có thể cấp phát bộ nhớ động cho bạn (hành vi này là GNU extension, không phải POSIX chuẩn).
  * **Giá trị trả về:** Cả hai hàm đều trả về `0` (chdir) hoặc con trỏ tới `buf` (getcwd) nếu thành công, `-1` hoặc `NULL` nếu thất bại.
* **Liên hệ Embedded Linux:** Hữu ích khi chương trình của bạn cần thao tác với các file trong nhiều thư mục khác nhau, hoặc khi bạn muốn biết vị trí hiện tại của chương trình trên filesystem.
* **Ví dụ (C++): `chdir()` và `getcwd()`**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <unistd.h> // For chdir, getcwd
  #include <stdio.h>  // For perror (for system() in other examples if needed)
  #include <stdlib.h> // For EXIT_FAILURE, EXIT_SUCCESS, getenv
  #include <errno.h>  // For errno
  #include <string.h> // For strerror

  int main() {
      char cwd_buffer[256];
      char *result_ptr;

      // Lấy thư mục hiện tại ban đầu
      result_ptr = getcwd(cwd_buffer, sizeof(cwd_buffer));
      if (result_ptr == NULL) {
          std::cerr << "ERROR   : Failed to get current directory: " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "INFO    : Initial current directory: " << cwd_buffer << std::endl;

      // Thử thay đổi thư mục
      const char *target_dir = "/tmp";
      std::cout << "INFO    : Attempting to change directory to: " << target_dir << std::endl;
      if (chdir(target_dir) == -1) {
          std::cerr << "ERROR   : Failed to change directory to " << target_dir << ": " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : Changed directory to: " << target_dir << std::endl;

      // Lấy thư mục hiện tại sau khi thay đổi
      result_ptr = getcwd(cwd_buffer, sizeof(cwd_buffer));
      if (result_ptr == NULL) {
          std::cerr << "ERROR   : Failed to get current directory after chdir: " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "INFO    : New current directory: " << cwd_buffer << std::endl;

      // Thử thay đổi thư mục về thư mục gốc của người dùng
      const char *home_dir = getenv("HOME"); // Lấy biến môi trường HOME
      if (home_dir != NULL) {
          std::cout << "INFO    : Attempting to change directory to HOME: " << home_dir << std::endl;
          if (chdir(home_dir) == -1) {
              std::cerr << "ERROR   : Failed to change directory to HOME: " << strerror(errno) << std::endl;
          } else {
              std::cout << "SUCCESS : Changed directory to HOME." << std::endl;
              result_ptr = getcwd(cwd_buffer, sizeof(cwd_buffer));
              if (result_ptr != NULL) {
                  std::cout << "INFO    : Current directory: " << cwd_buffer << std::endl;
              }
          }
      } else {
          std::cerr << "WARNING : HOME environment variable not set." << std::endl;
      }

      return EXIT_SUCCESS;
  }
  ```

---

### **Câu hỏi Tự đánh giá Module 4 🤔**

1. Giải thích sự khác biệt giữa việc thay đổi quyền hạn file bằng `chmod()` và thay đổi chủ sở hữu/nhóm bằng `chown()`. Quyền hạn nào là cần thiết để thực hiện mỗi thao tác?
2. Khi bạn gọi `unlink("my_file.txt")`, điều gì thực sự xảy ra với file trên đĩa? Khi nào thì không gian đĩa của file đó được giải phóng?
3. Nêu một trường hợp sử dụng thực tế của việc tạo một "transient file" (file tạm thời) bằng cách `open()` và sau đó `unlink()` ngay lập tức.
4. Bạn muốn tạo một thư mục `data_logs` mà chỉ chủ sở hữu có quyền đọc/ghi/thực thi, còn nhóm và người khác chỉ có quyền đọc/thực thi. Viết lệnh `mkdir()` với tham số `mode` phù hợp.
5. Tại sao `rmdir()` chỉ có thể xóa thư mục rỗng? Nếu bạn muốn xóa một thư mục không rỗng bằng chương trình C, bạn sẽ làm thế nào?
6. Giả sử chương trình của bạn đang chạy trong thư mục `/home/user/app`. Bạn muốn thay đổi thư mục làm việc hiện tại sang `/var/log` và sau đó lấy đường dẫn đầy đủ của thư mục đó. Viết đoạn code C sử dụng `chdir()` và `getcwd()`.

---

### **Bài tập Thực hành Module 4 ✍️**

1. **Chương trình Thay đổi Quyền hạn và Kiểm tra:**
   * Viết một chương trình C++ (`file_permissions.cpp`) nhận hai tham số dòng lệnh: `<filename>` và `<octal_permissions>`.
   * Chương trình sẽ tạo một file mới với tên `<filename>` (nếu chưa tồn tại, ghi đè nếu có) với nội dung "Hello".
   * Sau đó, sử dụng `chmod()` để thay đổi quyền hạn của file đó theo giá trị octal được cung cấp.
   * Sử dụng `stat()` để đọc lại thông tin file và in ra quyền hạn mới dưới dạng octal và dạng `rwx`.
   * Xử lý lỗi đầy đủ.
   * **Thử thách:** Sau khi thay đổi quyền, thử mở file đó với các quyền khác nhau (ví dụ: chỉ đọc, chỉ ghi) và quan sát kết quả.
2. **Chương trình Quản lý Thư mục Đơn giản:**
   * Viết một chương trình C++ (`dir_manager.cpp`) nhận một tham số dòng lệnh: `<command>` (ví dụ: "create", "delete", "cd", "pwd") và một tham số tùy chọn `<directory_name>`.
   * Thực hiện các chức năng sau:
     * `create <directory_name>`: Tạo một thư mục mới với quyền `0755`.
     * `delete <directory_name>`: Xóa một thư mục (chỉ khi rỗng).
     * `cd <directory_name>`: Thay đổi thư mục làm việc hiện tại.
     * `pwd`: In ra thư mục làm việc hiện tại.
   * Sử dụng `mkdir()`, `rmdir()`, `chdir()`, `getcwd()` tương ứng.
   * Xử lý lỗi cho từng lệnh.
   * **Thử thách:** Thêm lệnh `delete_recursive <directory_name>` để xóa một thư mục và tất cả nội dung của nó (sẽ cần kiến thức về quét thư mục ở Module 5).
3. **Chương trình Tạo và Kiểm tra Links:**
   * Viết một chương trình C++ (`link_creator.cpp`) nhận ba tham số dòng lệnh: `<original_file>`, `<hard_link_name>`, `<sym_link_name>`.
   * Chương trình sẽ:
     * Tạo file `<original_file>` với nội dung "Original content".
     * Tạo hard link `<hard_link_name>` tới `<original_file>`.
     * Tạo symbolic link `<sym_link_name>` tới `<original_file>`.
     * Sử dụng `stat()` và `lstat()` trên cả ba file để in ra inode number, kích thước, và xác định loại file (regular, hard link, symlink).
     * Xóa `<original_file>` và sau đó thử đọc nội dung từ `<hard_link_name>` và `<sym_link_name>`. Quan sát và giải thích kết quả.
     * Dọn dẹp tất cả các file đã tạo.

---

Hãy dành thời gian để thực hành các bài tập này. Chúng sẽ giúp bạn làm quen và thành thạo việc quản lý file và thư mục ở cấp độ System Call. Khi bạn đã sẵn sàng, hãy cho tôi biết để chuyển sang Module 5!
