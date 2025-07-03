# **Giáo trình: Data Management 📊**

**Mục tiêu của Giáo trình 🎯**

Sau khi hoàn thành giáo trình này, bạn sẽ có thể:

* Hiểu cách Linux quản lý bộ nhớ ảo và cách cấp phát/giải phóng bộ nhớ động.
* Nắm vững các cơ chế khóa file để đồng bộ hóa quyền truy cập dữ liệu giữa các tiến trình.
* Sử dụng thư viện cơ sở dữ liệu `dbm` để lưu trữ và truy xuất dữ liệu có khóa.
* Áp dụng các kiến thức này để giải quyết các vấn đề về quản lý tài nguyên và dữ liệu trong lập trình Linux và nhúng.

---

### **Cấu trúc Giáo trình 📚**

Giáo trình này sẽ được chia thành các Modules nhỏ để dễ dàng theo dõi và tiếp thu:

* **Module 1: Quản lý Bộ nhớ (Managing Memory)**
* **Module 2: Khóa File (File Locking)**
* **Module 3: Cơ sở dữ liệu DBM (The `dbm` Database)**

Mỗi Module sẽ bao gồm:

* **Lý thuyết chi tiết:** Giải thích các khái niệm, hàm, và cơ chế.
* **Ví dụ Code (C++):** Minh họa cách sử dụng các hàm.
* **Liên hệ với Embedded Linux:** Giải thích tầm quan trọng và ứng dụng trong hệ thống nhúng.
* **Câu hỏi Tự đánh giá:** Giúp bạn kiểm tra mức độ hiểu bài.
* **Bài tập Thực hành:** Các bài tập coding để bạn áp dụng kiến thức.

Hãy bắt đầu với Module đầu tiên!

---

### **Module 1: Quản lý Bộ nhớ (Managing Memory) 🧠**

Trong mọi hệ thống máy tính, bộ nhớ luôn là một tài nguyên khan hiếm. Linux cung cấp một cách tiếp cận rất sạch sẽ để quản lý bộ nhớ thông qua hệ thống bộ nhớ ảo (virtual memory) được Kernel kiểm soát chặt chẽ, bảo vệ các ứng dụng khỏi nhau và cho phép truy cập bộ nhớ lớn hơn RAM vật lý.

#### **1.1. Cấp phát Bộ nhớ Đơn giản (Simple Memory Allocation) ✨**

* **Lý thuyết:**

  * Các ứng dụng Linux không truy cập trực tiếp vào bộ nhớ vật lý. Thay vào đó, chúng tương tác với một **không gian địa chỉ bộ nhớ ảo (virtual memory address space)** rộng lớn và độc lập cho mỗi tiến trình. Kernel sẽ quản lý việc ánh xạ từ bộ nhớ ảo sang bộ nhớ vật lý (và không gian swap).
  * Để cấp phát bộ nhớ động (trên  **heap** ), chúng ta sử dụng hàm **`malloc()`** từ thư viện C chuẩn.
    **C++**

    ```
    #include <cstdlib> // Cho malloc, size_t, EXIT_SUCCESS, EXIT_FAILURE
    // void *malloc(size_t size);
    ```
  * **`size`** : Số byte cần cấp phát. `size_t` là kiểu số nguyên không dấu, đủ lớn để chứa kích thước bộ nhớ.
  * **Giá trị trả về:** Con trỏ `void*` tới đầu khối bộ nhớ được cấp phát. Con trỏ này được đảm bảo căn chỉnh (aligned) để có thể ép kiểu (cast) an toàn sang bất kỳ kiểu con trỏ nào. Trả về `NULL` nếu thất bại (không thể cấp phát).
* **Liên hệ Embedded Linux:** Mặc dù Linux quản lý bộ nhớ rất tốt, nhưng trong các hệ thống nhúng với RAM hạn chế, việc cấp phát bộ nhớ cần được theo dõi cẩn thận. Hiểu `malloc()` là cơ bản để quản lý các cấu trúc dữ liệu động.
* **Ví dụ (C++): `memory1.cpp` - Cấp phát bộ nhớ đơn giản**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <cstdlib> // For malloc, size_t, EXIT_SUCCESS, EXIT_FAILURE
  #include <cstdio>  // For sprintf
  #include <cstring> // For strlen (used implicitly by sprintf here)

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

  #define A_MEGABYTE (1024 * 1024)

  int main() {
      char *some_memory;
      int megabyte = A_MEGABYTE;
      int exit_code = EXIT_FAILURE;

      AppLogger::log(AppLogger::INFO_L, "Attempting to allocate 1 Megabyte of memory.");

      some_memory = (char *)malloc(megabyte);

      if (some_memory != NULL) {
          AppLogger::log(AppLogger::SUCCESS_L, "Memory allocated successfully at address: " + std::to_string(reinterpret_cast<long long>(some_memory)));
          // Ghi dữ liệu vào vùng nhớ đã cấp phát
          sprintf(some_memory, "Hello World\n");
          // In ra nội dung
          printf("%s", some_memory); // Sử dụng printf vì some_memory đã có '\n'
          exit_code = EXIT_SUCCESS;
          // Lưu ý: Chưa giải phóng bộ nhớ ở đây, sẽ được giải phóng khi chương trình kết thúc.
          // Điều này chỉ ổn cho các chương trình ngắn ngủi.
      } else {
          AppLogger::log(AppLogger::ERROR_L, "Failed to allocate memory.");
      }

      exit(exit_code);
  }
  ```

#### **1.2. Cấp phát nhiều Bộ nhớ (Allocating Lots of Memory) 📦**

* **Lý thuyết:**

  * Linux sử dụng hệ thống  **bộ nhớ ảo theo yêu cầu (demand-paged virtual memory)** . Khi `malloc()` được gọi, Kernel thường chỉ cấp phát  **bộ nhớ ảo** , không phải bộ nhớ vật lý ngay lập tức (Lazy Allocation / Overcommit).
  * Bộ nhớ vật lý (physical memory pages) chỉ thực sự được cấp phát khi tiến trình **truy cập lần đầu tiên** vào một trang bộ nhớ ảo cụ thể (ví dụ: ghi dữ liệu vào đó). Nếu trang bộ nhớ không có trong RAM, xảy ra  **page fault** , Kernel sẽ nạp nó từ đĩa (nếu nằm trong swap space) hoặc cấp phát một trang vật lý mới.
  * Điều này cho phép các chương trình dường như cấp phát nhiều bộ nhớ hơn cả RAM vật lý + swap space.
  * Khi cả RAM vật lý và swap space cạn kiệt, Kernel có thể kích hoạt **OOM Killer (Out Of Memory Killer)** để tự bảo vệ hệ thống bằng cách chấm dứt (kill) các tiến trình tiêu thụ nhiều bộ nhớ nhất. Hành vi này khác với một số hệ thống cũ nơi `malloc()` đơn giản thất bại.
* **Liên hệ Embedded Linux:** Rất quan trọng khi tối ưu hóa ứng dụng cho thiết bị nhúng. Hiểu rằng `malloc()` thành công không có nghĩa là bạn đã có RAM vật lý. Việc ghi vào bộ nhớ mới cấp phát có thể gây ra OOM và làm chương trình bị kill. Cần theo dõi mức sử dụng bộ nhớ và swap (qua `/proc/meminfo`) để tránh tình trạng này.
* **Ví dụ (C++): `memory2.cpp` - Yêu cầu nhiều bộ nhớ hơn RAM vật lý**

  * **Cảnh báo:** Chương trình này có thể làm chậm hệ thống hoặc bị OOM Killer kết thúc, đặc biệt trên máy có ít RAM. Chỉ chạy nếu bạn hiểu rõ rủi ro!

  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <cstdlib>  // For malloc, exit, EXIT_SUCCESS, EXIT_FAILURE
  #include <cstdio>   // For sprintf
  #include <unistd.h> // For sleep (optional, for observation)

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

  #define A_MEGABYTE (1024 * 1024)
  // Điều chỉnh số này dựa trên RAM vật lý của bạn. Ví dụ, nếu bạn có 4GB RAM, đặt 256 để thử 512MB
  #define PHY_MEM_MEGS 256 

  int main() {
      char *some_memory;
      size_t size_to_allocate = A_MEGABYTE;
      int megs_obtained = 0;

      AppLogger::log(AppLogger::INFO_L, "Attempting to allocate more memory than physical RAM.");
      AppLogger::log(AppLogger::WARNING_L, "This program may cause system slowdown or be terminated by OOM Killer!");
      AppLogger::log(AppLogger::INFO_L, "Targeting approx. " + std::to_string(PHY_MEM_MEGS * 2) + " Megabytes (twice physical memory for demonstration).");

      while (megs_obtained < (PHY_MEM_MEGS * 2)) {
          some_memory = (char *)malloc(size_to_allocate);
          if (some_memory != NULL) {
              megs_obtained++;
              // Ghi dữ liệu vào khối nhớ để buộc Kernel cấp phát trang vật lý
              sprintf(some_memory, "Hello World - Block %d", megs_obtained);
              AppLogger::log(AppLogger::INFO_L, "Allocated: " + std::to_string(megs_obtained) + " MB. Content: " + std::string(some_memory));
              // sleep(1); // Để quan sát quá trình cấp phát
          } else {
              AppLogger::log(AppLogger::CRITICAL_L, "Failed to allocate memory at " + std::to_string(megs_obtained + 1) + " MB.");
              AppLogger::log(AppLogger::INFO_L, "Exiting. Total allocated before failure: " + std::to_string(megs_obtained) + " MB.");
              exit(EXIT_FAILURE);
          }
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Successfully allocated " + std::to_string(megs_obtained) + " Megabytes (as much as requested/possible).");
      exit(EXIT_SUCCESS);
  }
  ```

#### **1.3. Lạm dụng Bộ nhớ (Abusing Memory) 😈**

* **Lý thuyết:** Lạm dụng bộ nhớ xảy ra khi chương trình truy cập vào vùng bộ nhớ mà nó không có quyền hoặc không được cấp phát, dẫn đến các lỗi nghiêm trọng. Linux, với cơ chế bảo vệ bộ nhớ của mình, sẽ chấm dứt chương trình đó để bảo vệ hệ thống.

  * **Segmentation Fault (Segfault / `SIGSEGV`):** Lỗi phổ biến nhất khi lạm dụng bộ nhớ. Xảy ra khi:
    * **Truy cập ngoài biên (Out-of-bounds access):** Đọc/ghi ngoài giới hạn của một mảng hoặc khối bộ nhớ được cấp phát.
    * **Dereferencing NULL pointer:** Cố gắng truy cập bộ nhớ thông qua một con trỏ `NULL`.
    * **Use-after-free:** Sử dụng bộ nhớ sau khi đã giải phóng nó. Vùng nhớ đó có thể đã được tái cấp phát hoặc chứa dữ liệu không hợp lệ.
    * **Double-free:** Giải phóng cùng một khối bộ nhớ hai lần.
  * Kernel sẽ gửi tín hiệu `SIGSEGV` đến tiến trình, dẫn đến việc chương trình bị chấm dứt và thường tạo ra `core dump` file để debug.
* **Liên hệ Embedded Linux:** Các lỗi này cực kỳ nguy hiểm trong nhúng. Chúng có thể làm treo toàn bộ thiết bị. Việc hiểu rõ nguyên nhân và sử dụng các công cụ debug bộ nhớ (ví dụ: Valgrind, GDB) là tối quan trọng.
* **Ví dụ (C++): `memory_abuse.cpp` - Cố tình gây lỗi Segfault**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <cstdlib>  // For malloc, free, exit, EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>  // For memset

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

  #define ONE_K (1024)

  int main() {
      char *some_memory;
      char *scan_ptr;

      // --- Ví dụ 1: Truy cập ngoài biên (Out-of-bounds write) ---
      AppLogger::log(AppLogger::INFO_L, "--- Demonstrating Out-of-bounds Write (should cause Segfault) ---");
      some_memory = (char *)malloc(ONE_K); // Cấp phát 1KB
      if (some_memory == NULL) {
          AppLogger::log(AppLogger::ERROR_L, "Malloc failed.");
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "1KB memory allocated at " + std::to_string(reinterpret_cast<long long>(some_memory)));

      scan_ptr = some_memory;
      AppLogger::log(AppLogger::WARNING_L, "Attempting to write past the allocated 1KB boundary. Expect Segfault.");

      // Vòng lặp ghi dữ liệu ra ngoài giới hạn 1KB
      for (int i = 0; i < ONE_K * 2; ++i) { // Cố tình ghi 2KB
          *(scan_ptr + i) = 'A'; // Ghi vào vị trí i
          if (i == ONE_K) {
              AppLogger::log(AppLogger::INFO_L, "Crossed 1KB boundary. Still writing. Segfault imminent...");
          }
      }

      AppLogger::log(AppLogger::INFO_L, "Program reached end of loop (unexpected if Segfault occurred).");

      free(some_memory);
      return EXIT_SUCCESS; // Sẽ không đạt được nếu Segfault xảy ra
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ memory_abuse.cpp -o memory_abuse
    ./memory_abuse
    ```
  * **Phân tích:** Chương trình sẽ chạy một đoạn, sau đó bạn sẽ thấy thông báo `Segmentation fault (core dumped)`. Điều này cho thấy Kernel đã bảo vệ hệ thống khỏi việc chương trình ghi vào vùng bộ nhớ không thuộc về nó.

#### **1.4. Con trỏ NULL (The Null Pointer) 👻**

* **Lý thuyết:**

  * Con trỏ `NULL` là một con trỏ đặc biệt không trỏ tới bất kỳ vị trí bộ nhớ hợp lệ nào. Nó được dùng để chỉ ra rằng một con trỏ không hợp lệ, hoặc một phép cấp phát bộ nhớ đã thất bại.
  * Trên Linux hiện đại, việc cố gắng **đọc hoặc ghi từ địa chỉ `NULL`** sẽ gây ra  **Segmentation Fault** . Mặc dù một số thư viện cũ (như GNU C library cũ) có thể trả về một chuỗi "magic" khi đọc từ `NULL`, nhưng việc ghi vào `NULL` luôn là lỗi nghiêm trọng.
  * **Luôn kiểm tra con trỏ `NULL`** sau khi gọi các hàm cấp phát bộ nhớ (`malloc`, `calloc`, `realloc`) để đảm bảo chúng thành công trước khi sử dụng con trỏ.
* **Liên hệ Embedded Linux:** Việc không kiểm tra con trỏ `NULL` là một trong những nguyên nhân phổ biến nhất gây ra lỗi nghiêm trọng và khó debug trong các ứng dụng nhúng.
* **Ví dụ (C++): `null_pointer.cpp` - Truy cập con trỏ NULL**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <cstdlib>  // For exit, EXIT_SUCCESS, EXIT_FAILURE
  #include <cstdio>   // For sprintf

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
      char *some_memory = nullptr; // Khởi tạo con trỏ NULL (char*)0

      AppLogger::log(AppLogger::INFO_L, "--- Demonstrating Null Pointer Access ---");

      // --- Ví dụ 1: Đọc từ con trỏ NULL ---
      // Hành vi có thể thay đổi tùy phiên bản thư viện C/Kernel.
      // Trên GNU libc hiện đại, printf("%s", NULL) thường in "(null)".
      AppLogger::log(AppLogger::WARNING_L, "Attempting to read from a NULL pointer using printf (may print '(null)').");
      printf("Content from null pointer: %s\n", some_memory);
      AppLogger::log(AppLogger::INFO_L, "Read attempt completed."); // Có thể không đạt được nếu segfault ngay đây

      // --- Ví dụ 2: Ghi vào con trỏ NULL (chắc chắn gây Segfault) ---
      AppLogger::log(AppLogger::CRITICAL_L, "Attempting to write to a NULL pointer. Expect Segfault!");
      sprintf(some_memory, "A write to null\n"); // Lỗi! Ghi vào NULL

      AppLogger::log(AppLogger::INFO_L, "Program reached end (unexpected if Segfault occurred).");

      return EXIT_SUCCESS; // Sẽ không đạt được
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ null_pointer.cpp -o null_pointer
    ./null_pointer
    ```
  * **Phân tích:** Chương trình sẽ in ra thông báo đầu tiên, sau đó sẽ gặp `Segmentation fault` khi cố gắng ghi vào con trỏ `NULL`.

#### **1.5. Giải phóng Bộ nhớ (Freeing Memory) 🗑️**

* **Lý thuyết:** Khi bạn không còn cần một khối bộ nhớ đã được cấp phát động (bằng `malloc()`, `calloc()`, `realloc()`), bạn phải trả nó về cho hệ thống bằng cách gọi hàm  **`free()`** .
  **C++**

  ```
  #include <cstdlib> // For free
  // void free(void *ptr_to_memory);
  ```

  * **`ptr_to_memory`** : Con trỏ tới đầu khối bộ nhớ cần giải phóng (phải là con trỏ được trả về bởi `malloc`/`calloc`/`realloc`).
  * **Quan trọng:**
    * Chỉ gọi `free()` trên bộ nhớ được cấp phát động.
    * Sau khi `free(ptr)`, `ptr` trở thành một **dangling pointer** (con trỏ lơ lửng). Vùng nhớ mà nó trỏ tới có thể đã được giải phóng và có thể được cấp phát lại cho một mục đích khác. **Tuyệt đối không được truy cập** vùng nhớ đó sau `free()` (lỗi use-after-free). Để tránh lỗi này, hãy đặt con trỏ về `nullptr` sau khi `free()`: `ptr = nullptr;`.
    * **Không giải phóng** cùng một khối bộ nhớ hai lần (lỗi double-free).
    * Nếu bạn không `free()` bộ nhớ đã cấp phát, sẽ xảy ra  **rò rỉ bộ nhớ (memory leak)** . Bộ nhớ này sẽ không được trả về cho hệ thống cho đến khi chương trình kết thúc. Trong các ứng dụng chạy liên tục (daemon, ứng dụng nhúng), rò rò rỉ bộ nhớ có thể làm cạn kiệt RAM và gây treo hệ thống.
* **Liên hệ Embedded Linux:** Việc quản lý bộ nhớ nghiêm ngặt là điều kiện tiên quyết cho các ứng dụng nhúng chạy trong thời gian dài. Rò rỉ bộ nhớ là một trong những nguyên nhân hàng đầu gây ra sự cố trong các thiết bị nhúng.
* **Ví dụ (C++): `memory_free.cpp` - Giải phóng Bộ nhớ Đúng cách**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <cstdlib> // For malloc, free, exit, EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring> // For memset, strlen

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

  #define ONE_K (1024)

  int main() {
      char *some_memory = nullptr;

      AppLogger::log(AppLogger::INFO_L, "--- Demonstrating Memory Freeing ---");

      // Cấp phát bộ nhớ
      AppLogger::log(AppLogger::INFO_L, "Attempting to allocate 1KB of memory.");
      some_memory = (char *)malloc(ONE_K);
      if (some_memory == NULL) {
          AppLogger::log(AppLogger::ERROR_L, "Malloc failed.");
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Memory allocated at " + std::to_string(reinterpret_cast<long long>(some_memory)));
      memset(some_memory, 'X', ONE_K); // Ghi dữ liệu vào để kiểm tra
      some_memory[0] = 'A'; // Đặt ký tự đầu tiên
      some_memory[ONE_K - 1] = 'Z'; // Đặt ký tự cuối cùng

      // Sử dụng bộ nhớ
      AppLogger::log(AppLogger::INFO_L, "Using allocated memory: First char = " + std::string(1, some_memory[0]) + ", Last char = " + std::string(1, some_memory[ONE_K - 1]));

      // Giải phóng bộ nhớ
      AppLogger::log(AppLogger::INFO_L, "Freeing allocated memory...");
      free(some_memory);
      some_memory = nullptr; // Rất quan trọng: đặt con trỏ về nullptr sau khi free
      AppLogger::log(AppLogger::SUCCESS_L, "Memory freed and pointer set to nullptr.");

      // Thử truy cập bộ nhớ sau khi giải phóng (sẽ gây lỗi nếu không đặt some_memory = nullptr)
      if (some_memory != nullptr) {
          AppLogger::log(AppLogger::CRITICAL_L, "Attempting to access freed memory (DANGEROUS!): " + std::string(1, some_memory[0]));
      } else {
          AppLogger::log(AppLogger::INFO_L, "Pointer is nullptr after free, preventing accidental access.");
      }

      // --- Ví dụ về lỗi double-free (KHÔNG NÊN LÀM) ---
      // char *double_free_ptr = (char *)malloc(10);
      // if (double_free_ptr) {
      //     AppLogger::log(AppLogger::WARNING_L, "Demonstrating double-free (DANGEROUS!)");
      //     free(double_free_ptr);
      //     free(double_free_ptr); // Lỗi double-free!
      // }

      return EXIT_SUCCESS;
  }
  ```

#### **1.6. Các hàm cấp phát Bộ nhớ khác 🧰**

* **Lý thuyết:**

  * **`calloc()`:**
    **C++**

    ```
    #include <cstdlib> // For calloc
    void *calloc(size_t num_elements, size_t element_size);
    ```

    * Cấp phát bộ nhớ cho một mảng gồm `num_elements`, mỗi phần tử có kích thước `element_size`.
    * **Điểm khác biệt quan trọng:** `calloc()`  **khởi tạo tất cả các byte của vùng bộ nhớ được cấp phát thành 0** . Rất hữu ích khi bạn cần một vùng bộ nhớ "sạch".
  * **`realloc()`:**
    **C++**

    ```
    #include <cstdlib> // For realloc
    void *realloc(void *existing_memory, size_t new_size);
    ```

    * Thay đổi kích thước của một khối bộ nhớ đã được cấp phát trước đó (bởi `malloc`, `calloc`, hoặc `realloc`).
    * **`existing_memory`** : Con trỏ tới khối bộ nhớ hiện có.
    * **`new_size`** : Kích thước mới mong muốn.
    * **Cách hoạt động:**

      * `realloc()` có thể trả về **cùng một con trỏ** nếu có đủ không gian để mở rộng tại chỗ.
      * Hoặc nó có thể trả về **một con trỏ mới** nếu phải di chuyển dữ liệu đến một vị trí khác có đủ không gian.
      * Dữ liệu từ khối cũ (tối đa bằng `new_size` hoặc `old_size`) sẽ được giữ lại. Các byte mới được thêm vào (nếu `new_size > old_size`) sẽ không được khởi tạo.
      * Nếu `existing_memory` là `NULL`, `realloc()` hoạt động như `malloc()`.
      * Nếu `new_size` là `0`, `realloc()` hoạt động như `free()`.
    * **Quan trọng:** **Luôn gán kết quả của `realloc()` vào một con trỏ tạm thời** và kiểm tra `NULL` trước khi gán lại vào con trỏ gốc. Nếu `realloc()` thất bại, nó trả về `NULL`, nhưng khối bộ nhớ gốc vẫn còn nguyên vẹn.
      **C++**

      ```
      char *temp_ptr = (char *)realloc(original_ptr, new_size);
      if (temp_ptr == NULL) {
          // Xử lý lỗi, original_ptr vẫn hợp lệ
      } else {
          original_ptr = temp_ptr; // Gán lại chỉ khi thành công
      }
      ```
* **Liên hệ Embedded Linux:**

  * `calloc()` hữu ích khi cần các buffer sạch hoặc mảng cấu trúc đã được khởi tạo về 0.
  * `realloc()` rất quan trọng để tối ưu hóa việc sử dụng bộ nhớ động khi kích thước dữ liệu cần lưu trữ thay đổi. Tránh cấp phát quá nhiều bộ nhớ ban đầu hoặc cấp phát/giải phóng liên tục.
* **Ví dụ (C++): `memory_other_allocs.cpp` - `calloc()` và `realloc()`**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <cstdlib>  // For malloc, free, calloc, realloc, exit, EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>  // For memset, strcpy

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
      int *int_array = nullptr;
      int initial_elements = 5;

      AppLogger::log(AppLogger::INFO_L, "--- Demonstrating calloc() ---");
      // Cấp phát 5 số nguyên, tất cả byte được khởi tạo thành 0
      int_array = (int *)calloc(initial_elements, sizeof(int));
      if (int_array == NULL) {
          AppLogger::log(AppLogger::ERROR_L, "calloc failed.");
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Allocated " + std::to_string(initial_elements) + " ints with calloc at " + std::to_string(reinterpret_cast<long long>(int_array)));
      AppLogger::log(AppLogger::INFO_L, "Initial values (should be 0): ");
      for (int i = 0; i < initial_elements; ++i) {
          std::cout << int_array[i] << " ";
      }
      std::cout << std::endl;

      // Gán giá trị cho các phần tử ban đầu
      for (int i = 0; i < initial_elements; ++i) {
          int_array[i] = i + 1;
      }
      AppLogger::log(AppLogger::INFO_L, "Values after assignment: ");
      for (int i = 0; i < initial_elements; ++i) {
          std::cout << int_array[i] << " ";
      }
      std::cout << std::endl;

      // --- Demonstrating realloc() ---
      AppLogger::log(AppLogger::INFO_L, "\n--- Demonstrating realloc() ---");
      int new_elements = 10;
      int *temp_ptr = nullptr;

      AppLogger::log(AppLogger::INFO_L, "Attempting to reallocate to " + std::to_string(new_elements) + " integers.");
      temp_ptr = (int *)realloc(int_array, new_elements * sizeof(int));

      if (temp_ptr == NULL) {
          AppLogger::log(AppLogger::ERROR_L, "realloc failed: " + std::string(strerror(errno)));
          free(int_array); // Giải phóng khối gốc nếu realloc thất bại
          return EXIT_FAILURE;
      }
      int_array = temp_ptr; // Gán lại con trỏ gốc chỉ khi realloc thành công
      AppLogger::log(AppLogger::SUCCESS_L, "Successfully reallocated to " + std::to_string(new_elements) + " ints at " + std::to_string(reinterpret_cast<long long>(int_array)));

      // Các phần tử mới không được khởi tạo
      AppLogger::log(AppLogger::INFO_L, "Values after realloc (new elements are uninitialized): ");
      for (int i = 0; i < new_elements; ++i) {
          std::cout << int_array[i] << " ";
      }
      std::cout << std::endl;

      // Sử dụng các phần tử mới
      for (int i = initial_elements; i < new_elements; ++i) {
          int_array[i] = (i + 1) * 10;
      }
      AppLogger::log(AppLogger::INFO_L, "Values after filling new elements: ");
      for (int i = 0; i < new_elements; ++i) {
          std::cout << int_array[i] << " ";
      }
      std::cout << std::endl;

      free(int_array);
      AppLogger::log(AppLogger::SUCCESS_L, "Memory freed.");

      return EXIT_SUCCESS;
  }
  ```

---

### **Câu hỏi Tự đánh giá Module 1 🤔**

1. Giải thích khái niệm "bộ nhớ ảo theo yêu cầu" (demand-paged virtual memory) trong Linux. Tại sao `malloc()` có thể thành công nhưng việc ghi vào bộ nhớ đó lại dẫn đến lỗi OOM?
2. "OOM Killer" là gì và vai trò của nó trong quản lý bộ nhớ của Linux?
3. Phân biệt các lỗi lạm dụng bộ nhớ sau: "out-of-bounds access", "dereferencing NULL pointer", "use-after-free", "double-free". Nêu hậu quả chung của những lỗi này trong môi trường Linux.
4. Tại sao việc đặt con trỏ về `nullptr` sau khi gọi `free()` lại là một thực hành tốt?
5. Sự khác biệt chính giữa `malloc()` và `calloc()` là gì? Khi nào bạn sẽ chọn `calloc()`?
6. Giải thích cách `realloc()` hoạt động khi thay đổi kích thước một khối bộ nhớ. Tại sao nên gán kết quả của `realloc()` vào một con trỏ tạm thời trước khi gán lại vào con trỏ gốc?

---

### **Bài tập Thực hành Module 1 ✍️**

1. **Chương trình Kiểm tra Rò rỉ Bộ nhớ (Memory Leak Detector):**
   * Viết một chương trình C++ (`memory_leak_test.cpp`) mà:
     * Trong một vòng lặp vô hạn, cấp phát một khối bộ nhớ nhỏ (ví dụ: 1MB) bằng `malloc()` hoặc `new char[... ]`.
     * **Không giải phóng** bộ nhớ đó.
     * Trong mỗi vòng lặp, in ra thông báo "Allocated [X] MB" và PID của chương trình.
     * **Thử thách:** Chạy chương trình này trong nền (`./memory_leak_test &`) và sử dụng lệnh `top` hoặc `htop` (hoặc `ps -aux --sort=-rss`) để quan sát mức sử dụng RAM của chương trình tăng dần. Sau một thời gian, nó có thể bị OOM Killer kết thúc.
2. **Chương trình Mô phỏng Truy cập Ngẫu nhiên Dữ liệu lớn:**
   * Viết một chương trình C++ (`large_data_access.cpp`) mà:
     * Cấp phát một khối bộ nhớ rất lớn (ví dụ: 512MB hoặc 1GB) bằng `malloc()`.
     * Nếu cấp phát thành công, ghi dữ liệu ngẫu nhiên (hoặc một chuỗi đơn giản lặp lại) vào **toàn bộ** khối bộ nhớ đó (để buộc Kernel cấp phát các trang vật lý).
     * Sau đó, trong một vòng lặp, truy cập ngẫu nhiên các vị trí khác nhau trong khối bộ nhớ đó (ví dụ: đọc/ghi từng byte tại 1000 vị trí ngẫu nhiên).
     * Đo thời gian thực hiện thao tác ghi ban đầu và thời gian thực hiện vòng lặp truy cập ngẫu nhiên.
     * **Thử thách:** Chạy chương trình này trên máy có RAM vật lý thấp hơn kích thước cấp phát và quan sát ảnh hưởng của swap space (ví dụ: tiếng ổ đĩa hoạt động, thời gian thực thi tăng lên).
3. **Chương trình Quản lý Mảng Động:**
   * Viết một chương trình C++ (`dynamic_array_manager.cpp`) mà:
     * Cấp phát một mảng số nguyên ban đầu với 5 phần tử bằng `calloc()`.
     * Yêu cầu người dùng nhập các số nguyên cho 5 phần tử đó.
     * Sau đó, sử dụng `realloc()` để tăng kích thước mảng lên 10 phần tử.
     * Yêu cầu người dùng nhập các số nguyên cho 5 phần tử mới.
     * In ra toàn bộ nội dung của mảng mới.
     * Cuối cùng, giải phóng bộ nhớ.
   * **Thử thách:** Thêm xử lý lỗi cho `calloc()` và `realloc()`. Đảm bảo rằng nếu `realloc()` thất bại, dữ liệu gốc vẫn được giữ nguyên và chương trình thoát một cách duyên dáng.
