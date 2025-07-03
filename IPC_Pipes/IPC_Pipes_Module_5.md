# **Module 5: Luyện tập Tổng hợp & Ứng dụng 🧩**

#### **5.1. Ôn tập và Kết nối Kiến thức 🔗**

Hãy cùng điểm lại các chủ đề chính và mối liên hệ giữa chúng trong bối cảnh IPC qua Pipes:

* **Module 1: Pipe là gì? và `popen()`/`pclose()`**
  * **Kiến thức cốt lõi:** Định nghĩa pipe (kênh một chiều FIFO), `popen()` (chạy lệnh qua shell và giao tiếp I/O), `pclose()` (đóng pipe và chờ lệnh).
  * **Kết nối:** Cung cấp cách đơn giản nhất để giao tiếp với các lệnh shell, nhưng có overhead và ít kiểm soát.
* **Module 2: System Call `pipe()` và `dup()`/`dup2()`**
  * **Kiến thức cốt lõi:** `pipe()` (tạo pipe không tên cấp thấp, trả về File Descriptors), `fork()` (để pipe qua các tiến trình cha-con), `dup()`/`dup2()` (chuyển hướng các File Descriptor chuẩn như `stdin`/`stdout` sang pipe).
  * **Kết nối:** Cho phép xây dựng các pipeline xử lý dữ liệu phức tạp giữa các tiến trình liên quan một cách hiệu quả và có kiểm soát, tương tự như `cmd1 | cmd2` trong shell.
* **Module 3: Named Pipes (FIFOs)**
  * **Kiến thức cốt lõi:** FIFOs (pipes có tên, tồn tại trên filesystem), `mkfifo()` (tạo FIFO), `open()`/`read()`/`write()` (truy cập FIFO), `O_NONBLOCK` (chế độ không chặn), `PIPE_BUF` (tính nguyên tử khi ghi).
  * **Kết nối:** Mở rộng khả năng của pipe để giao tiếp giữa các tiến trình  **không liên quan** , cung cấp đồng bộ hóa tự động khi mở.
* **Module 4: Ứng dụng Client/Server với FIFOs**
  * **Kiến thức cốt lõi:** Mô hình Client/Server sử dụng FIFOs (một Server FIFO cho yêu cầu, mỗi Client một Client FIFO cho phản hồi), giao diện tin nhắn có cấu trúc, xử lý race condition (`O_RDWR` cho Client FIFO).
  * **Kết nối:** Ứng dụng thực tế của FIFOs để xây dựng các hệ thống phân tán cục bộ.

**Mối liên hệ tổng thể trong lập trình hệ thống nhúng:**

IPC qua Pipes là nền tảng để xây dựng các kiến trúc phần mềm nhúng dạng mô-đun, nơi các thành phần (tiến trình) khác nhau cần trao đổi dữ liệu:

* **Phân chia công việc:** Tách các chức năng thành các tiến trình riêng biệt và dùng Pipes để liên kết chúng.
* **Tiết kiệm tài nguyên:** Pipes/FIFOs thường nhẹ hơn Sockets cho giao tiếp cục bộ.
* **Đồng bộ hóa:** Khả năng chặn tự động của Pipes/FIFOs giúp giảm thiểu việc "busy-wait" và tiết kiệm chu kỳ CPU.
* **Dễ tích hợp:** FIFOs có thể được truy cập như file thông thường, cho phép tích hợp với các script shell hoặc các tiện ích khác trên thiết bị nhúng.

---

#### **5.2. Câu hỏi Tổng hợp và Tình huống ❓**

1. Tình huống (Hệ thống Xử lý File Báo cáo Tự động):
   Bạn cần thiết kế một hệ thống trên một thiết bị nhúng Linux để tự động xử lý các file báo cáo.

   * **Bước 1: Thu thập:** Một tiến trình `collector` liên tục quét một thư mục nhất định và khi tìm thấy file `.raw` mới, nó sẽ đọc nội dung file đó.
   * **Bước 2: Chuyển đổi:** Dữ liệu thô từ `collector` cần được chuyển đổi sang định dạng `.csv` bởi một tiến trình `converter`.
   * **Bước 3: Lưu trữ:** Dữ liệu `.csv` cần được ghi vào một cơ sở dữ liệu `dbm` (hoặc một file log chung) bởi một tiến trình `storer`.
   * **Yêu cầu:** Các tiến trình này chạy độc lập và giao tiếp bằng cơ chế IPC đã học.

   Bạn sẽ sử dụng loại Pipe nào (unnamed hay named) và các hàm nào (`pipe()`, `popen()`, `mkfifo()`, `dup2()`, `execvp()`, `read()`, `write()`, `close()`) để kết nối `collector` với `converter`, và `converter` với `storer`? Hãy mô tả luồng dữ liệu và cách các tiến trình sẽ được khởi chạy/kết nối.
2. **Phân biệt và Ứng dụng:**

   * Giải thích sự khác biệt cơ bản về API và trường hợp sử dụng giữa `popen()` và `mkfifo()`. Khi nào bạn sẽ chọn mỗi hàm?
   * Mô tả cách thức một server sử dụng FIFOs có thể xử lý đồng thời nhiều client, bao gồm cả việc gửi phản hồi lại cho từng client riêng biệt.
   * Nếu một tiến trình A dùng `open("my_fifo", O_RDONLY)` và tiến trình B dùng `open("my_fifo", O_WRONLY)`, điều gì sẽ xảy ra nếu tiến trình A chạy trước B? Điều gì xảy ra nếu B chạy trước A?
   * Trong môi trường RTOS như FreeRTOS, bạn sẽ sử dụng cơ chế nào để đạt được chức năng tương tự như việc truyền dữ liệu qua pipe giữa hai task?
   * Bạn có một chương trình C++ cần đọc các bản ghi từ một file `input.data` và chuyển đổi chúng, sau đó gửi kết quả cho một chương trình C++ khác để xử lý tiếp. Bạn muốn làm điều này theo phong cách pipeline. Hãy đề xuất cách triển khai sử dụng `pipe()` và `dup2()`.
3. **Thách thức và Giải pháp:**

   * Một ứng dụng Client/Server sử dụng FIFOs đang gặp vấn đề "race condition" khi client cố gắng nhận phản hồi. Hãy mô tả vấn đề này và cách bạn đã giải quyết nó trong Module 4.
   * Bạn cần đảm bảo rằng các gói tin nhỏ được ghi vào FIFO bởi nhiều writer khác nhau không bị lẫn lộn. Bạn sẽ sử dụng kiến thức nào về `PIPE_BUF` để đảm bảo tính nguyên tử này?

---

#### **5.3. Bài tập Thực hành Tổng hợp ✍️**

**Bài tập: Hệ thống "Text Processing Pipeline"**

Xây dựng một hệ thống xử lý văn bản dạng pipeline sử dụng các tiến trình riêng biệt và giao tiếp qua Pipes/FIFOs.

**Yêu cầu chương trình:**

1. **Chương trình `producer.cpp`:**
   * Nhận một tham số dòng lệnh: `<input_file>`.
   * Đọc nội dung của `<input_file>` từng dòng một.
   * Mỗi dòng đọc được, ghi nó ra `stdout`.
   * Sau khi đọc hết file, thoát.
2. **Chương trình `uppercase_converter.cpp`:**
   * Đọc từng dòng từ `stdin`.
   * Chuyển đổi toàn bộ dòng đó thành chữ hoa.
   * Ghi dòng đã chuyển đổi ra `stdout`.
   * Sau khi đọc hết `stdin` (nhận EOF), thoát.
3. **Chương trình `word_counter.cpp`:**
   * Đọc từng dòng từ `stdin`.
   * Đếm số từ trong mỗi dòng.
   * Ghi ra `stdout` dòng đó và số từ của nó (ví dụ: `[số từ] <dòng chữ hoa>`).
   * Sau khi đọc hết `stdin`, thoát.
4. **Chương trình `pipeline_manager.cpp`:**
   * Chương trình chính nhận hai tham số dòng lệnh: `<input_file>` và `<output_file>`.
   * Nó sẽ thiết lập một pipeline gồm 3 tiến trình:
     producer.cpp <input_file> | uppercase_converter.cpp | word_counter.cpp > <output_file>
   * **Thực hiện:**
     * Tạo 2 unnamed pipes.
     * `fork()` 3 lần để tạo 3 tiến trình con.
     * Trong mỗi tiến trình con, sử dụng `dup2()` để chuyển hướng `stdin`/`stdout` phù hợp và sau đó `execvp()` chương trình tương ứng.
     * Tiến trình cha sẽ `waitpid()` cho tất cả các tiến trình con hoàn thành và kiểm tra mã thoát của chúng.
     * Đảm bảo đóng tất cả các File Descriptor không cần thiết trong mỗi tiến trình (cha và con) sau khi `fork()` và `dup2()`.
   * **Mục tiêu:** Đọc một file văn bản đầu vào, chuyển đổi nó thành chữ hoa, đếm số từ, và ghi kết quả cuối cùng vào một file đầu ra.

**Các Module kiến thức chính được sử dụng:**

* **Process Management:** `fork()`, `execvp()`, `getpid()`.
* **Child Process Management:** `waitpid()`.
* **File I/O:** `open()`, `read()`, `write()`, `close()`.
* **Pipes:** `pipe()`, `dup()`, `dup2()`.
* **Command-line Arguments:** `argc`, `argv`.
* **Error Handling:** `errno`, `strerror()`, `perror()`.
* **Chuỗi:** `strlen()`, `strcpy()`, `strtok()` (hoặc `std::istringstream` trong C++).

**Gợi ý `CMakeLists.txt`:**

**CMake**

```
cmake_minimum_required(VERSION 3.17)
project(TextProcessorPipeline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Các executable riêng lẻ
add_executable(producer src/producer.cpp)
add_executable(uppercase_converter src/uppercase_converter.cpp)
add_executable(word_counter src/word_counter.cpp)
add_executable(pipeline_manager src/pipeline_manager.cpp) # Chương trình điều khiển chính

# Đảm bảo liên kết với thư viện chuẩn C++ (nếu dùng các tính năng C++11 trở lên)
target_link_libraries(producer PRIVATE -lstdc++)
target_link_libraries(uppercase_converter PRIVATE -lstdc++)
target_link_libraries(word_counter PRIVATE -lstdc++)
target_link_libraries(pipeline_manager PRIVATE -lstdc++)
```

**Quy trình Thực hiện:**

1. **Viết code từng chương trình nhỏ:** `producer.cpp`, `uppercase_converter.cpp`, `word_counter.cpp` và kiểm tra chúng hoạt động độc lập (ví dụ: `cat input.txt | ./uppercase_converter`).
2. **Viết `pipeline_manager.cpp`:** Tập trung vào việc thiết lập các pipe và `fork()`/`exec()` các tiến trình con.
3. **Biên dịch:** `cmake -S . -B build && cmake --build build`.
4. **Kiểm tra:** Tạo một file `input.txt` với vài dòng văn bản hỗn hợp (chữ hoa, chữ thường, số, khoảng trắng).
   * `cat input.txt`
   * `./bin/pipeline_manager input.txt output.txt`
   * `cat output.txt` (Kiểm tra xem output có đúng định dạng và nội dung mong muốn không).
5. **Gỡ lỗi:** Sử dụng `GDB` và `Valgrind` nếu có lỗi. Đặc biệt chú ý đến các lỗi `errno` liên quan đến `pipe()` và `dup2()`.

---

Đây là một bài tập lớn và phức tạp, kết hợp rất nhiều kiến thức bạn đã học. Nó sẽ là một thử thách thực sự và cũng là cơ hội tuyệt vời để bạn củng cố và ứng dụng các kỹ năng lập trình IPC qua Pipes vào một dự án thực tế.

Chúc mừng bạn đã hoàn thành giáo trình "Giao tiếp Liên Tiến trình: Pipes"! Hãy dành thời gian để thực hiện bài tập này một cách kỹ lưỡng. Khi bạn đã hoàn thành, hãy cho tôi biết nhé!
