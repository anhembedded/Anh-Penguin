# **Module 7: Luyện tập Tổng hợp & Ứng dụng 🧩**

#### **7.1. Ôn tập và Kết nối Kiến thức 🔗**

Hãy cùng điểm lại các chủ đề chính và mối liên hệ giữa chúng:

* **Module 1: Luồng là gì? (What Is a Thread?)**
  * **Kiến thức cốt lõi:** Định nghĩa luồng, so sánh với tiến trình (chia sẻ bộ nhớ, FD, signal handlers; stack riêng), ưu nhược điểm (hiệu suất, chi phí thấp vs. đồng bộ hóa phức tạp, lỗi khó tìm).
  * **Kết nối:** Nền tảng để hiểu tại sao và khi nào sử dụng luồng thay vì tiến trình.
* **Module 2: Tạo và Quản lý Luồng Cơ bản**
  * **Kiến thức cốt lõi:** Các hàm chính: `pthread_create()` (tạo luồng, nhận hàm `void* func(void*)`, đối số `void*`), `pthread_self()` (ID luồng hiện tại), `pthread_exit()` (luồng tự thoát, trả về `void*`), `pthread_join()` (chờ luồng con, thu thập kết quả).
  * **Kết nối:** Các API cơ bản để khởi tạo và quản lý vòng đời của luồng.
* **Module 3: Đồng bộ hóa Luồng (Synchronization)**
  * **Kiến thức cốt lõi:** Vấn đề điều kiện tranh chấp (race condition), vùng găng (critical section). Các cơ chế: **Mutexes** (`pthread_mutex_init/lock/unlock/destroy` - độc quyền tương hỗ), **Semaphores** (`sem_init/wait/post/destroy` - đếm, kiểm soát tài nguyên, đồng bộ nhà sản xuất-người tiêu dùng).
  * **Kết nối:** Giải quyết thách thức lớn nhất của đa luồng, đảm bảo tính toàn vẹn dữ liệu.
* **Module 4: Thuộc tính Luồng (Thread Attributes)**
  * **Kiến thức cốt lõi:** Đối tượng `pthread_attr_t` (`init/destroy`). Các thuộc tính: **Trạng thái tách rời** (`PTHREAD_CREATE_JOINABLE` vs. `PTHREAD_CREATE_DETACHED`, `pthread_detach()`), **Lập lịch** (`SCHED_OTHER`, `SCHED_FIFO`, `SCHED_RR`, `sched_param`, `sched_get_priority_max/min`).
  * **Kết nối:** Tinh chỉnh hành vi của luồng để tối ưu hiệu suất, quản lý tài nguyên và đáp ứng yêu cầu thời gian thực.
* **Module 5: Hủy một Luồng (Canceling a Thread)**
  * **Kiến thức cốt lõi:** `pthread_cancel()` (gửi yêu cầu hủy), kiểm soát hành vi hủy (`pthread_setcancelstate/type` - `ENABLE/DISABLE`, `DEFERRED/ASYNCHRONOUS`), điểm hủy bỏ (`cancellation points`, `pthread_testcancel()`), Clean-up Handlers (`pthread_cleanup_push/pop`).
  * **Kết nối:** Quản lý vòng đời của luồng một cách duyên dáng, đảm bảo tài nguyên được giải phóng ngay cả khi luồng bị dừng đột ngột.
* **Module 6: Làm việc với nhiều Luồng (Threads in Abundance)**
  * **Kiến thức cốt lõi:** Thách thức khi có nhiều luồng (overhead, context switching, tiêu thụ stack), vấn đề truyền đối số (`&i` trong vòng lặp), giải pháp (truyền giá trị, cấp phát động). Khái niệm Thread Pool (nhóm luồng).
  * **Kết nối:** Nâng cao kỹ năng thiết kế hệ thống đa luồng lớn, tránh các lỗi phổ biến và tối ưu hóa tài nguyên.

**Mối liên hệ tổng thể trong lập trình hệ thống nhúng:**

Trong môi trường nhúng, các khái niệm này thường được kết hợp để xây dựng các hệ thống mạnh mẽ và bền vững:

* **Phân chia tác vụ:** Sử dụng luồng để chia nhỏ công việc (đọc cảm biến, xử lý dữ liệu, giao tiếp mạng, ghi log) thành các tác vụ song song.
* **Đảm bảo thời gian thực:** Áp dụng thuộc tính lập lịch thời gian thực cho các luồng quan trọng.
* **Quản lý tài nguyên hiệu quả:** Sử dụng luồng tách rời cho các tác vụ nền không cần `join`, kiểm soát kích thước stack, và sử dụng Thread Pool.
* **Độ tin cậy và phục hồi:** Đồng bộ hóa chặt chẽ dữ liệu chia sẻ, xử lý việc hủy luồng duyên dáng để tránh rò rỉ tài nguyên.
* **Gỡ lỗi:** Sử dụng GDB và Valgrind (đặc biệt là Helgrind/DRD) để tìm các lỗi đồng bộ hóa và bộ nhớ khó tái hiện.

---

#### **7.2. Câu hỏi Tổng hợp và Tình huống ❓**

1. Tình huống (Hệ thống Thu thập và Xử lý Dữ liệu Cảm biến):
   Bạn đang phát triển một ứng dụng cho một thiết bị nhúng Linux để thu thập dữ liệu từ cảm biến, xử lý dữ liệu đó, và ghi kết quả vào một file log. Dữ liệu cảm biến đến liên tục.
   * **Thiết kế Luồng:** Bạn sẽ thiết kế bao nhiêu luồng và mỗi luồng sẽ đảm nhiệm vai trò gì?
   * **Đồng bộ hóa:** Làm thế nào bạn sẽ đồng bộ hóa việc truyền dữ liệu từ luồng đọc cảm biến đến luồng xử lý, và từ luồng xử lý đến luồng ghi log? Hãy đề xuất cơ chế cụ thể (Mutex, Semaphore, Condition Variable) và giải thích lý do.
   * **Vòng đời Luồng:** Luồng ghi log nên được tạo ở trạng thái `joinable` hay `detached`? Tại sao?
   * **Tắt máy duyên dáng:** Làm thế nào để đảm bảo tất cả các luồng đều dừng lại một cách duyên dáng khi ứng dụng nhận được tín hiệu `SIGTERM`?
2. **Phân biệt và Ứng dụng:**
   * Khi nào bạn sẽ ưu tiên sử dụng `pthread_mutex_t` thay vì `sem_t` (binary semaphore) để bảo vệ một vùng găng?
   * Giải thích sự khác biệt về hành vi của một luồng khi nó nhận yêu cầu hủy nếu nó ở trạng thái `PTHREAD_CANCEL_DEFERRED` so với `PTHREAD_CANCEL_ASYNCHRONOUS`. Loại nào an toàn hơn và tại sao?
   * Bạn có một tác vụ nền cần chạy liên tục nhưng không được phép chiếm dụng CPU quá mức. Bạn sẽ đặt chính sách lập lịch nào cho luồng này? Nếu đó là tác vụ quan trọng cần phản hồi nhanh, bạn sẽ chọn chính sách nào và cần quyền gì?
3. **Xử lý Lỗi và Tối ưu:**
   * Bạn phát hiện ra rằng ứng dụng đa luồng của mình thỉnh thoảng bị treo hoặc cho ra kết quả không chính xác. Bạn nghi ngờ có lỗi điều kiện tranh chấp. Bạn sẽ sử dụng công cụ nào (từ giáo trình Debugging) để xác nhận và khoanh vùng lỗi này?
   * Nếu ứng dụng của bạn tạo ra hàng trăm luồng, bạn có thể gặp phải vấn đề gì về hiệu suất và tài nguyên? Giải pháp kiến trúc nào có thể giúp giảm thiểu những vấn đề này?
   * Mô tả một lỗi phổ biến khi truyền đối số cho nhiều luồng trong một vòng lặp và cách khắc phục nó một cách an toàn.

---

#### **7.3. Bài tập Thực hành Tổng hợp ✍️**

**Bài tập: Hệ thống Xử lý Dữ liệu Cảm biến Đa luồng (Multi-threaded Sensor Data Processing System)**

Bạn sẽ xây dựng một ứng dụng C++ mô phỏng việc đọc, xử lý và ghi log dữ liệu cảm biến bằng cách sử dụng nhiều luồng và các cơ chế đồng bộ hóa.

**Yêu cầu chương trình `sensor_processor.cpp`:**

1. **Cấu trúc Dữ liệu:**
   * Định nghĩa `struct SensorData { int id; double value; time_t timestamp; };`
   * Định nghĩa `struct ProcessedData { int id; double original_value; double processed_value; time_t processed_timestamp; };`
2. **Hàng đợi Dữ liệu Chia sẻ:**
   * Sử dụng `std::vector<SensorData>` làm `raw_data_queue` (hàng đợi dữ liệu thô).
   * Sử dụng `std::vector<ProcessedData>` làm `processed_data_queue` (hàng đợi dữ liệu đã xử lý).
   * Sử dụng **`pthread_mutex_t`** để bảo vệ truy cập vào mỗi hàng đợi.
   * Sử dụng **`pthread_cond_t`** (Condition Variables) để đồng bộ hóa nhà sản xuất-người tiêu dùng cho cả hai hàng đợi (ví dụ: `data_available_cond`, `space_available_cond`).
3. **Luồng Đọc Cảm biến (Sensor Reader Thread):**
   * Là một luồng riêng biệt.
   * Cứ mỗi `X` mili giây (tham số dòng lệnh), tạo một `SensorData` giả định (ví dụ: `value = sin(time)`).
   * Đặt dữ liệu vào `raw_data_queue`.
   * Sử dụng mutex và condition variable để đồng bộ hóa: chờ nếu `raw_data_queue` đầy, báo hiệu khi có dữ liệu mới.
   * Có thể được cấu hình để chạy với chính sách lập lịch `SCHED_FIFO` và độ ưu tiên cao (tham số dòng lệnh, cần `sudo`).
4. **Nhóm Luồng Xử lý Dữ liệu (Data Processor Thread Pool):**
   * Tạo `N` luồng worker (tham số dòng lệnh, ví dụ: `N=4`).
   * Mỗi luồng worker sẽ:
     * Lấy một `SensorData` từ `raw_data_queue`.
     * Sử dụng mutex và condition variable để đồng bộ hóa: chờ nếu `raw_data_queue` trống, báo hiệu khi có chỗ trống.
     * Mô phỏng xử lý dữ liệu (ví dụ: `processed_value = original_value * original_value`, `sleep` ngắn 50ms).
     * Đặt `ProcessedData` vào `processed_data_queue`.
     * Sử dụng mutex và condition variable để đồng bộ hóa với luồng ghi log: chờ nếu `processed_data_queue` đầy, báo hiệu khi có dữ liệu mới.
5. **Luồng Ghi Log (Logger Thread):**
   * Là một luồng riêng biệt, được tạo ở trạng thái  **`PTHREAD_CREATE_DETACHED`** .
   * Lấy `ProcessedData` từ `processed_data_queue`.
   * Sử dụng mutex và condition variable để đồng bộ hóa: chờ nếu `processed_data_queue` trống, báo hiệu khi có chỗ trống.
   * Ghi dữ liệu đã xử lý vào file log (`sensor_data.log`) với định dạng `[YYYY-MM-DD HH:MM:SS] ID: <id>, Original: <value>, Processed: <processed_value>`.
   * Đảm bảo dữ liệu được flush xuống đĩa ngay lập tức.
6. **Luồng Chính (Main Thread):**
   * Xử lý tham số dòng lệnh (`getopt_long`):
     * `-i <interval_ms>`: Khoảng thời gian đọc cảm biến.
     * `-w <num_workers>`: Số lượng luồng xử lý.
     * `-l <log_file>`: Tên file log.
     * `-p <priority>`: Độ ưu tiên (nice value) cho luồng chính (và các luồng khác nếu không được đặt riêng).
     * `--rt-reader`: Đặt luồng đọc cảm biến thành `SCHED_FIFO` với ưu tiên cao nhất.
   * Khởi tạo tất cả các mutex, condition variables.
   * Tạo luồng đọc cảm biến, các luồng xử lý (worker pool), và luồng ghi log.
   * Xử lý tín hiệu `SIGTERM` để yêu cầu tất cả các luồng con thoát duyên dáng (sử dụng cờ toàn cục và `pthread_cancel()`).
   * `pthread_join()` các luồng cần `join()` (reader, worker).
   * Dọn dẹp tất cả các tài nguyên (mutex, condition variables).

**Các Module kiến thức chính được sử dụng:**

* **POSIX Threads:** `pthread_create`, `pthread_join`, `pthread_exit`, `pthread_self`, `pthread_detach`.
* **Đồng bộ hóa:** `pthread_mutex_t`, `pthread_mutex_init/lock/unlock/destroy`, `pthread_cond_t`, `pthread_cond_init/wait/signal/broadcast/destroy`.
* **Thuộc tính Luồng:** `pthread_attr_t`, `pthread_attr_init/destroy/setdetachstate/setschedpolicy/setschedparam`.
* **Hủy Luồng:** `pthread_cancel`, `pthread_testcancel`, `pthread_cleanup_push/pop`.
* **Quản lý Tiến trình:** `getpid()` (để ghi log).
* **File I/O:** `open()`, `write()` hoặc `std::ofstream` (cho file log).
* **Logging:** `syslog()` hoặc ghi vào file log tùy chỉnh.
* **Command-line Arguments:** `getopt_long()`.
* **Error Handling:** `errno`, `strerror()`.
* **Thời gian:** `time()`, `localtime()`, `strftime()`.
* **Lập lịch:** `sched_get_priority_max/min`.

**Gợi ý `CMakeLists.txt`:**

**CMake**

```
cmake_minimum_required(VERSION 3.17)
project(SensorProcessor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(sensor_processor src/sensor_processor.cpp)
target_link_libraries(sensor_processor PRIVATE pthread) # Cần liên kết với thư viện pthreads
target_compile_options(sensor_processor PRIVATE -D_GNU_SOURCE) # Nếu dùng các GNU extensions
```

**Quy trình Thực hiện:**

1. **Thiết kế:** Phác thảo chi tiết các hàm, cấu trúc dữ liệu, và luồng điều khiển.
2. **Viết code từng phần:**
   * Bắt đầu với các hàm đồng bộ hóa (mutex, condition variables) và hàng đợi chia sẻ.
   * Viết luồng Reader.
   * Viết luồng Processor (worker).
   * Viết luồng Logger.
   * Viết hàm `main` để khởi tạo và quản lý.
3. **Biên dịch và Chạy:**
   * `cmake -S . -B build && cmake --build build`
   * `./bin/sensor_processor -i 1000 -w 4 -l /tmp/sensor_log.txt --rt-reader`
4. **Kiểm tra và Gỡ lỗi:**
   * Sử dụng `tail -f /tmp/sensor_log.txt` để xem log.
   * Sử dụng `htop` để quan sát CPU usage và trạng thái luồng (đặc biệt là luồng RT).
   * **Sử dụng `Valgrind --tool=helgrind` hoặc `--tool=drd`** để phát hiện các lỗi điều kiện tranh chấp hoặc deadlock.
   * Sử dụng `GDB` để từng bước thực thi và kiểm tra trạng thái khi gặp lỗi.
   * Kiểm tra việc tắt máy duyên dáng bằng `Ctrl+C` hoặc `kill PID`.
