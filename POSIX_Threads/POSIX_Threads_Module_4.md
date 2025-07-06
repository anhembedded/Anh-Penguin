# **Module 4: Thuộc tính Luồng (Thread Attributes) 🛠️**

#### **4.1. `pthread_attr_t`: Đối tượng Thuộc tính Luồng ⚙️**

* **Lý thuyết:** Khi bạn gọi `pthread_create()`, tham số thứ hai (`const pthread_attr_t *attr`) cho phép bạn chỉ định các thuộc tính tùy chỉnh cho luồng mới. Nếu bạn truyền `NULL`, luồng sẽ được tạo với các thuộc tính mặc định của hệ thống.
  * **`pthread_attr_t`** là một kiểu dữ liệu mờ (opaque type) đại diện cho một đối tượng thuộc tính luồng. Bạn không truy cập trực tiếp các thành viên của nó mà sử dụng các hàm API để thao tác.
* **Các hàm cơ bản để quản lý đối tượng thuộc tính:**
  * **`pthread_attr_init()`:** Khởi tạo một đối tượng thuộc tính luồng với các giá trị mặc định của hệ thống.
    **C++**

    ```
    #include <pthread.h>
    int pthread_attr_init(pthread_attr_t *attr);
    ```
  * **`pthread_attr_destroy()`:** Hủy một đối tượng thuộc tính luồng và giải phóng mọi tài nguyên mà nó có thể đang nắm giữ.
    **C++**

    ```
    #include <pthread.h>
    int pthread_attr_destroy(pthread_attr_t *attr);
    ```

    * **Lưu ý:** Bạn nên hủy đối tượng thuộc tính sau khi đã tạo luồng bằng nó, hoặc khi không còn cần đến nó nữa.
* **Giá trị trả về:** Cả hai hàm đều trả về `0` nếu thành công, khác `0` (mã lỗi) nếu thất bại.
* **Liên hệ Embedded Linux:** Rất quan trọng khi bạn cần tinh chỉnh hành vi của luồng để phù hợp với yêu cầu của phần cứng hoặc ứng dụng nhúng (ví dụ: điều chỉnh kích thước stack, đặt chính sách lập lịch thời gian thực).

#### **4.2. Trạng thái Tách rời (Detached State) 🔗**

* **Lý thuyết:** Một luồng có thể ở một trong hai trạng thái tách rời:

  * **`PTHREAD_CREATE_JOINABLE` (Mặc định):**
    * Luồng này có thể được `pthread_join()` bởi một luồng khác.
    * Khi luồng này kết thúc, tài nguyên của nó (ví dụ: mục nhập trong bảng luồng) **không được giải phóng hoàn toàn** cho đến khi có luồng khác `pthread_join()` nó. Nếu không `join()`, nó sẽ trở thành một dạng "zombie" của luồng, chiếm dụng tài nguyên.
    * Bạn có thể lấy giá trị trả về của luồng thông qua `pthread_join()`.
  * **`PTHREAD_CREATE_DETACHED`:**
    * Luồng này **không thể** được `pthread_join()` bởi luồng khác.
    * Khi luồng này kết thúc, tài nguyên của nó sẽ được **tự động giải phóng** bởi Kernel.
    * Bạn **không thể** thu thập giá trị trả về của luồng.
* **Cách đặt trạng thái tách rời:**

  * **Khi tạo luồng (sử dụng thuộc tính):**
    **C++**

    ```
    #include <pthread.h>
    int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
    // detachstate: PTHREAD_CREATE_JOINABLE hoặc PTHREAD_CREATE_DETACHED
    ```
  * **Sau khi tạo luồng (sử dụng ID luồng):**
    **C++**

    ```
    #include <pthread.h>
    int pthread_detach(pthread_t thread);
    ```

    * Hàm này thay đổi trạng thái của một luồng `joinable` thành `detached`.
* **Liên hệ Embedded Linux:**

  * Sử dụng `PTHREAD_CREATE_DETACHED` cho các luồng chạy nền (background tasks) không cần trả về kết quả và không muốn luồng chính phải `join()` chúng, giúp tránh rò rỉ tài nguyên luồng.
  * Nếu bạn cần thu thập kết quả hoặc đảm bảo luồng đã hoàn thành, hãy giữ trạng thái `PTHREAD_CREATE_JOINABLE` và `pthread_join()` nó.
* **Ví dụ (C++): `detached_thread.cpp` - Đặt trạng thái Tách rời**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <pthread.h> // For pthread_create, pthread_attr_t, pthread_attr_init, pthread_attr_setdetachstate, pthread_attr_destroy, pthread_exit, pthread_self
  #include <unistd.h>  // For sleep
  #include <cstdlib>   // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>   // For strerror (for non-pthread errors)

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

  // Biến cờ để luồng chính biết khi nào luồng con đã hoàn thành (chỉ để minh họa)
  volatile bool thread_finished_flag = false;

  // Hàm luồng cho luồng tách rời
  void *detached_thread_function(void *arg) {
      AppLogger::log(AppLogger::INFO_L, "Detached Thread (ID: " + std::to_string(pthread_self()) + "): Started.");
      AppLogger::log(AppLogger::INFO_L, "Detached Thread: Working for 4 seconds...");
      sleep(4);
      AppLogger::log(AppLogger::INFO_L, "Detached Thread: Setting finished flag and exiting.");
      thread_finished_flag = true; // Báo hiệu cho luồng chính
      pthread_exit(nullptr); // Luồng tự kết thúc
  }

  int main() {
      pthread_t detached_thread_id;
      pthread_attr_t thread_attributes; // Đối tượng thuộc tính luồng
      int res;

      AppLogger::log(AppLogger::INFO_L, "Main Thread: Starting Detached Thread Demonstration.");

      // 1. Khởi tạo đối tượng thuộc tính luồng
      res = pthread_attr_init(&thread_attributes);
      if (res != 0) {
          AppLogger::log(AppLogger::CRITICAL_L, "Main Thread: Failed to initialize thread attributes: " + strerror(res));
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Thread attributes initialized.");

      // 2. Đặt thuộc tính trạng thái tách rời thành PTHREAD_CREATE_DETACHED
      res = pthread_attr_setdetachstate(&thread_attributes, PTHREAD_CREATE_DETACHED);
      if (res != 0) {
          AppLogger::log(AppLogger::CRITICAL_L, "Main Thread: Failed to set detached state attribute: " + strerror(res));
          pthread_attr_destroy(&thread_attributes); // Hủy thuộc tính
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Detached state attribute set.");

      // 3. Tạo luồng với các thuộc tính đã đặt
      res = pthread_create(&detached_thread_id, &thread_attributes, detached_thread_function, nullptr);
      if (res != 0) {
          AppLogger::log(AppLogger::CRITICAL_L, "Main Thread: Failed to create detached thread: " + strerror(res));
          pthread_attr_destroy(&thread_attributes);
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Detached thread created with ID: " + std::to_string(detached_thread_id) + ".");

      // 4. Hủy đối tượng thuộc tính (không còn cần thiết sau khi tạo luồng)
      res = pthread_attr_destroy(&thread_attributes);
      if (res != 0) {
          AppLogger::log(AppLogger::ERROR_L, "Main Thread: Failed to destroy thread attributes: " + strerror(res));
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Thread attributes destroyed.");
      }

      // 5. Luồng chính chờ cờ hoàn thành (không thể pthread_join luồng detached)
      AppLogger::log(AppLogger::INFO_L, "Main Thread: Waiting for detached thread to finish (via flag)...");
      while (!thread_finished_flag) {
          AppLogger::log(AppLogger::INFO_L, "Main Thread: Still waiting...");
          sleep(1);
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Detached thread finished. Exiting.");

      return EXIT_SUCCESS;
  }
  ```

  * **Cách Biên dịch:**
    **Bash**

    ```
    g++ detached_thread.cpp -o detached_thread -pthread
    ```
  * **Cách Chạy:**
    **Bash**

    ```
    ./detached_thread
    ```
  * **Phân tích Output:** Bạn sẽ thấy luồng chính không bị chặn bởi `pthread_join()`. Luồng con chạy độc lập và tự dọn dẹp tài nguyên khi kết thúc.

#### **4.3. Thuộc tính Lập lịch (Scheduling Attributes) ⏱️**

* **Lý thuyết:** Bạn có thể kiểm soát cách Kernel lập lịch luồng của mình, đặc biệt quan trọng cho các ứng dụng thời gian thực (Real-Time - RT).

  * **Các chính sách lập lịch (Scheduling Policies):**

    * **`SCHED_OTHER` (Mặc định):** Chính sách lập lịch chia sẻ thời gian (timesharing). Đây là chính sách mặc định, công bằng cho mọi luồng, dựa trên giá trị nice.
    * **`SCHED_FIFO` (First-In, First-Out):** Chính sách thời gian thực. Luồng có ưu tiên cao hơn sẽ chạy cho đến khi nó tự nguyện nhường CPU, bị chặn (ví dụ: chờ I/O), hoặc bị ngắt bởi một luồng ưu tiên cao hơn. Luồng có cùng ưu tiên sẽ chạy theo thứ tự FIFO.
    * **`SCHED_RR` (Round-Robin):** Chính sách thời gian thực. Tương tự `SCHED_FIFO`, nhưng luồng sẽ được cấp một "lát cắt thời gian" (time slice). Khi hết lát cắt, nó sẽ bị tạm dừng và nhường CPU cho luồng khác có cùng ưu tiên.
  * **Độ ưu tiên (Priority):** Đối với các chính sách thời gian thực (`SCHED_FIFO`, `SCHED_RR`), bạn có thể đặt độ ưu tiên số học (numeric priority) cho luồng. Giá trị ưu tiên càng cao, luồng càng được ưu tiên.

    * Phạm vi ưu tiên được xác định bởi `sched_get_priority_min(policy)` và `sched_get_priority_max(policy)`.
  * **Các hàm đặt thuộc tính lập lịch:**
    **C++**

    ```
    #include <pthread.h>
    #include <sched.h> // For SCHED_*, struct sched_param

    int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy); // Đặt chính sách
    int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param); // Đặt tham số ưu tiên
    ```

    * `struct sched_param`: Chứa thành viên `int sched_priority;`.
  * **Lưu ý quan trọng:** Để đặt các chính sách lập lịch thời gian thực (`SCHED_FIFO`, `SCHED_RR`) và độ ưu tiên cao, chương trình của bạn thường  **cần quyền superuser (root)** . Nếu không có quyền, các cuộc gọi này có thể thất bại với lỗi `EPERM` (Operation not permitted).
* **Liên hệ Embedded Linux:** Cực kỳ quan trọng cho các ứng dụng thời gian thực cứng (hard real-time) hoặc mềm (soft real-time) trên thiết bị nhúng, nơi việc đảm bảo một tác vụ quan trọng được thực thi đúng thời hạn là thiết yếu (ví dụ: điều khiển động cơ, thu thập dữ liệu cảm biến tốc độ cao).
* **Ví dụ (C++): `realtime_thread.cpp` - Đặt thuộc tính Lập lịch**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <pthread.h> // For pthread_create, pthread_attr_t, pthread_attr_init, pthread_attr_setschedpolicy, pthread_attr_setschedparam, pthread_attr_destroy, pthread_exit, pthread_self
  #include <sched.h>   // For SCHED_*, sched_param, sched_get_priority_max, sched_get_priority_min
  #include <unistd.h>  // For sleep
  #include <cstdlib>   // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>   // For strerror (for non-pthread errors)
  #include <errno.h>   // For errno

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

  // Hàm luồng thời gian thực
  void *rt_thread_function(void *arg) {
      long thread_id = reinterpret_cast<long>(arg);
      AppLogger::log(AppLogger::INFO_L, "RT Thread (ID: " + std::to_string(pthread_self()) + "): Started.");

      // Lấy và in ra chính sách và độ ưu tiên hiện tại của luồng này
      int policy;
      struct sched_param param;
      pthread_getschedparam(pthread_self(), &policy, &param);
      std::string policy_str = (policy == SCHED_FIFO ? "SCHED_FIFO" : (policy == SCHED_RR ? "SCHED_RR" : "SCHED_OTHER"));
      AppLogger::log(AppLogger::INFO_L, "RT Thread: Current policy: " + policy_str + ", priority: " + std::to_string(param.sched_priority));

      AppLogger::log(AppLogger::INFO_L, "RT Thread: Performing heavy work for 5 seconds...");
      // Mô phỏng công việc nặng
      for (long i = 0; i < 5; ++i) {
          // std::cout << "RT Thread: Working... " << i << std::endl; // Tránh I/O trong RT thread nếu có thể
          sleep(1);
      }
      AppLogger::log(AppLogger::INFO_L, "RT Thread: Finished work and exiting.");
      pthread_exit(nullptr);
  }

  int main() {
      pthread_t rt_thread_id;
      pthread_attr_t thread_attributes;
      int res;

      AppLogger::log(AppLogger::INFO_L, "Main Thread: Starting Real-Time Thread Demonstration.");

      // 1. Khởi tạo đối tượng thuộc tính
      res = pthread_attr_init(&thread_attributes);
      if (res != 0) {
          AppLogger::log(AppLogger::CRITICAL_L, "Main Thread: Failed to initialize thread attributes: " + strerror(res));
          return EXIT_FAILURE;
      }

      // 2. Đặt chính sách lập lịch SCHED_FIFO
      res = pthread_attr_setschedpolicy(&thread_attributes, SCHED_FIFO);
      if (res != 0) {
          AppLogger::log(AppLogger::CRITICAL_L, "Main Thread: Failed to set scheduling policy to SCHED_FIFO: " + strerror(res));
          AppLogger::log(AppLogger::WARNING_L, "Main Thread: (This often requires root privileges. Try running with 'sudo'.)");
          pthread_attr_destroy(&thread_attributes);
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Scheduling policy set to SCHED_FIFO.");

      // 3. Đặt độ ưu tiên (thường là ưu tiên cao nhất cho SCHED_FIFO)
      int max_priority = sched_get_priority_max(SCHED_FIFO);
      if (max_priority == -1) {
          AppLogger::log(AppLogger::ERROR_L, "Main Thread: Failed to get max priority for SCHED_FIFO: " + strerror(errno));
          pthread_attr_destroy(&thread_attributes);
          return EXIT_FAILURE;
      }
      struct sched_param scheduling_param;
      scheduling_param.sched_priority = max_priority; // Đặt ưu tiên cao nhất
      res = pthread_attr_setschedparam(&thread_attributes, &scheduling_param);
      if (res != 0) {
          AppLogger::log(AppLogger::CRITICAL_L, "Main Thread: Failed to set scheduling priority: " + strerror(res));
          AppLogger::log(AppLogger::WARNING_L, "Main Thread: (This often requires root privileges. Try running with 'sudo'.)");
          pthread_attr_destroy(&thread_attributes);
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Scheduling priority set to " + std::to_string(max_priority) + ".");

      // 4. Tạo luồng với các thuộc tính đã đặt
      res = pthread_create(&rt_thread_id, &thread_attributes, rt_thread_function, reinterpret_cast<void*>(1L));
      if (res != 0) {
          AppLogger::log(AppLogger::CRITICAL_L, "Main Thread: Failed to create real-time thread: " + strerror(res));
          pthread_attr_destroy(&thread_attributes);
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Real-time thread created with ID: " + std::to_string(rt_thread_id) + ".");

      // 5. Hủy đối tượng thuộc tính
      pthread_attr_destroy(&thread_attributes);

      // 6. Luồng chính chờ luồng thời gian thực hoàn thành
      AppLogger::log(AppLogger::INFO_L, "Main Thread: Waiting for real-time thread to finish...");
      pthread_join(rt_thread_id, nullptr);
      AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Real-time thread finished. Exiting.");

      return EXIT_SUCCESS;
  }
  ```

  * **Cách Biên dịch:**
    **Bash**

    ```
    g++ realtime_thread.cpp -o realtime_thread -pthread
    ```
  * **Cách Chạy:**
    **Bash**

    ```
    ./realtime_thread # Có thể thất bại nếu không có quyền root
    sudo ./realtime_thread # Chạy với quyền root để đặt chính sách RT
    ```
  * **Phân tích Output:** Quan sát thông báo về chính sách và độ ưu tiên của luồng RT. Nếu chạy với `sudo`, bạn sẽ thấy nó thành công. Bạn có thể thử chạy một chương trình tiêu tốn CPU khác cùng lúc để xem luồng RT có được ưu tiên hơn không.

---

### **Câu hỏi Tự đánh giá Module 4 🤔**

1. Giải thích mục đích của đối tượng `pthread_attr_t`. Bạn sẽ làm gì với nó trước và sau khi tạo luồng?
2. Phân biệt giữa trạng thái `PTHREAD_CREATE_JOINABLE` và `PTHREAD_CREATE_DETACHED` của một luồng. Khi nào bạn sẽ chọn tạo một luồng ở trạng thái `detached`?
3. Bạn có thể thay đổi trạng thái của một luồng từ `joinable` sang `detached` sau khi nó đã được tạo không? Nếu có, bằng hàm nào?
4. Nêu ba chính sách lập lịch luồng chính trong Linux. Chính sách nào là mặc định và chính sách nào yêu cầu quyền `root` để đặt độ ưu tiên cao?
5. Giải thích cách `sched_get_priority_max()` và `sched_get_priority_min()` được sử dụng khi đặt độ ưu tiên cho luồng thời gian thực.

---

### **Bài tập Thực hành Module 4 ✍️**

1. **Chương trình "Background Task":**
   * Viết một chương trình C++ (`background_task.cpp`) mà:
     * Tạo một luồng con ở trạng thái  **detached** .
     * Hàm luồng con sẽ in ra "Background task running..." mỗi 2 giây trong 10 giây.
     * Luồng chính sẽ in ra "Main thread doing other work..." mỗi 1 giây trong 5 giây, sau đó thoát.
   * **Mục tiêu:** Quan sát rằng luồng chính thoát trước khi luồng con hoàn thành, và luồng con vẫn tiếp tục chạy và tự dọn dẹp tài nguyên mà không cần `pthread_join()`.
2. **Chương trình "Prioritized Worker":**
   * Viết một chương trình C++ (`prioritized_worker.cpp`) có một hàm `heavy_computation_task()` thực hiện một vòng lặp tính toán nặng (ví dụ: `for (long i = 0; i < 1000000000; ++i) { volatile double x = sqrt(i); }`).
   * Trong `main()`:
     * Tạo một luồng con để chạy `heavy_computation_task()`.
     * Đặt chính sách lập lịch của luồng con này là `SCHED_FIFO` với độ ưu tiên  **cao nhất** .
     * Luồng chính cũng thực hiện một tác vụ tính toán nặng tương tự hoặc in ra thông báo định kỳ.
   * **Mục tiêu:**
     * Chạy chương trình với quyền `root` (`sudo ./prioritized_worker`).
     * Quan sát xem luồng có độ ưu tiên cao hơn có hoàn thành nhanh hơn đáng kể so với luồng chính (hoặc các luồng khác) không.
     * **Thử thách:** Thêm một luồng thứ ba với chính sách `SCHED_OTHER` và quan sát cách nó bị ảnh hưởng bởi luồng `SCHED_FIFO`.
