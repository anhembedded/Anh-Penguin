# **Module 4: Assertions ✅**

#### **4.1. Assertion là gì? (What is an Assertion?) 🤔**

* **Lý thuyết:** Một **Assertion** là một câu lệnh (macro) được sử dụng để **kiểm tra một giả định (assumption)** về trạng thái của chương trình tại một điểm cụ thể trong quá trình thực thi.

  * Nếu giả định đó (`expression`) là **đúng** (khác 0), assertion không làm gì cả và chương trình tiếp tục chạy bình thường.
  * Nếu giả định đó là **sai** (bằng 0), assertion sẽ:
    1. In ra một thông báo chẩn đoán (diagnostic message) lên `stderr`, bao gồm tên file, số dòng, tên hàm, và biểu thức đã thất bại.
    2. Gọi hàm `abort()` để **chấm dứt chương trình ngay lập tức** (thường tạo core dump).
* **Mục đích:** Assertions được dùng để phát hiện các lỗi lập trình hoặc các điều kiện "không thể xảy ra" trong quá trình phát triển và kiểm thử. Chúng giúp lập trình viên nhanh chóng xác định nơi mà một giả định quan trọng đã bị vi phạm.
* **Vị trí:** Assertions được khai báo trong header file **`<cassert>`** (hoặc `<assert.h>` trong C).
  **C++**

  ```
  #include <cassert> // Hoặc <assert.h>
  // void assert(int expression);
  ```
* **Liên hệ Embedded Linux:** Cực kỳ hữu ích trong nhúng để:

  * Xác nhận các điều kiện tiên quyết (preconditions) hoặc hậu điều kiện (postconditions) của hàm.
  * Kiểm tra tính hợp lệ của các con trỏ, chỉ số mảng, hoặc giá trị trả về từ các API phần cứng.
  * Phát hiện sớm các lỗi logic hoặc lỗi dữ liệu bị hỏng trước khi chúng gây ra hành vi không xác định hoặc crash khó hiểu.
  * Trong môi trường debug, chúng cung cấp thông tin chi tiết về lỗi ngay tại điểm xảy ra.

#### **4.2. Bật/Tắt Assertions (`NDEBUG`) 💡**

* **Lý thuyết:** Assertions được thiết kế để có thể dễ dàng bật hoặc tắt tại thời điểm biên dịch.
  * Nếu macro **`NDEBUG`** được định nghĩa **trước khi bao gồm `<cassert>`** (hoặc `<assert.h>`), thì macro `assert()` sẽ được định nghĩa là "không làm gì cả" (essentially nothing). Điều này có nghĩa là code assertions sẽ không được biên dịch vào chương trình cuối cùng.
  * **Cách định nghĩa `NDEBUG`:**
    * Trong mã nguồn: `#define NDEBUG` (đặt trước `#include <cassert>`).
    * Khi biên dịch: Sử dụng cờ `-DNDEBUG` cho trình biên dịch (ví dụ: `g++ -DNDEBUG your_program.cpp`). Đây là cách phổ biến nhất.
* **Khi nào bật/tắt?**
  * **Bật (không định nghĩa `NDEBUG`):** Trong môi trường  **phát triển và kiểm thử** . Assertions giúp tìm lỗi nhanh chóng.
  * **Tắt (định nghĩa `NDEBUG`):** Trong môi trường  **sản phẩm (production code)** . Lý do:
    * Assertions sẽ chấm dứt chương trình một cách đột ngột, điều không mong muốn đối với người dùng cuối.
    * Chúng có thể gây ra overhead nhỏ về hiệu suất.
    * Nếu biểu thức trong assertion có **side effects** (tác dụng phụ, ví dụ: gọi một hàm thay đổi trạng thái), thì side effect đó sẽ không xảy ra trong bản release khi assertions bị tắt, dẫn đến hành vi khác biệt giữa bản debug và bản release. **Tuyệt đối tránh side effects trong assertion expressions!**
* **Liên hệ Embedded Linux:** Trong nhúng, bạn thường sẽ bật assertions trong giai đoạn phát triển/kiểm thử trên board hoặc emulator. Khi triển khai sản phẩm cuối cùng, bạn sẽ tắt chúng để đảm bảo thiết bị không bị crash đột ngột trước mặt người dùng và để tránh overhead.

#### **4.3. Ví dụ (C++): `assert_example.cpp` - Sử dụng `assert()`**

**C++**

```
#include <iostream>  // For std::cout, std::cerr
#include <string>    // For std::string
#include <cassert>   // For assert
#include <cmath>     // For sqrt
#include <cstdlib>   // For EXIT_SUCCESS, EXIT_FAILURE

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

// Hàm ví dụ: tính căn bậc hai, yêu cầu đối số không âm
double my_sqrt(double x) {
    AppLogger::log(AppLogger::INFO_L, "my_sqrt: Checking assertion for x = " + std::to_string(x));
    // Assertion: x phải lớn hơn hoặc bằng 0
    assert(x >= 0.0); // Nếu x < 0.0, chương trình sẽ chấm dứt tại đây
  
    // Nếu assertion vượt qua, tiếp tục tính toán
    return sqrt(x);
}

// Hàm ví dụ có side effect trong assertion (KHÔNG NÊN LÀM)
int process_data(int* data_ptr) {
    // KHÔNG NÊN LÀM: Assertion có side effect
    // assert(data_ptr != nullptr && (*data_ptr)++ > 0); // *data_ptr++ là side effect
  
    // CÁCH ĐÚNG: Tách side effect ra khỏi assertion
    bool is_valid_data = (data_ptr != nullptr);
    if (is_valid_data) {
        // Thực hiện side effect
        (*data_ptr)++; 
    }
    assert(is_valid_data); // Assertion chỉ kiểm tra điều kiện
  
    return *data_ptr;
}


int main() {
    AppLogger::log(AppLogger::INFO_L, "--- Demonstrating Assertions ---");

    // --- Ví dụ 1: Assertion thành công ---
    AppLogger::log(AppLogger::INFO_L, "Calling my_sqrt with positive value (2.0)");
    double result1 = my_sqrt(2.0);
    AppLogger::log(AppLogger::SUCCESS_L, "sqrt(2.0) = " + std::to_string(result1));

    // --- Ví dụ 2: Assertion thất bại ---
    // Chương trình sẽ chấm dứt tại đây nếu assertions được bật
    AppLogger::log(AppLogger::INFO_L, "Calling my_sqrt with negative value (-2.0)");
    double result2 = my_sqrt(-2.0); // Dòng này sẽ kích hoạt assertion nếu NDEBUG không được định nghĩa

    // Dòng này sẽ không bao giờ được thực thi nếu assertion thất bại
    AppLogger::log(AppLogger::SUCCESS_L, "sqrt(-2.0) = " + std::to_string(result2)); 

    // Ví dụ về side effect (nếu bạn bỏ comment và thử)
    // int val = 5;
    // AppLogger::log(AppLogger::INFO_L, "Initial val: " + std::to_string(val));
    // int processed_val = process_data(&val);
    // AppLogger::log(AppLogger::INFO_L, "Processed val: " + std::to_string(processed_val));


    AppLogger::log(AppLogger::INFO_L, "--- Assertions Demonstration Finished ---");

    return EXIT_SUCCESS; // Sẽ không đạt được nếu assertion thất bại
}
```

* **Cách Biên dịch và Chạy:**
  1. **Với Assertions BẬT (mặc định cho debug):**
     **Bash**

     ```
     g++ assert_example.cpp -o assert_example -lm
     ./assert_example
     ```

     * **Output:** Chương trình sẽ chạy, in kết quả `sqrt(2.0)`, sau đó khi gọi `my_sqrt(-2.0)`, nó sẽ in thông báo lỗi assertion và chấm dứt với `Aborted` (hoặc `core dumped`).
  2. **Với Assertions TẮT (cho production):**
     **Bash**

     ```
     g++ -DNDEBUG assert_example.cpp -o assert_example -lm
     ./assert_example
     ```

     * **Output:** Chương trình sẽ chạy, in kết quả `sqrt(2.0)`, và khi gọi `my_sqrt(-2.0)`, nó sẽ bỏ qua assertion. Hàm `sqrt()` của C/C++ sẽ nhận đối số âm và có thể gây ra "Floating point exception" hoặc trả về `NaN` (Not a Number) tùy thuộc vào hệ thống và thư viện toán học. Chương trình có thể không chấm dứt ngay lập tức bởi assertion mà bởi lỗi toán học sau đó.

---

#### **4.4. Cân nhắc khi sử dụng Assertions ⚖️**

* **Không phải để xử lý lỗi đầu vào người dùng:** Assertions không nên được dùng để kiểm tra các điều kiện mà bạn mong đợi có thể xảy ra trong quá trình hoạt động bình thường của chương trình (ví dụ: người dùng nhập sai dữ liệu, file không tồn tại). Đối với những trường hợp này, bạn nên sử dụng các cơ chế xử lý lỗi tường minh (kiểm tra giá trị trả về, `if/else`, `try/catch` trong C++).
* **Dành cho lỗi lập trình:** Assertions dành cho các điều kiện mà theo logic của chương trình, "không bao giờ được xảy ra". Nếu một assertion thất bại, điều đó có nghĩa là có một lỗi lập trình nghiêm trọng trong code của bạn.
* **Side Effects:** **Tuyệt đối tránh** đặt các biểu thức có side effects (biểu thức làm thay đổi trạng thái của chương trình) bên trong `assert()`. Vì khi `NDEBUG` được định nghĩa, biểu thức đó sẽ không được đánh giá, và side effect sẽ không xảy ra, dẫn đến hành vi khác biệt giữa bản debug và bản release.
* **Tạo cơ chế kiểm tra lỗi riêng:** Một số lập trình viên thích tự viết macro assertion của riêng mình để có thể kiểm soát hành vi khi assertion thất bại (ví dụ: ghi log chi tiết hơn, không gọi `abort()` mà thoát duyên dáng hơn).

---

### **Câu hỏi Tự đánh giá Module 4 🤔**

1. Assertion là gì và nó hoạt động như thế nào khi biểu thức đúng và sai?
2. Mục đích chính của việc sử dụng assertions trong quá trình phát triển phần mềm là gì?
3. Giải thích vai trò của macro `NDEBUG` đối với `assert()`.
4. Tại sao việc đặt một biểu thức có side effect bên trong `assert()` lại là một thực hành tồi? Cho một ví dụ.
5. Assertions có nên được sử dụng để kiểm tra các lỗi mà người dùng có thể gây ra (ví dụ: nhập sai dữ liệu vào form) không? Giải thích lý do.

---

### **Bài tập Thực hành Module 4 ✍️**

1. **Chương trình Kiểm tra Giả định Mảng:**
   * Viết một chương trình C++ (`array_access_assert.cpp`) có một hàm `get_element(int* arr, int size, int index)` trả về phần tử tại `index` của mảng `arr`.
   * Trong hàm `get_element`, sử dụng `assert()` để kiểm tra hai giả định quan trọng:
     1. `arr` không phải là `nullptr`.
     2. `index` nằm trong phạm vi hợp lệ của mảng (`0 <= index < size`).
   * Trong `main()`, tạo một mảng và gọi `get_element()` với các trường hợp:
     * Truy cập hợp lệ.
     * Truyền `nullptr` cho `arr`.
     * Truy cập `index` ngoài biên.
   * **Mục tiêu:** Biên dịch và chạy chương trình có/không có `-DNDEBUG` để quan sát hành vi của assertions.
2. **Chương trình Xử lý Input với Assertion và Xử lý Lỗi Tường minh:**
   * Viết một chương trình C++ (`input_validator.cpp`) nhận một số nguyên từ dòng lệnh.
   * **Yêu cầu:** Số nguyên phải nằm trong khoảng `1` đến `100`.
   * Sử dụng `assert()` để kiểm tra rằng số đó **không âm** (giả định lập trình viên không bao giờ truyền số âm).
   * Sử dụng xử lý lỗi **tường minh** (ví dụ: `if/else` và in thông báo lỗi thân thiện) để kiểm tra rằng số đó nằm trong khoảng `1` đến `100`. Nếu không, in ra lỗi và thoát mà không dùng assertion.
   * **Thử thách:**
     * Chạy chương trình với đối số hợp lệ (ví dụ: `50`).
     * Chạy chương trình với đối số không hợp lệ (ví dụ: `0`, `101`).
     * Chạy chương trình với đối số âm (ví dụ: `-5`). Quan sát assertion.
     * Biên dịch với `-DNDEBUG` và chạy lại với đối số âm để thấy sự khác biệt.

---

Chúc mừng bạn đã hoàn thành Module 4! Assertions là một công cụ debug rất hiệu quả khi được sử dụng đúng cách.

Khi bạn đã sẵn sàng, hãy cho tôi biết để chuyển sang  **Module 5: Gỡ lỗi Sử dụng Bộ nhớ (Memory Debugging)** , nơi chúng ta sẽ khám phá các công cụ chuyên biệt để tìm lỗi bộ nhớ động!
