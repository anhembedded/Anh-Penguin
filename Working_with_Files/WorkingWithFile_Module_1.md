
---

### **Mục tiêu của Bài học 🎯**

Sau khi hoàn thành bài học này, bạn sẽ có thể:
* Hiểu triết lý "Mọi thứ là File" trong Linux và tầm quan trọng của nó.
* Phân biệt và sử dụng hiệu quả giữa **System Calls** (cấp thấp) và **Standard I/O Library Functions** (cấp cao) để thao tác với file.
* Thực hiện các thao tác cơ bản và nâng cao với file: tạo, mở, đọc, ghi, đóng, tìm kiếm vị trí.
* Quản lý thuộc tính file: quyền hạn, chủ sở hữu, liên kết.
* Thao tác với thư mục: tạo, xóa, quét nội dung.
* Hiểu và sử dụng hệ thống file ảo `/proc` để truy cập thông tin Kernel và tiến trình.
* Nắm vững cách xử lý lỗi khi làm việc với file.
* Áp dụng các kiến thức này để giải quyết các vấn đề thực tế trong lập trình hệ thống và nhúng.

---

### **Cấu trúc Bài học 📚**

Chúng ta sẽ chia bài học này thành các Modules nhỏ để dễ theo dõi:

* **Module 1: Giới thiệu về File và Cấu trúc File trong Linux**
* **Module 2: Tương tác File Cấp thấp (System Calls)**
* **Module 3: Thư viện I/O Chuẩn (Standard I/O Library - `stdio`)**
* **Module 4: Quản lý File và Thư mục**
* **Module 5: Quét Thư mục (Directory Scanning)**
* **Module 6: Xử lý Lỗi và Hệ thống File Ảo `/proc`**
* **Module 7: Các Chủ đề Nâng cao (Advanced Topics: `fcntl` & `mmap`)**

Mỗi Module sẽ bao gồm:
* **Lý thuyết chi tiết:** Giải thích các khái niệm, hàm, và cơ chế.
* **Ví dụ Code (C/C++):** Minh họa cách sử dụng các hàm.
* **Liên hệ với Embedded Linux:** Giải thích tầm quan trọng và ứng dụng trong hệ thống nhúng.
* **Câu hỏi Tự đánh giá:** Giúp bạn kiểm tra mức độ hiểu bài.
* **Bài tập Thực hành:** Các bài tập coding để bạn áp dụng kiến thức.

Hãy bắt đầu với Module đầu tiên!

---

### **Module 1: Giới thiệu về File và Cấu trúc File trong Linux 🌳**

#### **1.1. Triết lý "Mọi thứ là File" (Everything is a File) 💡**

* **Lý thuyết:** Trong Linux (và các hệ điều hành Unix-like), đây là một triết lý thiết kế cốt lõi. Nó có nghĩa là hầu hết mọi tài nguyên trong hệ thống đều được biểu diễn dưới dạng một **file** hoặc có thể được truy cập thông qua một giao diện giống file.
    * **File thông thường (Regular Files):** Các file chứa dữ liệu (văn bản, hình ảnh, chương trình).
    * **Thư mục (Directories):** Là các file đặc biệt chứa danh sách các file và thư mục khác.
    * **Thiết bị (Devices):** Các thiết bị phần cứng (ổ đĩa, cổng nối tiếp, máy in, bàn phím) được biểu diễn dưới dạng các file đặc biệt trong thư mục `/dev`.
    * **Socket, Pipes:** Các kênh giao tiếp liên tiến trình cũng có thể được coi là file (ví dụ: Named Pipes/FIFOs).
    * **Thông tin Kernel/Tiến trình:** Được truy cập thông qua các file ảo trong `/proc` và `/sys`.
* **Ưu điểm:** Tạo ra một giao diện lập trình (API) nhất quán và đơn giản để tương tác với các tài nguyên khác nhau của hệ thống. Bạn có thể sử dụng cùng một bộ hàm (`open()`, `read()`, `write()`, `close()`) để thao tác với một file văn bản, một cổng nối tiếp hay một ổ đĩa.
* **Liên hệ Embedded Linux:** Triết lý này cực kỳ quan trọng trong nhúng. Thay vì phải học các API phức tạp riêng cho từng phần cứng, bạn có thể tương tác với GPIO, I2C, SPI, UART thông qua các file tương ứng trong `/dev` hoặc `/sys` (nếu driver hỗ trợ giao diện sysfs/procfs). Điều này đơn giản hóa việc phát triển ứng dụng và tăng tính di động.

#### **1.2. Cấu trúc Thư mục Hệ thống File Linux (Linux Filesystem Hierarchy Standard - FHS) 📂**

* **Lý thuyết:** FHS là một tiêu chuẩn xác định cấu trúc và mục đích của các thư mục chính trong hệ thống file Linux. Nó đảm bảo tính đồng nhất giữa các bản phân phối, giúp người dùng và phần mềm dễ dàng tìm thấy các file cần thiết.
* **Các thư mục chính (ôn lại):**
    * `/` (Root): Gốc của toàn bộ cây thư mục.
    * `/bin`, `/usr/bin`: Chứa các lệnh thực thi của người dùng.
    * `/sbin`, `/usr/sbin`: Chứa các lệnh thực thi của quản trị viên hệ thống.
    * `/etc`: File cấu hình hệ thống.
    * `/dev`: File thiết bị (devices).
    * `/home`: Thư mục cá nhân của người dùng.
    * `/lib`, `/usr/lib`: Thư viện dùng chung.
    * `/var`: Dữ liệu biến đổi (logs, cache).
    * `/tmp`: File tạm thời.
    * `/proc`: Hệ thống file ảo chứa thông tin tiến trình và Kernel.
    * `/sys`: Hệ thống file ảo để tương tác với thiết bị và module Kernel.
* **Liên hệ Embedded Linux:** Trên các hệ thống nhúng, FHS giúp tổ chức các file hệ thống, ứng dụng, và cấu hình một cách có trật tự. Bạn sẽ biết nơi để đặt các binary của mình, các file cấu hình, và nơi để tìm log.

#### **1.3. Inodes và Liên kết (Inodes and Links) 🔗**

* **Lý thuyết:**
    * **Inode (Index Node):** Là một cấu trúc dữ liệu trên hệ thống file Linux lưu trữ tất cả thông tin quản trị về một file hoặc thư mục, ngoại trừ tên file và dữ liệu thực tế. Thông tin này bao gồm:
        * Quyền hạn (permissions).
        * Chủ sở hữu (owner UID và GID).
        * Thời gian (tạo, sửa đổi, truy cập lần cuối).
        * Kích thước file.
        * Số lượng liên kết cứng (hard links) trỏ đến inode này.
        * Địa chỉ các khối dữ liệu vật lý trên đĩa nơi nội dung file được lưu trữ.
    * **Tên file:** Tên file mà bạn thấy trong thư mục chỉ là một "liên kết" (link) đến một inode. Một inode có thể có nhiều tên file (nhiều liên kết cứng).
    * **Hard Link (Liên kết cứng):** Một tên file khác trỏ đến **cùng một inode**. Khi bạn xóa một hard link, file thực sự chỉ bị xóa khi số lượng hard link về 0 và không còn tiến trình nào mở file đó.
    #### 📊 Hình dung đơn giản
    ```
    Tên file      →      Inode      →      Nội dung
    file1.txt     ─┐
                  ├──►  inode1234  ───►  "Xin chào"
    file2.txt     ─┘
    ```
    * **Symbolic Link (Soft Link / Symlink - Liên kết mềm):** Một file đặc biệt chứa **đường dẫn** đến một file hoặc thư mục khác. Nó giống như một shortcut. Nếu file gốc bị xóa, symlink sẽ bị "đứt".
* **Ví dụ:**
    * Bạn có thể xem inode number của file bằng `ls -i`.
    * `ln file_goc hard_link`: Tạo hard link.
    * `ln -s file_goc sym_link`: Tạo symlink.
* **Liên hệ Embedded Linux:** Hiểu về inode và link giúp bạn quản lý không gian lưu trữ hiệu quả hơn, tạo các shortcut cho các đường dẫn dài, và debug các vấn đề liên quan đến file bị xóa nhưng vẫn chiếm dụng không gian (do còn hard link hoặc tiến trình đang mở).

#### **1.4. File Thiết bị Đặc biệt (Special Device Files) 🔌**

* **Lý thuyết:** Các file trong thư mục `/dev` đại diện cho các thiết bị phần cứng hoặc các khái niệm hệ thống đặc biệt. Chúng cho phép các chương trình tương tác với phần cứng thông qua giao diện file chuẩn.
    * **`/dev/console`:** Thiết bị console của hệ thống. Các thông báo lỗi và chẩn đoán thường được gửi đến đây.
    * **`/dev/tty`:** Một alias (thiết bị logic) cho terminal điều khiển của tiến trình hiện tại. Hữu ích khi `stdout` bị chuyển hướng nhưng bạn vẫn muốn ghi trực tiếp ra terminal của người dùng.
    * **`/dev/null`:** Thiết bị "null". Mọi dữ liệu ghi vào đây đều bị loại bỏ. Đọc từ đây sẽ trả về ngay EOF. Thường dùng để loại bỏ output không mong muốn (`command > /dev/null`).
    * **`/dev/zero`:** Thiết bị "zero". Đọc từ đây sẽ trả về các byte `NULL` (0x00). Hữu ích để tạo các file chứa toàn số 0.
    * **`/dev/random`, `/dev/urandom`:** Cung cấp dữ liệu ngẫu nhiên từ Kernel.
* **Phân loại Thiết bị:**
    * **Character Devices (Thiết bị ký tự):** Xử lý dữ liệu dưới dạng luồng byte (stream of bytes) mà không có cấu trúc cố định. Ví dụ: bàn phím, chuột, cổng nối tiếp, `/dev/null`.
    * **Block Devices (Thiết bị khối):** Xử lý dữ liệu theo các khối (blocks) có kích thước cố định và hỗ trợ truy cập ngẫu nhiên. Ví dụ: ổ đĩa cứng, USB flash drive.
* **Liên hệ Embedded Linux:** Trong nhúng, bạn sẽ thường xuyên tương tác với các thiết bị thông qua các file trong `/dev`. Ví dụ: `/dev/ttyS0` cho UART, `/dev/i2c-0` cho I2C, `/dev/spidev0.0` cho SPI. Việc hiểu cách chúng hoạt động là nền tảng để viết driver hoặc ứng dụng giao tiếp phần cứng.

---

### **Câu hỏi Tự đánh giá Module 1 🤔**

1.  Giải thích ý nghĩa của triết lý "Everything is a File" trong Linux. Nêu 3 ví dụ cụ thể về việc áp dụng triết lý này.
2.  Phân biệt giữa Hard Link và Symbolic Link. Khi nào bạn nên sử dụng mỗi loại? Điều gì xảy ra nếu file gốc của một Symbolic Link bị xóa?
3.  Nêu và giải thích chức năng của ít nhất 3 thư mục quan trọng trong FHS mà bạn thường xuyên tương tác khi lập trình nhúng.
4.  `/dev/null` và `/dev/zero` khác nhau như thế nào? Nêu một trường hợp sử dụng cho mỗi thiết bị.
5.  Tại sao việc hiểu về Character Devices và Block Devices lại quan trọng đối với một kỹ sư phần mềm nhúng?

---

### **Bài tập Thực hành Module 1 ✍️**

1.  **Tạo và Thao tác với Links:**
    * Tạo một file văn bản `my_file.txt` với nội dung bất kỳ.
    * Tạo một hard link `my_hard_link` tới `my_file.txt`.
    * Tạo một symbolic link `my_sym_link` tới `my_file.txt`.
    * Sử dụng `ls -li` để xem inode numbers và quyền của cả ba file. Quan sát sự khác biệt.
    * Xóa `my_file.txt`. Quan sát trạng thái của `my_hard_link` và `my_sym_link`. Thử đọc nội dung của chúng.
    * Xóa `my_hard_link` và `my_sym_link`.
2.  **Khám phá `/dev`:**
    * Sử dụng lệnh `ls -l /dev` để liệt kê các file thiết bị. Quan sát ký tự đầu tiên trong quyền hạn (`b` cho block, `c` cho character).
    * Thử ghi một chuỗi bất kỳ vào `/dev/null` và quan sát output.
    * Thử đọc 10 byte từ `/dev/urandom` và hiển thị chúng (dùng lệnh `head` hoặc viết chương trình C nhỏ).
    * **Thử thách (C):** Viết một chương trình C nhỏ đọc 16 byte ngẫu nhiên từ `/dev/random` và in chúng ra màn hình dưới dạng hex.

---

Hãy cho tôi biết khi bạn đã sẵn sàng để chuyển sang Module 2!