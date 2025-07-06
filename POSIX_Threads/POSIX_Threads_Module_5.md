# **Module 5: Hủy một Luồng (Canceling a Thread) 🛑**

#### **5.1. Yêu cầu Hủy Luồng (`pthread_cancel()`) ✉️**

* **Lý thuyết:** Hàm **`pthread_cancel()`** được sử dụng để gửi một **yêu cầu hủy (cancellation request)** đến một luồng cụ thể.
  * **Syntax:**
    **C++**

    ```cpp
    #include <pthread.h>
    // int pthread_cancel(pthread_t thread);
    ```
  * **`thread`** : ID của luồng mà bạn muốn hủy.
  * **Giá trị trả về:** `0` nếu thành công, khác `0` (mã lỗi) nếu thất bại (ví dụ: `ESRCH` nếu luồng không tồn tại).
* **Lưu ý:** `pthread_cancel()` chỉ gửi một  **yêu cầu** . Luồng mục tiêu sẽ không bị chấm dứt ngay lập tức trừ khi nó được cấu hình để hủy đồng bộ (asynchronous cancellation) hoặc đang ở một điểm hủy bỏ (cancellation point).

#### **5.2. Kiểm soát Hành vi Hủy Luồng (Cancellation State and Type) ⚙️**

Luồng mục tiêu có thể kiểm soát cách nó phản ứng với yêu cầu hủy bằng cách đặt trạng thái và loại hủy bỏ của nó.

* **Trạng thái Hủy (Cancellation State):**
  * **Lý thuyết:** Xác định liệu luồng có chấp nhận yêu cầu hủy hay không.
    **C++**

    ```cpp
    #include <pthread.h>
    // int pthread_setcancelstate(int state, int *oldstate);
    ```

    * **`state`** :
    * `PTHREAD_CANCEL_ENABLE` (Mặc định): Luồng có thể bị hủy.
    * `PTHREAD_CANCEL_DISABLE`: Luồng sẽ bỏ qua các yêu cầu hủy. Các yêu cầu hủy sẽ được giữ lại (pending) cho đến khi luồng chuyển lại trạng thái `PTHREAD_CANCEL_ENABLE`.
    * **`oldstate`** : Con trỏ tới `int` để lưu trạng thái hủy bỏ trước đó (có thể là `NULL` nếu không cần).
* **Loại Hủy (Cancellation Type):**
  * **Lý thuyết:** Xác định thời điểm mà luồng sẽ bị hủy nếu nó chấp nhận yêu cầu hủy.
    **C++**

    ```
    #include <pthread.h>
    // int pthread_setcanceltype(int type, int *oldtype);
    ```

    * **`type`** :
    * `PTHREAD_CANCEL_DEFERRED` (Mặc định): Luồng chỉ bị hủy tại các  **điểm hủy bỏ (cancellation points)** . Đây là loại hủy bỏ an toàn nhất và được khuyến nghị sử dụng.
    * `PTHREAD_CANCEL_ASYNCHRONOUS`: Luồng có thể bị hủy tại **bất kỳ thời điểm nào** (trừ khi đang giữ một mutex). **Rất nguy hiểm và hiếm khi được sử dụng** vì nó có thể làm hỏng dữ liệu hoặc tài nguyên nếu luồng bị hủy giữa chừng một thao tác quan trọng.
    * **`oldtype`** : Con trỏ tới `int` để lưu loại hủy bỏ trước đó (có thể là `NULL`).

#### **5.3. Điểm Hủy bỏ (Cancellation Points) 📍**

* **Lý thuyết:** Khi loại hủy bỏ là `PTHREAD_CANCEL_DEFERRED`, luồng chỉ bị hủy tại các **điểm hủy bỏ** đã được xác định. Đây thường là các System Call hoặc hàm thư viện có thể bị chặn (blocking calls) hoặc các hàm được thiết kế để là điểm hủy bỏ.
  * **Các điểm hủy bỏ phổ biến:**
    * Các hàm `pthread_join()`, `pthread_cond_wait()`, `sem_wait()`, `sigsuspend()`.
    * Các System Call có thể bị chặn như `read()`, `write()`, `sleep()`, `accept()`, `connect()`.
    * Hàm  **`pthread_testcancel()`** : Bạn có thể chèn hàm này vào bất kỳ đâu trong code của mình để tạo một điểm hủy bỏ tường minh. Nếu có yêu cầu hủy đang chờ và luồng đang ở trạng thái `PTHREAD_CANCEL_ENABLE`, `pthread_testcancel()` sẽ khiến luồng bị hủy ngay lập tức tại điểm đó.
      **C++**

      ```cpp
      #include <pthread.h>
      // void pthread_testcancel(void);
      ```
* **Liên hệ Embedded Linux:**
  * Việc hủy luồng là một cách để dừng các tác vụ nền khi không còn cần thiết (ví dụ: dừng luồng thu thập dữ liệu khi thiết bị tắt).
  * Hiểu rõ các điểm hủy bỏ và sử dụng `pthread_testcancel()` là quan trọng để đảm bảo luồng của bạn có thể phản hồi yêu cầu hủy một cách kịp thời và duyên dáng.
  * Luôn ưu tiên `PTHREAD_CANCEL_DEFERRED` để tránh các lỗi không mong muốn.

#### **5.4. Clean-up Handlers (Trình xử lý Dọn dẹp) 🧹**

* **Lý thuyết:** Khi một luồng bị hủy (hoặc tự thoát bằng `pthread_exit()`), nó có thể đang giữ các tài nguyên (ví dụ: mutex, file descriptor, bộ nhớ động). Để tránh rò rỉ tài nguyên, bạn cần đảm bảo các tài nguyên này được giải phóng. **Clean-up handlers** là các hàm được đăng ký để tự động chạy khi luồng bị hủy hoặc thoát.
  * **Các hàm:**
    **C++**

    ```cpp
    #include <pthread.h>
    // void pthread_cleanup_push(void (*routine)(void *), void *arg);
    // void pthread_cleanup_pop(int execute);
    ```
  * `pthread_cleanup_push()`: Đẩy một hàm dọn dẹp (`routine`) và một đối số (`arg`) vào một stack nội bộ của luồng.
  * `pthread_cleanup_pop()`: Bỏ hàm dọn dẹp khỏi stack.

    * Nếu `execute` là `0`, hàm dọn dẹp bị bỏ đi mà không thực thi.
    * Nếu `execute` là `1`, hàm dọn dẹp sẽ được thực thi trước khi bị bỏ đi.
  * **Cách dùng:** Các cặp `pthread_cleanup_push()` và `pthread_cleanup_pop()` phải được đặt trong cùng một khối mã (scope) để đảm bảo cân bằng.
* **Liên hệ Embedded Linux:** Cực kỳ quan trọng để đảm bảo tính bền vững của ứng dụng. Nếu một luồng bị hủy giữa chừng khi đang giữ một mutex hoặc mở một thiết bị, clean-up handlers sẽ đảm bảo tài nguyên được giải phóng, ngăn chặn deadlock hoặc rò rỉ tài nguyên.

#### **5.5. Ví dụ (C++): `thread_cancellation.cpp` - Hủy một Luồng**

**C++**

```cpp
#include <iostream>
#include <string>
#include <pthread.h> // For pthreads API
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

// Hàm dọn dẹp (cleanup handler)
void cleanup_handler(void *arg) {
    std::string resource_name = static_cast<char*>(arg);
    AppLogger::log(AppLogger::INFO_L, "Cleanup Handler: Releasing resource '" + resource_name + "' for thread " + std::to_string(pthread_self()) + ".");
    // Trong ứng dụng thực tế: giải phóng mutex, đóng file, giải phóng bộ nhớ, v.v.
}

// Hàm mà luồng con sẽ thực thi và có thể bị hủy
void *cancellable_thread_function(void *arg) {
    AppLogger::log(AppLogger::INFO_L, "Cancellable Thread (ID: " + std::to_string(pthread_self()) + "): Started.");

    // 1. Đẩy một cleanup handler vào stack của luồng
    pthread_cleanup_push(cleanup_handler, (void*)"MyCriticalResource");

    // 2. Cấu hình trạng thái và loại hủy bỏ (mặc định là ENABLE và DEFERRED)
    // int res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    // if (res != 0) { AppLogger::log(AppLogger::ERROR_L, "pthread_setcancelstate failed: " + strerror(res)); }
    // res = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, nullptr);
    // if (res != 0) { AppLogger::log(AppLogger::ERROR_L, "pthread_setcanceltype failed: " + strerror(res)); }

    AppLogger::log(AppLogger::INFO_L, "Cancellable Thread: Loop started. Will check for cancellation every second.");
    for (int i = 0; i < 10; ++i) {
        AppLogger::log(AppLogger::INFO_L, "Cancellable Thread: Still running (step " + std::to_string(i) + ")...");
        sleep(1); // sleep() là một cancellation point
        pthread_testcancel(); // Chèn một cancellation point tường minh
    }

    AppLogger::log(AppLogger::INFO_L, "Cancellable Thread: Finished loop normally (should not happen if canceled).");

    // 3. Bỏ cleanup handler khỏi stack. Tham số 1 nghĩa là thực thi handler.
    pthread_cleanup_pop(1); 

    pthread_exit(nullptr); // Luồng tự kết thúc
}

int main() {
    pthread_t a_thread_id;
    void *thread_result_ptr;
    int res;

    AppLogger::log(AppLogger::INFO_L, "Main Thread: Starting Thread Cancellation Demonstration.");

    // Tạo luồng con
    res = pthread_create(&a_thread_id, nullptr, cancellable_thread_function, nullptr);
    if (res != 0) {
        AppLogger::log(AppLogger::CRITICAL_L, "Main Thread: Failed to create thread: " + strerror(res));
        return EXIT_FAILURE;
    }
    AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Cancellable thread created with ID: " + std::to_string(a_thread_id) + ".");

    AppLogger::log(AppLogger::INFO_L, "Main Thread: Sleeping for 3 seconds to let child run...");
    sleep(3); // Cho luồng con chạy một lát

    // Gửi yêu cầu hủy đến luồng con
    AppLogger::log(AppLogger::INFO_L, "Main Thread: Sending cancellation request to thread " + std::to_string(a_thread_id) + ".");
    res = pthread_cancel(a_thread_id);
    if (res != 0) {
        AppLogger::log(AppLogger::ERROR_L, "Main Thread: pthread_cancel failed: " + strerror(res));
    } else {
        AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Cancellation request sent.");
    }

    // Chờ luồng con kết thúc (để thu thập tài nguyên và biết trạng thái hủy)
    AppLogger::log(AppLogger::INFO_L, "Main Thread: Waiting for thread " + std::to_string(a_thread_id) + " to terminate...");
    res = pthread_join(a_thread_id, &thread_result_ptr);
    if (res != 0) {
        AppLogger::log(AppLogger::ERROR_L, "Main Thread: pthread_join failed: " + strerror(res));
    } else {
        if (thread_result_ptr == PTHREAD_CANCELED) { // Kiểm tra nếu luồng bị hủy
            AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Thread " + std::to_string(a_thread_id) + " was successfully CANCELED.");
        } else {
            AppLogger::log(AppLogger::WARNING_L, "Main Thread: Thread " + std::to_string(a_thread_id) + " terminated normally (unexpected).");
        }
    }

    AppLogger::log(AppLogger::INFO_L, "Main Thread: Exiting.");
    return EXIT_SUCCESS;
}
```

* **Cách Biên dịch:**
  **Bash**

  ```
  g++ thread_cancellation.cpp -o thread_cancellation -pthread
  ```
* **Cách Chạy:**
  **Bash**

  ```
  ./thread_cancellation
  ```
* **Phân tích Output:**

  * Bạn sẽ thấy luồng con chạy được vài bước (`Thread is still running (0)... (1)... (2)...`).
  * Sau 3 giây, luồng chính gửi yêu cầu hủy.
  * Luồng con sẽ nhận yêu cầu hủy tại `sleep(1)` hoặc `pthread_testcancel()` tiếp theo.
  * `cleanup_handler` sẽ được gọi.
  * Cuối cùng, luồng chính sẽ in ra thông báo rằng luồng con đã bị  **CANCELED** .

---

### **Câu hỏi Tự đánh giá Module 5 🤔**

1. Hàm `pthread_cancel()` có đảm bảo chấm dứt luồng ngay lập tức không? Giải thích lý do.
2. Phân biệt giữa `PTHREAD_CANCEL_ENABLE` và `PTHREAD_CANCEL_DISABLE`. Khi nào bạn sẽ sử dụng `PTHREAD_CANCEL_DISABLE`?
3. Phân biệt giữa `PTHREAD_CANCEL_DEFERRED` và `PTHREAD_CANCEL_ASYNCHRONOUS`. Loại nào được khuyến nghị sử dụng và tại sao?
4. "Điểm hủy bỏ" (cancellation point) là gì? Nêu hai ví dụ về các hàm là điểm hủy bỏ. Hàm nào bạn có thể chèn vào code để tạo một điểm hủy bỏ tường minh?
5. Giải thích mục đích của `pthread_cleanup_push()` và `pthread_cleanup_pop()`. Tại sao chúng lại quan trọng khi làm việc với việc hủy luồng?
6. Nếu một luồng bị hủy khi đang giữ một mutex, điều gì có thể xảy ra nếu bạn không sử dụng clean-up handlers?

---

### **Bài tập Thực hành Module 5 ✍️**

1. **Chương trình "Cancellable Worker":**
   * Viết một chương trình C++ (`cancellable_worker.cpp`) mà:
     * Luồng chính tạo một luồng con.
     * Luồng con sẽ đi vào một vòng lặp vô hạn, cứ mỗi 1 giây in ra "Worker is busy..."
     * Luồng chính sẽ cho phép người dùng nhập lệnh từ bàn phím. Nếu người dùng nhập "cancel", luồng chính sẽ gửi yêu cầu hủy đến luồng con.
     * Luồng con phải:
       * Cấu hình để chấp nhận hủy bỏ ở chế độ `DEFERRED`.
       * Sử dụng `pthread_testcancel()` ít nhất một lần trong vòng lặp của nó.
       * Có một `cleanup_handler` để in ra "Worker: Releasing resources..." khi bị hủy.
     * Luồng chính sẽ `pthread_join()` luồng con và in ra trạng thái kết thúc của nó (bị hủy hay thoát bình thường).
   * **Thử thách:**
     * Thử chạy chương trình và không nhập "cancel", để nó chạy bình thường.
     * Thử chạy chương trình và nhập "cancel" sau vài giây. Quan sát cách luồng con bị hủy.
2. **Bài tập "Resource Guard with Cancellation":**
   * Viết một chương trình C++ (`resource_guard.cpp`) có một biến toàn cục `int protected_resource_counter = 0;` và một `pthread_mutex_t resource_mutex;`.
   * Viết một hàm luồng `void* worker_thread(void* arg)`:
     * Trong vòng lặp, luồng này sẽ:
       * Khóa `resource_mutex`.
       * Tăng `protected_resource_counter`.
       * In ra "Worker: Counter = [value]".
       * Mở khóa `resource_mutex`.
       * Ngủ 1 giây.
       * Sử dụng `pthread_testcancel()`.
     * Đảm bảo luồng này có `cleanup_handler` để **mở khóa `resource_mutex`** nếu nó bị hủy khi đang giữ khóa.
   * Trong `main()`:
     * Khởi tạo `resource_mutex`.
     * Tạo luồng `worker_thread`.
     * Sau 3 giây, gửi yêu cầu hủy đến luồng `worker_thread`.
     * `pthread_join()` luồng và in ra trạng thái kết thúc.
     * Hủy `resource_mutex`.
   * **Mục tiêu:** Quan sát rằng `protected_resource_counter` vẫn được cập nhật đúng và `resource_mutex` được hủy thành công, chứng tỏ `cleanup_handler` đã hoạt động.
