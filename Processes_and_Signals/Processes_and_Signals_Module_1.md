# **Giáo trình: Processes and Signals 🏃‍♂️🚦**

**Mục tiêu của Giáo trình 🎯**

Sau khi hoàn thành giáo trình này, bạn sẽ có thể:

* Hiểu định nghĩa và cấu trúc của một tiến trình trong Linux.
* Nắm vững các loại tiến trình hệ thống và cách Kernel lập lịch chúng.
* Biết cách khởi động các tiến trình mới (`system()`, `fork()`, `exec()`) và phân biệt các phương pháp này.
* Quản lý mối quan hệ giữa tiến trình cha và con, bao gồm việc chờ đợi và tránh "zombie processes".
* Hiểu rõ khái niệm tín hiệu, các loại tín hiệu phổ biến, và cách gửi/nhận/xử lý chúng.
* Áp dụng kiến thức về tiến trình và tín hiệu để phát triển các ứng dụng Linux mạnh mẽ, có khả năng phản hồi và quản lý tài nguyên hiệu quả, đặc biệt trong môi trường nhúng.

---

### **Cấu trúc Giáo trình 📚**

Giáo trình này sẽ được chia thành các Modules nhỏ để dễ dàng theo dõi và tiếp thu:

* **Module 1: Tiến trình là gì? (What Is a Process?)**
* **Module 2: Khởi động Tiến trình Mới (Starting New Processes)**
* **Module 3: Quản lý Tiến trình Con (Child Process Management)**
* **Module 4: Signals (Tín hiệu)**
* **Module 5: Luyện tập Tổng hợp & Ứng dụng**

Mỗi Module sẽ bao gồm:

* **Lý thuyết chi tiết:** Giải thích các khái niệm, hàm, và cơ chế.
* **Ví dụ Code (C++):** Minh họa cách sử dụng các hàm.
* **Liên hệ với Embedded Linux:** Giải thích tầm quan trọng và ứng dụng trong hệ thống nhúng.
* **Câu hỏi Tự đánh giá:** Giúp bạn kiểm tra mức độ hiểu bài.
* **Bài tập Thực hành:** Các bài tập coding để bạn áp dụng kiến thức.

Hãy bắt đầu với Module đầu tiên!

---

### **Module 1: Tiến trình là gì? (What Is a Process?) 🏃‍♂️**

Trong Linux, mọi hoạt động đều xoay quanh khái niệm  **tiến trình** . Hiểu rõ tiến trình là chìa khóa để làm chủ hệ thống.

#### **1.1. Định nghĩa và Cấu trúc Tiến trình (Definition and Process Structure) 🏗️**

* **Lý thuyết:**
  * Một **tiến trình (process)** là một  **chương trình đang thực thi** . Nó là một thể độc lập, tự chứa (self-contained entity) bao gồm không chỉ mã lệnh mà còn tất cả các tài nguyên cần thiết để chạy.
  * **Thực thể đa nhiệm:** Linux là một hệ điều hành đa nhiệm (multitasking), cho phép nhiều chương trình (tiến trình) chạy đồng thời (hoặc dường như đồng thời) và chia sẻ tài nguyên hệ thống (CPU, bộ nhớ, đĩa).
  * **Thực thể đa người dùng:** Linux cũng là hệ thống đa người dùng, cho phép nhiều người dùng chạy nhiều tiến trình cùng lúc.
  * **Thành phần của một Tiến trình:** Một tiến trình bao gồm:
    * **Mã lệnh chương trình (Program Code / Text Segment):** Mã thực thi của chương trình. Vùng này là **chỉ đọc (read-only)** và có thể được **chia sẻ** giữa nhiều tiến trình nếu chúng cùng thực thi một chương trình (ví dụ: nhiều người dùng chạy `grep` cùng lúc).
    * **Dữ liệu (Data Segment / BSS Segment):** Chứa các biến toàn cục và biến tĩnh. Vùng này là riêng biệt cho mỗi tiến trình.
    * **Heap:** Vùng bộ nhớ động, nơi `malloc()`/`new` cấp phát bộ nhớ. Là riêng biệt cho mỗi tiến trình.
    * **Stack:** Vùng bộ nhớ cho các biến cục bộ, tham số hàm, địa chỉ trả về. Là riêng biệt cho mỗi tiến trình.
    * **File Descriptors:** Tập hợp các file và thiết bị mà tiến trình đang mở. Là riêng biệt cho mỗi tiến trình.
    * **Môi trường (Environment):** Các biến môi trường (`PATH`, `HOME`, v.v.) được kế thừa từ tiến trình cha.
    * **Con trỏ lệnh (Program Counter):** Ghi lại vị trí hiện tại của tiến trình trong quá trình thực thi.
    * **Luồng thực thi (Execution Thread):** Mỗi tiến trình có ít nhất một luồng. (Chúng ta sẽ đi sâu vào Threads trong chương riêng).
  * **Process ID (PID):** Một số nguyên **duy nhất** được Kernel gán cho mỗi tiến trình (thường từ 2 đến 32768, sau đó quay vòng). PID 1 thường dành riêng cho tiến trình `init` (hoặc `systemd`).
  * **Process Table (Bảng Tiến trình):** Kernel duy trì một cấu trúc dữ liệu mô tả tất cả các tiến trình đang hoạt động, sử dụng PID làm chỉ mục. Kích thước của bảng này là có hạn, nhưng trên các hệ thống hiện đại thì rất lớn, chủ yếu bị giới hạn bởi bộ nhớ khả dụng.
* **Liên hệ Embedded Linux:**
  * Hiểu cấu trúc tiến trình là cơ bản để phân tích việc sử dụng bộ nhớ (stack, heap, code/data size) của ứng dụng trên các thiết bị nhúng có RAM hạn chế.
  * Khái niệm chia sẻ mã lệnh và thư viện giúp tiết kiệm RAM khi nhiều ứng dụng dựa trên cùng một bộ thư viện.

#### **1.2. Xem Tiến trình (Viewing Processes) 🔍**

* **Lý thuyết:** Các lệnh sau đây cho phép bạn xem thông tin về các tiến trình đang chạy:

  * **`ps` (Process Status):** Hiển thị ảnh chụp (snapshot) các tiến trình tại một thời điểm.
    * `ps -f`: Hiển thị định dạng đầy đủ (full format), bao gồm `UID`, `PID`, `PPID`, `C` (CPU usage), `STIME` (start time), `TTY`, `TIME` (CPU time), `CMD`.
    * `ps -ef`: Hiển thị tất cả tiến trình trên hệ thống ở định dạng đầy đủ (mô hình System V).
    * `ps aux`: Hiển thị tất cả tiến trình trên hệ thống ở định dạng chuẩn BSD (user-oriented).
  * **`top`:** Cung cấp chế độ xem động, cập nhật liên tục về các tiến trình hàng đầu (top processes) tiêu thụ tài nguyên (CPU, RAM).
  * **`htop`:** Phiên bản tương tác và thân thiện hơn của `top`, có giao diện màu sắc và dễ quản lý hơn.
  * **`/proc` file system:** Một hệ thống file ảo (đã học ở chương "Working with Files") nơi mỗi thư mục con có tên là một PID chứa thông tin chi tiết về tiến trình đó (ví dụ: `/proc/<PID>/cmdline`, `/proc/<PID>/status`).
* **Liên hệ Embedded Linux:** Các lệnh `ps`, `top`, `htop` là công cụ debug và giám sát thiết yếu trên thiết bị nhúng. Khi bạn SSH vào một thiết bị, đây thường là các lệnh đầu tiên bạn dùng để kiểm tra tình trạng hệ thống và các daemon. `/proc` cung cấp khả năng lập trình để ứng dụng tự giám sát hoặc báo cáo trạng thái của các tiến trình khác.
* **Ví dụ (Shell): Sử dụng `ps` và `top`**
  **Bash**

  ```bash
  # Chạy lệnh này trên terminal của bạn
  echo "--- ps -ef output (abbreviated) ---"
  ps -ef | head -n 5 # Hiển thị 5 dòng đầu
  echo "--- ps aux output (abbreviated) ---"
  ps aux | head -n 5 # Hiển thị 5 dòng đầu
  echo "--- pstree output ---"
  pstree | head -n 10 # Hiển thị 10 dòng đầu của cây tiến trình
  echo "--- top (exit with 'q') ---"
  ps -e -o pid,ppid,user,stat,start,time,cmd
  # top # Chạy top, nhấn 'q' để thoát.
  ```

  * **Phân tích output:**
    * Quan sát các cột `PID`, `PPID` để thấy mối quan hệ cha-con.
    * Cột `STAT` hoặc `S` (State) hiển thị trạng thái của tiến trình (Running `R`, Sleeping `S`, Disk sleep `D`, Zombie `Z`, Stopped `T`).
    * Cột `TTY` cho biết terminal điều khiển. `?` hoặc `_` cho thấy tiến trình không có terminal điều khiển (daemon).
    * Cột `TIME` là tổng thời gian CPU mà tiến trình đã sử dụng.

#### **1.3. Tiến trình Hệ thống (System Processes) và Lập lịch (Scheduling) ⚙️⏱️**

* **Lý thuyết:**
  * **Tiến trình Hệ thống:** Ngoài các ứng dụng người dùng, Linux chạy nhiều tiến trình hệ thống (thường với quyền `root`) để quản lý tài nguyên và dịch vụ.
    * **`init` / `systemd` (PID 1):** Tiến trình đầu tiên và là tổ tiên của mọi tiến trình khác. Nó chịu trách nhiệm khởi động hệ thống và quản lý vòng đời của các dịch vụ.
    * Các daemon khác: `syslogd` (ghi log), `sshd` (SSH server), `cron` (lập lịch tác vụ).
  * **Lập lịch Tiến trình (Process Scheduling):** Kernel Linux sử dụng một **bộ lập lịch (scheduler)** để phân chia thời gian CPU giữa các tiến trình đang chạy hoặc sẵn sàng chạy.
    * **Timesharing (Chia sẻ thời gian):** Mỗi tiến trình được cấp một "lát cắt thời gian" (time slice) nhỏ trên CPU. Khi hết lát cắt, Kernel chuyển sang tiến trình khác.
    * **Preemptive Multitasking:** Kernel có thể tạm dừng (preempt) một tiến trình đang chạy và chuyển sang tiến trình khác mà không cần sự hợp tác từ tiến trình đó.
    * **Priority (Độ ưu tiên):** Kernel sử dụng độ ưu tiên để quyết định tiến trình nào sẽ nhận thời gian CPU. Tiến trình ưu tiên cao hơn sẽ chạy thường xuyên hơn.
    * **Nice Value (`NI`):** Một giá trị từ `-20` (ưu tiên cao nhất) đến `19` (ưu tiên thấp nhất) ảnh hưởng đến độ ưu tiên của tiến trình.
      * Người dùng bình thường có thể tăng giá trị nice (giảm ưu tiên) bằng lệnh `nice` hoặc `renice`.
      * Chỉ superuser (root) mới có thể giảm giá trị nice (tăng ưu tiên).
* **Liên hệ Embedded Linux:**
  * Hiểu về `init`/`systemd` là rất quan trọng để quản lý dịch vụ và khởi động ứng dụng trên thiết bị nhúng.
  * Quản lý độ ưu tiên (`nice`, `renice`) và hiểu lập lịch là cần thiết để đảm bảo các tác vụ quan trọng (ví dụ: điều khiển động cơ, thu thập dữ liệu thời gian thực) có đủ thời gian CPU và phản hồi nhanh chóng.

---

### **Câu hỏi Tự đánh giá Module 1 🤔**

1. Giải thích triết lý "mọi thứ là file" của Linux trong ngữ cảnh của một tiến trình. Những thành phần nào của một tiến trình được chia sẻ giữa các tiến trình khác, và những thành phần nào là riêng biệt?
2. Process ID (PID) là gì? PID 1 thường thuộc về tiến trình nào và vai trò của nó là gì?
3. Bạn sẽ sử dụng lệnh shell nào để:
   a. Xem tất cả các tiến trình đang chạy trên hệ thống.
   b. Xem các tiến trình đang tiêu thụ nhiều CPU nhất và cập nhật liên tục.
   c. Xem mối quan hệ cha-con của các tiến trình.
4. Phân biệt trạng thái `R`, `S`, `D` của một tiến trình trong output của `ps`.
5. Giải thích khái niệm "Nice Value" (`NI`). Giá trị `NI` nào cho thấy một tiến trình có độ ưu tiên cao hơn? Làm thế nào một người dùng bình thường có thể thay đổi `NI` của tiến trình của mình?

---

### **Bài tập Thực hành Module 1 ✍️**

1. **Chương trình "Process Info":**
   * Viết một chương trình C++ (`process_info.cpp`) mà:

     * In ra PID của chính nó.
     * In ra PPID (Parent Process ID) của nó.
     * In ra UID (User ID) và GID (Group ID) của tiến trình.
     * Sử dụng `sleep(5)` để giữ chương trình chạy trong 5 giây.
   * **Cách chạy và quan sát:**
     **Bash**

     ```
     g++ process_info.cpp -o process_info
     ./process_info &     # Chạy nền
     ps -ef | grep process_info # Quan sát PID, PPID, UID, GID, TTY
     # Hoặc dùng htop để xem trực quan hơn
     ```
   * **Thử thách:** Chạy chương trình và quan sát nó trong `htop`. Quan sát cột `PRI` và `NI`.
2. **Chương trình "CPU Hog":**
   * Viết một chương trình C++ (`cpu_hog.cpp`) mà:

     * Đi vào một vòng lặp vô hạn và thực hiện một phép tính toán học nặng (ví dụ: `sqrt(log(rand() * rand()))`) để tiêu tốn CPU.
     * Trong mỗi vòng lặp, in ra PID và số lần lặp đã thực hiện.
   * **Cách chạy và quan sát:**
     **Bash**

     ```
     g++ cpu_hog.cpp -o cpu_hog -lm
     ./cpu_hog &      # Chạy nền
     top              # Quan sát CPU usage của cpu_hog
     # Thử thay đổi nice value:
     # PID_HOG=$(ps -aux | grep cpu_hog | awk '{print $2}' | head -n 1) # Lấy PID
     # renice -n 10 -p $PID_HOG # Giảm ưu tiên
     # renice -n -5 -p $PID_HOG # Tăng ưu tiên (cần sudo)
     ```
   * **Thử thách:** Quan sát sự thay đổi của cột `NI` và `%CPU` khi bạn thay đổi `nice value` bằng `renice`.

---

Hãy dành thời gian để hiểu sâu lý thuyết và thực hành các bài tập này. Đây là nền tảng vững chắc để bạn tiến xa hơn trong lập trình Linux! Khi bạn đã sẵn sàng, hãy cho tôi biết để chuyển sang  **Module 2: Khởi động Tiến trình Mới** !
