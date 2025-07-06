# **Module 2: Tạo và Quản lý Luồng Cơ bản 🚀**

#### **2.1. Chuẩn bị Môi trường Lập trình Pthreads 🛠️**

* **Lý thuyết:** Để làm việc với POSIX Threads trong C/C++, bạn cần:
  1. **Bao gồm Header File:** `#include <pthread.h>` để truy cập các khai báo hàm và kiểu dữ liệu của Pthreads.
  2. **Định nghĩa Macro `_REENTRANT` (thường không cần thiết với các trình biên dịch hiện đại):** Trong các hệ thống UNIX/Linux cũ hơn, bạn có thể cần `#define _REENTRANT` trước khi bao gồm các header file. Macro này yêu cầu thư viện C cung cấp các phiên bản "an toàn luồng" (thread-safe) của một số hàm (ví dụ: `errno` trở thành biến cục bộ cho mỗi luồng, các hàm `stdio` có thể xử lý đệm riêng cho từng luồng). Với các trình biên dịch và thư viện C hiện đại (như GCC và Glibc trên Linux), hành vi này thường là mặc định khi bạn liên kết với thư viện Pthreads, nên bạn không cần định nghĩa tường minh `_REENTRANT`.
  3. **Liên kết với Thư viện Pthreads:** Khi biên dịch chương trình, bạn phải liên kết (link) với thư viện Pthreads bằng cờ `-pthread` (hoặc `-lpthread`).
     **Bash**

     ```
     g++ your_program.cpp -o your_program -pthread
     ```
* **Liên hệ Embedded Linux:** Việc này là bắt buộc để bạn có thể viết và biên dịch bất kỳ ứng dụng đa luồng nào trên thiết bị nhúng.

#### **2.2. Hàm Luồng (Thread Function) 🎯**

* **Lý thuyết:** Mỗi luồng mới bạn tạo ra phải có một hàm để nó bắt đầu thực thi. Hàm này phải tuân thủ một chữ ký (signature) cụ thể:
  **C++**

  ```cpp
  void *(*start_routine)(void *);
  ```

  * Nó nhận một con trỏ kiểu `void*` làm đối số duy nhất.
  * Nó trả về một con trỏ kiểu `void*`.
  * **Ý nghĩa:** Bạn có thể truyền bất kỳ kiểu dữ liệu nào (bằng cách ép kiểu sang `void*`) vào hàm luồng và trả về bất kỳ kiểu dữ liệu nào. Nếu không cần đối số hoặc giá trị trả về, bạn có thể truyền/trả về `NULL`.

#### **2.3. Tạo Luồng (`pthread_create()`) 🚀**

* **Lý thuyết:** Hàm **`pthread_create()`** là hàm chính để tạo một luồng mới.
  **C++**

  ```cpp
  #include <pthread.h>
  int pthread_create(pthread_t *thread,       // [OUT] Con trỏ tới biến pthread_t để lưu ID luồng mới
                     const pthread_attr_t *attr, // [IN] Thuộc tính của luồng (thường là NULL cho mặc định)
                     void *(*start_routine)(void *), // [IN] Con trỏ tới hàm luồng mà luồng mới sẽ chạy
                     void *arg);              // [IN] Đối số được truyền vào hàm luồng
  ```

  * `thread`: Sau khi hàm thành công, biến `pthread_t` mà con trỏ này trỏ tới sẽ chứa  **ID duy nhất của luồng mới** . Bạn sẽ sử dụng ID này để tham chiếu đến luồng (ví dụ: để chờ hoặc hủy nó).
  * `attr`: Cho phép bạn chỉ định các thuộc tính tùy chỉnh cho luồng (ví dụ: kích thước stack, chính sách lập lịch). Đối với các thuộc tính mặc định, truyền `NULL`. (Chúng ta sẽ đi sâu vào thuộc tính luồng trong Module 4).
  * `start_routine`: Con trỏ đến hàm mà luồng mới sẽ bắt đầu thực thi.
  * `arg`: Đối số duy nhất được truyền cho `start_routine`. Nếu bạn cần truyền nhiều đối số, hãy đóng gói chúng vào một `struct` và truyền con trỏ tới `struct` đó.
  * **Giá trị trả về:** `0` nếu thành công. Khác `0` (là một mã lỗi) nếu thất bại (ví dụ: `EAGAIN` nếu không đủ tài nguyên để tạo luồng). **Lưu ý:** Pthreads API không sử dụng `errno` cho các lỗi trả về của hàm `pthread_` mà trả về mã lỗi trực tiếp.
* **`pthread_self()`:** Hàm `pthread_self()` trả về ID của luồng hiện tại đang gọi nó. Hữu ích để phân biệt các luồng trong output log.

#### **2.4. Kết thúc Luồng (`pthread_exit()`) 🔚**

* **Lý thuyết:** Hàm **`pthread_exit()`** được một luồng gọi để **tự kết thúc** việc thực thi của nó.
  **C++**

  ```cpp
  #include <pthread.h>
  void pthread_exit(void *retval);
  ```

  * `retval`: Con trỏ tới giá trị mà luồng này sẽ trả về cho bất kỳ luồng nào `pthread_join()` nó.
  * **Quan trọng:** **Không bao giờ truyền con trỏ tới một biến cục bộ** của hàm luồng cho `retval`. Biến cục bộ nằm trên stack của luồng, và stack sẽ bị giải phóng khi luồng kết thúc, khiến con trỏ trở thành "dangling pointer" và gây ra lỗi nghiêm trọng nếu luồng khác cố gắng truy cập. Nếu cần trả về dữ liệu, cấp phát động nó hoặc trỏ đến dữ liệu toàn cục/chia sẻ.

#### **2.5. Chờ Luồng (`pthread_join()`) 🤝**

* **Lý thuyết:** Hàm **`pthread_join()`** được một luồng (thường là luồng chính hoặc một luồng quản lý khác) gọi để **chờ một luồng con kết thúc** và thu thập giá trị trả về của nó. Tương tự như `wait()` cho các tiến trình.
  **C++**

  ```
  #include <pthread.h>
  int pthread_join(pthread_t th,         // [IN] ID của luồng cần chờ
                   void **thread_return); // [OUT] Con trỏ tới nơi lưu trữ giá trị trả về từ hàm luồng
  ```

  * `th`: ID của luồng bạn muốn chờ (ID này được trả về bởi `pthread_create()`).
  * `thread_return`: Con trỏ tới một con trỏ `void*`. `pthread_join()` sẽ ghi giá trị `retval` từ `pthread_exit()` của luồng con vào vị trí này. Nếu không quan tâm đến giá trị trả về, truyền `NULL`.
  * **Giá trị trả về:** `0` nếu thành công. Khác `0` (là một mã lỗi) nếu thất bại (ví dụ: `ESRCH` nếu luồng không tồn tại, `EDEADLK` nếu deadlock).
  * **Quan trọng:** Nếu một luồng được tạo ở trạng thái "joinable" (mặc định) và không có luồng nào `pthread_join()` nó, khi nó kết thúc, tài nguyên của nó sẽ không được giải phóng hoàn toàn và nó sẽ trở thành một dạng "zombie" của luồng. Để tránh điều này, bạn phải `pthread_join()` các luồng joinable hoặc tạo chúng ở trạng thái "detached" (sẽ học trong Module 4).
* **Liên hệ Embedded Linux:**

  * `pthread_create()`, `pthread_exit()`, `pthread_join()` là các hàm cơ bản để triển khai các tác vụ song song trên thiết bị nhúng.
  * `pthread_join()` rất quan trọng để quản lý tài nguyên và đảm bảo các luồng con đã hoàn thành công việc của mình trước khi chương trình chính thoát.

#### **2.6. Ví dụ (C++): `first_thread_program.cpp` - Chương trình Luồng Cơ bản**

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

// Biến toàn cục được chia sẻ giữa các luồng
char global_message[] = "Hello from main thread!"; 

// Hàm mà luồng con sẽ thực thi
// Chữ ký phải là void* func(void*)
void *thread_function(void *arg) {
    // Ép kiểu đối số (void*) thành kiểu mong muốn
    char *received_message = static_cast<char*>(arg);
    AppLogger::log(AppLogger::INFO_L, "Child Thread (ID: " + std::to_string(pthread_self()) + "): Running.");
    AppLogger::log(AppLogger::INFO_L, "Child Thread: Received argument: '" + std::string(received_message) + "'");
  
    // Giả vờ làm việc
    AppLogger::log(AppLogger::INFO_L, "Child Thread: Working for 3 seconds...");
    sleep(3);

    // Sửa đổi biến toàn cục (hiển thị việc chia sẻ bộ nhớ)
    strcpy(global_message, "Bye from child thread!"); 
    AppLogger::log(AppLogger::INFO_L, "Child Thread: Modified global_message.");

    // Trả về một giá trị từ luồng
    static char *thread_return_value = (char*)"Thread completed successfully!"; 
    // Quan trọng: Phải là static hoặc cấp phát động, không phải biến cục bộ
    AppLogger::log(AppLogger::INFO_L, "Child Thread: Exiting with message.");
    pthread_exit(static_cast<void*>(thread_return_value)); // Luồng tự kết thúc và trả về giá trị
}

int main() {
    pthread_t a_thread_id; // Biến để lưu ID của luồng mới
    void *thread_result_ptr; // Con trỏ để lưu kết quả trả về từ luồng

    AppLogger::log(AppLogger::INFO_L, "Main Thread (ID: " + std::to_string(pthread_self()) + "): Starting.");
    AppLogger::log(AppLogger::INFO_L, "Main Thread: Initial global_message: '" + std::string(global_message) + "'");

    // Tạo một luồng mới
    // pthread_create(&a_thread_id,   // Địa chỉ biến lưu ID luồng
    //                nullptr,        // Thuộc tính luồng (NULL cho mặc định)
    //                thread_function, // Hàm mà luồng mới sẽ chạy
    //                static_cast<void*>(global_message)); // Đối số truyền vào hàm luồng
    int res = pthread_create(&a_thread_id, nullptr, thread_function, static_cast<void*>(global_message));
    if (res != 0) {
        AppLogger::log(AppLogger::CRITICAL_L, "Main Thread: Failed to create thread: " + std::string(strerror(res))); // Dùng strerror(res)
        return EXIT_FAILURE;
    }
    AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: New thread created with ID: " + std::to_string(a_thread_id) + ".");
    AppLogger::log(AppLogger::INFO_L, "Main Thread: Waiting for the new thread to finish...");

    // Chờ luồng con kết thúc và lấy giá trị trả về
    res = pthread_join(a_thread_id, &thread_result_ptr);
    if (res != 0) {
        AppLogger::log(AppLogger::ERROR_L, "Main Thread: Failed to join thread: " + std::string(strerror(res)));
    } else {
        AppLogger::log(AppLogger::SUCCESS_L, "Main Thread: Thread ID " + std::to_string(a_thread_id) + " has finished.");
        AppLogger::log(AppLogger::INFO_L, "Main Thread: Received result from thread: '" + std::string(static_cast<char*>(thread_result_ptr)) + "'");
    }

    AppLogger::log(AppLogger::INFO_L, "Main Thread: Final global_message: '" + std::string(global_message) + "'");
    AppLogger::log(AppLogger::INFO_L, "Main Thread: Exiting.");

    return EXIT_SUCCESS;
}
```

* **Cách Biên dịch:**
  **Bash**

  ```
  g++ first_thread_program.cpp -o first_thread_program -pthread -lstdc++
  ```

  * **`-pthread`** : Rất quan trọng để liên kết với thư viện Pthreads.
  * **`-lstdc++`** : Để liên kết với thư viện chuẩn C++ cho các tính năng như `std::string`, `std::cout`.
* **Cách Chạy:**
  **Bash**

  ```
  ./first_thread_program
  ```
* **Phân tích Output:**

  * Bạn sẽ thấy output từ cả hai luồng (main và child) được in xen kẽ.
  * Quan sát giá trị của `global_message` trong luồng chính **sau khi** `pthread_join()` trả về – nó đã được luồng con sửa đổi. Điều này minh họa việc chia sẻ bộ nhớ.
  * Giá trị trả về từ luồng con cũng được luồng chính thu thập.

---

### **Câu hỏi Tự đánh giá Module 2 🤔**

1. Để sử dụng POSIX Threads trong chương trình C++, bạn cần bao gồm header file nào và liên kết với thư viện nào khi biên dịch?
2. Hàm mà một luồng mới sẽ thực thi (`start_routine`) phải có chữ ký (signature) như thế nào? Tại sao?
3. Phân biệt `pthread_create()` và `fork()`. Chúng tạo ra các thực thể thực thi như thế nào và tài nguyên được chia sẻ/sao chép ra sao?
4. Khi nào bạn sẽ dùng `pthread_exit()`? Tại sao việc trả về con trỏ tới một biến cục bộ từ `pthread_exit()` lại là một lỗi nghiêm trọng?
5. Giải thích vai trò của `pthread_join()`. Điều gì xảy ra nếu bạn không gọi `pthread_join()` cho một luồng "joinable"?

---

### **Bài tập Thực hành Module 2 ✍️**

1. **Chương trình "Tính Tổng Đa Luồng":**
   * Viết một chương trình C++ (`multi_thread_sum.cpp`) mà:
     * Tạo một mảng số nguyên lớn (ví dụ: 100,000 phần tử) và khởi tạo các giá trị ngẫu nhiên.
     * Tạo 2 luồng con.
     * Luồng 1 sẽ tính tổng các phần tử từ đầu mảng đến giữa mảng.
     * Luồng 2 sẽ tính tổng các phần tử từ giữa mảng đến cuối mảng.
     * Mỗi luồng con sẽ trả về tổng cục bộ của nó thông qua `pthread_exit()`.
     * Luồng chính sẽ `pthread_join()` cả hai luồng con, thu thập các tổng cục bộ và in ra tổng cuối cùng của toàn bộ mảng.
   * **Thử thách:** Kiểm tra tổng cuối cùng bằng cách tính tổng toàn bộ mảng trong luồng chính và so sánh với tổng từ các luồng con.
2. **Chương trình "Kiểm tra ID Luồng":**
   * Viết một chương trình C++ (`thread_id_checker.cpp`) mà:
     * Trong `main()`, in ra ID của luồng chính bằng `pthread_self()`.
     * Tạo 3 luồng con trong một vòng lặp.
     * Mỗi luồng con sẽ in ra ID của chính nó và ID của luồng chính (sử dụng đối số được truyền vào hàm luồng).
     * Luồng chính sẽ `pthread_join()` tất cả các luồng con.
   * **Mục tiêu:** Quan sát xem ID của luồng chính có giống nhau trong tất cả các luồng con không (chúng phải giống nhau vì chúng cùng thuộc một tiến trình).
3. **Chương trình "Truyền nhiều đối số":**
   * Viết một chương trình C++ (`multi_arg_thread.cpp`) mà:
     * Định nghĩa một `struct ThreadArgs { int id; std::string message; };`.
     * Trong `main()`, tạo một đối tượng `ThreadArgs` và khởi tạo nó.
     * Cấp phát động đối tượng này và truyền con trỏ tới nó cho `pthread_create()`.
     * Hàm luồng sẽ nhận đối số, in ra các thành viên của `struct ThreadArgs` và sau đó giải phóng bộ nhớ đã cấp phát động cho `ThreadArgs`.
     * Luồng chính sẽ `pthread_join()` luồng con.
   * **Thử thách:** Đảm bảo bộ nhớ được cấp phát động cho `ThreadArgs` được giải phóng đúng cách (trong hàm luồng) để tránh rò rỉ bộ nhớ.



---

## 🚀 **Bước 1: Chuẩn bị môi trường Pthreads**

### ✅ Phải có:

| Thành phần             | Vai trò                                                           |
| ------------------------ | ------------------------------------------------------------------ |
| `#include <pthread.h>` | Header khai báo API luồng của POSIX                             |
| `-pthread`             | Cờ biên dịch & liên kết thư viện Pthreads (`libpthread`)  |
| `_REENTRANT`(optional) | Với compiler đời cũ, giúp libc hỗ trợ các hàm thread-safe |

💡 Trên GCC hiện đại (Linux nhúng, PC), không cần `#define _REENTRANT`, chỉ cần `-pthread`.

### 📦 Cách biên dịch:

```bash
g++ my_thread.cpp -o my_thread -pthread
```

---

## 🎯 **Bước 2: Viết hàm cho luồng mới**

Luồng cần một "điểm bắt đầu", giống như `main()` với tiến trình.

```cpp
void* my_thread_func(void* arg) {
    // xử lý ở đây...
    return NULL;
}
```

💡 Hàm luồng **phải** nhận `void*` và trả về `void*` → có thể truyền bất kỳ dữ liệu nào bằng cách ép kiểu.

---

## 👷‍♂️ **Bước 3: Tạo luồng bằng `pthread_create()`**

```cpp
pthread_t tid;

pthread_create(&tid, NULL, my_thread_func, NULL);
```

| Tham số           | Giải thích                                          |
| ------------------ | ----------------------------------------------------- |
| `&tid`           | Địa chỉ biến `pthread_t`lưu ID luồng          |
| `NULL`           | Không cấu hình đặc biệt cho thuộc tính luồng |
| `my_thread_func` | Hàm mà luồng mới sẽ chạy                        |
| `NULL`           | Đối số truyền cho hàm luồng                     |

💡 Truyền dữ liệu? Tạo struct rồi ép con trỏ sang `void*`!

---

## 📌 Bonus: Lấy ID của luồng hiện tại

```cpp
pthread_self(); // trả về pthread_t của luồng đang chạy
```

---

## 🔚 **Bước 4: Luồng kết thúc bằng `pthread_exit()`**

```cpp
void* my_thread_func(void* arg) {
    // xử lý
    pthread_exit(NULL); // hoặc truyền con trỏ trả về
}
```

🧨 **Lưu ý cực quan trọng:**

* Đừng bao giờ `return &biến_cục_bộ` trong luồng!
* Stack luồng sẽ biến mất → con trỏ sẽ trở thành "điểm chết" (dangling pointer)

→ Nếu muốn trả về dữ liệu: dùng `new`, `malloc`, hoặc dữ liệu tĩnh/toàn cục.

---

## 🤝 **Bước 5: Chờ luồng bằng `pthread_join()`**

Trong `main()` hoặc luồng khác:

```cpp
void* retval;
pthread_join(tid, &retval);
```

| Tham số    | Giải thích                               |
| ----------- | ------------------------------------------ |
| `tid`     | ID của luồng cần chờ                   |
| `&retval` | Biến nhận giá trị trả về của luồng |

💡 Nếu không cần trả giá trị → truyền `NULL`.

🔐 Nếu không `join()` luồng → luồng trở thành "zombie", không được thu hồi tài nguyên.

---

## 📦 Tổng hợp khung chương trình tối thiểu:

```cpp
#include <pthread.h>
#include <iostream>

void* my_thread_func(void* arg) {
    std::cout << "🧵 Hello from thread!\n";
    pthread_exit(NULL);
}

int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, my_thread_func, NULL);
    pthread_join(tid, NULL);
    std::cout << "✅ Thread finished.\n";
    return 0;
}
```

---
