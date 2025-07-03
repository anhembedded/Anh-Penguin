# **Module 6: Làm việc với nhiều Luồng (Threads in Abundance) 🤯**

#### **6.1. Thách thức khi có nhiều Luồng (Challenges with Many Threads) 💥**

* **Lý thuyết:** Mặc dù luồng nhẹ hơn tiến trình, nhưng việc tạo ra một số lượng quá lớn luồng (hàng trăm, hàng ngàn) có thể gây ra các vấn đề hiệu suất và ổn định:
  1. **Overhead Quản lý Luồng (Thread Management Overhead):**
     * Mỗi luồng cần Kernel quản lý một cấu trúc dữ liệu nhỏ (thread control block).
     * Việc tạo và hủy luồng liên tục (nếu không được tái sử dụng) sẽ tốn tài nguyên CPU và bộ nhớ cho Kernel.
  2. **Chuyển đổi Ngữ cảnh (Context Switching Overhead):**
     * Khi có nhiều luồng sẵn sàng chạy hơn số lõi CPU, Kernel phải thường xuyên chuyển đổi giữa các luồng.
     * Mỗi lần chuyển đổi ngữ cảnh đều có chi phí (CPU phải lưu trạng thái của luồng hiện tại và tải trạng thái của luồng mới).
     * Nếu có quá nhiều luồng, thời gian dành cho việc chuyển đổi ngữ cảnh có thể trở nên đáng kể, làm giảm hiệu suất tổng thể của ứng dụng và hệ thống.
  3. **Tiêu thụ Bộ nhớ Stack (Stack Memory Consumption):**
     * Mỗi luồng có một ngăn xếp (stack) riêng. Kích thước stack mặc định trên Linux thường khá lớn (ví dụ: 8MB cho mỗi luồng).
     * Việc tạo hàng ngàn luồng có thể dẫn đến việc tiêu thụ một lượng lớn bộ nhớ ảo (virtual memory), và có thể gây ra lỗi Out-of-Memory (OOM) hoặc làm cạn kiệt không gian swap.
  4. **Giới hạn Hệ thống (System Limits):**
     * Hệ điều hành có giới hạn về tổng số luồng mà một tiến trình có thể tạo hoặc tổng số luồng trên toàn hệ thống. Vượt quá giới hạn này sẽ khiến `pthread_create()` thất bại.
  5. **Tăng độ phức tạp Đồng bộ hóa (Increased Synchronization Complexity):**
     * Càng nhiều luồng, càng có nhiều khả năng xảy ra điều kiện tranh chấp và deadlock, khiến việc thiết kế và gỡ lỗi trở nên khó khăn hơn nhiều.
* **Liên hệ Embedded Linux:** Các vấn đề này càng trở nên trầm trọng hơn trên các thiết bị nhúng với tài nguyên (CPU, RAM) giới hạn. Việc quản lý số lượng luồng và tài nguyên liên quan là tối quan trọng để đảm bảo thiết bị hoạt động ổn định và hiệu quả.

#### **6.2. Vấn đề truyền đối số cho nhiều luồng (Argument Passing Pitfalls) ⚠️**

* **Lý thuyết:** Một lỗi tinh vi nhưng phổ biến khi tạo nhiều luồng trong một vòng lặp là truyền địa chỉ của một biến cục bộ trong vòng lặp làm đối số cho hàm luồng.
  * **Vấn đề:** Biến cục bộ trong vòng lặp (ví dụ: biến lặp `i` hoặc `lots_of_threads`) được tái sử dụng trong mỗi lần lặp. Khi bạn truyền địa chỉ của nó (`&i`), tất cả các luồng con được tạo ra có thể sẽ nhận được  **cùng một địa chỉ** , và khi luồng con cố gắng đọc giá trị tại địa chỉ đó, nó sẽ đọc **giá trị hiện tại** của biến trong luồng chính, chứ không phải giá trị tại thời điểm luồng đó được tạo. Nếu luồng chính chạy nhanh hơn, giá trị này có thể đã thay đổi trước khi luồng con kịp đọc.
  * Điều này dẫn đến các luồng nhận được cùng một đối số hoặc đối số không mong muốn, gây ra hành vi sai lệch và rất khó debug.
* **Giải pháp:** Để đảm bảo mỗi luồng nhận được đối số riêng của nó:
  1. **Truyền đối số theo giá trị (Pass by Value):** Nếu đối số là một kiểu dữ liệu nhỏ (ví dụ: `int`, `long`), bạn có thể ép kiểu trực tiếp giá trị đó sang `void*` và truyền đi. Hàm luồng sẽ ép kiểu ngược lại.
     **C++**

     ```
     // Khi tạo luồng
     pthread_create(&thread_id, NULL, thread_function, (void*)(long)my_number);
     // Trong hàm luồng
     void *thread_function(void *arg) {
         int my_number = (int)(long)arg; // Ép kiểu ngược lại
     }
     ```
  2. **Cấp phát động và sao chép (Dynamic Allocation and Copy):** Nếu đối số là một cấu trúc lớn hoặc một chuỗi, cấp phát động một bản sao của dữ liệu đó cho mỗi luồng, truyền con trỏ tới bản sao đó. Luồng con có trách nhiệm giải phóng bộ nhớ này.
     **C++**

     ```
     // Khi tạo luồng
     MyArgs* args = new MyArgs; // Cấp phát động
     *args = original_args;     // Sao chép dữ liệu
     pthread_create(&thread_id, NULL, thread_function, (void*)args);
     // Trong hàm luồng
     void *thread_function(void *arg) {
         MyArgs* my_args = static_cast<MyArgs*>(arg);
         // ... sử dụng my_args ...
         delete my_args; // Giải phóng bộ nhớ
     }
     ```
* **Liên hệ Embedded Linux:** Lỗi này rất phổ biến và nguy hiểm trong nhúng, nơi các luồng thường xử lý dữ liệu cảm biến hoặc điều khiển phần cứng dựa trên các tham số. Việc truyền sai đối số có thể dẫn đến hoạt động sai lệch của thiết bị.

#### **6.3. Giải pháp: Thread Pool (Nhóm Luồng) 🏊**

* **Lý thuyết:** Một **Thread Pool (Nhóm Luồng)** là một tập hợp các luồng "làm việc" (worker threads) được tạo ra sẵn và duy trì. Thay vì tạo một luồng mới cho mỗi tác vụ, các tác vụ được đưa vào một  **hàng đợi (task queue)** . Các luồng làm việc trong pool sẽ lấy tác vụ từ hàng đợi để xử lý.
* **Ưu điểm:**
  * **Giảm Overhead:** Tránh chi phí tạo/hủy luồng liên tục.
  * **Quản lý Tài nguyên:** Kiểm soát số lượng luồng tối đa, ngăn chặn việc tạo quá nhiều luồng gây quá tải hệ thống.
  * **Tái sử dụng:** Các luồng được tái sử dụng cho nhiều tác vụ.
* **Liên hệ Embedded Linux:** Rất quan trọng cho các server, daemon hoặc ứng dụng xử lý nhiều yêu cầu đồng thời trên thiết bị nhúng (ví dụ: server web nhúng, xử lý nhiều kết nối cảm biến).

#### **6.4. Ví dụ (C++): `many_threads_bug.cpp` - Minh họa lỗi truyền đối số**

Đây là phiên bản có lỗi truyền đối số, tương tự như `thread8.c` trong bài học.

**C++**

```
#include <iostream>
#include <string>
#include <pthread.h> // For pthreads API
#include <unistd.h>  // For sleep
#include <cstdlib>   // For EXIT_SUCCESS, EXIT_FAILURE, rand, srand
#include <ctime>     // For time (for srand)

// Logger namespace
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

#define NUM_THREADS 6

// Hàm luồng (có lỗi truyền đối số)
void *thread_function_buggy(void *arg) {
    // Ép kiểu con trỏ tới int cục bộ của vòng lặp chính
    int my_number = *(static_cast<int*>(arg)); 
  
    AppLogger::log(AppLogger::INFO_L, "Thread (ID: " + std::to_string(pthread_self()) + "): Running. Argument was " + std::to_string(my_number));
  
    // Ngủ một khoảng thời gian ngẫu nhiên
    int rand_num = 1 + (int)(9.0 * rand() / (RAND_MAX + 1.0));
    sleep(rand_num);
  
    AppLogger::log(AppLogger::INFO_L, "Thread (ID: " + std::to_string(pthread_self()) + "): Bye from " + std::to_string(my_number));
    pthread_exit(nullptr);
}

int main() {
    pthread_t a_thread[NUM_THREADS];
    int res;
  
    srand(time(nullptr)); // Khởi tạo seed cho rand()

    AppLogger::log(AppLogger::INFO_L, "Main Thread: Starting Many Threads (BUGGY ARG PASSING) Demonstration.");

    // Tạo nhiều luồng trong vòng lặp
    for (int i = 0; i < NUM_THREADS; ++i) {
        // Lỗi ở đây: truyền địa chỉ của biến vòng lặp 'i'
        // 'i' là biến cục bộ của main, nó sẽ thay đổi trong vòng lặp
        // Các luồng con có thể đọc cùng một giá trị 'i' hoặc giá trị không mong muốn
        res = pthread_create(&(a_thread[i]), nullptr, thread_function_buggy, static_cast<void*>(&i));
        if (res != 0) {
            AppLogger::log(AppLogger::ERROR_L, "Main Thread: Thread creation failed for i=" + std::to_string(i) + ": " + strerror(res));
            return EXIT_FAILURE;
        }
        // sleep(1); // Nếu bỏ sleep này, lỗi sẽ dễ tái hiện hơn
    }

    AppLogger::log(AppLogger::INFO_L, "Main Thread: Waiting for threads to finish...");
    // Chờ các luồng hoàn thành (theo thứ tự ngược lại để minh họa)
    for (int i = NUM_THREADS - 1; i >= 0; --i) {
        res = pthread_join(a_thread[i], nullptr);
        if (res == 0) {
            AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Picked up a thread (ID: " + std::to_string(a_thread[i]) + ").");
        } else {
            AppLogger::log(AppLogger::ERROR_L, "Main Thread: pthread_join failed for thread " + std::to_string(a_thread[i]) + ": " + strerror(res));
        }
    }

    AppLogger::log(AppLogger::INFO_L, "Main Thread: All done.");
    return EXIT_SUCCESS;
}
```

* **Cách Biên dịch:**
  **Bash**

  ```
  g++ many_threads_bug.cpp -o many_threads_bug -pthread
  ```
* **Cách Chạy và Phân tích:**
  **Bash**

  ```
  ./many_threads_bug
  # Chạy nhiều lần. Bạn có thể thấy các số được in ra bởi "Thread: Running. Argument was X" có thể bị trùng lặp hoặc không theo thứ tự 0,1,2,3,4,5.
  # Thử bỏ dòng "sleep(1);" trong vòng lặp tạo luồng của main để lỗi dễ tái hiện hơn.
  ```

  * **Phân tích:** Lỗi xảy ra vì `&i` truyền địa chỉ của biến `i` trong vòng lặp. Khi luồng con bắt đầu chạy và đọc `*arg`, giá trị của `i` trong luồng chính có thể đã thay đổi (do vòng lặp tiếp tục).

#### **6.5. Ví dụ (C++): `many_threads_fixed.cpp` - Sửa lỗi truyền đối số**

Đây là phiên bản đã sửa lỗi truyền đối số.

**C++**

```
#include <iostream>
#include <string>
#include <pthread.h> // For pthreads API
#include <unistd.h>  // For sleep
#include <cstdlib>   // For EXIT_SUCCESS, EXIT_FAILURE, rand, srand
#include <ctime>     // For time (for srand)

// Logger namespace
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

#define NUM_THREADS 6

// Hàm luồng (đã sửa lỗi truyền đối số)
void *thread_function_fixed(void *arg) {
    // Sửa lỗi: Ép kiểu trực tiếp giá trị (long) từ void*
    int my_number = static_cast<int>(reinterpret_cast<long>(arg)); 
  
    AppLogger::log(AppLogger::INFO_L, "Thread (ID: " + std::to_string(pthread_self()) + "): Running. Argument was " + std::to_string(my_number));
  
    // Ngủ một khoảng thời gian ngẫu nhiên
    int rand_num = 1 + (int)(9.0 * rand() / (RAND_MAX + 1.0));
    sleep(rand_num);
  
    AppLogger::log(AppLogger::INFO_L, "Thread (ID: " + std::to_string(pthread_self()) + "): Bye from " + std::to_string(my_number));
    pthread_exit(nullptr);
}

int main() {
    pthread_t a_thread[NUM_THREADS];
    int res;
  
    srand(time(nullptr)); // Khởi tạo seed cho rand()

    AppLogger::log(AppLogger::INFO_L, "Main Thread: Starting Many Threads (FIXED ARG PASSING) Demonstration.");

    // Tạo nhiều luồng trong vòng lặp
    for (long i = 0; i < NUM_THREADS; ++i) { // Dùng long cho i để ép kiểu an toàn hơn
        // Sửa lỗi: truyền giá trị 'i' trực tiếp, không phải địa chỉ của nó
        // Giá trị 'i' được ép kiểu thành void*
        res = pthread_create(&(a_thread[i]), nullptr, thread_function_fixed, reinterpret_cast<void*>(i));
        if (res != 0) {
            AppLogger::log(AppLogger::ERROR_L, "Main Thread: Thread creation failed for i=" + std::to_string(i) + ": " + strerror(res));
            return EXIT_FAILURE;
        }
        // sleep(1); // Có thể bỏ sleep này, lỗi sẽ không tái hiện
    }

    AppLogger::log(AppLogger::INFO_L, "Main Thread: Waiting for threads to finish...");
    // Chờ các luồng hoàn thành (theo thứ tự ngược lại để minh họa)
    for (int i = NUM_THREADS - 1; i >= 0; --i) {
        res = pthread_join(a_thread[i], nullptr);
        if (res == 0) {
            AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Picked up a thread (ID: " + std::to_string(a_thread[i]) + ").");
        } else {
            AppLogger::log(AppLogger::ERROR_L, "Main Thread: pthread_join failed for thread " + std::to_string(a_thread[i]) + ": " + strerror(res));
        }
    }

    AppLogger::log(AppLogger::INFO_L, "Main Thread: All done.");
    return EXIT_SUCCESS;
}
```

* **Cách Biên dịch:**
  **Bash**

  ```
  g++ many_threads_fixed.cpp -o many_threads_fixed -pthread
  ```
* **Cách Chạy và Phân tích:**
  **Bash**

  ```
  ./many_threads_fixed
  # Chạy nhiều lần. Bạn sẽ thấy các số được in ra bởi "Thread: Running. Argument was X" luôn là 0,1,2,3,4,5 (hoặc thứ tự đó, nhưng không bị trùng lặp giá trị).
  ```

  * **Phân tích:** Lỗi đã được khắc phục vì mỗi luồng nhận được một bản sao giá trị của `i` tại thời điểm nó được tạo, chứ không phải địa chỉ của biến `i` đang thay đổi.

---

### **Câu hỏi Tự đánh giá Module 6 🤔**

1. Nêu ba thách thức chính khi làm việc với một số lượng lớn luồng trong một chương trình.
2. Giải thích vấn đề "overhead chuyển đổi ngữ cảnh" (context switching overhead) khi có quá nhiều luồng.
3. Mô tả lỗi phổ biến khi truyền đối số cho nhiều luồng trong một vòng lặp bằng cách truyền địa chỉ của biến vòng lặp. Tại sao lỗi này lại xảy ra?
4. Bạn sẽ sửa lỗi truyền đối số đó như thế nào nếu đối số là một số nguyên nhỏ?
5. Bạn sẽ sửa lỗi truyền đối số đó như thế nào nếu đối số là một cấu trúc dữ liệu lớn hoặc một chuỗi?
6. "Thread Pool" (Nhóm Luồng) là gì? Nêu hai ưu điểm chính của việc sử dụng Thread Pool trong các ứng dụng đa luồng.

---

### **Bài tập Thực hành Module 6 ✍️**

1. **Bài tập "Tìm số Nguyên tố Đa Luồng":**
   * Viết một chương trình C++ (`prime_finder.cpp`) để tìm tất cả các số nguyên tố trong một phạm vi lớn (ví dụ: từ 2 đến 1,000,000).
   * Chương trình sẽ tạo `N` luồng (ví dụ: `N=4`).
   * Chia phạm vi tìm kiếm thành `N` phần bằng nhau cho mỗi luồng.
   * Mỗi luồng sẽ tìm số nguyên tố trong phần của nó và in ra các số nguyên tố tìm được.
   * **Quan trọng:** Đảm bảo mỗi luồng nhận được phạm vi tìm kiếm của riêng nó một cách chính xác (tránh lỗi truyền đối số).
   * **Thử thách:**
     * Đo thời gian chạy của chương trình với 1 luồng và với `N` luồng. So sánh hiệu suất.
     * Sử dụng một Mutex để bảo vệ `std::cout` khi các luồng in ra số nguyên tố, để output không bị lẫn lộn.
2. **Bài tập "Xử lý File Song song":**
   * Tạo một thư mục chứa nhiều file văn bản nhỏ (ví dụ: 100 file, mỗi file 1KB).
   * Viết một chương trình C++ (`parallel_file_processor.cpp`) mà:
     * Quét thư mục để lấy danh sách tất cả các file văn bản.
     * Tạo `N` luồng (ví dụ: `N=4`).
     * Mỗi luồng sẽ lấy một file từ danh sách, đọc nội dung của nó, đếm số từ, và sau đó ghi kết quả vào một file tổng hợp (`summary.log`).
     * **Quan trọng:** Đảm bảo việc truy cập `summary.log` được đồng bộ hóa bằng một Mutex.
     * **Thử thách:**
       * Sử dụng một Thread Pool đơn giản (không cần triển khai đầy đủ, chỉ cần một hàng đợi tác vụ và một số luồng làm việc cố định) để quản lý việc xử lý file.
       * So sánh thời gian chạy với phiên bản đơn luồng.
3. **Bài tập "Phát hiện Race Condition (Nâng cao)":**
   * Viết một chương trình C++ (`race_detector.cpp`) có một biến toàn cục `int counter = 0;`.
   * Tạo 5 luồng. Mỗi luồng sẽ tăng `counter` 100,000 lần  **mà không sử dụng bất kỳ cơ chế đồng bộ hóa nào** .
   * Luồng chính sẽ chờ tất cả các luồng hoàn thành và in ra giá trị cuối cùng của `counter`.
   * **Mục tiêu:**
     * Chạy chương trình nhiều lần. Quan sát rằng giá trị cuối cùng của `counter` **thường không phải là 500,000** do race condition.
     * Biên dịch chương trình với cờ `-g`.
     * **Thử thách:** Sử dụng `Valgrind` với công cụ `Helgrind` hoặc `DRD` để phát hiện race condition:
       **Bash**

       ```
       valgrind --tool=helgrind ./race_detector
       # Hoặc: valgrind --tool=drd ./race_detector
       ```

       * `Valgrind` sẽ báo cáo các lỗi race condition mà nó phát hiện được.
