# **Module 3: Các Công cụ Gỡ lỗi Khác 🛠️🔍**

#### **3.1. Phân tích Tĩnh (Static Analysis) 🧐**

Phân tích tĩnh là quá trình kiểm tra mã nguồn của chương trình  **mà không cần chạy nó** . Các công cụ này tìm kiếm các lỗi tiềm ẩn, vấn đề về cú pháp, lỗi logic, hoặc các thực hành code đáng ngờ.

##### **3.1.1. `lint` và `Splint` 🧹**

* **Lý thuyết:**

  * **`lint`** : Là một tiện ích cũ hơn của UNIX, hoạt động như một trình biên dịch C bổ sung, với các kiểm tra mở rộng để tìm ra các vấn đề mà compiler thông thường có thể bỏ qua (ví dụ: biến chưa khởi tạo, đối số hàm không dùng). Nó thường là công cụ "kiểm tra chất lượng code" sớm.
  * **`Splint`** : Là một công cụ hiện đại hơn, kế thừa từ `lint` và được thiết kế để phân tích code C/C++ với các kiểm tra chặt chẽ hơn về an toàn bộ nhớ, sử dụng con trỏ, và các lỗi logic. Nó có thể phát hiện các lỗi mà compiler thông thường (ngay cả với các cảnh báo cao) cũng bỏ qua.
* **Cách sử dụng:** Bạn chạy `splint` trực tiếp trên file mã nguồn của mình.
  **Bash**

  ```
  splint -strict your_program.c
  ```

  * `-strict`: Bật các kiểm tra nghiêm ngặt.
* **Liên hệ Embedded Linux:** Cực kỳ hữu ích trong nhúng để:

  * Bắt các lỗi bộ nhớ (null pointer dereference, out-of-bounds access) sớm, trước khi chúng gây ra Segfault khó debug trên phần cứng.
  * Đảm bảo chất lượng code và tuân thủ các quy tắc lập trình an toàn, rất quan trọng cho các hệ thống có yêu cầu độ tin cậy cao.
  * Tìm các lỗi không tương thích hoặc các đoạn mã không di động.

##### **3.1.2. `ctags`: Tạo chỉ mục Hàm/Biến 📚**

* **Lý thuyết:** `ctags` (thường là `Exuberant Ctags` hoặc `Universal Ctags` trên Linux hiện đại) tạo ra một file chỉ mục (index) của các định nghĩa hàm, biến, và các biểu tượng khác trong mã nguồn của bạn.
* **Mục đích:** Hỗ trợ điều hướng code nhanh chóng trong các trình soạn thảo văn bản (text editors) hoặc IDE. Thay vì cuộn tìm kiếm, bạn có thể nhảy thẳng đến định nghĩa của một hàm hoặc biến.
* **Cách sử dụng:**
  **Bash**

  ```cpp
  ctags -R .  # Tạo file 'tags' trong thư mục hiện tại, quét đệ quy
  ```

  * File `tags` chứa các dòng như: `tên_hàm file_định_nghĩa /pattern_để_tìm/`
* **Liên hệ Embedded Linux:** Khi làm việc với các codebase lớn (ví dụ: mã nguồn Kernel, các thư viện phức tạp của SoC), `ctags` là công cụ không thể thiếu để lập trình viên nhanh chóng di chuyển giữa các file và định nghĩa.

##### **3.1.3. `cxref`: Tạo tham chiếu chéo (Cross-Reference) 🧩**

* **Lý thuyết:** `cxref` phân tích mã nguồn C và tạo một danh sách tham chiếu chéo. Nó hiển thị nơi mỗi biểu tượng (symbol) – biến, macro, hàm – được định nghĩa và được sử dụng trong chương trình.
* **Mục đích:** Giúp hiểu mối quan hệ và sự phụ thuộc giữa các phần khác nhau của code, đặc biệt hữu ích khi refactoring hoặc sửa lỗi ảnh hưởng đến nhiều nơi.
* **Cách sử dụng:**
  **Bash**

  ```
  cxref *.c *.h # Phân tích tất cả các file .c và .h trong thư mục hiện tại
  ```
* **Liên hệ Embedded Linux:** Hiểu được tất cả các điểm mà một biến toàn cục hoặc một hàm quan trọng được sử dụng có thể giúp bạn phát hiện các điều kiện tranh chấp (race conditions) hoặc các side effects không mong muốn trong các hệ thống đa luồng/đa tiến trình.

##### **3.1.4. `cflow`: Hiển thị Cây gọi Hàm (Function Call Tree) 🌲**

* **Lý thuyết:** `cflow` phân tích mã nguồn và in ra một cây gọi hàm, tức là một biểu đồ cho thấy hàm nào gọi hàm nào, và các hàm đó lại gọi những hàm nào khác, v.v.
* **Mục đích:** Giúp hiểu cấu trúc luồng điều khiển của một chương trình, theo dõi một yêu cầu từ đầu đến cuối, hoặc dự đoán tác động của việc thay đổi một hàm.
* **Cách sử dụng:**
  **Bash**

  ```
  cflow your_program.c
  ```

  * Cũng có tùy chọn `-i` để tạo biểu đồ luồng gọi ngược (inverted call graph), liệt kê các hàm gọi đến một hàm cụ thể.
* **Liên hệ Embedded Linux:** Rất hữu ích khi phân tích code của các driver hoặc các module Kernel, nơi luồng thực thi có thể phức tạp với các lời gọi hàm lồng nhau.

#### **3.2. Phân tích Động (Dynamic Analysis) 🔬**

Phân tích động là việc thu thập thông tin về hành vi của chương trình  **khi nó đang chạy** .

##### **3.2.1. `prof` / `gprof`: Đánh giá Hiệu suất (Execution Profiling) ⏱️**

* **Lý thuyết:** `prof` và `gprof` (phiên bản GNU của `prof`) là các công cụ  **profiling** . Chúng giúp bạn xác định **thời gian CPU** mà chương trình của bạn dành cho từng hàm.
* **Mục đích:** Tìm ra "nút cổ chai" (bottlenecks) về hiệu suất – tức là những phần của code đang tiêu tốn nhiều thời gian CPU nhất.
* **Cách sử dụng:**
  1. **Biên dịch chương trình với cờ profiling:**
     **Bash**

     ```
     g++ your_program.cpp -o your_program -pg # -pg cho gprof
     ```
  2. **Chạy chương trình:**
     **Bash**

     ```
     ./your_program # Sau khi chạy, một file gmon.out sẽ được tạo ra
     ```
  3. **Tạo báo cáo profiling:**
     **Bash**

     ```
     gprof your_program gmon.out > profile_report.txt
     ```
* **Output:** Báo cáo `gprof` hiển thị:
  * Thời gian lũy kế (cumulative time).
  * Thời gian tự thân (self time) mà mỗi hàm tiêu thụ.
  * Số lần mỗi hàm được gọi.
  * Thông tin về lời gọi hàm (call graph).
* **Liên hệ Embedded Linux:** Cực kỳ quan trọng để tối ưu hiệu suất của các ứng dụng nhúng, nơi tài nguyên CPU thường hạn chế. Giúp bạn xác định các hàm cần tối ưu hóa để tăng tốc độ phản hồi của thiết bị.

---

### **Câu hỏi Tự đánh giá Module 3 🤔**

1. Phân biệt giữa phân tích tĩnh (Static Analysis) và phân tích động (Dynamic Analysis) trong gỡ lỗi. Cho ví dụ về một công cụ cho mỗi loại.
2. `Splint` hữu ích như thế nào trong việc tìm lỗi so với việc chỉ sử dụng `gcc` với các cảnh báo cao?
3. Khi nào bạn sẽ sử dụng `ctags` trong quá trình lập trình?
4. Bạn có một chương trình phức tạp và muốn hiểu luồng điều khiển giữa các hàm. Công cụ nào bạn sẽ dùng để tạo "cây gọi hàm"?
5. Giải thích mục đích chính của `gprof`. Bạn sẽ làm gì để sử dụng `gprof` trên chương trình C++ của mình?

---

### **Bài tập Thực hành Module 3 ✍️**

1. **Bài tập Phân tích `buggy_sort.c` với `Splint`:**
   * Sử dụng file `buggy_sort.c` (chứa các lỗi từ Module 1) mà bạn đã có.
   * Cài đặt `splint` nếu chưa có: `sudo apt install splint` (Ubuntu/Debian) hoặc tìm cách cài đặt tương ứng với bản phân phối của bạn.
   * Chạy `splint` trên `buggy_sort.c` với cờ nghiêm ngặt:
     **Bash**

     ```
     splint -strict buggy_sort.c
     ```
   * **Mục tiêu:**

     * Quan sát các cảnh báo và lỗi mà `splint` báo cáo.
     * Đối chiếu các cảnh báo đó với các lỗi mà bạn đã tìm thấy bằng GDB trong Module 2 (ví dụ: lỗi biến `s` chưa khởi tạo, lỗi toán tử `&` thay vì `&&`).
     * Cảnh báo nào bạn đã không phát hiện ra khi chỉ biên dịch với `gcc`?
2. **Bài tập Sử dụng `ctags` và `cflow`:**
   * Lấy mã nguồn của một dự án C/C++ nhỏ bạn đang có (hoặc một ví dụ nào đó từ các Module trước của giáo trình này, ví dụ: `resource_monitor_daemon.cpp`).
   * **Sử dụng `ctags`:**
     **Bash**

     ```
     cd <thư_mục_chứa_mã_nguồn>
     ctags -R .
     # Sau đó, nếu dùng Vim/NeoVim: mở file source và dùng Ctrl+] trên tên hàm để nhảy đến định nghĩa.
     # Hoặc xem nội dung file 'tags' bằng 'cat tags'.
     ```
   * **Sử dụng `cflow`:**

     * Cài đặt `cflow` nếu chưa có: `sudo apt install cflow`.
     * Chạy `cflow` trên file mã nguồn chính của bạn:
       **Bash**

       ```
       cflow your_main_file.cpp
       ```
     * **Mục tiêu:** Quan sát output của `cflow`. Nó có hiển thị cây gọi hàm của chương trình bạn không? Hàm `main()` gọi những hàm nào? Có hàm nào bạn không mong đợi trong cây gọi không?
3. **Bài tập Đánh giá Hiệu suất với `gprof`:**
   * Sử dụng chương trình `resource_limits.cpp` từ Module 7 của giáo trình "The Linux Environment" (phiên bản có hàm `do_work()` tiêu tốn CPU).
   * **Biên dịch với cờ profiling:**
     **Bash**

     ```
     g++ resource_limits.cpp -o resource_limits_profile -pg -lm
     ```
   * **Chạy chương trình:**
     **Bash**

     ```
     ./resource_limits_profile
     ```

     * Sau khi chạy, bạn sẽ thấy một file `gmon.out` được tạo trong thư mục hiện tại.
   * **Tạo báo cáo `gprof`:**
     **Bash**

     ```
     gprof resource_limits_profile gmon.out > profile_report.txt
     cat profile_report.txt
     ```
   * **Mục tiêu:**

     * Phân tích báo cáo `profile_report.txt`.
     * Hàm `do_work()` có phải là hàm tiêu tốn nhiều thời gian CPU nhất không? (`log()` trong `do_work` cũng tiêu tốn khá nhiều).
     * Quan sát các cột "cumulative seconds" và "self seconds". Hàm nào có "self seconds" cao nhất?
     * Điều này khẳng định những gì chúng ta đã thảo luận về "nút cổ chai" hiệu suất như thế nào?
