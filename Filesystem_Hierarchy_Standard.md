### **1. Khái niệm cơ bản về Filesystem Hierarchy Standard (FHS) 🌳**

FHS là một tiêu chuẩn xác định cấu trúc các thư mục chính (directories) và nội dung của chúng trong các hệ điều hành dựa trên Linux. Mục tiêu là giúp người dùng và phần mềm dễ dàng tìm thấy các file mà họ cần, bất kể bản phân phối Linux nào đang được sử dụng.

Hãy hình dung nó như một **cây thư mục** 🌲, với gốc là thư mục **root** được ký hiệu bằng dấu gạch chéo `/`. Mọi thứ khác đều nằm trong thư mục gốc này.

---

### **2. Các Thư mục Chính và Chức năng của chúng 📂**

Dưới đây là một số thư mục quan trọng nhất mà bạn sẽ thường xuyên gặp, cùng với giải thích kỹ thuật về mục đích của chúng:

#### **2.1. `/` (Root Directory) 🌳**

* **Chức năng:** Đây là thư mục gốc của toàn bộ hệ thống file. Mọi thư mục và file khác đều là con của thư mục này. Nó không chứa bất kỳ file nào, mà là điểm gắn kết (mount point) cho toàn bộ cây thư mục.
* **Giải thích kỹ thuật:** Nó là **entry point** của cây thư mục ảo (virtual directory tree) được kernel quản lý. Khi hệ thống khởi động, kernel sẽ mount filesystem gốc lên `/`.

#### **2.2. `/bin` (User Binaries) 💻**

* **Chức năng:** Chứa các lệnh thực thi (executable binaries) cần thiết cho người dùng thông thường và quản trị hệ thống để vận hành hệ thống ở chế độ  **single-user mode** . Các lệnh này có thể được sử dụng bởi tất cả người dùng.
* **Ví dụ:** `ls`, `cp`, `mv`, `rm`, `cat`, `echo`, `bash`.
* **Giải thích kỹ thuật:** Các chương trình tại đây không phụ thuộc vào các filesystem khác được mount. Chúng là một phần của filesystem gốc và có sẵn ngay từ khi hệ thống boot.

#### **2.3. `/sbin` (System Binaries) ⚙️**

* **Chức năng:** Chứa các lệnh thực thi dành cho quản trị viên hệ thống (system administrator - root) để duy trì và quản lý hệ thống. Các lệnh này thường chỉ được chạy bởi `root` hoặc thông qua `sudo`.
* **Ví dụ:** `fdisk`, `mkfs`, `mount`, `reboot`, `shutdown`, `ifconfig`.
* **Giải thích kỹ thuật:** Tương tự như `/bin`, các binary ở đây cũng là một phần của filesystem gốc và cần thiết cho quá trình khởi động và phục hồi hệ thống.

#### **2.4. `/etc` (Configuration Files) 📝**

* **Chức năng:** Chứa các file cấu hình hệ thống (system-wide configuration files). Đây là nơi bạn sẽ tìm thấy các cài đặt cho hầu hết các dịch vụ và ứng dụng.
* **Ví dụ:** `/etc/passwd` (thông tin người dùng), `/etc/fstab` (cấu hình mount filesystem), `/etc/hosts`, `/etc/resolv.conf`.
* **Giải thích kỹ thuật:** Các file trong `/etc` thường là các file văn bản thuần túy (plain text files) có thể chỉnh sửa được, định dạng của chúng tuân theo từng dịch vụ cụ thể. Khi một dịch vụ khởi động, nó sẽ đọc cấu hình từ các file tương ứng trong thư mục này.

#### **2.5. `/dev` (Device Files) 🔌**

* **Chức năng:** Chứa các file đặc biệt đại diện cho các thiết bị phần cứng (device files). Trong Linux, thiết bị phần cứng được truy cập thông qua các file này.
* **Ví dụ:** `/dev/sda` (ổ đĩa cứng đầu tiên), `/dev/tty0` (thiết bị terminal ảo), `/dev/null` (thiết bị "null" để loại bỏ output).
* **Giải thích kỹ thuật:** Các file này không chứa dữ liệu theo nghĩa truyền thống, mà là các điểm giao tiếp (interface points) với các driver thiết bị trong kernel. Khi một chương trình đọc hoặc ghi vào `/dev/sda`, thực chất nó đang tương tác với driver của ổ đĩa cứng. Có hai loại chính: **block devices** (ví dụ: ổ đĩa, truy cập dữ liệu theo block) và **character devices** (ví dụ: terminal, truy cập dữ liệu theo byte).

#### **2.6. `/proc` (Process Information Pseudo-filesystem) 🧠**

* **Chức năng:** Một filesystem ảo (virtual filesystem) chứa thông tin về các tiến trình đang chạy (running processes) và trạng thái hệ thống (system state).
* **Ví dụ:** `/proc/cpuinfo` (thông tin CPU), `/proc/meminfo` (thông tin bộ nhớ), `/proc/<PID>` (thông tin chi tiết về tiến trình có ID `<PID>`).
* **Giải thích kỹ thuật:** Các file trong `/proc` không thực sự tồn tại trên đĩa mà được tạo ra "on-the-fly" bởi kernel khi được truy cập. Đây là một cơ chế giao tiếp giữa không gian người dùng (user-space) và kernel-space, cho phép các công cụ như `ps`, `top` thu thập thông tin về hệ thống và tiến trình.

#### **2.7. `/sys` (System Information Pseudo-filesystem) 🖥️**

* **Chức năng:** Tương tự như `/proc`, `/sys` là một filesystem ảo cung cấp giao diện để tương tác với các thiết bị phần cứng và các module kernel.
* **Ví dụ:** Bạn có thể tìm thấy thông tin về các thiết bị USB, PCI, nhiệt độ CPU, v.v.
* **Giải thích kỹ thuật:** `/sys` được thiết kế để cung cấp một giao diện rõ ràng và có cấu trúc hơn `/proc` cho việc quản lý thiết bị và driver. Nó sử dụng một mô hình dựa trên các đối tượng (objects) và thuộc tính (attributes) để biểu diễn các thành phần phần cứng và cài đặt của chúng.

#### **2.8. `/home` (User Home Directories) 🏠**

* **Chức năng:** Chứa thư mục cá nhân cho mỗi người dùng trên hệ thống. Đây là nơi người dùng lưu trữ tài liệu, cài đặt ứng dụng cá nhân, và các file cấu hình ứng dụng.
* **Ví dụ:** `/home/hoanganh`, `/home/john`.
* **Giải thích kỹ thuật:** Mỗi thư mục con trong `/home` thường được gán quyền sở hữu (ownership) cho người dùng tương ứng, đảm bảo quyền riêng tư và bảo mật dữ liệu cá nhân.

#### **2.9. `/var` (Variable Data Files) 📊**

* **Chức năng:** Chứa các file dữ liệu có nội dung thay đổi thường xuyên trong quá trình hoạt động của hệ thống.
* **Ví dụ:**
  * `/var/log`: Nhật ký hệ thống (system logs) 📜, ví dụ: `/var/log/syslog`, `/var/log/auth.log`.
  * `/var/cache`: Dữ liệu cache của ứng dụng 💨.
  * `/var/spool`: Dữ liệu hàng đợi (ví dụ: email, print jobs) 📧🖨️.
  * `/var/tmp`: File tạm thời (cũng có `/tmp`, nhưng `/var/tmp` thường tồn tại qua các lần khởi động lại) ⏳.
* **Giải thích kỹ thuật:** Việc tách `/var` ra khỏi filesystem gốc giúp ngăn chặn việc filesystem gốc bị đầy do các file log lớn hoặc dữ liệu tạm thời, đảm bảo tính ổn định của hệ thống.

#### **2.10. `/tmp` (Temporary Files) 🗑️**

* **Chức năng:** Chứa các file tạm thời (temporary files) được tạo bởi các ứng dụng và người dùng. Nội dung của thư mục này thường bị xóa khi hệ thống khởi động lại (reboot).
* **Giải thích kỹ thuật:** Các ứng dụng thường sử dụng `/tmp` để lưu trữ dữ liệu tạm thời cần thiết cho các hoạt động ngắn hạn. Mọi người dùng đều có quyền ghi vào thư mục này, nhưng chỉ có thể xóa các file của chính họ.

#### **2.11. `/usr` (Unix System Resources) 📦**

* **Chức năng:** Chứa hầu hết các file thực thi, thư viện, tài liệu và mã nguồn chung (shared resources) của hệ thống. Đây là một trong những thư mục lớn nhất và quan trọng nhất.
* **Ví dụ:**
  * `/usr/bin`: Các lệnh thực thi của người dùng không quan trọng cho việc boot hệ thống (ví dụ: trình duyệt web, text editor) 🌐.
  * `/usr/lib`: Thư viện dùng chung (shared libraries) 📚.
  * `/usr/local`: Nơi dành cho các phần mềm được cài đặt thủ công (manually installed software) không thuộc quản lý của trình quản lý gói (package manager) 🛠️.
  * `/usr/share`: Dữ liệu dùng chung (shared data) như tài liệu, icon, font chữ 🖼️.
* **Giải thích kỹ thuật:** Mục đích của `/usr` là chứa các tài nguyên " **chỉ đọc** " (read-only) và có thể **chia sẻ** (shareable) giữa nhiều hệ thống hoặc được gắn kết (mount) từ một máy chủ mạng (network server).

#### **2.12. `/opt` (Optional Application Software Packages) ➕**

* **Chức năng:** Chứa các gói phần mềm tùy chọn (optional software packages) của bên thứ ba (third-party applications). Các ứng dụng lớn, độc lập thường được cài đặt tại đây.
* **Ví dụ:** `/opt/google/chrome`, `/opt/spotify`.
* **Giải thích kỹ thuật:** Mỗi ứng dụng thường có thư mục riêng của mình trong `/opt`, chứa tất cả các file cần thiết của nó (binary, libraries, configuration). Điều này giúp dễ dàng gỡ bỏ hoặc quản lý các ứng dụng này mà không ảnh hưởng đến các thành phần hệ thống khác.

#### **2.13. `/mnt` (Mount Point for Temporarily Mounted Filesystems) 🏞️**

* **Chức năng:** Một thư mục tạm thời để gắn kết (mount) các filesystem không thường xuyên (ví dụ: USB drives, network shares, CD/DVD).
* **Giải thích kỹ thuật:** Đây chỉ là một convention. Bạn có thể mount filesystem ở bất kỳ đâu, nhưng `/mnt` được dùng để chỉ ra rằng đây là điểm mount tạm thời do người dùng tạo ra.

#### **2.14. `/media` (Mount Point for Removable Media) 💿**

* **Chức năng:** Tương tự như `/mnt`, nhưng dành riêng cho các thiết bị lưu trữ di động (removable media) như USB flash drives, CD-ROMs, thẻ nhớ, được gắn kết tự động bởi hệ thống.
* **Giải thích kỹ thuật:** Các môi trường desktop hiện đại thường tự động tạo thư mục con trong `/media` khi bạn cắm một thiết bị, ví dụ: `/media/hoanganh/USB_DRIVE`.

#### **2.15. `/srv` (Service Data) 🌐**

* **Chức năng:** Chứa dữ liệu dành cho các dịch vụ được cung cấp bởi hệ thống.
* **Ví dụ:** `/srv/www` (dữ liệu web server) 🕸️, `/srv/ftp` (dữ liệu FTP server) 📁.
* **Giải thích kỹ thuật:** Mục đích là cung cấp một vị trí tiêu chuẩn cho dữ liệu cụ thể của dịch vụ, tách biệt với các file cấu hình (trong `/etc`) hoặc binary của dịch vụ (trong `/usr/bin` hoặc `/usr/sbin`).

---

### **3. Quản lý File và Thư mục với các Lệnh Cơ bản ⌨️**

Là một kỹ sư, việc nắm vững các lệnh cơ bản để điều hướng và quản lý file là cực kỳ quan trọng.

* **`pwd` (Print Working Directory):** Hiển thị đường dẫn thư mục hiện tại của bạn.

  **Bash**

  ```
  logger.info('Current directory: {pwd}')
  pwd
  ```
* **`ls` (List Directory Contents):** Liệt kê nội dung của một thư mục.

  * `ls -l`: Hiển thị chi tiết (long format), bao gồm quyền, chủ sở hữu, nhóm, kích thước và thời gian sửa đổi.
  * `ls -a`: Hiển thị tất cả các file, bao gồm cả file ẩn (bắt đầu bằng dấu `.`).

  <!-- end list -->

  **Bash**

  ```
  logger.info('Listing contents of current directory:')
  ls -la
  ```
* **`cd` (Change Directory):** Thay đổi thư mục hiện tại.

  * `cd /path/to/directory`: Di chuyển đến một đường dẫn tuyệt đối.
  * `cd ..`: Di chuyển lên một cấp thư mục.
  * `cd ~`: Di chuyển về thư mục home của người dùng hiện tại.

  <!-- end list -->

  **Bash**

  ```
  logger.info('Changing directory to /var/log...')
  cd /var/log
  logger.info('Current directory after cd: {pwd}')
  pwd
  ```
* **`mkdir` (Make Directory):** Tạo một thư mục mới.

  **Bash**

  ```
  logger.info('Creating a new directory named my_project in /tmp...')
  mkdir /tmp/my_project
  ```
* **`rmdir` (Remove Directory):** Xóa một thư mục rỗng.

  **Bash**

  ```bash
  logger.info('Removing the empty directory /tmp/my_project...')
  rmdir /tmp/my_project
  ```
* **`rm` (Remove):** Xóa file hoặc thư mục (bao gồm cả thư mục không rỗng với tùy chọn `-r`). **Cẩn thận khi sử dụng lệnh này!** ⚠️

  * `rm file.txt`: Xóa file `file.txt`.
  * `rm -r directory_name`: Xóa thư mục và tất cả nội dung bên trong.
  * `rm -rf directory_name`: Xóa thư mục và nội dung mà không hỏi xác nhận (force). **CỰC KỲ NGUY HIỂM!** 💀

  <!-- end list -->

  **Bash**

  ```
  logger.warning('Attempting to remove a file. Be careful with rm -rf!')
  # Assuming /tmp/test_file.txt exists for demonstration
  # echo "Hello" > /tmp/test_file.txt
  # rm /tmp/test_file.txt
  ```
* **`cp` (Copy):** Sao chép file hoặc thư mục. 📁➡️📂

  * `cp source_file destination_file`: Sao chép file.
  * `cp -r source_directory destination_directory`: Sao chép thư mục.

  <!-- end list -->

  **Bash**

  ```
  logger.info('Copying a file from /etc to /tmp...')
  cp /etc/hostname /tmp/my_hostname.txt
  ```
* **`mv` (Move):** Di chuyển hoặc đổi tên file/thư mục. 🚚➡️📁

  * `mv old_name new_name`: Đổi tên file/thư mục.
  * `mv file.txt /path/to/new/location`: Di chuyển file.

  <!-- end list -->

  **Bash**

  ```
  logger.info('Renaming /tmp/my_hostname.txt to /tmp/system_hostname.txt...')
  mv /tmp/my_hostname.txt /tmp/system_hostname.txt
  ```
* **`cat` (Concatenate and Display Files):** Hiển thị nội dung của file ra màn hình. 📜

  **Bash**

  ```
  logger.info('Displaying content of /tmp/system_hostname.txt:')
  cat /tmp/system_hostname.txt
  ```
* **`less` hoặc `more`:** Xem nội dung file từng trang. `less` linh hoạt hơn `more`. 📖

  **Bash**

  ```
  logger.info('Viewing content of /etc/fstab with less...')
  less /etc/fstab
  ```
* **`head` và `tail`:** Xem N dòng đầu/cuối của file. 🔝⬇️

  * `head -n 5 file.txt`: Xem 5 dòng đầu.
  * `tail -n 10 file.txt`: Xem 10 dòng cuối.
  * `tail -f file.txt`: Theo dõi nội dung file khi nó được ghi thêm (hữu ích cho log files).

  <!-- end list -->

  **Bash**

  ```
  logger.info('Displaying the last 3 lines of /var/log/syslog (if available):')
  tail -n 3 /var/log/syslog
  ```

---

### **4. Quyền Hạn File (File Permissions) 🔒**

Trong Linux, mỗi file và thư mục đều có một tập hợp các quyền hạn để kiểm soát ai có thể truy cập và làm gì với chúng. Đây là một khái niệm rất quan trọng về bảo mật.

Quyền hạn được biểu diễn bằng 9 ký tự, chia thành ba bộ 3 ký tự, cộng thêm một ký tự đầu tiên cho loại file:

```
rwxrwxrwx
| | | |
| | | `- Other (người dùng khác) 🌍
| | `--- Group (nhóm của file) 👥
| `---- User (chủ sở hữu file) 👤
`------ File Type (loại file) 📄
```

**Các loại quyền:**

* **`r` (Read - Đọc):** 📖
  * **File:** Có thể xem nội dung file.
  * **Directory:** Có thể liệt kê nội dung thư mục (sử dụng `ls`).
* **`w` (Write - Ghi):** ✏️
  * **File:** Có thể sửa đổi, thêm vào hoặc xóa file.
  * **Directory:** Có thể tạo, xóa, đổi tên file trong thư mục đó (quan trọng: không phải là xóa chính thư mục đó).
* **`x` (Execute - Thực thi):** 🏃‍♂️
  * **File:** Có thể chạy file đó như một chương trình.
  * **Directory:** Có thể vào trong thư mục đó (sử dụng `cd`).

**Loại file (ký tự đầu tiên):**

* **`-`:** File thông thường (regular file).
* **`d`:** Thư mục (directory).
* **`l`:** Liên kết mềm (symbolic link).
* **`b`:** Thiết bị khối (block device).
* **`c`:** Thiết bị ký tự (character device).

**Ví dụ:**

Output của `ls -l` cho một file:

```
-rw-r--r-- 1 hoanganh staff 1234 May 19 10:30 my_document.txt
```

* `-`: Đây là một file thông thường.
* `rw-`: Chủ sở hữu (`hoanganh`) có quyền đọc và ghi. 👤
* `r--`: Nhóm (`staff`) chỉ có quyền đọc. 👥
* `r--`: Người dùng khác chỉ có quyền đọc. 🌍

Output của `ls -l` cho một thư mục:

```
drwxr-xr-x 2 hoanganh staff 4096 May 19 10:30 my_directory
```

* `d`: Đây là một thư mục.
* `rwx`: Chủ sở hữu (`hoanganh`) có quyền đọc, ghi, và thực thi (đi vào). 👤
* `r-x`: Nhóm (`staff`) có quyền đọc và thực thi (đi vào), nhưng không được ghi. 👥
* `r-x`: Người dùng khác có quyền đọc và thực thi (đi vào), nhưng không được ghi. 🌍

**Thay đổi quyền hạn với `chmod`:**

Bạn có thể thay đổi quyền hạn bằng cách sử dụng `chmod`. Có hai cách chính:

1. **Chế độ ký hiệu (Symbolic Mode):** ➕➖

   * `u` (user), `g` (group), `o` (others), `a` (all).
   * `+` (thêm quyền), `-` (bỏ quyền), `=` (gán chính xác quyền).

   <!-- end list -->

   **Bash**

   ```
   logger.info('Changing permissions for a file to add write for group...')
   # Giả sử có file test_permissions.txt
   # touch test_permissions.txt
   chmod g+w test_permissions.txt # Thêm quyền ghi cho nhóm
   chmod o-r test_permissions.txt # Bỏ quyền đọc của người dùng khác
   chmod u=rw,go=r test_permissions.txt # Gán cụ thể quyền cho user, group, others
   ```
2. Chế độ số (Octal Mode): 🔢
   Mỗi quyền r, w, x được gán một giá trị số:

   * `r = 4`
   * `w = 2`
   * `x = 1`
     Giá trị tổng cho mỗi bộ quyền (User, Group, Other) là tổng của các quyền đó.
   * `rwx = 4 + 2 + 1 = 7`
   * `rw- = 4 + 2 + 0 = 6`
   * `r-x = 4 + 0 + 1 = 5`
   * `r-- = 4 + 0 + 0 = 4`

   <!-- end list -->

   **Bash**

   ```
   logger.info('Changing permissions for a file using octal mode...')
   chmod 755 my_script.sh # Chủ sở hữu: rwx, Nhóm: r-x, Khác: r-x
   # Ý nghĩa: Chủ sở hữu có thể đọc, ghi, thực thi. Nhóm và người khác chỉ có thể đọc và thực thi.
   ```

**Thay đổi chủ sở hữu/nhóm với `chown` và `chgrp`:**

* **`chown user:group file/directory`:** Thay đổi chủ sở hữu và nhóm của file/thư mục. 👥
  **Bash**

  ```
  logger.info('Changing owner and group of a file...')
  # chown newuser:newgroup my_file.txt
  ```
* **`chgrp group file/directory`:** Thay đổi nhóm của file/thư mục. 🤝
  **Bash**

  ```
  logger.info('Changing group of a file...')
  # chgrp newgroup my_file.txt
  ```

---

### **5. Liên kết (Links) trong Linux 🔗**

Trong Linux, có hai loại liên kết chính, tương tự như các **pointer** hoặc **shortcut** mà bạn có thể đã quen thuộc trong các hệ thống khác, nhưng với những khác biệt kỹ thuật quan trọng.

#### **5.1. Hard Link (Liên kết Cứng) ⛓️**

* **Khái niệm:** Một Hard Link là một tên file khác trỏ đến cùng một **inode** (index node) trên đĩa. Inode là một cấu trúc dữ liệu lưu trữ thông tin về file, bao gồm các block dữ liệu vật lý trên đĩa, quyền hạn, chủ sở hữu, thời gian, v.v. Mỗi file vật lý có một inode duy nhất.
* **Đặc điểm:**

  * Không thể tạo Hard Link cho thư mục (chỉ áp dụng cho file) 🚫📁.
  * Không thể tạo Hard Link trên các phân vùng đĩa khác nhau (khác filesystem) 🚫💾.
  * Khi bạn xóa một Hard Link, file gốc vẫn tồn tại miễn là vẫn còn ít nhất một Hard Link khác trỏ đến cùng inode đó. File chỉ bị xóa vật lý khi tất cả các Hard Link đến nó đã bị xóa.
  * Tất cả các Hard Link đều có kích thước và nội dung y hệt file gốc vì chúng thực chất là cùng một file.
* **Lệnh tạo:** `ln source_file hard_link_name`
  **Bash**

  ```
  logger.info('Creating a hard link to /etc/hostname...')
  # Tạo file gốc để minh họa
  echo "test-host" > /tmp/original_host.txt
  # Tạo hard link
  ln /tmp/original_host.txt /tmp/hard_link_host.txt
  logger.debug('Inode of original file: {inode_original}')
  # ls -i /tmp/original_host.txt
  logger.debug('Inode of hard link: {inode_hard_link}')
  # ls -i /tmp/hard_link_host.txt
  ```

  Bạn sẽ thấy cả hai file `/tmp/original_host.txt` và `/tmp/hard_link_host.txt` có cùng một inode number khi dùng `ls -i`.

#### **5.2. Symbolic Link (Soft Link / Symlink - Liên kết Mềm) ➡️**

* **Khái niệm:** Một Symbolic Link là một file đặc biệt chứa đường dẫn đến một file hoặc thư mục khác. Nó giống như một **shortcut** trong Windows hoặc **alias** trong macOS.
* **Đặc điểm:**

  * Có thể tạo Symbolic Link cho cả file và thư mục ✔️.
  * Có thể trỏ đến file/thư mục trên các phân vùng đĩa khác nhau ✔️💾.
  * Nếu file/thư mục gốc bị xóa, Symbolic Link sẽ bị " **đứt** " (broken link) 💔 và không còn trỏ đến đâu nữa.
  * Kích thước của Symbolic Link là kích thước của chuỗi đường dẫn mà nó chứa, không phải kích thước của file gốc.
  * Quyền hạn của Symbolic Link thường là `lrwxrwxrwx` (l cho link, và quyền đầy đủ vì nó chỉ là một con trỏ). Quyền truy cập thực tế sẽ được xác định bởi quyền của file/thư mục gốc mà nó trỏ tới.
* **Lệnh tạo:** `ln -s source_file_or_directory symbolic_link_name`
  **Bash**

  ```
  logger.info('Creating a symbolic link to /etc/hostname...')
  ln -s /etc/hostname /tmp/symlink_host.txt
  logger.debug('Content of symlink: {symlink_content}')
  cat /tmp/symlink_host.txt # Sẽ hiển thị nội dung của /etc/hostname
  logger.debug('Listing symlink details:')
  ls -l /tmp/symlink_host.txt # Sẽ thấy 'l' ở đầu quyền và mũi tên '->' chỉ đến file gốc
  ```

---

### **6. Một số Thư mục khác thường thấy (ít quan trọng hơn cho người dùng thông thường nhưng có thể gặp trong Embedded Linux) ➕**

* **`/boot`:** Chứa các file cần thiết cho quá trình khởi động hệ thống 🚀, bao gồm kernel Linux, initramfs (initial RAM filesystem), và các file cấu hình bootloader (ví dụ: GRUB).
* **`/root`:** Thư mục home của người dùng `root` (quản trị viên) 👑. Nó tách biệt với `/home` để đảm bảo `root` có thể đăng nhập ngay cả khi `/home` gặp sự cố hoặc không được mount.
* **`/run`:** Chứa dữ liệu runtime của các dịch vụ 🏃. Nó là một filesystem ảo (tmpfs) và nội dung sẽ bị xóa khi hệ thống khởi động lại. Ví dụ: `/run/systemd`, `/run/user`.
* **`/snap`:** Thư mục dành riêng cho các gói Snap (một định dạng gói ứng dụng của Canonical) 🧩.
* **`/lost+found`:** Một thư mục đặc biệt được tạo ra bởi công cụ kiểm tra filesystem (`fsck`) khi phát hiện và phục hồi các file bị hỏng hoặc không liên kết trong quá trình kiểm tra 🔎🩹.

---

### **Tổng kết ✨**

Với nền tảng kỹ sư cơ điện tử và kinh nghiệm về OOP, bạn có thể coi cấu trúc file Linux như một hệ thống quản lý tài nguyên được tổ chức rất chặt chẽ, nơi mỗi "đối tượng" (file, thư mục) có các "thuộc tính" (quyền hạn, chủ sở hữu, thời gian) và được "truy cập" thông qua các "phương thức" (các lệnh shell).

Việc hiểu rõ FHS không chỉ giúp bạn làm việc hiệu quả hơn trên Linux mà còn là kiến thức nền tảng vững chắc cho bất kỳ ai làm việc với các hệ thống nhúng dựa trên Linux. Nó giúp bạn biết nơi để đặt các file cấu hình cho ứng dụng của mình, nơi để tìm log khi debug, và cách quản lý quyền truy cập một cách an toàn.
