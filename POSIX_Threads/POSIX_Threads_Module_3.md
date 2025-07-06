# **Module 3: Đồng bộ hóa Luồng (Synchronization) 🤝**

#### **3.1. Vấn đề Đồng bộ hóa (The Synchronization Problem) 💥**

* **Lý thuyết:** Khi nhiều luồng trong cùng một tiến trình chia sẻ **chung các biến toàn cục** hoặc **dữ liệu trên heap**, nếu chúng cùng cố gắng truy cập và sửa đổi dữ liệu đó đồng thời mà không có sự phối hợp, sẽ xảy ra  **điều kiện tranh chấp (race condition)** .
  * **Ví dụ về Race Condition:**
    * Hai luồng cùng đọc giá trị `X` của một biến.
    * Luồng 1 tăng `X` lên 1, sau đó ghi `X+1` trở lại.
    * Luồng 2 cũng tăng `X` lên 1, sau đó ghi `X+1` trở lại.
    * Kết quả cuối cùng là `X` chỉ tăng 1 đơn vị, thay vì 2 đơn vị như mong đợi.
  * **Hậu quả:** Dữ liệu bị hỏng, không nhất quán, hoặc chương trình có hành vi không thể dự đoán được. Các lỗi này rất khó debug vì chúng thường chỉ xảy ra dưới những điều kiện thời gian cụ thể.
* **Vùng Găng (Critical Section):** Là một đoạn mã mà tại một thời điểm chỉ được phép có **một luồng duy nhất** thực thi. Các cơ chế đồng bộ hóa được dùng để bảo vệ các vùng găng này.
* **Liên hệ Embedded Linux:** Cực kỳ phổ biến và nguy hiểm trong các hệ thống nhúng. Ví dụ:
  * Nhiều luồng cùng cập nhật một biến trạng thái của thiết bị.
  * Nhiều luồng cùng truy cập một buffer dữ liệu cảm biến.
  * Nhiều luồng cùng ghi vào một file log.
  * Nếu không đồng bộ hóa đúng cách, dữ liệu có thể bị sai lệch, dẫn đến thiết bị hoạt động không chính xác hoặc crash.

#### **3.2. Đồng bộ hóa với Mutexes (Mutex Synchronization) 🔐**

* **Lý thuyết:** **Mutex (Mutual Exclusion - Độc quyền tương hỗ)** là cơ chế đồng bộ hóa phổ biến nhất để bảo vệ một **vùng găng** hoặc một biến dữ liệu chung. Nó đảm bảo rằng **chỉ một luồng duy nhất** tại một thời điểm có thể giữ khóa (lock) của mutex.

  * **Trạng thái của Mutex:** Có hai trạng thái: **khóa (locked)** hoặc  **mở khóa (unlocked)** .
  * **Cách hoạt động:**

    1. Một luồng muốn truy cập vùng găng sẽ cố gắng **khóa (lock)** mutex.
    2. Nếu mutex đang ở trạng thái  **mở khóa** , luồng sẽ khóa nó và tiếp tục vào vùng găng.
    3. Nếu mutex đang ở trạng thái  **khóa** , luồng sẽ bị **chặn (block)** – tức là tạm dừng việc thực thi – cho đến khi mutex được mở khóa.
    4. Sau khi hoàn thành công việc trong vùng găng, luồng phải **mở khóa (unlock)** mutex để cho phép các luồng khác truy cập.
  * **Các hàm Pthreads Mutex:**
    **C++**

    ```cpp
    #include <pthread.h>
    // pthread_mutex_t mutex_object; // Khai báo biến mutex

    int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr); // Khởi tạo mutex
    int pthread_mutex_lock(pthread_mutex_t *mutex);   // Khóa mutex
    int pthread_mutex_unlock(pthread_mutex_t *mutex); // Mở khóa mutex
    int pthread_mutex_destroy(pthread_mutex_t *mutex); // Hủy mutex
    ```

    * `pthread_mutex_init()`: Khởi tạo một mutex. `mutexattr` thường là `NULL` cho thuộc tính mặc định (fast mutex).
    * `pthread_mutex_lock()`: Cố gắng khóa mutex. Nếu đã bị khóa, luồng gọi sẽ bị chặn.
    * `pthread_mutex_unlock()`: Mở khóa mutex.
    * `pthread_mutex_destroy()`: Hủy mutex. Cần hủy mutex khi không còn sử dụng để giải phóng tài nguyên.
    * **Giá trị trả về:** `0` nếu thành công, khác `0` (mã lỗi) nếu thất bại. **Lưu ý:** Các hàm `pthread_mutex_`  **không thiết lập `errno`** .

  ---

  ## 🔒 Deadlock với Mutex

  ### ✅ Trường hợp 1: Luồng tự khóa lại chính mutex nó đã giữ

  **Giả định:**


  * Dùng loại mutex mặc định là `PTHREAD_MUTEX_DEFAULT` (thường là “fast mutex”)
  * Luồng A đã **lock** `M1`
  * Sau đó, nó **gọi lại `pthread_mutex_lock(M1)`** → vì đã giữ rồi, nó sẽ **đợi chính nó** → không bao giờ xong

  📛 → **Bị kẹt (deadlock) vĩnh viễn**

  > 💡 Nếu muốn cho phép mutex được luồng gọi nhiều lần, phải dùng loại `recursive mutex`.
  >

  ---

  ### ✅ Trường hợp 2: Hai luồng giữ hai mutex, rồi cùng chờ nhau – thứ tự ngược nhau

  **Ví dụ cụ thể:**

  | Luồng A            | Luồng B            |
  | ------------------- | ------------------- |
  | Lock `M1`         | Lock `M2`         |
  | → rồi Lock `M2` | → rồi Lock `M1` |

  📛 → Mỗi luồng đang giữ một mutex và **muốn khóa mutex còn lại**

  → Nhưng mutex đó **đang bị luồng kia giữ rồi**

  → → **Cả hai chờ nhau mãi mãi** →  **Deadlock** !

  ---

  ## 🧠 Giải pháp: Dùng **thứ tự nhất quán**

  * Quy ước rõ: nếu nhiều mutex phải khóa, **luôn khóa theo thứ tự tăng (M1 → M2 → M3...)**
  * Dù ở bất kỳ luồng hay hàm nào, **giữ nguyên thứ tự**

  ✅ Nhờ đó, không bao giờ xảy ra “luồng A giữ M1 rồi đòi M2” trong khi “luồng B giữ M2 rồi đòi M1”


* **Liên hệ Embedded Linux:** Mutex là công cụ đồng bộ hóa cơ bản và được sử dụng rộng rãi nhất trong các ứng dụng nhúng đa luồng để bảo vệ các biến trạng thái, buffer dữ liệu, hoặc truy cập phần cứng.
* **Ví dụ (C++): `mutex_sync.cpp` - Đồng bộ hóa với Mutex**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <pthread.h> // For pthread_mutex_t, pthread_mutex_init, pthread_mutex_lock, pthread_mutex_unlock, pthread_mutex_destroy
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

  // Biến toàn cục được chia sẻ giữa các luồng
  int shared_counter = 0;
  pthread_mutex_t counter_mutex; // Mutex để bảo vệ shared_counter

  // Hàm luồng: tăng biến shared_counter
  void *increment_counter_thread(void *arg) {
      long thread_id = reinterpret_cast<long>(arg);
      AppLogger::log(AppLogger::INFO_L, "Thread " + std::to_string(thread_id) + ": Started.");

      for (int i = 0; i < 100000; ++i) {
          // Khóa mutex trước khi truy cập vùng găng
          int res = pthread_mutex_lock(&counter_mutex);
          if (res != 0) {
              AppLogger::log(AppLogger::ERROR_L, "Thread " + std::to_string(thread_id) + ": Failed to lock mutex: " + strerror(res));
              pthread_exit(nullptr);
          }
          shared_counter++; // Vùng găng
          // Mở khóa mutex sau khi hoàn thành truy cập
          res = pthread_mutex_unlock(&counter_mutex);
          if (res != 0) {
              AppLogger::log(AppLogger::ERROR_L, "Thread " + std::to_string(thread_id) + ": Failed to unlock mutex: " + strerror(res));
              pthread_exit(nullptr);
          }
      }
      AppLogger::log(AppLogger::INFO_L, "Thread " + std::to_string(thread_id) + ": Finished incrementing.");
      pthread_exit(nullptr);
  }

  int main() {
      pthread_t threads[5]; // Mảng để lưu ID của 5 luồng
      int res;

      AppLogger::log(AppLogger::INFO_L, "Main Thread: Starting Mutex Synchronization Demonstration.");

      // 1. Khởi tạo mutex
      res = pthread_mutex_init(&counter_mutex, nullptr);
      if (res != 0) {
          AppLogger::log(AppLogger::CRITICAL_L, "Main Thread: Failed to initialize mutex: " + strerror(res));
          return EXIT_FAILURE;
      }
      AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Mutex initialized.");

      // 2. Tạo 5 luồng
      AppLogger::log(AppLogger::INFO_L, "Main Thread: Creating 5 threads to increment a shared counter.");
      for (long i = 0; i < 5; ++i) {
          res = pthread_create(&threads[i], nullptr, increment_counter_thread, reinterpret_cast<void*>(i + 1));
          if (res != 0) {
              AppLogger::log(AppLogger::ERROR_L, "Main Thread: Failed to create thread " + std::to_string(i + 1) + ": " + strerror(res));
              // Dọn dẹp các luồng đã tạo và mutex
              for (int j = 0; j < i; ++j) pthread_join(threads[j], nullptr);
              pthread_mutex_destroy(&counter_mutex);
              return EXIT_FAILURE;
          }
      }

      // 3. Chờ tất cả các luồng con hoàn thành
      AppLogger::log(AppLogger::INFO_L, "Main Thread: Waiting for all threads to finish...");
      for (int i = 0; i < 5; ++i) {
          pthread_join(threads[i], nullptr);
      }

      // 4. In giá trị cuối cùng của biến đếm
      AppLogger::log(AppLogger::INFO_L, "Main Thread: All threads finished.");
      AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Final shared_counter value: " + std::to_string(shared_counter));

      // 5. Hủy mutex
      res = pthread_mutex_destroy(&counter_mutex);
      if (res != 0) {
          AppLogger::log(AppLogger::ERROR_L, "Main Thread: Failed to destroy mutex: " + strerror(res));
      } else {
          AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Mutex destroyed.");
      }

      return EXIT_SUCCESS;
  }
  ```

  * **Cách Biên dịch:**
    **Bash**

    ```
    g++ mutex_sync.cpp -o mutex_sync -pthread
    ```
  * **Cách Chạy:**
    **Bash**

    ```
    ./mutex_sync
    ```
  * **Phân tích:** Giá trị `shared_counter` cuối cùng sẽ là `5 * 100000 = 500000`. Nếu bạn **bỏ comment** các dòng `pthread_mutex_lock` và `pthread_mutex_unlock`, bạn sẽ thấy giá trị này thường **nhỏ hơn 500000** do race condition.

#### **3.3. Đồng bộ hóa với Semaphores (Semaphore Synchronization) 🚦**

* **Lý thuyết:** **Semaphore** là một biến nguyên (integer variable) được sử dụng để kiểm soát quyền truy cập vào một tài nguyên chung. Nó có thể được dùng để:

  * **Bảo vệ vùng găng:** Tương tự mutex (khi giá trị đếm là 1).
  * **Đồng bộ hóa nhà sản xuất-người tiêu dùng (Producer-Consumer):** Điều phối việc truy cập vào một buffer chia sẻ giữa các luồng sản xuất dữ liệu và luồng tiêu thụ dữ liệu.
  * **Giới hạn số lượng truy cập đồng thời:** Cho phép một số lượng giới hạn luồng truy cập một tài nguyên cụ thể cùng một lúc (ví dụ: 3 luồng truy cập một server có giới hạn 3 kết nối).
* **Các loại Semaphore:**

  * **Binary Semaphore (Semaphore nhị phân):** Giá trị đếm chỉ là 0 hoặc 1. Hoạt động giống như một mutex.
  * **Counting Semaphore (Semaphore đếm):** Giá trị đếm có thể lớn hơn 1.
* **Các hàm Pthreads Semaphore:**
  **C++**

  ```cpp
  #include <semaphore.h> // For sem_t, sem_init, sem_wait, sem_post, sem_destroy
  // sem_t semaphore_object; // Khai báo biến semaphore

  int sem_init(sem_t *sem, int pshared, unsigned int value); // Khởi tạo semaphore
  int sem_wait(sem_t *sem);   // Giảm giá trị semaphore (P operation / "down")
  int sem_post(sem_t *sem);   // Tăng giá trị semaphore (V operation / "up")
  int sem_destroy(sem_t *sem); // Hủy semaphore
  ```

  * `sem_init()`: Khởi tạo semaphore.
    * `pshared`: `0` cho semaphore chỉ dùng giữa các luồng trong cùng tiến trình. Khác `0` cho semaphore chia sẻ giữa các tiến trình (thường không được hỗ trợ đầy đủ trên Linux cho `sem_init` hoặc cần các API khác).
    * `value`: Giá trị khởi tạo của semaphore (giá trị đếm ban đầu).
  * `sem_wait()`: Giảm giá trị semaphore đi 1. Nếu giá trị là 0, luồng bị chặn cho đến khi giá trị lớn hơn 0. (Thao tác "kiểm tra và đặt" atomic).
  * `sem_post()`: Tăng giá trị semaphore đi 1. Nếu có luồng nào đang bị chặn trên semaphore, một trong số chúng sẽ được đánh thức. (Thao tác atomic).
  * `sem_destroy()`: Hủy semaphore.
  * **Giá trị trả về:** `0` nếu thành công, khác `0` (mã lỗi) nếu thất bại. **Lưu ý:** Các hàm `sem_` cũng **không thiết lập `errno`** cho lỗi trả về của hàm.
* **So sánh Mutex và Semaphore:**

  * **Mutex:** Dành cho **độc quyền tương hỗ** (chỉ một luồng). Thích hợp cho việc bảo vệ một vùng dữ liệu/code duy nhất.
  * **Semaphore:** Linh hoạt hơn. Có thể dùng để bảo vệ vùng găng (binary semaphore) hoặc để **kiểm soát số lượng tài nguyên** hoặc **đồng bộ hóa sự kiện** giữa các luồng/tiến trình (counting semaphore).
* **Liên hệ Embedded Linux:** Semaphore rất hữu ích trong nhúng để:

  * Kiểm soát quyền truy cập vào các tài nguyên phần cứng có giới hạn (ví dụ: chỉ 1 luồng được phép truy cập ADC tại một thời điểm).
  * Đồng bộ hóa giữa các luồng sản xuất dữ liệu (ví dụ: từ driver cảm biến) và luồng tiêu thụ dữ liệu (ví dụ: xử lý dữ liệu).
* **Ví dụ (C++): `semaphore_sync.cpp` - Đồng bộ hóa với Semaphore (Producer-Consumer)**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <pthread.h> // For pthread_create, pthread_join, pthread_exit
  #include <semaphore.h> // For sem_t, sem_init, sem_wait, sem_post, sem_destroy
  #include <unistd.h>  // For sleep
  #include <cstdlib>   // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>   // For strerror (for non-semaphore errors)
  #include <vector>    // For std::vector

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

  // Semaphore để báo hiệu dữ liệu có sẵn (được producer post)
  sem_t data_available_sem;
  // Semaphore để báo hiệu buffer trống (được consumer post)
  sem_t buffer_empty_sem;

  #define BUFFER_CAPACITY 5 // Kích thước buffer
  std::vector<int> shared_buffer; // Buffer chia sẻ
  pthread_mutex_t buffer_mutex; // Mutex để bảo vệ truy cập buffer

  // Biến cờ để báo hiệu kết thúc
  volatile bool producer_finished = false;

  // Hàm luồng Producer
  void *producer_thread(void *arg) {
      long thread_id = reinterpret_cast<long>(arg);
      AppLogger::log(AppLogger::INFO_L, "Producer Thread " + std::to_string(thread_id) + ": Started.");

      for (int i = 0; i < 10; ++i) {
          // Chờ buffer trống (nếu đầy)
          sem_wait(&buffer_empty_sem); 

          pthread_mutex_lock(&buffer_mutex); // Khóa mutex để truy cập buffer
          shared_buffer.push_back(i);
          AppLogger::log(AppLogger::INFO_L, "Producer " + std::to_string(thread_id) + ": Produced item " + std::to_string(i) + ". Buffer size: " + std::to_string(shared_buffer.size()));
          pthread_mutex_unlock(&buffer_mutex); // Mở khóa mutex

          sem_post(&data_available_sem); // Báo hiệu dữ liệu có sẵn
          sleep(1); // Giả vờ làm việc
      }
      producer_finished = true;
      AppLogger::log(AppLogger::INFO_L, "Producer Thread " + std::to_string(thread_id) + ": Finished producing.");
      // Post thêm lần nữa để đảm bảo consumer không bị chặn mãi nếu còn chờ
      sem_post(&data_available_sem); 
      pthread_exit(nullptr);
  }

  // Hàm luồng Consumer
  void *consumer_thread(void *arg) {
      long thread_id = reinterpret_cast<long>(arg);
      AppLogger::log(AppLogger::INFO_L, "Consumer Thread " + std::to_string(thread_id) + ": Started.");

      while (true) {
          // Chờ dữ liệu có sẵn (nếu trống)
          sem_wait(&data_available_sem); 

          if (producer_finished && shared_buffer.empty()) {
              AppLogger::log(AppLogger::INFO_L, "Consumer Thread " + std::to_string(thread_id) + ": Producer finished and buffer empty. Exiting.");
              break; // Thoát nếu producer xong và buffer trống
          }

          pthread_mutex_lock(&buffer_mutex); // Khóa mutex để truy cập buffer
          if (!shared_buffer.empty()) {
              int item = shared_buffer.front();
              shared_buffer.erase(shared_buffer.begin());
              AppLogger::log(AppLogger::INFO_L, "Consumer " + std::to_string(thread_id) + ": Consumed item " + std::to_string(item) + ". Buffer size: " + std::to_string(shared_buffer.size()));
          }
          pthread_mutex_unlock(&buffer_mutex); // Mở khóa mutex

          sem_post(&buffer_empty_sem); // Báo hiệu buffer trống
          sleep(2); // Giả vờ làm việc
      }
      AppLogger::log(AppLogger::INFO_L, "Consumer Thread " + std::to_string(thread_id) + ": Finished consuming.");
      pthread_exit(nullptr);
  }

  int main() {
      pthread_t producer_tid, consumer_tid;
      int res;

      AppLogger::log(AppLogger::INFO_L, "Main Thread: Starting Semaphore Synchronization Demonstration (Producer-Consumer).");

      // 1. Khởi tạo Semaphores
      // data_available_sem: Ban đầu 0 (không có dữ liệu)
      res = sem_init(&data_available_sem, 0, 0);
      if (res != 0) { AppLogger::log(AppLogger::CRITICAL_L, "sem_init data_available_sem failed: " + strerror(res)); return EXIT_FAILURE; }
      // buffer_empty_sem: Ban đầu BUFFER_CAPACITY (buffer trống hoàn toàn)
      res = sem_init(&buffer_empty_sem, 0, BUFFER_CAPACITY);
      if (res != 0) { AppLogger::log(AppLogger::CRITICAL_L, "sem_init buffer_empty_sem failed: " + strerror(res)); sem_destroy(&data_available_sem); return EXIT_FAILURE; }
      // Khởi tạo Mutex bảo vệ buffer
      res = pthread_mutex_init(&buffer_mutex, nullptr);
      if (res != 0) { AppLogger::log(AppLogger::CRITICAL_L, "pthread_mutex_init failed: " + strerror(res)); sem_destroy(&data_available_sem); sem_destroy(&buffer_empty_sem); return EXIT_FAILURE; }

      AppLogger::log(AppLogger::SUCCESS_L, "Semaphores and Mutex initialized.");

      // 2. Tạo luồng Producer và Consumer
      res = pthread_create(&producer_tid, nullptr, producer_thread, reinterpret_cast<void*>(1L));
      if (res != 0) { AppLogger::log(AppLogger::ERROR_L, "Failed to create producer thread: " + strerror(res)); goto cleanup; }
      res = pthread_create(&consumer_tid, nullptr, consumer_thread, reinterpret_cast<void*>(2L));
      if (res != 0) { AppLogger::log(AppLogger::ERROR_L, "Failed to create consumer thread: " + strerror(res)); pthread_join(producer_tid, nullptr); goto cleanup; }

      // 3. Chờ cả hai luồng hoàn thành
      AppLogger::log(AppLogger::INFO_L, "Main Thread: Waiting for producer and consumer threads to finish...");
      pthread_join(producer_tid, nullptr);
      pthread_join(consumer_tid, nullptr);

      AppLogger::log(AppLogger::INFO_L, "Main Thread: All threads finished.");

  cleanup:
      // 4. Hủy Semaphores và Mutex
      res = sem_destroy(&data_available_sem);
      if (res != 0) AppLogger::log(AppLogger::ERROR_L, "Failed to destroy data_available_sem: " + strerror(res));
      res = sem_destroy(&buffer_empty_sem);
      if (res != 0) AppLogger::log(AppLogger::ERROR_L, "Failed to destroy buffer_empty_sem: " + strerror(res));
      res = pthread_mutex_destroy(&buffer_mutex);
      if (res != 0) AppLogger::log(AppLogger::ERROR_L, "Failed to destroy buffer_mutex: " + strerror(res));
      AppLogger::log(AppLogger::SUCCESS_L, "Semaphores and Mutex destroyed.");

      return EXIT_SUCCESS;
  }
  ```

  * **Cách Biên dịch:**
    **Bash**

    ```
    g++ semaphore_sync.cpp -o semaphore_sync -pthread
    ```
  * **Cách Chạy:**
    **Bash**

    ```
    ./semaphore_sync
    ```
  * **Phân tích Output:** Quan sát cách các luồng Producer và Consumer xen kẽ nhau. Producer sẽ bị chặn khi buffer đầy, và Consumer sẽ bị chặn khi buffer trống, đảm bảo luồng dữ liệu được điều phối một cách an toàn.

---

### **Câu hỏi Tự đánh giá Module 3 🤔**

1. "Điều kiện tranh chấp" (race condition) là gì trong lập trình đa luồng? Nêu một ví dụ cụ thể về cách nó có thể xảy ra.
2. Giải thích vai trò của Mutex trong đồng bộ hóa luồng. Khi nào một luồng bị chặn khi cố gắng khóa một Mutex?
3. Phân biệt giữa Mutex và Semaphore. Nêu một trường hợp bạn sẽ ưu tiên sử dụng Semaphore hơn Mutex.
4. Khi khởi tạo Semaphore bằng `sem_init()`, tham số `pshared` có ý nghĩa gì?
5. Giải thích cách `sem_wait()` và `sem_post()` hoạt động để điều phối quyền truy cập tài nguyên.
6. Tại sao các hàm Pthreads (như `pthread_mutex_lock()`) và Semaphore (như `sem_wait()`) không thiết lập `errno` khi trả về lỗi? Làm thế nào để bạn kiểm tra lỗi của chúng?
7. "Deadlock" là gì trong ngữ cảnh đồng bộ hóa luồng? Nêu một ví dụ về cách nó có thể xảy ra với Mutex và một cách để phòng tránh.

---

### **Bài tập Thực hành Module 3 ✍️**

1. **Chương trình "Đếm Ngược An toàn Luồng":**
   * Viết một chương trình C++ (`safe_countdown.cpp`) có một biến toàn cục `counter` được khởi tạo bằng 10.
   * Tạo 3 luồng con.
   * Mỗi luồng sẽ thực hiện một vòng lặp 5 lần. Trong mỗi lần lặp:
     * Khóa một Mutex.
     * Giảm `counter` đi 1.
     * In ra thông báo "Thread [ID] decremented counter to [giá trị mới]".
     * Mở khóa Mutex.
     * Ngủ một khoảng thời gian ngắn (ví dụ: 100ms).
   * Luồng chính sẽ chờ tất cả các luồng con hoàn thành và in ra giá trị cuối cùng của `counter`.
   * **Thử thách:** Chạy chương trình mà không có Mutex và quan sát output để thấy race condition.
2. **Chương trình "Giới hạn Kết nối Server":**
   * Viết một chương trình C++ (`connection_limiter.cpp`) mô phỏng một server có thể xử lý tối đa 3 kết nối đồng thời.
   * Sử dụng một Counting Semaphore được khởi tạo với giá trị 3.
   * Tạo 5 luồng "client" đồng thời.
   * Mỗi luồng client sẽ:
     * In ra "Client [ID] trying to connect..."
     * Gọi `sem_wait()` trên Semaphore.
     * Nếu thành công, in ra "Client [ID] CONNECTED. Processing request..."
     * Ngủ một khoảng thời gian ngẫu nhiên (ví dụ: 1-5 giây) để mô phỏng xử lý.
     * In ra "Client [ID] DISCONNECTED."
     * Gọi `sem_post()` trên Semaphore.
   * Luồng chính sẽ chờ tất cả các client hoàn thành.
   * **Mục tiêu:** Quan sát output để thấy rằng không bao giờ có quá 3 client "CONNECTED" cùng một lúc.
3. **Chương trình "Sản xuất-Tiêu thụ với Condition Variables" (Nâng cao - Giới thiệu):**
   * (Đây là một bước tiến từ Semaphore, sử dụng `pthread_cond_t` và `pthread_cond_wait`/`pthread_cond_signal`).
   * Viết một chương trình C++ (`prod_cons_condvar.cpp`) mô phỏng Producer-Consumer với một buffer chia sẻ có kích thước cố định.
   * Sử dụng **một Mutex** để bảo vệ buffer và **hai Condition Variables** (một cho "buffer đầy", một cho "buffer trống").
   * **Producer:**
     * Khóa Mutex.
     * `while (buffer_is_full)`: `pthread_cond_wait` trên "buffer đầy".
     * Thêm item vào buffer.
     * `pthread_cond_signal` trên "buffer trống".
     * Mở khóa Mutex.
   * **Consumer:**
     * Khóa Mutex.
     * `while (buffer_is_empty)`: `pthread_cond_wait` trên "buffer trống".
     * Lấy item từ buffer.
     * `pthread_cond_signal` trên "buffer đầy".
     * Mở khóa Mutex.
   * **Mục tiêu:** Hiểu rằng Condition Variables cho phép luồng ngủ hiệu quả hơn khi chờ một điều kiện cụ thể, thay vì chỉ chờ một giá trị đếm.
