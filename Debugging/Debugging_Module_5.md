# **Module 5: Gỡ lỗi Sử dụng Bộ nhớ (Memory Debugging) 🧠🐞**

#### **5.1. Các vấn đề phổ biến của Bộ nhớ Động (Common Dynamic Memory Problems) 💥**

Khi bạn sử dụng `malloc()`, `free()` (hoặc `new`/`delete` trong C++), có một số loại lỗi phổ biến có thể xảy ra:

1. **Rò rỉ Bộ nhớ (Memory Leaks) 💧:**
   * **Lý thuyết:** Xảy ra khi chương trình cấp phát bộ nhớ động nhưng **không bao giờ giải phóng** nó khi không còn cần dùng nữa. Khối bộ nhớ đó trở nên không thể truy cập được (orphaned) nhưng vẫn được hệ thống coi là đang sử dụng.
   * **Hậu quả:** Chương trình sẽ dần dần tiêu thụ nhiều RAM hơn, dẫn đến việc hệ thống chậm lại, các ứng dụng khác bị thiếu bộ nhớ, và cuối cùng có thể kích hoạt OOM Killer hoặc làm treo toàn bộ thiết bị nhúng.
   * **Nguyên nhân:** Quên gọi `free()` (hoặc `delete`), mất con trỏ tới khối bộ nhớ đã cấp phát.
2. **Truy cập Bộ nhớ không hợp lệ (Invalid Memory Access) 🚫:**
   * **Lý thuyết:** Chương trình cố gắng đọc hoặc ghi vào một vùng bộ nhớ mà nó không được phép truy cập.
   * **Các loại phổ biến:**
     * **Truy cập ngoài biên (Out-of-bounds access):** Đọc/ghi trước byte đầu tiên hoặc sau byte cuối cùng của một khối bộ nhớ được cấp phát. Đây là lỗi phổ biến nhất.
     * **Use-after-free:** Truy cập bộ nhớ sau khi nó đã được giải phóng bằng `free()`. Vùng nhớ đó có thể đã được tái cấp phát cho mục đích khác hoặc chứa dữ liệu rác.
     * **Double-free:** Giải phóng cùng một khối bộ nhớ hai lần. Điều này làm hỏng cấu trúc dữ liệu nội bộ của trình quản lý bộ nhớ heap.
   * **Hậu quả:** Thường gây ra **Segmentation Fault (`SIGSEGV`)** và làm chương trình crash. Lỗi này có thể xảy ra ngay lập tức hoặc sau một thời gian dài, khiến việc tìm nguyên nhân gốc rễ cực kỳ khó khăn.

#### **5.2. Công cụ Gỡ lỗi Bộ nhớ (Memory Debugging Tools) 🛠️**

Việc tìm và sửa các lỗi bộ nhớ này bằng cách thủ công (ví dụ: chỉ dùng `printf`) là cực kỳ khó khăn. May mắn thay, Linux cung cấp các công cụ chuyên biệt để giúp bạn.

##### **5.2.1. `ElectricFence` ⚡**

* **Lý thuyết:** `ElectricFence` (EF) là một thư viện thay thế các hàm cấp phát bộ nhớ chuẩn (`malloc`, `free`, v.v.) bằng các phiên bản đặc biệt sử dụng tính năng **bộ nhớ ảo (virtual memory)** của CPU.

  * **Cách hoạt động:** EF đặt một "trang bảo vệ" (guard page) ngay sau (hoặc trước) mỗi khối bộ nhớ được cấp phát. Trang bảo vệ này được đánh dấu là không thể truy cập.
  * **Phát hiện lỗi:** Nếu chương trình cố gắng truy cập bộ nhớ ngoài giới hạn của khối cấp phát (ví dụ: `ptr[1024]` sau khi cấp phát 1024 byte), nó sẽ ngay lập tức chạm vào trang bảo vệ, gây ra **Segmentation Fault** tại **đúng thời điểm và vị trí** lỗi xảy ra.
  * **Ưu điểm:** Phát hiện lỗi truy cập ngoài biên rất sớm và chính xác.
  * **Hạn chế:** Chỉ phát hiện lỗi truy cập ngoài biên, không phải rò rỉ bộ nhớ. Có thể gây ra overhead bộ nhớ đáng kể (mỗi khối cấp phát cần thêm ít nhất một trang bảo vệ).
* **Cách sử dụng:** Bạn cần liên kết chương trình của mình với thư viện `ElectricFence` khi biên dịch.
  **Bash**

  ```
  g++ your_program.cpp -o your_program -lefence -g # -g để dùng với GDB
  ```
* **Liên hệ Embedded Linux:** EF có thể được sử dụng trong giai đoạn phát triển trên thiết bị nhúng để nhanh chóng xác định các lỗi truy cập bộ nhớ quan trọng. Tuy nhiên, do overhead bộ nhớ, nó không phù hợp cho môi trường sản phẩm.
* **Ví dụ (C++): `efence_example.cpp` - Sử dụng `ElectricFence`**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <cstdlib> // For malloc, free, exit, EXIT_SUCCESS
  #include <cstring> // For memset

  // Logger namespace (as defined in previous modules)
  namespace AppLogger {
      enum LogLevel { TRACE_L, DEBUG_L, INFO_L, SUCCESS_L, WARNING_L, ERROR_L, CRITICAL_L };
      static const std::map<LogLevel, std::string> LogLevelNames = {
          {TRACE_L,    "TRACE   "}, {DEBUG_L,    "DEBUG   "}, {INFO_L,     "INFO    "},
          {SUCCESS_L,  "SUCCESS "}, {WARNING_L,  "WARNING "}, {ERROR_L,    "ERROR   "},
          {CRITICAL_L, "CRITICAL"}
      };
      void log(LogLevel level, const std::string& message) {
          std::cout << LogLevelNames.at(level) << ": " << message << std::endl;
      }
  }

  int main() {
      char *ptr = (char *)malloc(1024); // Cấp phát 1024 bytes
      if (ptr == nullptr) {
          AppLogger::log(AppLogger::ERROR_L, "Malloc failed.");
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Allocated 1024 bytes at " + std::to_string(reinterpret_cast<long long>(ptr)));

      // Ghi vào byte đầu tiên (hợp lệ)
      ptr[0] = 'A';
      AppLogger::log(AppLogger::INFO_L, "Wrote 'A' to ptr[0].");

      // --- Cố tình ghi ngoài giới hạn ---
      AppLogger::log(AppLogger::CRITICAL_L, "Attempting to write to ptr[1024] (one byte beyond allocated block). Expect Segfault!");
      ptr[1024] = 'Z'; // Lỗi: Truy cập ngoài biên!

      // Dòng này sẽ không được thực thi nếu ElectricFence hoạt động
      AppLogger::log(AppLogger::INFO_L, "Program reached end (unexpected).");

      free(ptr);
      return EXIT_SUCCESS;
  }
  ```

  * **Cách Biên dịch và Chạy:**
    1. **Không dùng ElectricFence:**
       **Bash**

       ```
       g++ efence_example.cpp -o efence_example
       ./efence_example
       # Có thể không Segfault ngay, hoặc Segfault ở nơi khác
       ```
    2. **Dùng ElectricFence (cần cài đặt `libefence-dev` trên Ubuntu/Debian):**
       **Bash**

       ```
       sudo apt install libefence-dev # Cài đặt thư viện
       g++ efence_example.cpp -o efence_example -lefence -g # Liên kết với -lefence, -g để dùng GDB
       ./efence_example
       # Sẽ Segfault ngay lập tức tại dòng 'ptr[1024] = 'Z';'
       ```
    3. **Dùng ElectricFence với GDB:**
       **Bash**

       ```
       gdb ./efence_example
       (gdb) run
       # GDB sẽ dừng lại và báo lỗi tại dòng 10 (ptr[1024] = 'Z';)
       (gdb) backtrace
       ```

##### **5.2.2. `Valgrind` 🕵️**

* **Lý thuyết:** `Valgrind` là một bộ công cụ phân tích động (dynamic analysis) mạnh mẽ và đa năng. Nó chạy chương trình của bạn trên một CPU giả (synthetic CPU) và theo dõi mọi thao tác truy cập bộ nhớ, phát hiện nhiều loại lỗi khác nhau.
* **Các công cụ chính của Valgrind:**

  * **Memcheck (Mặc định):** Công cụ chính để phát hiện các lỗi liên quan đến bộ nhớ:
    * Đọc/ghi vào bộ nhớ chưa khởi tạo.
    * Đọc/ghi vào bộ nhớ đã giải phóng (use-after-free).
    * Đọc/ghi ngoài giới hạn của khối cấp phát (out-of-bounds access).
    * Rò rỉ bộ nhớ (memory leaks): Phát hiện bộ nhớ được cấp phát nhưng không bao giờ được giải phóng.
    * Double-free.
    * Truy cập các vùng bộ nhớ không hợp lệ khác.
  * **Cachegrind:** Phân tích hiệu suất cache.
  * **Callgrind:** Phân tích hiệu suất lời gọi hàm.
  * **Helgrind / DRD:** Phát hiện lỗi điều kiện tranh chấp (race conditions) và deadlock trong các chương trình đa luồng.
* **Ưu điểm:**

  * **Không cần biên dịch lại:** Không yêu cầu biên dịch lại chương trình với cờ đặc biệt (mặc dù biên dịch với `-g` vẫn rất hữu ích để Valgrind hiển thị thông tin dòng code).
  * **Phát hiện nhiều loại lỗi:** Rất toàn diện trong việc tìm lỗi bộ nhớ và luồng.
  * **Thông tin chi tiết:** Báo cáo chính xác vị trí lỗi (file, dòng code) và stack trace.
* **Hạn chế:**

  * **Chậm:** Chương trình chạy dưới Valgrind sẽ chậm hơn đáng kể (thường từ 5 đến 50 lần) do quá trình theo dõi chi tiết.
  * **Không có trên mọi hệ thống nhúng:** Valgrind yêu cầu một môi trường Linux đầy đủ và kiến trúc CPU tương thích (thường là x86/x86_64, ARM). Nó có thể không có sẵn trên các thiết bị nhúng rất nhỏ.
* **Cách sử dụng:** Bạn chạy chương trình của mình thông qua lệnh `valgrind`.
  **Bash**

  ```
  valgrind --leak-check=yes --track-origins=yes --show-leak-kinds=all ./your_program [args]
  ```

  * `--leak-check=yes`: Bật kiểm tra rò rỉ bộ nhớ.
  * `--track-origins=yes`: Cố gắng theo dõi nguồn gốc của các giá trị chưa khởi tạo.
  * `--show-leak-kinds=all`: Hiển thị tất cả các loại rò rỉ (definitely lost, indirectly lost, possibly lost, reachable).
  * `-v` (verbose): Hiển thị thông tin chi tiết hơn.
* **Liên hệ Embedded Linux:** Valgrind là công cụ **vô giá** để gỡ lỗi các ứng dụng phức tạp trên các hệ thống Embedded Linux. Nó giúp bạn tìm ra các rò rỉ bộ nhớ và lỗi truy cập bộ nhớ mà có thể không gây crash ngay lập tức nhưng sẽ làm suy giảm hiệu suất và độ ổn định của thiết bị theo thời gian.
* **Ví dụ (C++): `valgrind_example.cpp` - Sử dụng `Valgrind`**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <cstdlib> // For malloc, free, exit
  #include <cstring> // For memset

  // Logger namespace (as defined in previous modules)
  namespace AppLogger {
      enum LogLevel { TRACE_L, DEBUG_L, INFO_L, SUCCESS_L, WARNING_L, ERROR_L, CRITICAL_L };
      static const std::map<LogLevel, std::string> LogLevelNames = {
          {TRACE_L,    "TRACE   "}, {DEBUG_L,    "DEBUG   "}, {INFO_L,     "INFO    "},
          {SUCCESS_L,  "SUCCESS "}, {WARNING_L,  "WARNING "}, {ERROR_L,    "ERROR   "},
          {CRITICAL_L, "CRITICAL"}
      };
      void log(LogLevel level, const std::string& message) {
          std::cout << LogLevelNames.at(level) << ": " << message << std::endl;
      }
  }

  void buggy_function() {
      char *leak_ptr = (char *)malloc(100); // Rò rỉ bộ nhớ: không bao giờ free
      if (leak_ptr) {
          memset(leak_ptr, 'L', 100);
          AppLogger::log(AppLogger::INFO_L, "Buggy function: Allocated 100 bytes (will leak).");
      }

      char *oob_ptr = (char *)malloc(50); // Lỗi truy cập ngoài biên
      if (oob_ptr) {
          AppLogger::log(AppLogger::INFO_L, "Buggy function: Allocated 50 bytes (will have OOB access).");
          oob_ptr[50] = 'X'; // Lỗi: Ghi ngoài giới hạn
          AppLogger::log(AppLogger::INFO_L, "Buggy function: Wrote 'X' out-of-bounds.");
          free(oob_ptr);
      }

      char *double_free_ptr = (char *)malloc(20); // Lỗi double-free
      if (double_free_ptr) {
          AppLogger::log(AppLogger::INFO_L, "Buggy function: Allocated 20 bytes (will double-free).");
          free(double_free_ptr);
          AppLogger::log(AppLogger::INFO_L, "Buggy function: First free.");
          free(double_free_ptr); // Lỗi: Double-free
          AppLogger::log(AppLogger::INFO_L, "Buggy function: Second free (double-free occurred).");
      }
  }

  int main() {
      AppLogger::log(AppLogger::INFO_L, "--- Valgrind Example Program (with intentional bugs) ---");

      buggy_function();

      char *main_leak_ptr = (char *)malloc(200); // Rò rỉ bộ nhớ trong main
      if (main_leak_ptr) {
          AppLogger::log(AppLogger::INFO_L, "Main: Allocated 200 bytes (will also leak).");
      }

      AppLogger::log(AppLogger::INFO_L, "Program finished.");
      return EXIT_SUCCESS;
  }
  ```

  * **Cách Biên dịch và Chạy với Valgrind:**
    **Bash**

    ```
    g++ valgrind_example.cpp -o valgrind_example -g # Biên dịch với -g để có thông tin dòng code
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -v ./valgrind_example
    ```
  * **Phân tích Output của Valgrind:**

    * Valgrind sẽ báo cáo chi tiết từng lỗi: `Invalid write of size 1` (cho `oob_ptr[50]`), `Invalid free()` hoặc `double free or corruption` (cho `double_free_ptr`).
    * Quan trọng hơn, nó sẽ cung cấp `LEAK SUMMARY` ở cuối, chỉ ra bao nhiêu byte bộ nhớ đã bị `definitely lost` (rò rỉ chắc chắn), `possibly lost`, v.v., cùng với stack trace nơi bộ nhớ đó được cấp phát.

---

### **Câu hỏi Tự đánh giá Module 5 🤔**

1. Phân biệt giữa "rò rỉ bộ nhớ" (memory leak) và "truy cập bộ nhớ không hợp lệ" (invalid memory access). Nêu một hậu quả của mỗi loại lỗi.
2. Giải thích cách `ElectricFence` giúp phát hiện lỗi truy cập ngoài biên. Ưu điểm chính của nó so với việc chỉ dùng GDB là gì?
3. Tại sao `Valgrind` được coi là một công cụ gỡ lỗi bộ nhớ mạnh mẽ và toàn diện hơn `ElectricFence`? Nêu ít nhất ba loại lỗi bộ nhớ mà `Valgrind` có thể phát hiện.
4. Khi sử dụng `Valgrind`, tại sao việc biên dịch chương trình với cờ `-g` lại rất hữu ích, mặc dù `Valgrind` không yêu cầu nó?
5. Nêu một ưu điểm và một nhược điểm của việc sử dụng `Valgrind` trong quá trình phát triển.

---

### **Bài tập Thực hành Module 5 ✍️**

1. **Bài tập Debugging Bộ nhớ (Toàn diện):**
   * Lấy lại file `buggy_sort.c` (phiên bản đã sửa lỗi Segfault và `n--` từ Module 2, nhưng vẫn giữ `data[4096]` lớn) hoặc tạo một file mới có chứa các lỗi bộ nhớ cố ý (ví dụ: một rò rỉ bộ nhớ nhỏ, một lỗi use-after-free, hoặc một lỗi double-free).
   * Biên dịch chương trình với cờ `-g`.
   * **Bước 1: Chẩn đoán với `Valgrind`:**
     **Bash**

     ```
     valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./your_buggy_program
     ```

     * **Mục tiêu:** Phân tích output của Valgrind. Nó báo cáo những lỗi nào? Vị trí (file, dòng) của lỗi là ở đâu? Loại rò rỉ (nếu có) là gì?
   * **Bước 2: Xác nhận với `GDB`:**

     * Nếu Valgrind báo cáo lỗi truy cập bộ nhớ hoặc crash, hãy chạy chương trình trong GDB.
     * Đặt breakpoint tại dòng mà Valgrind báo lỗi.
     * `run` và khi GDB dừng, kiểm tra các biến liên quan.
     * **Mục tiêu:** Xác nhận lỗi bằng GDB và hiểu rõ nguyên nhân.
   * **Bước 3: Sửa lỗi:**

     * Sửa tất cả các lỗi bộ nhớ mà bạn đã tìm thấy.
     * Biên dịch lại và chạy lại với `Valgrind` để xác nhận rằng tất cả các lỗi đã được khắc phục (Valgrind báo `ERROR SUMMARY: 0 errors from 0 contexts`).
2. **Bài tập Phát hiện Rò rỉ Bộ nhớ trong Vòng lặp:**
   * Viết một chương trình C++ (`loop_leak.cpp`) mà trong hàm `main()`, có một vòng lặp chạy 1000 lần.
   * Trong mỗi lần lặp, cấp phát một khối bộ nhớ nhỏ (ví dụ: 100 byte) bằng `malloc()` hoặc `new char[]`.
   * **Không giải phóng** bộ nhớ đó.
   * Sau vòng lặp, in ra thông báo "Loop finished. Check for leaks."
   * **Mục tiêu:**
     * Chạy chương trình này với `Valgrind`.
     * Quan sát `LEAK SUMMARY` để thấy tổng số byte bị rò rỉ và số khối.
     * Giải thích tại sao lỗi này lại nguy hiểm trong các ứng dụng chạy liên tục trên thiết bị nhúng.
