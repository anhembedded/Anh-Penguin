

-----

### **Giáo Trình Tự Học: Lập Trình Hệ Thống File trên Linux với Ngôn ngữ C**

**Lời Mở Đầu**

Chào mừng bạn, một kỹ sư với nền tảng C++ và tư duy hệ thống, đến với thế giới lập trình cấp thấp trên Linux. Khác với lập trình ứng dụng thông thường, lập trình hệ thống đòi hỏi chúng ta phải hiểu sâu sắc cách hệ điều hành hoạt động.

Giáo trình này được thiết kế để trở thành người cố vấn đồng hành cùng bạn. Chúng ta sẽ mổ xẻ chương "Working with Files", biến những dòng text dày đặc thành một lộ trình học tập rõ ràng, thực tiễn và có chiều sâu. Mục tiêu cuối cùng không chỉ là "biết" các hàm, mà là "thành thạo" tư duy và kỹ năng để tương tác với hệ thống file Linux một cách hiệu quả.

**Điều kiện tiên quyết:**

  * Kiến thức cơ bản về ngôn ngữ C (biến, con trỏ, hàm, cấu trúc).
  * Môi trường phát triển Linux (hoặc WSL trên Windows) với trình biên dịch `gcc`.
  * Một tinh thần sẵn sàng "làm bẩn tay" với code và dòng lệnh.

**Lộ trình của chúng ta:**

  * **Chương 1:** Khám phá triết lý và các khối xây dựng cơ bản của hệ thống file Linux.
  * **Chương 2:** Giao tiếp trực tiếp với Kernel qua các System Calls cấp thấp.
  * **Chương 3:** Tối ưu hóa hiệu năng với Thư viện I/O Chuẩn (`stdio`).
  * **Chương 4:** Trở thành người quản lý thực thụ: Thao tác với file và thư mục.
  * **Dự án tổng kết:** Xây dựng một tiện ích dòng lệnh của riêng bạn.

-----

### **Chương 1: Triết lý & Nền tảng Hệ thống File Linux**

Trước khi viết dòng code đầu tiên, việc hiểu "tại sao" và "như thế nào" là cực kỳ quan trọng. Chương này đặt nền móng tư duy cho mọi thứ chúng ta sẽ làm sau này.

#### **1.1. "Trong Linux, Mọi thứ là một File"**

Đây không chỉ là một câu nói cửa miệng, mà là một triết lý thiết kế cốt lõi. Nó có nghĩa là kernel Linux cố gắng trừu tượng hóa mọi tài nguyên thành một giao diện nhất quán: giao diện file.

  * **File thông thường:** `my_document.txt` (chứa dữ liệu)
  * **Thư mục:** `/home/hoanganh` (một file đặc biệt liệt kê các file khác)
  * **Thiết bị phần cứng:** `/dev/sda1` (phân vùng ổ cứng), `/dev/tty` (terminal của bạn).
  * **Thông tin Kernel:** `/proc/cpuinfo` (thông tin CPU), `/proc/meminfo` (thông tin bộ nhớ).
  * **Kết nối mạng:** (Một trong số ít ngoại lệ, sử dụng giao diện *socket*, nhưng cũng có thể được trừu tượng hóa).

**Tại sao điều này lại quan trọng?**
Nó mang lại sự **đơn giản** và **nhất quán**. Bạn có thể dùng cùng một bộ công cụ cơ bản (`open`, `read`, `write`, `close`) để đọc một file text, gửi lệnh đến máy in, hay lấy thông tin từ kernel.

#### **1.2. Inode: "Linh hồn" của File**

Khi bạn tạo một file, bạn thấy một cái tên trong một thư mục. Nhưng đối với hệ thống, "danh tính" thực sự của file nằm ở **inode** (index node).

Hãy tưởng tượng **inode** như một tấm thẻ trong thư viện. Tấm thẻ này chứa tất cả thông tin về cuốn sách, *ngoại trừ tên của nó*.

  * **Metadata trong Inode:**
      * **Kiểu file:** Là file thường, thư mục, hay symbolic link?
      * **Quyền truy cập:** Ai được đọc, ghi, thực thi? (`rwx`)
      * **Chủ sở hữu:** User ID (UID) và Group ID (GID).
      * **Kích thước file:** Tính bằng byte.
      * **Dấu thời gian:** Thời gian truy cập, sửa đổi, thay đổi trạng thái.
      * **Link Count:** Có bao nhiêu tên file đang trỏ đến inode này.
      * **Con trỏ đến các khối dữ liệu:** Vị trí thực sự của nội dung file trên ổ đĩa.

**Thư mục** chính là cuốn sổ ghi chép ánh xạ "tên sách" (tên file) tới "số thẻ thư viện" (số inode).

```bash
# Dùng `ls -i` để xem số inode
$ ls -i my_file.txt
1312345 my_file.txt
```

Số `1312345` chính là "danh tính" thật của file.

#### **1.3. Hard Links vs. Symbolic Links**

Hiểu về inode giúp ta phân biệt hai loại link này một cách dễ dàng:

  * **Hard Link (`ln`):**

      * Tạo ra một **tên mới** cho **cùng một inode**.
      * Giống như việc dán thêm một nhãn tên nữa lên cùng một cuốn sách.
      * Cả hai tên đều "bình đẳng". Xóa một tên không ảnh hưởng đến tên kia.
      * File chỉ thực sự bị xóa khỏi đĩa khi `link count` trong inode giảm về 0.
      * Không thể tạo hard link cho thư mục và không thể link qua các filesystem khác nhau.

  * **Symbolic Link (hay soft link, `ln -s`):**

      * Tạo ra một **file mới** (có inode riêng) mà nội dung của nó chỉ là **đường dẫn** đến file gốc.
      * Giống như tạo một mẩu giấy ghi "Cuốn sách X đang ở kệ A".
      * Nếu file gốc bị xóa, symbolic link sẽ bị "hỏng" (broken link).
      * Có thể link tới thư mục và link qua các filesystem khác nhau.

#### **1.4. Lab 1: Khám phá Filesystem qua Dòng lệnh**

Mở terminal của bạn và thực hành các lệnh sau để củng cố kiến thức.

1.  **Xem Inode và tạo Hard link:**

    ```bash
    cd ~
    echo "Nội dung gốc" > file_goc.txt
    ls -i file_goc.txt
    ln file_goc.txt file_hardlink.txt
    ls -li file_goc.txt file_hardlink.txt  # Chú ý số inode và link count (số 2)
    rm file_goc.txt
    cat file_hardlink.txt  # Nội dung vẫn còn!
    ```

2.  **Tạo Symbolic link:**

    ```bash
    ln -s file_hardlink.txt file_symlink.txt
    ls -li file_hardlink.txt file_symlink.txt # Chú ý inode khác nhau, và quyền 'l'
    rm file_hardlink.txt
    cat file_symlink.txt # Lỗi! "No such file or directory"
    ```

3.  **Tương tác với device files:**

    ```bash
    # Ghi dữ liệu vào hư không
    echo "this will vanish" > /dev/null

    # Tạo một file 10MB chứa toàn byte 0 (hữu ích để test)
    dd if=/dev/zero of=test_file_10M bs=1M count=10

    # Xem thông tin CPU
    cat /proc/cpuinfo
    ```

-----

### **Chương 2: Giao tiếp với Kernel: System Calls Cấp thấp**

Bây giờ, chúng ta sẽ viết code C để thực hiện các thao tác mà chúng ta vừa làm trên dòng lệnh. Chúng ta sẽ dùng **System Calls**, là các hàm C đặc biệt để yêu cầu kernel thực hiện một tác vụ.

#### **2.1. System Calls và File Descriptors**

Khi chương trình của bạn muốn đọc một file, nó không thể tự ý truy cập ổ cứng. Nó phải **yêu cầu kernel** làm điều đó hộ. Lời yêu cầu này chính là một *system call*.

Khi bạn yêu cầu kernel mở một file, nếu thành công, kernel sẽ không trả lại cho bạn cả cái file. Thay vào đó, nó trả về một "vé nhận đồ" - một số nguyên không âm gọi là **File Descriptor**.

> **File Descriptor (fd):** Một số nguyên nhỏ (`int`) đại diện cho một file đã được mở bởi một process. Nó là "tay cầm" để bạn nói với kernel: "Này, tôi muốn đọc/ghi vào cái file tương ứng với vé số 5 này."

Mọi process Linux khi khởi chạy đều được tặng sẵn 3 file descriptor:

  * `0`: Standard Input (`stdin`) - Thường là bàn phím.
  * `1`: Standard Output (`stdout`) - Thường là màn hình terminal.
  * `2`: Standard Error (`stderr`) - Cũng thường là màn hình terminal.

#### **2.2. Các System Calls Thiết Yếu**

Đây là bộ tứ siêu đẳng của I/O cấp thấp. Bạn sẽ cần include `<unistd.h>`, `<fcntl.h>`, `<sys/stat.h>`.

  * **`int open(const char *path, int oflags, mode_t mode)`**

      * `path`: Đường dẫn đến file.
      * `oflags`: Cờ xác định cách mở file (bitwise OR):
          * `O_RDONLY`: Chỉ đọc.
          * `O_WRONLY`: Chỉ ghi.
          * `O_RDWR`: Đọc và ghi.
          * `O_CREAT`: Tạo file nếu chưa tồn tại.
          * `O_APPEND`: Ghi vào cuối file.
          * `O_TRUNC`: Xóa sạch nội dung file nếu nó đã tồn tại.
      * `mode`: Quyền truy cập (ví dụ `0644`) nếu file được tạo mới (cần cờ `O_CREAT`).
      * **Trả về:** File descriptor mới, hoặc `-1` nếu lỗi.

  * **`ssize_t read(int fd, void *buf, size_t nbytes)`**

      * Đọc tối đa `nbytes` từ `fd` vào buffer `buf`.
      * **Trả về:** Số byte thực sự đọc được. `0` nếu đã đến cuối file (EOF). `-1` nếu lỗi.

  * **`ssize_t write(int fd, const void *buf, size_t nbytes)`**

      * Ghi `nbytes` từ buffer `buf` vào `fd`.
      * **Trả về:** Số byte thực sự ghi được (có thể ít hơn `nbytes`\!). `-1` nếu lỗi.

  * **`int close(int fd)`**

      * Đóng file descriptor, giải phóng tài nguyên.
      * **Trả về:** `0` nếu thành công, `-1` nếu lỗi.

#### **2.3. Luôn luôn kiểm tra lỗi\!**

Lập trình hệ thống không có chỗ cho sự lạc quan. Mọi system call đều có thể thất bại (đĩa đầy, không có quyền, file không tồn tại...). Luôn kiểm tra giá trị trả về. Nếu là `-1`, biến toàn cục `errno` sẽ được thiết lập. Hàm `perror()` là bạn thân của bạn để in ra lỗi một cách tường minh.

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int fd = open("non_existent_file.txt", O_RDONLY);
    if (fd == -1) {
        // Thay vì chỉ in "Error", hãy dùng perror
        perror("open failed"); 
        exit(1);
    }
    // ... làm gì đó với fd ...
    close(fd);
    return 0;
}
```

Biên dịch và chạy file trên, bạn sẽ thấy output: `open failed: No such file or directory`. Rất rõ ràng\!

#### **2.4. Vấn đề Hiệu năng: Block I/O \> Character I/O**

Tài liệu có một ví dụ kinh điển: copy file bằng cách đọc/ghi từng ký tự một (`nbytes = 1`) so với đọc/ghi theo khối (`nbytes = 1024`).

**Tại sao theo khối lại nhanh hơn hàng ngàn lần?**
Mỗi lần gọi `read()` hay `write()` là một lần chuyển ngữ cảnh (context switch) tốn kém từ user space sang kernel space và ngược lại.

  * Copy file 1MB từng ký tự: \~2 triệu system calls.
  * Copy file 1MB theo khối 1KB: \~2 ngàn system calls.

\=\> **Quy tắc vàng:** Tối ưu hóa số lượng system calls bằng cách cho mỗi call làm nhiều việc nhất có thể (đọc/ghi khối dữ liệu lớn).

#### **2.5. Lab 2: Xây dựng các Công cụ Dòng lệnh Đơn giản**

Hãy áp dụng kiến thức trên để xây dựng các tiện ích của riêng bạn. **Yêu cầu:** Mọi chương trình đều phải kiểm tra lỗi cẩn thận sau mỗi system call.

1.  **`my_touch.c`**:

      * **Mục tiêu:** Tạo ra một file rỗng, giống lệnh `touch`.
      * **Hướng dẫn:** Nhận tên file từ `argv[1]`. Gọi `open()` với cờ `O_WRONLY | O_CREAT | O_TRUNC` và mode `0664`. Đừng quên `close()` ngay sau đó.
      * **Chạy thử:** `./my_touch new_file.txt`

2.  **`my_cat.c`**:

      * **Mục tiêu:** In nội dung của một file ra màn hình, giống lệnh `cat`.
      * **Hướng dẫn:**
          * Mở file được chỉ định trong `argv[1]` với `O_RDONLY`.
          * Tạo một buffer (ví dụ `char buffer[4096];`).
          * Bắt đầu một vòng lặp `while`. Trong mỗi lần lặp, gọi `read()` để đọc dữ liệu vào buffer.
          * Nếu `read()` trả về \> 0, gọi `write()` để ghi số byte vừa đọc được ra `stdout` (fd = 1).
          * Nếu `read()` trả về 0, thoát vòng lặp.
          * Nếu `read()` trả về -1, báo lỗi và thoát.
          * Đóng file khi xong.
      * **Chạy thử:** `./my_cat my_cat.c` (để nó tự in source code của chính nó).

3.  **`my_append.c` (Thử thách)**:

      * **Mục tiêu:** Nối một chuỗi vào cuối một file.
      * **Hướng dẫn:** Chương trình nhận 2 đối số: tên file và chuỗi cần nối.
      * Mở file với cờ `O_WRONLY | O_CREAT | O_APPEND`.
      * Dùng `write()` để ghi chuỗi (và có thể cả ký tự xuống dòng `\n`) vào file.
      * **Chạy thử:** `./my_append my_log.txt "Server started."`

-----

Chắc chắn rồi. Chúng ta sẽ tiếp tục xây dựng toàn bộ giáo trình. Hãy sẵn sàng cho một lượng kiến thức có chiều sâu nhé\!

-----

### **Giáo Trình Tự Học: Lập Trình Hệ Thống File trên Linux với Ngôn ngữ C (Tiếp theo)**

### **Chương 3: Tối ưu hóa với Thư viện I/O Chuẩn (stdio)**

Ở Chương 2, chúng ta đã làm việc trực tiếp với Kernel. Cách làm đó rất mạnh mẽ và cho ta toàn quyền kiểm soát, nhưng cũng khá "thô" và đòi hỏi phải tự quản lý buffer để có hiệu năng tốt. Giờ đây, chúng ta sẽ bước lên một tầng cao hơn, sử dụng một trong những thư viện C kinh điển và mạnh mẽ nhất: **Standard I/O Library (`stdio.h`)**.

#### **3.1. Tại sao cần `stdio`? Giới thiệu về Streams và Buffering**

Hãy tưởng tượng bạn cần gửi 1000 lá thư.

  * **Cách của System Call (Chương 2):** Bạn chạy ra bưu điện 1000 lần, mỗi lần gửi một lá. Rất tốn công\!
  * **Cách của `stdio`:** Bạn gom 1000 lá thư vào một cái túi lớn, rồi chỉ chạy ra bưu điện một lần duy nhất để gửi cả túi.

Cái "túi lớn" đó chính là **buffer** (bộ đệm), và `stdio` giúp chúng ta quản lý nó một cách tự động. Thay vì làm việc với `int fd` (file descriptor), `stdio` giới thiệu một khái niệm trừu tượng hơn: **`FILE*` stream**.

> **`FILE*` stream:** Là một con trỏ tới một cấu trúc `FILE` do `stdio` quản lý. Cấu trúc này không chỉ chứa file descriptor bên dưới, mà còn chứa cả buffer, con trỏ vị trí trong buffer, và các cờ trạng thái (lỗi, cuối file).

**Lợi ích của Buffering:**

  * **Tăng hiệu năng 🚀:** Giảm đáng kể số lượng system calls `read`/`write` tốn kém. Dữ liệu được đọc/ghi theo từng khối lớn vào buffer, tối ưu hóa hoạt động của ổ đĩa.
  * **Dễ sử dụng hơn:** Cung cấp các hàm I/O tiện lợi hơn nhiều (ví dụ đọc cả dòng, ghi dữ liệu có định dạng).

#### **3.2. Các Thao tác Cơ bản với Stream**

Đây là những hàm tương ứng với bộ tứ `open/close/read/write` nhưng làm việc với `FILE*`.

  * **`FILE *fopen(const char *filename, const char *mode)`**

      * `mode` bây giờ là một chuỗi, không phải cờ bitwise:
          * `"r"`: Read only. File phải tồn tại.
          * `"w"`: Write only. Tạo file mới hoặc xóa sạch nội dung file cũ.
          * `"a"`: Append. Ghi vào cuối file. Tạo file nếu chưa có.
          * Thêm `+` (ví dụ `"r+"`, `"w+"`) để cho phép cả đọc và ghi.
          * Thêm `b` (ví dụ `"rb"`, `"wb"`) cho file nhị phân (trên Linux không có sự khác biệt, nhưng tốt cho tính tương thích).
      * **Trả về:** Con trỏ `FILE*` hoặc `NULL` nếu lỗi.

  * **`int fclose(FILE *stream)`**

      * Đóng stream. Trước khi đóng, nó sẽ tự động **flush** (đẩy) nốt phần dữ liệu còn lại trong buffer ghi vào file. Đây là lý do `fclose` rất quan trọng\!
      * **Trả về:** `0` nếu thành công, `EOF` nếu lỗi.

  * **I/O từng ký tự:**

      * `int fgetc(FILE *stream)`: Đọc một ký tự.
      * `int fputc(int c, FILE *stream)`: Ghi một ký tự.

  * **I/O theo khối (tương tự read/write):**

      * `size_t fread(void *ptr, size_t size, size_t nitems, FILE *stream)`
      * `size_t fwrite(const void *ptr, size_t size, size_t nitems, FILE *stream)`
          * `ptr`: Con trỏ đến buffer của bạn.
          * `size`: Kích thước của một phần tử.
          * `nitems`: Số lượng phần tử cần đọc/ghi.
          * **Trả về:** Số lượng phần tử (`nitems`) thực sự được đọc/ghi.

#### **3.3. I/O theo Dòng và có Định dạng**

Đây là nơi `stdio` thực sự tỏa sáng.

  * **`char *fgets(char *s, int n, FILE *stream)`**

      * Đọc một dòng từ `stream` vào chuỗi `s`.
      * Nó sẽ dừng khi đọc được `n-1` ký tự, gặp ký tự xuống dòng `\n`, hoặc hết file.
      * **Cực kỳ an toàn** vì nó giới hạn số ký tự đọc được, chống lại lỗi buffer overflow.
      * **Tuyệt đối không bao giờ dùng `gets()`\!** Hàm này không có tham số giới hạn kích thước, là một trong những lỗ hổng bảo mật kinh điển nhất trong C.

  * **`int fprintf(FILE *stream, const char *format, ...)`**

      * Hoạt động y hệt `printf`, nhưng ghi kết quả ra `stream` bạn chọn thay vì `stdout`.

#### **3.4. Kiểm tra Lỗi và Trạng thái Stream**

  * `int ferror(FILE *stream)`: Trả về giá trị khác 0 nếu có lỗi xảy ra trên stream.
  * `int feof(FILE *stream)`: Trả về giá trị khác 0 nếu đã đọc đến cuối file.
  * `void clearerr(FILE *stream)`: Xóa cờ lỗi và cờ EOF trên stream.

#### **3.5. Lab 3: Viết lại các Công cụ với `stdio`**

Bây giờ hãy xem code của chúng ta trở nên gọn gàng và dễ đọc hơn như thế nào.

1.  **`my_cat_stdio.c`**:

      * **Mục tiêu:** Viết lại `my_cat` từ Lab 2 bằng `stdio`.
      * **Hướng dẫn:**
        ```c
        #include <stdio.h>
        #include <stdlib.h>

        #define BUFFER_SIZE 4096

        int main(int argc, char *argv[]) {
            if (argc != 2) {
                fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
                exit(1);
            }

            FILE *fp = fopen(argv[1], "r");
            if (fp == NULL) {
                perror("fopen failed");
                exit(1);
            }

            char buffer[BUFFER_SIZE];
            size_t bytes_read;
            
            // Dùng fread/fwrite để copy theo khối
            while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
                fwrite(buffer, 1, bytes_read, stdout);
            }

            if (ferror(fp)) {
                perror("Error reading file");
            }

            fclose(fp);
            return 0;
        }
        ```
      * **So sánh:** Chú ý code xử lý vòng lặp đơn giản hơn nhiều so với việc phải kiểm tra giá trị trả về của `read` và `write` một cách thủ công.

2.  **`config_parser.c`**:

      * **Mục tiêu:** Đọc một file cấu hình dạng `key=value` và in ra.
      * **File cấu hình mẫu (`config.ini`):**
        ```ini
        HOST=127.0.0.1
        PORT=8080
        USER=hoanganh
        ```
      * **Hướng dẫn:** Dùng `fopen()` để mở file. Dùng vòng lặp với `fgets()` để đọc từng dòng. Với mỗi dòng, bạn có thể dùng `sscanf(line, "%[^=]=%s", key, value)` hoặc `strtok()` để tách key và value.

3.  **`csv_writer.c` (Thử thách)**:

      * **Mục tiêu:** Tạo ra một file CSV chứa dữ liệu của một vài user.
      * **Hướng dẫn:** Tạo một `struct` User. Tạo một mảng các User. Dùng `fopen` với mode `"w"` để tạo file `users.csv`. Dùng `fprintf()` để ghi dòng tiêu đề, sau đó lặp qua mảng và `fprintf` thông tin mỗi user ra file, phân cách bằng dấu phẩy.

-----

### **Chương 4: Quản lý Hệ thống File**

Chúng ta đã biết đọc và ghi. Bây giờ là lúc học cách làm những việc mà một file manager làm: lấy thông tin file, đổi quyền, tạo thư mục, và liệt kê nội dung.

#### **4.1. `stat()` - "Soi" Metadata của File**

Đây là công cụ cực kỳ mạnh mẽ để lấy thông tin từ inode của một file.

  * **`int stat(const char *path, struct stat *buf)`**: Lấy thông tin của file qua đường dẫn.
  * **`int lstat(const char *path, struct stat *buf)`**: Giống `stat`, nhưng nếu `path` là symbolic link, nó sẽ lấy thông tin của chính cái link, **không đi theo link**. Rất quan trọng khi bạn muốn biết một thứ có phải là symlink hay không.
  * **`int fstat(int fd, struct stat *buf)`**: Lấy thông tin qua file descriptor đã mở.

Cấu trúc `struct stat` chứa rất nhiều thứ hay ho:

  * `st_mode`: Kiểu file và quyền.
  * `st_size`: Kích thước (kiểu `off_t`).
  * `st_uid`, `st_gid`: ID của chủ sở hữu và nhóm.
  * `st_nlink`: Số hard link.
  * `st_mtime`: Thời gian sửa đổi cuối (kiểu `time_t`).

Để làm việc với `st_mode`, hãy dùng các macro được định nghĩa sẵn trong `<sys/stat.h>`:

  * `S_ISREG(m)`: Có phải file thường?
  * `S_ISDIR(m)`: Có phải thư mục?
  * `S_ISLNK(m)`: Có phải symbolic link?
  * ... và nhiều macro khác (`S_ISCHR`, `S_ISBLK`, ...).

#### **4.2. Thao tác với File và Thư mục**

Các hàm này là phiên bản C của các lệnh `chmod`, `rm`, `mkdir`, `rmdir`, `cd`.

  * **`int chmod(const char *path, mode_t mode)`**: Thay đổi quyền của file. `mode` là một số bát phân, ví dụ `0755`.
  * **`int unlink(const char *path)`**: Giảm link count của inode đi 1. Nếu về 0, file bị xóa. Đây là system call đằng sau lệnh `rm`.
  * **`int mkdir(const char *path, mode_t mode)`**: Tạo thư mục mới.
  * **`int rmdir(const char *path)`**: Xóa một thư mục **rỗng**.
  * **`int chdir(const char *path)`**: Thay đổi thư mục làm việc hiện tại của process.
  * **`char *getcwd(char *buf, size_t size)`**: Lấy đường dẫn thư mục làm việc hiện tại.

#### **4.3. Quét Thư mục: Khám phá thế giới bên trong**

Làm sao để `ls` biết có những gì trong một thư mục? Nó dùng bộ ba hàm này:

  * **`DIR *opendir(const char *name)`**: Mở một thư mục, trả về một con trỏ `DIR*` (tương tự `FILE*`).
  * **`struct dirent *readdir(DIR *dirp)`**: Đọc entry tiếp theo trong thư mục. Trả về một con trỏ tới `struct dirent`, hoặc `NULL` nếu hết hoặc lỗi. `struct dirent` chứa `d_name` (tên file/thư mục con).
  * **`int closedir(DIR *dirp)`**: Đóng thư mục.

**Mô hình quét thư mục kinh điển:**

```c
#include <dirent.h>
#include <stdio.h>
#include <string.h>

// ...

DIR *dp = opendir("."); // Mở thư mục hiện tại
if (dp == NULL) {
    perror("opendir");
    return 1;
}

struct dirent *entry;
while ((entry = readdir(dp)) != NULL) {
    // Bỏ qua "." và ".." để tránh đệ quy vô hạn
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
    }
    printf("%s\n", entry->d_name);
}

closedir(dp);
```

#### **4.4. Lab 4: Xây dựng `ls -l` phiên bản đơn giản**

Đây là một bài tập tổng hợp cực kỳ giá trị, kết hợp tất cả kiến thức của chương này.

  * **Mục tiêu:** Viết chương trình `my_ls_l` nhận một đường dẫn thư mục và liệt kê nội dung theo định dạng dài.
  * **Hướng dẫn:**
    1.  Dùng `opendir`/`readdir` để lặp qua các entry trong thư mục.
    2.  Với mỗi `entry->d_name`, bạn cần xây dựng đường dẫn đầy đủ của nó (ví dụ, nếu đang quét `/home` và `d_name` là `hoanganh`, đường dẫn đầy đủ là `/home/hoanganh`). Bạn có thể dùng `sprintf` để làm việc này.
    3.  Gọi `lstat()` trên đường dẫn đầy đủ đó để lấy `struct stat`.
    4.  Từ `struct stat`, bạn cần lấy và định dạng các thông tin sau để in ra một dòng:
          * **Kiểu file và quyền:** Phân tích `st_mode`. In ra `-` cho file thường, `d` cho thư mục, `l` cho symlink. Sau đó in quyền `rwx` cho user, group, other.
          * **Số hard link:** `st_nlink`.
          * **Tên user và group:** `st_uid` và `st_gid` chỉ là số. Bạn sẽ cần dùng các hàm `getpwuid()` và `getgrgid()` (xem `man getpwuid`) để lấy tên.
          * **Kích thước:** `st_size`.
          * **Thời gian sửa đổi:** Dùng `ctime()` hoặc `strftime()` để định dạng `st_mtime`.
          * **Tên file:** `entry->d_name`.

-----

### **Dự án Tổng kết: Xây dựng `my_cp -r`**

Giờ là lúc kết hợp tất cả lại để xây dựng một tiện ích thực sự hữu dụng.

**Mô tả:**
Chương trình `my_cp` sẽ có khả năng copy file và thư mục một cách đệ quy.
`./my_cp [-r] source destination`

**Luồng logic:**

1.  **Phân tích đối số (Argument Parsing):**

      * Kiểm tra `argc` để đảm bảo có đủ đối số.
      * Kiểm tra xem có cờ `-r` hay không.

2.  **Phân tích `source` và `destination`:**

      * Dùng `stat()` hoặc `lstat()` để kiểm tra `source` và `destination`.
      * Có nhiều trường hợp cần xử lý:
          * Copy file -\> file
          * Copy file -\> thư mục
          * Copy thư mục -\> thư mục (chỉ khi có `-r`)

3.  **Hàm `copy_file(src_path, dest_path)`:**

      * Đây là phần bạn đã làm ở các lab trước. Dùng `open/read/write` hoặc `fopen/fread/fwrite` để sao chép nội dung.
      * **Thử thách:** Sau khi copy xong, dùng `stat()` trên file nguồn và `chmod()` trên file đích để sao chép cả quyền truy cập.

4.  **Hàm `copy_directory_recursive(src_path, dest_path)`:**

      * Đây là phần cốt lõi của việc copy đệ quy.
      * Dùng `mkdir()` để tạo thư mục `dest_path`.
      * Dùng `opendir()` trên `src_path`.
      * Lặp qua các entry bằng `readdir()`. Nhớ bỏ qua `.` và `..`.
      * Với mỗi entry:
          * Xây dựng đường dẫn nguồn và đích đầy đủ cho entry đó.
          * Dùng `lstat()` để kiểm tra entry là file hay thư mục.
          * Nếu là file, gọi `copy_file()`.
          * Nếu là thư mục, **gọi đệ quy** chính hàm `copy_directory_recursive()`.
      * `closedir()` khi xong việc.

**Lời khuyên:**

  * Chia chương trình thành các hàm nhỏ, có chức năng rõ ràng.
  * Xử lý lỗi ở mọi nơi\! Việc copy file có thể thất bại ở nhiều bước.
  * Bắt đầu với chức năng copy file đơn giản, sau đó mới xây dựng phần copy thư mục.

-----

**Lời Kết**

Chúc mừng bạn đã hoàn thành giáo trình này\! Bạn đã đi một chặng đường dài từ việc hiểu triết lý của Linux đến việc xây dựng các công cụ hệ thống của riêng mình. Bạn không chỉ học các hàm, mà đã học cách "suy nghĩ" như một lập trình viên hệ thống: chú ý đến hiệu năng, quản lý tài nguyên cẩn thận và luôn sẵn sàng cho các trường hợp lỗi.

**Các bước tiếp theo có thể là gì?**

  * **File Locking (`fcntl`)**: Tìm hiểu cách các process khác nhau có thể truy cập cùng một file một cách an toàn.
  * **Inter-Process Communication (IPC)**: Khám phá cách các process nói chuyện với nhau qua `pipes`, `sockets`.
  * **Memory Mapping (`mmap`)**: Một kỹ thuật I/O nâng cao và cực kỳ hiệu quả.

Con đường lập trình hệ thống rất thử thách nhưng cũng vô cùng thú vị. Hy vọng giáo trình này đã trang bị cho bạn những nền tảng vững chắc để tiếp tục khám phá.

**Happy coding\!**