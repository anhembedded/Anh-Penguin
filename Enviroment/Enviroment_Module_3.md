
### **Module 3: Thời gian và Ngày tháng (Time and Date) ⏰**

Trong lập trình Linux, việc làm việc với thời gian và ngày tháng là rất phổ biến. Bạn sẽ cần chúng để ghi dấu thời gian vào các file log, lên lịch các tác vụ chạy định kỳ, đo lường thời gian thực thi của chương trình (benchmarking), hoặc đơn giản là hiển thị thời gian hiện tại cho người dùng.

#### **3.1. Khái niệm `Epoch` và `time_t` 🕰️**

* **Lý thuyết:**

  * Hệ thống Unix/Linux sử dụng một điểm tham chiếu chung cho thời gian được gọi là  **Unix Epoch** . Epoch được định nghĩa là  **00:00:00 UTC (Coordinated Universal Time - Giờ phối hợp quốc tế) ngày 1 tháng 1 năm 1970** .
  * Tất cả các thời gian trong hệ thống Linux đều được đo bằng **số giây đã trôi qua kể từ Epoch** (không tính giây nhuận).
  * Kiểu dữ liệu **`time_t`** (`#include <time.h>`) là kiểu số nguyên được định nghĩa để lưu trữ giá trị thời gian này. Trên hầu hết các hệ thống Linux 64-bit hiện đại, `time_t` là một `long int` hoặc `long long int` để tránh sự cố "Year 2038 problem" (vấn đề năm 2038) xảy ra với `time_t` 32-bit.
* **Hàm `time()`:**

  * Hàm `time()` được dùng để lấy giá trị thời gian hiện tại dưới dạng số giây kể từ Epoch.
    **C++**

    ```
    #include <time.h> // For time_t, time
    time_t time(time_t *tloc);
    ```
  * **`tloc`** : Con trỏ tới một biến `time_t`. Nếu không phải `NULL`, hàm sẽ ghi giá trị thời gian trả về vào vị trí này.
  * **Giá trị trả về:** Số giây kể từ Epoch, hoặc `(time_t)-1` nếu thất bại.
* **Hàm `difftime()`:**

  * Đôi khi, bạn muốn tính khoảng thời gian giữa hai mốc thời gian. Hàm `difftime()` là cách di động nhất để làm điều này.
    **C++**

    ```
    #include <time.h> // For difftime
    double difftime(time_t time1, time_t time2);
    ```
  * Trả về `time1 - time2` dưới dạng số thực `double`, biểu thị số giây chênh lệch.
* **Liên hệ Embedded Linux:** Cực kỳ quan trọng cho các hệ thống nhúng cần:

  * Ghi dấu thời gian chính xác vào nhật ký (logs).
  * Đo lường thời gian thực thi của các tác vụ (performance benchmarking).
  * Đồng bộ hóa thời gian giữa các thiết bị hoặc với server NTP.
* **Ví dụ (C++): `time_ops.cpp` - Sử dụng `time()` và `difftime()`**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <chrono> // For std::this_thread::sleep_for
  #include <thread> // For std::this_thread
  #include <ctime>  // For time_t, time, difftime
  #include <cstdlib> // For EXIT_SUCCESS

  int main() {
      time_t start_time, current_time;

      std::cout << "INFO    : Program starting. Measuring time for 5 iterations." << std::endl;

      // Lấy thời gian bắt đầu
      start_time = time(nullptr); // nullptr thay cho (time_t*)0

      for (int i = 1; i <= 5; ++i) {
          current_time = time(nullptr);
          std::cout << "INFO    : Iteration " << i << ". The raw time (seconds since Epoch) is: " << current_time << std::endl;
          std::this_thread::sleep_for(std::chrono::seconds(2)); // Ngủ 2 giây
      }

      // Lấy thời gian kết thúc
      time_t end_time = time(nullptr);
      std::cout << "INFO    : Program finished. Calculating total elapsed time." << std::endl;

      // Tính thời gian chênh lệch
      double elapsed_seconds = difftime(end_time, start_time);
      std::cout << "SUCCESS : Total elapsed time: " << elapsed_seconds << " seconds." << std::endl;

      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ time_ops.cpp -o time_ops
    ./time_ops
    ```

#### **3.2. Chuyển đổi Thời gian sang Cấu trúc (`tm`) 📅**

* **Lý thuyết:** Giá trị `time_t` là số nguyên khó đọc. Để chuyển đổi thành các thành phần dễ hiểu hơn (năm, tháng, ngày, giờ, phút, giây), bạn cần sử dụng các hàm chuyển đổi sang cấu trúc `struct tm`.

  * **`struct tm`:** Định nghĩa trong `<time.h>`, chứa ít nhất các thành viên sau:

    * `int tm_sec`: Giây (0-61, cho phép giây nhuận).
    * `int tm_min`: Phút (0-59).
    * `int tm_hour`: Giờ (0-23).
    * `int tm_mday`: Ngày trong tháng (1-31).
    * `int tm_mon`: Tháng trong năm (0-11, 0 là tháng 1).
    * `int tm_year`: Số năm kể từ năm 1900.
    * `int tm_wday`: Ngày trong tuần (0-6, 0 là Chủ Nhật).
    * `int tm_yday`: Ngày trong năm (0-365).
    * `int tm_isdst`: Chỉ báo giờ tiết kiệm ánh sáng ban ngày (Daylight Saving Time - DST): dương nếu đang áp dụng, 0 nếu không, âm nếu không biết.
  * **`gmtime()`:** Chuyển đổi `time_t` sang `struct tm` theo  **giờ UTC (GMT)** .
    **C++**

    ```
    #include <time.h> // For gmtime, struct tm
    struct tm *gmtime(const time_t *timeval);
    ```
  * **`localtime()`:** Chuyển đổi `time_t` sang `struct tm` theo  **giờ địa phương (local time zone)** , có tính đến múi giờ và DST.
    **C++**

    ```
    #include <time.h> // For localtime
    struct tm *localtime(const time_t *timeval);
    ```
  * **Lưu ý:** Cả `gmtime()` và `localtime()` đều trả về con trỏ tới một cấu trúc `tm` được lưu trữ tĩnh nội bộ. Bạn không nên thay đổi cấu trúc này và nó có thể bị ghi đè bởi các lệnh gọi hàm tiếp theo. Nếu cần giữ lại, hãy sao chép nó.
* **Hàm `mktime()`:**

  * Chuyển đổi ngược từ `struct tm` sang `time_t`.
    **C++**

    ```
    #include <time.h> // For mktime
    time_t mktime(struct tm *timeptr);
    ```
  * Hữu ích khi bạn muốn tạo một mốc thời gian cụ thể từ các thành phần (năm, tháng, ngày).
* **Liên hệ Embedded Linux:** Rất quan trọng khi bạn cần hiển thị thời gian chính xác theo múi giờ địa phương hoặc UTC, hoặc khi cần tính toán các mốc thời gian phức tạp (ví dụ: giờ bật/tắt thiết bị theo lịch trình).
* **Ví dụ (C++): `time_struct_ops.cpp` - `gmtime()`, `localtime()`, `mktime()`**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <ctime>   // For time_t, time, struct tm, gmtime, localtime, mktime
  #include <cstdlib> // For EXIT_SUCCESS

  void print_tm_struct(const std::string& prefix, const struct tm* tm_ptr) {
      if (!tm_ptr) {
          std::cerr << prefix << " : Invalid tm pointer." << std::endl;
          return;
      }
      std::cout << prefix << ":" << std::endl;
      std::cout << "  Date: "
                << (tm_ptr->tm_year + 1900) << "/" // tm_year là số năm kể từ 1900
                << (tm_ptr->tm_mon + 1) << "/"   // tm_mon là 0-11
                << tm_ptr->tm_mday << std::endl;
      std::cout << "  Time: "
                << tm_ptr->tm_hour << ":"
                << tm_ptr->tm_min << ":"
                << tm_ptr->tm_sec << std::endl;
      std::cout << "  Day of week (0=Sun): " << tm_ptr->tm_wday << std::endl;
      std::cout << "  Day of year (0-365): " << tm_ptr->tm_yday << std::endl;
      std::cout << "  Is DST: " << tm_ptr->tm_isdst << std::endl;
  }

  int main() {
      time_t current_time = time(nullptr);
      struct tm *tm_ptr;

      std::cout << "INFO    : Raw time (seconds since Epoch): " << current_time << std::endl;

      // --- Sử dụng gmtime (UTC/GMT) ---
      tm_ptr = gmtime(&current_time);
      print_tm_struct("INFO    : gmtime (UTC)", tm_ptr);

      // --- Sử dụng localtime (Local Time Zone) ---
      tm_ptr = localtime(&current_time);
      print_tm_struct("INFO    : localtime (Local Time)", tm_ptr);

      // --- Sử dụng mktime (Chuyển đổi ngược từ tm sang time_t) ---
      struct tm my_custom_time_info = *tm_ptr; // Sao chép cấu trúc tm hiện tại
      my_custom_time_info.tm_hour = 8;        // Đặt giờ là 8 AM
      my_custom_time_info.tm_min = 30;        // Đặt phút là 30
      my_custom_time_info.tm_sec = 0;         // Đặt giây là 0

      std::cout << "\nINFO    : Attempting to convert custom tm struct to time_t (8:30:00 AM local time)." << std::endl;
      time_t custom_time_t = mktime(&my_custom_time_info);
      if (custom_time_t == (time_t)-1) {
          std::cerr << "ERROR   : mktime failed to convert custom time." << std::endl;
      } else {
          std::cout << "SUCCESS : Converted custom time_t: " << custom_time_t << std::endl;
          // Xác nhận lại bằng cách chuyển đổi ngược về tm
          print_tm_struct("INFO    : mktime result verified (localtime)", localtime(&custom_time_t));
      }

      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ time_struct_ops.cpp -o time_struct_ops
    ./time_struct_ops
    ```
  * **Phân tích:** Bạn sẽ thấy sự khác biệt giữa `gmtime` và `localtime` nếu múi giờ của bạn không phải UTC. `mktime` cho phép bạn tạo ra một `time_t` từ các thành phần ngày/giờ cụ thể.

#### **3.3. Định dạng và Phân tích Chuỗi Thời gian (`asctime`, `ctime`, `strftime`, `strptime`) 📝**

* **Lý thuyết:** Để hiển thị thời gian theo định dạng dễ đọc cho con người hoặc để phân tích cú pháp chuỗi thời gian, bạn có các hàm:

  * **`asctime()` và `ctime()` (Định dạng Chuỗi Cố định):**

    * `char *asctime(const struct tm *timeptr);`: Chuyển `struct tm` sang chuỗi định dạng cố định (ví dụ: "Sun Jun 6 12:30:34 1999\n").
    * `char *ctime(const time_t *timeval);`: Tương đương `asctime(localtime(timeval))`, chuyển `time_t` sang chuỗi địa phương có định dạng cố định.
    * **Lưu ý:** Trả về con trỏ tới bộ đệm tĩnh nội bộ, không an toàn cho luồng nếu gọi đồng thời.
  * **`strftime()` (Định dạng Chuỗi Tùy chỉnh - "printf" cho thời gian):**

    * Cung cấp kiểm soát chi tiết về định dạng chuỗi ngày/giờ.

    **C++**

    ```
    #include <time.h> // For strftime
    size_t strftime(char *s, size_t maxsize, const char *format, const struct tm *timeptr);
    ```

    * `s`: Buffer để lưu chuỗi kết quả.
    * `maxsize`: Kích thước tối đa của `s`.
    * `format`: Chuỗi định dạng chứa các **conversion specifiers** (ví dụ: `%Y` cho năm, `%m` cho tháng, `%H` cho giờ 24h, `%M` cho phút, `%S` cho giây).
    * `timeptr`: Con trỏ tới `struct tm` chứa dữ liệu thời gian.
    * **Giá trị trả về:** Số ký tự được ghi vào `s` (không bao gồm `\0`), hoặc `0` nếu lỗi.
  * **`strptime()` (Phân tích Chuỗi Thời gian - "scanf" cho thời gian):**

    * Chuyển đổi một chuỗi đại diện ngày/giờ thành `struct tm`.

    **C++**

    ```
    #include <time.h> // For strptime
    // #define _XOPEN_SOURCE // Có thể cần định nghĩa này trước #include <time.h> trên một số hệ thống
    char *strptime(const char *buf, const char *format, struct tm *timeptr);
    ```

    * `buf`: Chuỗi input chứa ngày/giờ.
    * `format`: Chuỗi định dạng để phân tích cú pháp (tương tự `strftime`).
    * `timeptr`: Con trỏ tới `struct tm` để lưu kết quả.
    * **Giá trị trả về:** Con trỏ tới ký tự theo sau ký tự cuối cùng đã được xử lý trong `buf` nếu thành công, hoặc `NULL` nếu không khớp.
    * **Lưu ý:** `strptime` linh hoạt hơn `strftime` trong việc chấp nhận các biến thể (ví dụ: tên viết tắt hoặc đầy đủ cho ngày/tháng).
* **Liên hệ Embedded Linux:**

  * `strftime()` là công cụ chính để tạo các dấu thời gian tùy chỉnh cho log files, hiển thị trên giao diện người dùng (ví dụ: màn hình LCD), hoặc định dạng dữ liệu gửi qua mạng.
  * `strptime()` hữu ích khi phân tích cú pháp các chuỗi thời gian nhận được từ người dùng hoặc từ các giao thức mạng, để chuyển chúng thành các giá trị thời gian mà chương trình có thể sử dụng.
* **Ví dụ (C++): `strftime_strptime_ops.cpp` - `strftime()` và `strptime()`**
  **C++**

  ```
  #include <iostream>
  #include <string>
  #include <ctime>   // For time_t, time, struct tm, localtime, strftime, strptime
  #include <cstdlib> // For EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring> // For strcpy
  // #define _XOPEN_SOURCE // Thường không cần trên các hệ thống Linux hiện đại, nhưng nếu báo lỗi strptime thì thêm vào

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
      time_t the_time = time(nullptr);
      struct tm *tm_ptr = localtime(&the_time); // Lấy thời gian địa phương

      char buf[256];
      char *result_ptr;

      // --- Sử dụng strftime để định dạng thời gian hiện tại ---
      // Định dạng: "Thứ Sáu 14 Tháng Sáu, 03:07 PM" (ví dụ)
      std::cout << "INFO    : Formatting current local time with strftime." << std::endl;
      size_t bytes_formatted = strftime(buf, sizeof(buf), "%A %d %B, %I:%M %p", tm_ptr);
      if (bytes_formatted == 0) {
          std::cerr << "ERROR   : strftime failed or buffer too small." << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : strftime gives: " << buf << std::endl;

      // --- Sử dụng strptime để phân tích cú pháp một chuỗi thời gian ---
      const char *date_string = "Sat 26 July 2003, 17:53 will do fine";
      struct tm parsed_time_info;

      // Khởi tạo struct tm về 0 để tránh các giá trị rác
      memset(&parsed_time_info, 0, sizeof(struct tm)); 

      std::cout << "\nINFO    : Calling strptime with input: '" << date_string << "'" << std::endl;
      // Định dạng: "%a %d %b %Y, %R"
      // %a: tên ngày trong tuần viết tắt (Sat)
      // %d: ngày trong tháng (26)
      // %b: tên tháng viết tắt (July)
      // %Y: năm (2003)
      // %R: giờ và phút (HH:MM) - tương đương %H:%M
      result_ptr = strptime(date_string, "%a %d %b %Y, %R", &parsed_time_info);

      if (result_ptr == nullptr) {
          std::cerr << "ERROR   : strptime failed to parse the string." << std::endl;
          return EXIT_FAILURE;
      }

      std::cout << "SUCCESS : strptime consumed up to: '" << result_ptr << "'" << std::endl;
      std::cout << "INFO    : strptime gives:" << std::endl;
      std::cout << "  Date: "
                << (parsed_time_info.tm_year % 100) << "/" // Năm (từ 1900)
                << (parsed_time_info.tm_mon + 1) << "/"    // Tháng (0-11)
                << parsed_time_info.tm_mday << std::endl;
      std::cout << "  Time: "
                << parsed_time_info.tm_hour << ":"
                << parsed_time_info.tm_min << std::endl;

      return EXIT_SUCCESS;
  }
  ```

  * **Cách biên dịch:**
    **Bash**

    ```
    g++ strftime_strptime_ops.cpp -o strftime_strptime_ops
    ./strftime_strptime_ops
    ```

---

### **Câu hỏi Tự đánh giá Module 3 🤔**

1. Giải thích ý nghĩa của "Unix Epoch". Giá trị `time_t` biểu thị điều gì?
2. Khi nào bạn nên sử dụng `gmtime()` thay vì `localtime()`?
3. Bạn muốn tính khoảng thời gian chính xác (đến phần giây) giữa hai sự kiện trong chương trình C++. Hàm nào là thích hợp nhất để làm điều này một cách di động?
4. Nêu hai lý do tại sao `strftime()` được ưa dùng hơn `asctime()` và `ctime()` để định dạng ngày/giờ.
5. Giả sử bạn có chuỗi "2025-07-02 21:30:00". Viết đoạn mã C++ sử dụng `strptime()` để phân tích chuỗi này thành một `struct tm`.

---

### **Bài tập Thực hành Module 3 ✍️**

1. **Chương trình Hẹn giờ đơn giản:**
   * Viết một chương trình C++ (`simple_timer.cpp`) mà:
     * Sử dụng `time()` để lấy thời gian bắt đầu.
     * Yêu cầu người dùng nhấn Enter để kết thúc hẹn giờ.
     * Khi người dùng nhấn Enter, lấy thời gian kết thúc.
     * Sử dụng `difftime()` để tính và in ra tổng thời gian đã trôi qua (bằng giây, với độ chính xác `double`).
     * **Thử thách:** In ra thời gian đã trôi qua dưới định dạng `HH:MM:SS` (sử dụng `strftime` với một `struct tm` tạm thời).
2. **Chương trình Log sự kiện với Dấu thời gian:**
   * Viết một chương trình C++ (`event_logger_time.cpp`) mà:
     * Tạo một file log `app_events.log`.
     * Mỗi khi chương trình chạy, ghi một dòng mới vào file log với định dạng: `[YYYY-MM-DD HH:MM:SS] Event description here.`
     * Dấu thời gian phải là thời gian địa phương.
     * Sử dụng `localtime()` và `strftime()` để tạo chuỗi thời gian.
     * Chương trình sẽ ghi 5 sự kiện mẫu, mỗi sự kiện cách nhau 1 giây.
     * Đảm bảo dữ liệu được ghi vào file một cách đáng tin cậy (dùng `fflush()` hoặc `fclose()` đúng cách).
3. **Chương trình Phân tích Log Giả:**
   * Viết một chương trình C++ (`log_parser.cpp`) mà:
     * Giả định bạn có một file log với các dòng có định dạng: `[YYYY-MM-DD HH:MM:SS] <Severity>: <Message>`
       * Ví dụ: `[2025-07-02 21:30:00] INFO: Application started.`
     * Chương trình của bạn sẽ đọc file log này từng dòng.
     * Đối với mỗi dòng, sử dụng `strptime()` để phân tích cú pháp dấu thời gian thành `struct tm`.
     * In ra dấu thời gian đã phân tích được và phần `Severity: Message` còn lại của dòng.
     * **Thử thách:**
       * Chuyển đổi `struct tm` đã phân tích thành `time_t` bằng `mktime()`.
       * Tính toán thời gian đã trôi qua giữa dòng log hiện tại và dòng log trước đó (nếu có), và in ra.

---
