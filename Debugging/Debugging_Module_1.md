# **Giáo trình: Debugging 🐞**

**Mục tiêu của Giáo trình 🎯**

Sau khi hoàn thành giáo trình này, bạn sẽ có thể:

* Phân loại các loại lỗi phần mềm và hiểu nguồn gốc của chúng.
* Áp dụng các kỹ thuật gỡ lỗi chung hiệu quả.
* Sử dụng trình gỡ lỗi **GDB** để kiểm soát thực thi chương trình, kiểm tra biến và theo dõi stack.
* Sử dụng các công cụ phân tích tĩnh như `lint`/`splint`, `ctags`, `cxref`, `cflow` để hiểu cấu trúc code.
* Thực hiện phân tích hiệu suất với `prof`/`gprof`.
* Hiểu và sử dụng các **Assertion** để xác nhận logic nội bộ.
* Chẩn đoán và giải quyết các vấn đề liên quan đến bộ nhớ động như rò rỉ bộ nhớ và truy cập bộ nhớ không hợp lệ bằng **ElectricFence** và  **Valgrind** .

---

### **Cấu trúc Giáo trình 📚**

Giáo trình này sẽ được chia thành các Modules nhỏ để dễ dàng theo dõi và tiếp thu:

* **Module 1: Các loại Lỗi và Kỹ thuật Gỡ lỗi Chung**
* **Module 2: GDB - Trình gỡ lỗi GNU**
* **Module 3: Các Công cụ Gỡ lỗi Khác (Phân tích tĩnh và động)**
* **Module 4: Assertions**
* **Module 5: Gỡ lỗi Sử dụng Bộ nhớ (Memory Debugging)**

Mỗi Module sẽ bao gồm:

* **Lý thuyết chi tiết:** Giải thích các khái niệm, hàm, và cơ chế.
* **Ví dụ Code (C++):** Minh họa cách sử dụng các hàm/công cụ.
* **Liên hệ với Embedded Linux:** Giải thích tầm quan trọng và ứng dụng trong hệ thống nhúng.
* **Câu hỏi Tự đánh giá:** Giúp bạn kiểm tra mức độ hiểu bài.
* **Bài tập Thực hành:** Các bài tập coding/sử dụng công cụ để bạn áp dụng kiến thức.

Hãy bắt đầu với Module đầu tiên!

---

### **Module 1: Các loại Lỗi và Kỹ thuật Gỡ lỗi Chung 🐛**

Trong phần mềm, lỗi (defects hoặc bugs) là không thể tránh khỏi. Việc hiểu các loại lỗi và cách tiếp cận gỡ lỗi là bước đầu tiên để trở thành một lập trình viên hiệu quả.

#### **1.1. Các loại Lỗi Phần mềm (Types of Errors) 😵**

Một lỗi phần mềm thường xuất phát từ một trong những nguyên nhân sau:

1. **Lỗi Đặc tả (Specification Errors) 📝:**
   * **Lý thuyết:** Đây là lỗi xảy ra  **trước khi viết code** , khi yêu cầu của chương trình không rõ ràng, không chính xác, hoặc không đầy đủ. Chương trình hoạt động đúng theo những gì được chỉ định, nhưng những gì được chỉ định lại là sai so với nhu cầu thực tế.
   * **Cách phát hiện:** Xem xét kỹ lưỡng yêu cầu, thảo luận với người dùng/khách hàng, tạo các tài liệu thiết kế rõ ràng.
   * **Liên hệ Embedded Linux:** Trong nhúng, lỗi đặc tả có thể dẫn đến việc chọn sai cảm biến, sai giao thức truyền thông, hoặc bỏ qua các yêu cầu về thời gian thực, gây ra thiết bị hoạt động sai hoặc không đáp ứng.
2. **Lỗi Thiết kế (Design Errors) 📐:**
   * **Lý thuyết:** Xảy ra khi kiến trúc hoặc thiết kế của chương trình có vấn đề (ví dụ: cấu trúc dữ liệu không phù hợp, thuật toán sai, luồng logic không hợp lý), dẫn đến việc khó triển khai hoặc hoạt động không hiệu quả/sai.
   * **Cách phát hiện:** Xem xét thiết kế, lập sơ đồ luồng, chạy thử trên giấy (dry running), thử nghiệm các trường hợp biên.
   * **Liên hệ Embedded Linux:** Lỗi thiết kế trong nhúng có thể liên quan đến việc quản lý tài nguyên (bộ nhớ, CPU), đồng bộ hóa luồng/tiến trình, hoặc thiết kế driver, dẫn đến deadlock, race conditions hoặc hiệu suất kém.
3. **Lỗi Mã hóa (Coding Errors) ⌨️:**
   * **Lý thuyết:** Đây là lỗi mà hầu hết lập trình viên nghĩ đến khi nói về "bug" – các lỗi cú pháp (syntax errors), lỗi logic, lỗi đánh máy, lỗi xử lý biến, v.v., xảy ra trong quá trình viết mã nguồn.
   * **Cách phát hiện:**
     * **Trình biên dịch (Compiler):** Phát hiện lỗi cú pháp và một số cảnh báo (warnings) về các thực hành đáng ngờ (ví dụ: `gcc -Wall -pedantic -ansi`). **Luôn biên dịch với các cờ cảnh báo cao nhất!**
     * **Đọc lại mã (Code Inspection):** Tự mình đọc lại code hoặc nhờ người khác đọc (peer review/code review).
     * **Chạy thử trên giấy (Dry Running):** Theo dõi giá trị biến từng bước như CPU sẽ làm.
   * **Liên hệ Embedded Linux:** Là nguồn lỗi phổ biến nhất. Các lỗi này trong nhúng thường khó tái hiện và gỡ lỗi hơn do môi trường hạn chế và các vấn đề về thời gian/phần cứng.

#### **1.2. Các Kỹ thuật Gỡ lỗi Chung (General Debugging Techniques) 🕵️‍♀️**

Gỡ lỗi là một quá trình có hệ thống, không chỉ là sửa lỗi. Các giai đoạn chính bao gồm:

1. **Kiểm thử (Testing) ✅:**
   * **Mục đích:** Tìm ra các lỗi đang tồn tại.
   * **Cách thực hiện:** Chạy chương trình với nhiều bộ dữ liệu đầu vào và điều kiện khác nhau (unit tests, integration tests, system tests, stress tests).
2. **Ổn định lỗi (Stabilization) 🎯:**
   * **Mục đích:** Khi tìm thấy một lỗi, điều quan trọng là phải làm cho lỗi đó **tái hiện được (reproducible)** một cách đáng tin cậy. Nếu lỗi chỉ xảy ra ngẫu nhiên, việc sửa nó gần như không thể.
   * **Cách thực hiện:** Ghi lại chính xác các bước, điều kiện, dữ liệu đầu vào dẫn đến lỗi.
3. **Khoanh vùng lỗi (Localization) 📍:**
   * **Mục đích:** Xác định chính xác dòng (hoặc vùng) mã nguồn nào chịu trách nhiệm gây ra lỗi.
   * **Cách thực hiện:**
     * **Kiểm tra mã (Code Inspection):** Đọc lại code một cách cẩn thận.
     * **Instrumentation (Thêm mã kiểm tra):** Thêm các câu lệnh `printf` (hoặc logging) để in ra giá trị của biến, trạng thái chương trình tại các điểm khác nhau. (Sẽ học chi tiết hơn ở phần sau).
     * **Sử dụng trình gỡ lỗi (Debugger):** Chạy chương trình dưới sự kiểm soát của debugger (như GDB) để từng bước thực thi code, kiểm tra biến, và theo dõi luồng. (Đây là kỹ thuật mạnh mẽ nhất và sẽ được học chi tiết trong Module 2).
4. **Sửa lỗi (Correction) 🩹:**
   * **Mục đích:** Sửa chữa mã nguồn đã được khoanh vùng.
   * **Cách thực hiện:** Dựa vào hiểu biết về nguyên nhân gốc rễ, sửa lỗi logic, cú pháp, hoặc thiết kế.
5. **Xác minh (Verification) ✅:**
   * **Mục đích:** Đảm bảo rằng lỗi đã được sửa và không có lỗi mới nào được tạo ra.
   * **Cách thực hiện:** Chạy lại các bài kiểm thử đã được sử dụng để phát hiện lỗi ban đầu, và mở rộng thêm các bài kiểm thử hồi quy (regression tests) để đảm bảo các chức năng khác không bị ảnh hưởng.

#### **1.3. Ví dụ về Chương trình có Lỗi 🐞**

Đoạn văn giới thiệu một chương trình ví dụ (`debug1.c`, sau đó `debug2.c`, `debug3.c`) có lỗi. Mục tiêu là một hàm `sort` (bubble sort) sắp xếp một mảng `item` theo `key`.

* **Lỗi 1 (trong `debug3.c`):** **Segmentation Fault (`SIGSEGV`)** khi truy cập ngoài biên mảng.
  * Xảy ra ở dòng `if(a[j].key > a[j+1].key) {` (dòng 23) hoặc `a[j] = a[j+1];` (dòng 25) do biến `j` đạt giá trị `n-1` trong vòng lặp `for(j=0; j<n; j++)`, khiến `a[j+1]` truy cập `a[n]`, vượt quá giới hạn mảng (mảng có `n` phần tử, chỉ số từ `0` đến `n-1`).
  * Việc lỗi `SIGSEGV` có xuất hiện hay không phụ thuộc vào cấu hình phần cứng và cách Kernel cấp phát bộ nhớ. Đôi khi, truy cập ngoài biên mảng vẫn nằm trong vùng bộ nhớ được cấp phát cho chương trình, nên Kernel không phát hiện được ngay, dẫn đến hành vi không xác định (undefined behavior) hoặc lỗi ngẫu nhiên sau này.

---

### **1.4. Kiểm tra Mã nguồn (Code Inspection) 🧑‍💻**

* **Lý thuyết:** Đơn giản là việc đọc lại mã nguồn một cách cẩn thận để tìm lỗi.
* **Công cụ hỗ trợ:**
  * **Trình biên dịch (Compiler):** Không chỉ phát hiện lỗi cú pháp mà còn đưa ra các **cảnh báo (warnings)** về các thực hành code đáng ngờ (ví dụ: `gcc -Wall -pedantic -ansi`). **Luôn bật các cảnh báo này!**
  * **Công cụ phân tích tĩnh (Static Analysis Tools):** Như `lint` hoặc `Splint` (sẽ học chi tiết hơn ở Module 3). Chúng phân tích mã nguồn mà không cần chạy chương trình, tìm kiếm các lỗi tiềm ẩn, vấn đề về phong cách, hoặc các thực hành không an toàn.
* **Liên hệ Embedded Linux:** Trong các dự án nhúng, code review (kiểm tra mã) là cực kỳ quan trọng để bắt các lỗi sớm, đặc biệt là các lỗi liên quan đến quản lý bộ nhớ, đồng bộ hóa, và tương tác phần cứng.

---

### **1.5. Instrumentation (Thêm mã kiểm tra) 🧪**

* **Lý thuyết:** Là việc thêm các đoạn mã vào chương trình để thu thập thêm thông tin về hành vi của chương trình khi nó chạy.
* **Kỹ thuật phổ biến:**

  * **Sử dụng `printf` hoặc Logging:** In ra giá trị của biến, thông báo trạng thái, hoặc các điểm đánh dấu (timestamps) tại các giai đoạn khác nhau của chương trình.
    * **Ưu điểm:** Đơn giản, dễ thực hiện.
    * **Nhược điểm:** Yêu cầu biên dịch lại mỗi khi thay đổi, cần loại bỏ code debug khi release.
  * **Biên dịch có điều kiện (`#ifdef DEBUG`)** :
    **C++**

  ```
  #ifdef DEBUG
  // Code debug chỉ được biên dịch khi DEBUG được định nghĩa
  std::cout << "DEBUG   : Variable x = " << x << std::endl;
  #endif
  ```

  * Bạn biên dịch với cờ `-DDEBUG` để bao gồm code debug, hoặc không có cờ để loại bỏ nó.
  * Có thể sử dụng các cấp độ debug số học (ví dụ: `#define DEBUG_LEVEL 4`, `#if (DEBUG_LEVEL & SOME_FLAG)`).
  * **Macros tiền xử lý chuẩn (`__LINE__`, `__FILE__`, `__DATE__`, `__TIME__`)** :
  * `__LINE__`: Số dòng hiện tại.
  * `__FILE__`: Tên file hiện tại.
  * `__DATE__`: Ngày biên dịch (`"Mmm dd yyyy"`).
  * `__TIME__`: Thời gian biên dịch (`"hh:mm:ss"`).
  * **Ứng dụng:** Rất hữu ích để thêm thông tin vị trí và thời gian vào các thông báo lỗi hoặc debug.
* **Kỹ thuật Debug không cần Biên dịch lại (No-Recompile Debugging):**

  * Thêm một biến toàn cục làm cờ debug (ví dụ: `bool debug_enabled = false;`).
  * Cho phép người dùng bật cờ này qua tham số dòng lệnh (ví dụ: `--debug` hoặc `-d`).
  * Sử dụng hàm logging tùy chỉnh (có thể ghi ra `stderr` hoặc `syslog`).
  * **Lợi ích:** Có thể bật debug ngay cả trên bản phát hành (release version) nếu người dùng gặp sự cố.
* **Ví dụ (C++): `debug_info.cpp` - Thông tin Biên dịch và Instrumentation**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <cstdlib> // For EXIT_SUCCESS

  // Logger namespace (for consistent output style)
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

  // Định nghĩa macro DEBUG nếu chưa được định nghĩa
  #ifndef DEBUG
  #define DEBUG 0 // Mặc định tắt debug
  #endif

  // Một biến cờ debug toàn cục có thể bật/tắt qua dòng lệnh (ví dụ: -d)
  bool runtime_debug_enabled = false; 

  // Hàm debug tùy chỉnh
  void my_debug_log(const std::string& message, int line, const std::string& file) {
      if (runtime_debug_enabled) { // Chỉ in nếu cờ debug bật
          std::cout << "RUNTIME_DEBUG: " << file << ":" << line << ": " << message << std::endl;
      }
  }


  int main(int argc, char* argv[]) {
      // --- Xử lý tham số dòng lệnh đơn giản cho runtime_debug_enabled ---
      if (argc > 1 && std::string(argv[1]) == "-d") {
          runtime_debug_enabled = true;
          AppLogger::log(AppLogger::INFO_L, "Runtime debugging enabled via command line.");
      }

      // --- 1. Thông tin biên dịch (#ifdef DEBUG) ---
  #if DEBUG // Chỉ được biên dịch nếu DEBUG được định nghĩa là khác 0
      AppLogger::log(AppLogger::INFO_L, "--- Compile-time Debug Info ---");
      printf("INFO    : Compiled: " __DATE__ " at " __TIME__ "\n");
      printf("INFO    : This is line %d of file %s\n", __LINE__, __FILE__);
  #endif

      AppLogger::log(AppLogger::INFO_L, "--- Program Execution ---");

      int x = 10;
      AppLogger::log(AppLogger::INFO_L, "Value of x before operation: " + std::to_string(x));

      // --- 2. Instrumentation runtime ---
      my_debug_log("About to perform calculation.", __LINE__, __FILE__);
      x = x * 2 + 5;
      my_debug_log("Value of x after calculation: " + std::to_string(x), __LINE__, __FILE__);

      AppLogger::log(AppLogger::INFO_L, "Hello World!");

      return EXIT_SUCCESS;
  }
  ```

  * **Cách biên dịch và chạy:**
    **Bash**

    ```
    g++ debug_info.cpp -o debug_info               # Tắt debug (DEBUG=0 mặc định)
    g++ debug_info.cpp -o debug_info -DDEBUG=1     # Bật debug biên dịch (DEBUG=1)

    ./debug_info                                   # Chạy không có debug output (nếu DEBUG=0)
    ./debug_info -d                                # Bật debug runtime
    ./debug_info -d # Nếu biên dịch với -DDEBUG=1, sẽ có cả 2 loại debug
    ```
  * **Phân tích:** Quan sát cách các thông báo `INFO` luôn xuất hiện, trong khi thông báo `Compiled:` chỉ xuất hiện khi biên dịch với `-DDEBUG`, và `RUNTIME_DEBUG:` chỉ xuất hiện khi chạy với `-d`.

---

### **Câu hỏi Tự đánh giá Module 1 🤔**

1. Nêu ba loại lỗi phần mềm chính được đề cập và giải thích ngắn gọn nguồn gốc của mỗi loại. Lỗi nào thường được phát hiện sớm nhất trong quá trình phát triển?
2. Giải thích các giai đoạn trong quy trình gỡ lỗi (Testing, Stabilization, Localization, Correction, Verification). Tại sao việc làm cho lỗi "reproducible" lại quan trọng?
3. Khi một chương trình C gây ra `Segmentation Fault`, điều gì thường là nguyên nhân gốc rễ? Tại sao `SIGSEGV` không phải lúc nào cũng xuất hiện ngay cả khi có lỗi truy cập bộ nhớ?
4. Bạn muốn thêm các thông báo debug vào code của mình mà không muốn chúng xuất hiện trong bản release cuối cùng. Nêu hai kỹ thuật tiền xử lý C bạn có thể sử dụng.
5. Giải thích ý nghĩa của các macros tiền xử lý chuẩn như `__LINE__` và `__FILE__`. Chúng hữu ích như thế nào trong debugging?

---

### **Bài tập Thực hành Module 1 ✍️**

1. **Chương trình "Buggy Sort" (Phần 1 - Tái hiện lỗi):**
   * Sao chép mã nguồn của `debug1.c` (hoặc `debug3.c` như trong bài học) vào một file mới `buggy_sort.c`.
     **C**

     ```cpp
     // buggy_sort.c
     #include <stdio.h>
     #include <stdlib.h> // For exit
     #include <string.h> // For strcpy

     typedef struct {
         char data[4096]; // Make data larger to provoke SIGSEGV more reliably
         int key;
     } item;

     item array[] = {
         {"bill", 3},
         {"neil", 4},
         {"john", 2},
         {"rick", 5},
         {"alex", 1},
     };

     // This sort function has bugs
     void sort(item *a, int n) { // Changed to void, explicit params
         int i = 0, j = 0;
         int s = 1;

         for (; i < n && s != 0; i++) {
             s = 0;
             for (j = 0; j < n; j++) { // This loop condition is wrong
                 if (a[j].key > a[j+1].key) { // This can cause out-of-bounds access
                     item t = a[j];
                     a[j] = a[j+1];
                     a[j+1] = t;
                     s++;
                 }
             }
             n--; // This also contributes to the bug
         }
     }

     int main() {
         int i;
         printf("--- Initial Array ---\n");
         for (i = 0; i < 5; i++) {
             printf("array[%d] = {%s, %d}\n", i, array[i].data, array[i].key);
         }
         printf("---------------------\n");

         sort(array, 5); // Pass array and its size

         printf("--- Sorted Array (after sort attempt) ---\n");
         for (i = 0; i < 5; i++) {
             printf("array[%d] = {%s, %d}\n", i, array[i].data, array[i].key);
         }
         printf("---------------------\n");
         exit(EXIT_SUCCESS);
     }
     ```
   * Biên dịch chương trình này (`gcc buggy_sort.c -o buggy_sort`) và chạy nó.
   * **Mục tiêu:** Quan sát xem bạn có nhận được `Segmentation fault` hay không, hoặc output có bị sai (mảng không được sắp xếp đúng hoặc có giá trị rác) không. Ghi lại kết quả quan sát của bạn.
2. **Chương trình "Custom Debugging":**
   * Viết một chương trình C++ (`custom_debug.cpp`) mà:
     * Định nghĩa một macro `DEBUG_LEVEL` (ví dụ: `0`, `1`, `2`) bằng cờ biên dịch (`-DDEBUG_LEVEL=X`).
     * Viết một hàm `my_log(int level, const std::string& message, const char* file, int line)` tùy chỉnh. Hàm này chỉ in thông báo nếu `level` nhỏ hơn hoặc bằng `DEBUG_LEVEL`.
     * Sử dụng `my_log` với các cấp độ khác nhau (`1` cho INFO, `2` cho DEBUG) trong code của bạn.
     * Sử dụng `__FILE__` và `__LINE__` khi gọi `my_log`.
     * Thêm một tùy chọn dòng lệnh `-d` hoặc `--debug` để bật/tắt một biến cờ `runtime_debug_flag` toàn cục. Nếu cờ này bật, tất cả các thông báo `my_log` sẽ được in ra, bất kể `DEBUG_LEVEL` biên dịch (ví dụ: ghi đè hoặc tăng mức độ log nếu `runtime_debug_flag` bật).
   * **Thử thách:**
     * Chạy chương trình với các tổ hợp cờ biên dịch và dòng lệnh khác nhau để quan sát output.

---
