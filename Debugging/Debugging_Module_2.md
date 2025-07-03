# **Module 2: GDB - Trình gỡ lỗi GNU 🐛🛠️**

#### **2.1. GDB là gì và tại sao nó quan trọng? (What is GDB and Why is it Important?)**

* **Lý thuyết:**
  * **GDB** (GNU Debugger) là trình gỡ lỗi dòng lệnh tiêu chuẩn cho Linux (và nhiều hệ thống Unix-like khác). Nó là một công cụ cực kỳ mạnh mẽ để tìm lỗi trong các chương trình viết bằng C, C++, Fortran, và nhiều ngôn ngữ khác.
  * GDB cho phép bạn:
    * **Chạy chương trình của bạn dưới sự kiểm soát:** Tạm dừng ở bất kỳ điểm nào, chạy từng dòng code.
    * **Kiểm tra và sửa đổi biến:** Xem giá trị của các biến cục bộ, toàn cục, hoặc các biểu thức phức tạp.
    * **Theo dõi Stack Trace:** Xem chuỗi các hàm đã được gọi để đến vị trí hiện tại (backtrace).
    * **Đặt Breakpoints và Watchpoints:** Tạm dừng chương trình tại một dòng code cụ thể hoặc khi giá trị của một biến thay đổi.
    * **Kiểm tra Core Dumps:** Phân tích trạng thái chương trình sau khi nó crash.
    * **Attach vào tiến trình đang chạy:** Gỡ lỗi các chương trình đã và đang chạy (daemon, server) mà không cần khởi động lại.
  * **Tầm quan trọng:**
    * **Gỡ lỗi sâu:** Khác với `printf` chỉ cho bạn biết những gì bạn in ra, GDB cho phép bạn "nhìn thấy" mọi thứ bên trong chương trình tại bất kỳ thời điểm nào.
    * **Khắc phục lỗi khó tái hiện:** Đặc biệt hữu ích với các lỗi về bộ nhớ hoặc điều kiện tranh chấp (race conditions) thường không xảy ra một cách nhất quán.
* **Compile cho Debugging:** Để GDB có thể hoạt động hiệu quả, bạn cần biên dịch chương trình của mình với cờ `-g`. Cờ này yêu cầu trình biên dịch (ví dụ: `gcc`, `g++`) bao gồm thông tin gỡ lỗi bổ sung (symbols, line numbers) vào file thực thi.
  * **Lưu ý:** Việc thêm thông tin debug có thể làm tăng kích thước file thực thi lên nhiều lần, nhưng không ảnh hưởng đến mức RAM chương trình cần khi chạy. Nên xóa thông tin debug (sử dụng `strip`) trước khi phát hành sản phẩm cuối cùng.

#### **2.2. Bắt đầu với GDB (Starting GDB) 🚀**

* **Lý thuyết:**

  1. **Biên dịch chương trình với `-g`:**
     **Bash**

     ```
     g++ buggy_sort.c -o buggy_sort -g # Dùng lại ví dụ buggy_sort.c từ Module 1
     ```
  2. **Khởi động GDB:**
     **Bash**

     ```
     gdb <tên_chương_trình_thực_thi>
     # Ví dụ: gdb ./buggy_sort
     ```

  * GDB sẽ khởi động, hiển thị thông tin bản quyền và nhắc lệnh `(gdb)`.
  * Bạn có thể dùng `help` để xem danh sách các lệnh.
* **Chạy Chương trình trong GDB:**

  * Lệnh `run` sẽ thực thi chương trình của bạn bên trong GDB.
  * Mọi đối số bạn truyền cho `run` sẽ được chuyển cho chương trình của bạn.

  **Code snippet**

  ```
  (gdb) run [đối_số_chương_trình]
  # Ví dụ: (gdb) run
  ```

  * Nếu chương trình gặp lỗi (ví dụ: Segmentation Fault), GDB sẽ tạm dừng thực thi và hiển thị thông báo lỗi cùng với vị trí (file và dòng code) nơi lỗi xảy ra.
* **Ví dụ (Dùng `buggy_sort.c` từ Module 1):**
  **Bash**

  ```
  # Biên dịch với cờ -g
  g++ buggy_sort.c -o buggy_sort -g

  # Khởi động GDB và chạy chương trình
  gdb ./buggy_sort
  ```

  **Code snippet**

  ```
  (gdb) run
  Starting program: /path/to/your/project/buggy_sort 
  --- Initial Array ---
  array[0] = {bill, 3}
  array[1] = {neil, 4}
  array[2] = {john, 2}
  array[3] = {rick, 5}
  array[4] = {alex, 1}
  ---------------------

  Program received signal SIGSEGV, Segmentation fault.
  0x000000000040084d in sort (a=0x400b060, n=5) at buggy_sort.c:23
  23        if (a[j].key > a[j+1].key) {
  (gdb) 
  ```

  * **Phân tích:** GDB báo hiệu chương trình nhận `SIGSEGV` tại dòng 23 của `buggy_sort.c` trong hàm `sort`. Đây chính là lỗi truy cập ngoài biên mà chúng ta đã dự đoán.

#### **2.3. Dấu vết Stack (Stack Trace) 🚶‍♀️**

* **Lý thuyết:** Khi chương trình dừng lại (do lỗi hoặc breakpoint), bạn cần biết làm thế nào nó đến được vị trí đó. Lệnh `backtrace` (viết tắt `bt`) hiển thị chuỗi các hàm đã được gọi để dẫn đến vị trí thực thi hiện tại của chương trình. Đây là một "dấu vết" của các hàm trong stack.
  **Code snippet**

  ```
  (gdb) backtrace [full] # 'full' để hiển thị các biến cục bộ
  # Hoặc (gdb) bt
  ```

  * Mỗi "frame" (khung) trong backtrace đại diện cho một hàm đang hoạt động trong stack.
  * Frame 0 là hàm hiện tại. Frame 1 là hàm đã gọi Frame 0, v.v.
* **Liên hệ Embedded Linux:** Cực kỳ hữu ích để tìm ra nguyên nhân gốc rễ của một crash. Khi một daemon trên thiết bị nhúng bị crash và tạo core dump, bạn có thể tải core dump đó vào GDB và dùng `backtrace` để xem stack trace tại thời điểm crash.
* **Ví dụ (Tiếp tục từ phiên GDB trước):**
  **Code snippet**

  ```
  (gdb) backtrace
  #0  0x000000000040084d in sort (a=0x400b060, n=5) at buggy_sort.c:23
  #1  0x000000000040092c in main () at buggy_sort.c:37
  #2  0x00007ffff7a250b3 in __libc_start_main (main=0x4008ec <main>, argc=1, argv=0x7fffffffe0c8, init=<optimized out>, 
      fini=<optimized out>, rtld_fini=<optimized out>, stack_end=0x7fffffffe0b8) at ../csu/libc-start.c:292
  #3  0x0000000000400789 in _start ()
  (gdb) 
  ```

  * **Phân tích:** Output cho thấy lỗi xảy ra ở dòng 23 của hàm `sort` (`Frame 0`). Hàm `sort` được gọi từ dòng 37 của hàm `main` (`Frame 1`).

#### **2.4. Kiểm tra Biến (Examining Variables) 🧐**

* **Lý thuyết:** Khi đã xác định được vị trí lỗi, bạn cần kiểm tra giá trị của các biến để hiểu tại sao lỗi lại xảy ra. Lệnh `print` (viết tắt `p`) cho phép bạn in ra giá trị của các biến, biểu thức C hợp lệ.
  **Code snippet**

  ```
  (gdb) print <tên_biến_hoặc_biểu_thức>
  # Ví dụ: (gdb) print j
  # (gdb) print a[j].key
  # (gdb) print array[0]@5 # Để in 5 phần tử đầu tiên của mảng 'array'
  ```

  * GDB cũng lưu kết quả của các lệnh `print` vào các biến giả (`$1`, `$2`, ...). `$`: kết quả gần nhất, `$$`: kết quả trước đó.
* **Liên hệ Embedded Linux:** Vô cùng quan trọng. Cho phép bạn xem trạng thái của các thanh ghi phần cứng được ánh xạ, giá trị của các bộ đệm giao tiếp, hoặc trạng thái của các cấu trúc dữ liệu phức tạp trên thiết bị mục tiêu.
* **Ví dụ (Tiếp tục từ phiên GDB trước, tại dòng 23):**
  **Code snippet**

  ```
  (gdb) print j
  $1 = 4
  (gdb) print n
  $2 = 5
  (gdb) print a[j].key
  $3 = 1
  (gdb) print a[j+1].key # Lỗi xảy ra ở đây: truy cập a[5]
  Cannot access memory at address 0x400b0b0
  (gdb) print array[0]@5
  $4 = {{data = '\000' <repeats 4096 times>, key = 3}, {data = '\000' <repeats 4096 times>, key = 4}, {data = '\000' <repeats 4096 times>, key = 2}, {data = '\000' <repeats 4096 times>, key = 5}, {data = '\000' <repeats 4096 times>, key = 1}}
  (gdb) 
  ```

  * **Phân tích:** `j=4`, `n=5`. Lỗi xảy ra khi truy cập `a[j+1]`, tức `a[5]`. Mảng `array` chỉ có 5 phần tử (chỉ số từ 0 đến 4), nên `a[5]` là truy cập ngoài biên.

#### **2.5. Xem Mã nguồn (Listing the Program) 📜**

* **Lý thuyết:** Lệnh `list` (viết tắt `l`) hiển thị một phần mã nguồn xung quanh vị trí hiện tại của chương trình.
  **Code snippet**

  ```
  (gdb) list [số_dòng_hoặc_tên_hàm]
  # Ví dụ: (gdb) list 20
  # (gdb) list sort
  ```
* **Phân tích lỗi (tiếp tục từ GDB):**
  **Code snippet**

  ```
  (gdb) list 20
  18        int s = 1;
  19   
  20        for (; i < n && s != 0; i++) {
  21            s = 0;
  22            for (j = 0; j < n; j++) {
  23                if (a[j].key > a[j+1].key) {
  24                    item t = a[j];
  25                    a[j] = a[j+1];
  26                    a[j+1] = t;
  27                    s++;
  (gdb) 
  ```

  * **Phân tích:** Nhận thấy vòng lặp bên trong là `for (j = 0; j < n; j++)`. Khi `n=5`, `j` sẽ chạy từ 0 đến 4. Nhưng khi `j=4`, `a[j+1]` sẽ là `a[5]`, gây lỗi.
  * **Sửa lỗi 1 (trong `debug4.c`):** Điều kiện vòng lặp bên trong phải là `j < n - 1`.
    **C**

    ```
    // debug4.c (fixed part)
    // ...
    // 22            for (j = 0; j < n - 1; j++) { // Corrected line
    // ...
    ```
  * Biên dịch lại `g++ debug4.c -o debug4 -g`. Chạy `./debug4`. Lần này sẽ không bị Segfault nữa, nhưng mảng vẫn chưa được sắp xếp đúng.

#### **2.6. Đặt Breakpoints (Setting Breakpoints) 🛑**

* **Lý thuyết:** Breakpoint là một điểm trong code mà bạn muốn chương trình tạm dừng khi đến đó. GDB sẽ trả lại quyền điều khiển cho bạn, cho phép bạn kiểm tra trạng thái chương trình.
  **Code snippet**

  ```
  (gdb) break <dòng_code_hoặc_tên_hàm>
  # Ví dụ: (gdb) break 20
  # (gdb) break sort
  ```

  * `info breakpoints`: Xem danh sách các breakpoints đã đặt.
  * `disable <số_breakpoint>`: Tạm thời vô hiệu hóa breakpoint.
  * `enable <số_breakpoint>`: Kích hoạt lại breakpoint.
  * `delete <số_breakpoint>`: Xóa breakpoint.
* **Tiếp tục chạy:**

  * `cont` (continue): Tiếp tục chạy chương trình cho đến breakpoint tiếp theo hoặc khi kết thúc.
  * `next` (n): Chạy qua dòng code hiện tại (nhảy qua các lời gọi hàm).
  * `step` (s): Chạy vào trong lời gọi hàm.
* **Ví dụ (Tiếp tục với `debug4.c`):**
  **Bash**

  ```
  gdb ./debug4
  (gdb) break 20 # Đặt breakpoint ở đầu vòng lặp ngoài
  (gdb) run
  ```

  **Code snippet**

  ```
  Breakpoint 1, sort (a=0x400b060, n=5) at buggy_sort.c:20
  20        for (; i < n && s != 0; i++) {
  (gdb) print array[0]@5 # Xem trạng thái mảng
  $1 = {{data = "bill", '\000' <repeats 4091 times>, key = 3}, {data = "neil", '\000' <repeats 4091 times>, key = 4}, {data = "john", '\000' <repeats 4091 times>, key = 2}, {data = "rick", '\000' <repeats 4091 times>, key = 5}, {data = "alex", '\000' <repeats 4091 times>, key = 1}}
  (gdb) cont # Tiếp tục
  ```

  * Chạy lại `cont`, bạn sẽ thấy GDB dừng lại ở dòng 20 mỗi khi vòng lặp ngoài lặp lại.

#### **2.7. Tự động hiển thị và Lệnh Breakpoint (Auto-Display and Breakpoint Commands) ⚙️**

* **Lý thuyết:**
  * `display <biểu_thức>`: Tự động hiển thị giá trị của biểu thức mỗi khi chương trình dừng lại (tại breakpoint hoặc do lỗi).
  * `commands <số_breakpoint>`: Cho phép bạn chỉ định một chuỗi các lệnh GDB sẽ tự động thực thi mỗi khi breakpoint đó được kích hoạt. Bạn kết thúc chuỗi lệnh bằng một dòng chỉ chứa chữ `end`. Rất mạnh mẽ để tự động in thông tin và tiếp tục chạy.
* **Phân tích lỗi (tiếp tục với `debug4.c`):**
  * Sau khi sửa lỗi 1, mảng vẫn chưa sắp xếp đúng. Vấn đề là ở dòng `n--;` (dòng 30), làm giảm giá trị `n` được dùng trong điều kiện vòng lặp ngoài, khiến vòng lặp không chạy đủ số lần.
  * **Sửa lỗi 2 (Patching với Debugger - trong GDB):**

    * Xóa breakpoint cũ: `delete 1`
    * Đặt breakpoint tại dòng `n--;` (dòng 30): `break 30`
    * Khi breakpoint 2 được kích hoạt, ta muốn *thực thi* lệnh `set variable n = n + 1` để đảo ngược hiệu ứng `n--;` và sau đó `cont` để chương trình tiếp tục chạy.

    **Code snippet**

    ```
    (gdb) break 30
    (gdb) commands 2
    > set variable n = n + 1
    > cont
    > end
    (gdb) run
    ```

    * **Phân tích:** Lần này, chương trình sẽ sắp xếp đúng! GDB đã giúp chúng ta kiểm tra sửa lỗi mà không cần biên dịch lại code.
  * **Sửa lỗi 2 (Trong code):** Đơn giản là xóa dòng `n--;` (dòng 30) trong `buggy_sort.c` (hoặc `debug4.c`).

#### **2.8. Tìm hiểu thêm về GDB (Learning More about GDB) 🧠**

* **`watch` / `awatch` / `rwatch` (Watchpoints):** Dừng chương trình khi một biến thay đổi giá trị (`watch`), hoặc khi nó được truy cập (`awatch`), hoặc chỉ khi nó được đọc (`rwatch`). Cần phần cứng hỗ trợ hoặc có thể chậm.
* **Conditional Breakpoints:** Đặt breakpoint chỉ khi một điều kiện cụ thể đúng. `break <dòng_code> if <điều_kiện>`
* **Attach vào tiến trình đang chạy:** `gdb attach <PID>`. Rất hữu ích để gỡ lỗi các daemon hoặc server.
* **Core Dumps:** Khi chương trình crash, Linux/Unix thường tạo ra file `core`. Bạn có thể phân tích nó bằng `gdb <chương_trình_thực_thi> <core_file>`.
* **Tối ưu hóa và Debugging (`-O` và `-g`):** Bạn có thể biên dịch với cả cờ tối ưu hóa (`-O`) và cờ debug (`-g`). Tuy nhiên, việc tối ưu hóa có thể sắp xếp lại mã, làm cho việc bước qua từng dòng trong GDB trở nên khó hiểu hơn.
* **GDB Frontends:** Có các công cụ giao diện người dùng đồ họa (GUI) cho GDB như `xxgdb`, `tgdb`, `ddd`, hoặc tích hợp trong các IDE (ví dụ: VS Code, CLion, Eclipse) để giúp việc gỡ lỗi trực quan hơn.

---

### **Câu hỏi Tự đánh giá Module 2 🤔**

1. Tại sao việc biên dịch chương trình với cờ `-g` lại cần thiết để sử dụng GDB một cách hiệu quả?
2. Khi một chương trình C++ của bạn gặp `Segmentation fault` trong quá trình chạy, bạn sẽ làm gì đầu tiên trong GDB để tìm ra nguyên nhân? Nêu các lệnh GDB bạn sẽ sử dụng.
3. Phân biệt giữa lệnh `next` và `step` trong GDB. Khi nào bạn sẽ dùng mỗi lệnh?
4. Bạn muốn theo dõi giá trị của biến `my_variable` mỗi khi chương trình dừng ở bất kỳ breakpoint nào. Lệnh GDB nào sẽ giúp bạn làm điều này?
5. Giải thích cách bạn có thể sử dụng GDB để "vá" (patch) tạm thời một lỗi trong chương trình mà không cần sửa đổi mã nguồn và biên dịch lại.

---

### **Bài tập Thực hành Module 2 ✍️**

1. **Bài tập Debug `buggy_sort.c` (Toàn diện):**
   * Biên dịch lại `buggy_sort.c` (từ Module 1) với cờ `-g`.
   * Chạy chương trình trong GDB (`gdb ./buggy_sort`).
   * **Bước 1: Tìm lỗi Segfault:**
     * Chạy chương trình (`run`).
     * Khi nó crash, dùng `backtrace` để xem stack trace.
     * Dùng `print` để kiểm tra giá trị của `j` và `n` tại thời điểm crash.
     * Dùng `list` để xem mã nguồn xung quanh.
     * **Mục tiêu:** Xác định chính xác nguyên nhân của lỗi Segfault.
   * **Bước 2: Sửa lỗi Segfault (trong GDB):**
     * Xóa breakpoint cũ (nếu có).
     * Đặt breakpoint tại dòng gây lỗi Segfault.
     * Sử dụng `commands` cho breakpoint này để sửa giá trị của `j` hoặc `n` một cách tạm thời để tránh lỗi Segfault.
     * `cont` để tiếp tục.
     * Chạy lại chương trình (`run`) và xác nhận rằng nó không còn bị Segfault nữa.
   * **Bước 3: Tìm lỗi logic (sắp xếp sai):**
     * Đặt breakpoint ở dòng 20 (`for (; i < n && s != 0; i++)`).
     * Sử dụng `display array[0]@5` để tự động hiển thị mảng.
     * Chạy chương trình và dùng `cont` để quan sát trạng thái của mảng sau mỗi vòng lặp ngoài.
     * **Mục tiêu:** Xác định lý do mảng không được sắp xếp đúng (liên quan đến `n--`).
   * **Bước 4: Sửa lỗi logic (trong GDB):**
     * Đặt breakpoint tại dòng `n--;` (dòng 30).
     * Sử dụng `commands` để đặt `n = n + 1` (đảo ngược hiệu ứng của `n--`).
     * Chạy lại chương trình và xác nhận mảng được sắp xếp đúng.
   * **Bước 5: Sửa lỗi vĩnh viễn:**
     * Thoát GDB.
     * Sửa `buggy_sort.c` trong editor của bạn để khắc phục cả hai lỗi (điều kiện vòng lặp bên trong và `n--`).
     * Biên dịch lại và chạy chương trình mà không cần GDB để xác nhận nó hoạt động đúng.
2. **Bài tập Gỡ lỗi Daemon với GDB (Nâng cao):**
   * Sử dụng chương trình `resource_monitor_daemon.cpp` từ Module 8 của giáo trình trước.
   * Biên dịch nó với cờ `-g`.
   * Chạy daemon ở chế độ nền: `./resource_monitor_daemon -p /tmp/daemon.pid &`
   * Mở GDB và `attach` vào tiến trình daemon đang chạy: `gdb -p $(cat /tmp/daemon.pid)` (hoặc `gdb <tên_chương_trình> <PID>`).
   * **Mục tiêu:**
     * Đặt một breakpoint tại đầu hàm `collect_resource_info()`.
     * `cont` để daemon chạy đến breakpoint đó.
     * Khi GDB dừng lại, `next` từng dòng để theo dõi quá trình đọc `/proc`.
     * Dùng `print` để kiểm tra các biến cục bộ (ví dụ: `total_mem`, `open_fds`).
     * Thử đặt một breakpoint trong `signal_handler()` và gửi `kill -HUP <PID>` từ terminal khác để xem GDB có tạm dừng ở đó không.
     * Thoát GDB (`detach` rồi `quit`). Gửi `kill <PID>` để dừng daemon.

---

Chúc mừng bạn đã khám phá Module 2 về GDB! Đây là một công cụ cực kỳ mạnh mẽ mà bạn sẽ dùng rất nhiều trong sự nghiệp lập trình. Hãy dành thời gian để thực hành và làm quen với nó.

Khi bạn đã sẵn sàng, hãy cho tôi biết để chuyển sang  **Module 3: Các Công cụ Gỡ lỗi Khác** !
