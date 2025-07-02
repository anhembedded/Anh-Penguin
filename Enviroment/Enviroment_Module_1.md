### **Giáo trình: The Linux Environment 🌍**

Chào Hoàng Anh,

Đây là một Module cực kỳ quan trọng, cung cấp cái nhìn tổng quan về môi trường mà chương trình Linux của bạn hoạt động. Việc nắm vững những kiến thức này sẽ giúp bạn viết các ứng dụng linh hoạt, tự nhận biết môi trường, và xử lý tài nguyên hiệu quả, đặc biệt là trong bối cảnh các hệ thống nhúng có tài nguyên hạn chế.

---

### **Mục tiêu của Giáo trình 🎯**

Sau khi hoàn thành giáo trình này, bạn sẽ có thể:

* Hiểu cách chương trình Linux nhận và xử lý các tham số dòng lệnh.
* Sử dụng biến môi trường để cấu hình và lấy thông tin hệ thống.
* Làm việc với thời gian và ngày tháng trong chương trình C/C++.
* Quản lý file tạm thời một cách an toàn.
* Truy vấn thông tin về người dùng và máy chủ.
* Cấu hình và ghi các thông báo log hệ thống.
* Kiểm soát và giám sát các giới hạn tài nguyên của tiến trình.

---

### **Cấu trúc Giáo trình 📚**

Giáo trình này sẽ được chia thành các Modules nhỏ để bạn dễ dàng theo dõi và tiếp thu:

* **Module 1: Tham số Chương trình (Program Arguments)**
* **Module 2: Biến Môi trường (Environment Variables)**
* **Module 3: Thời gian và Ngày tháng (Time and Date)**
* **Module 4: File Tạm thời (Temporary Files)**
* **Module 5: Thông tin Người dùng và Máy chủ (User and Host Information)**
* **Module 6: Ghi Log (Logging)**
* **Module 7: Tài nguyên và Giới hạn (Resources and Limits)**

Mỗi Module sẽ bao gồm:

* **Lý thuyết chi tiết:** Giải thích các khái niệm, hàm, và cơ chế.
* **Ví dụ Code (C++):** Minh họa cách sử dụng các hàm.
* **Liên hệ với Embedded Linux:** Giải thích tầm quan trọng và ứng dụng trong hệ thống nhúng.
* **Câu hỏi Tự đánh giá:** Giúp bạn kiểm tra mức độ hiểu bài.
* **Bài tập Thực hành:** Các bài tập coding để bạn áp dụng kiến thức.

Hãy bắt đầu với Module đầu tiên!

---

### **Module 1: Tham số Chương trình (Program Arguments) 🚦**

#### **1.1. `main()` và Tham số Dòng lệnh 📜**

* **Lý thuyết:** Khi một chương trình C++ được chạy trên Linux, điểm bắt đầu của nó là hàm `main()`. Hàm này có thể nhận hai tham số đặc biệt để truy cập các đối số (arguments) được truyền từ dòng lệnh:
  **C++**

  ```cpp
  int main(int argc, char *argv[])
  ```

  * **`argc` (Argument Count):** Là một số nguyên cho biết **tổng số đối số** trên dòng lệnh, bao gồm cả tên của chương trình đang chạy.
  * **`argv` (Argument Vector):** Là một  **mảng các con trỏ chuỗi (`char *[]`)** , trong đó mỗi phần tử trỏ tới một chuỗi ký tự (một đối số).
    * `argv[0]` luôn là **tên của chương trình** đang chạy.
    * `argv[1]`, `argv[2]`, ... là các đối số tiếp theo.
    * Mảng `argv` kết thúc bằng một con trỏ `NULL`.
* **Ví dụ:** Nếu bạn chạy `$ myprog left right 'and center'`, thì:

  * `argc` sẽ là `4`.
  * `argv` sẽ chứa `{"myprog", "left", "right", "and center"}`.
* **Lưu ý:** Shell Linux tự động thực hiện việc mở rộng wildcard (ví dụ: `*.txt`) trước khi các đối số được truyền cho chương trình. Các dấu nháy đơn hoặc kép được sử dụng để nhóm các từ có khoảng trắng thành một đối số duy nhất.
* **Tham số Tùy chọn (Options/Flags):** Các chương trình thường sử dụng các đối số bắt đầu bằng dấu gạch ngang (`-`) để biểu thị các tùy chọn hoặc cờ (flags), ví dụ: `ls -l`, `sort -r file`.
* **Ví dụ (C++): `args.cpp` - Xem xét các Tham số**
  **C++**

  ```c
  #include <iostream>
  #include <string>
  #include <vector>
  #include <cstdlib> // For EXIT_SUCCESS

  int main(int argc, char *argv[]) {
      std::cout << "INFO    : Program started with " << argc << " arguments." << std::endl;
      for (int arg = 0; arg < argc; arg++) {
          if (argv[arg][0] == '-') {
              std::cout << "INFO    : Option: " << (argv[arg] + 1) << std::endl;
          } else {
              std::cout << "INFO    : Argument " << arg << ": " << argv[arg] << std::endl;
          }
      }
      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ args.cpp -o args
    ./args -i -lr 'hi there' -f fred.c
    ```
  * **Phân tích:** Chương trình duyệt qua `argv` và kiểm tra ký tự đầu tiên để phân biệt tùy chọn (bắt đầu bằng `-`) và đối số thông thường.

#### **1.2. `getopt()`: Xử lý Tùy chọn Dòng lệnh Ngắn 📜**

* **Lý thuyết:** `getopt()` là một hàm thư viện chuẩn POSIX được thiết kế để phân tích cú pháp các tùy chọn dòng lệnh ngắn (single-character options, ví dụ: `-a`, `-b`, `-c value`).
  **C++**

  ```cpp
  #include <unistd.h> // For getopt
  // Các biến toàn cục được sử dụng bởi getopt:
  extern char *optarg;  // Con trỏ tới giá trị của tùy chọn (nếu có)
  extern int optind, opterr, optopt;
  // int getopt(int argc, char *const argv[], const char *optstring);
  ```

  * **`optstring`** : Một chuỗi các ký tự đại diện cho các tùy chọn hợp lệ.
  * Nếu một ký tự được theo sau bởi dấu hai chấm (`:`), tùy chọn đó yêu cầu một đối số (ví dụ: `"f:"` cho `-f filename`).
  * Nếu theo sau bởi hai dấu hai chấm (`::`), tùy chọn đó có một đối số tùy chọn (optional argument).
  * **Cách hoạt động:** Bạn gọi `getopt()` lặp đi lặp lại trong một vòng lặp `while` cho đến khi nó trả về `-1` (không còn tùy chọn nào để xử lý).
    * Trả về ký tự của tùy chọn tìm thấy (ví dụ: `'i'`, `'l'`, `'f'`).
    * Nếu tùy chọn có đối số, `optarg` sẽ trỏ tới đối số đó.
    * Nếu gặp tùy chọn không nhận dạng được hoặc lỗi, trả về `?` hoặc `:`. `optopt` lưu trữ ký tự tùy chọn không hợp lệ.
    * `optind`: Biến này được `getopt` tự động cập nhật, chỉ ra chỉ số của đối số tiếp theo trong `argv` mà không phải là tùy chọn. Khi vòng lặp kết thúc, các đối số còn lại (non-option arguments) sẽ bắt đầu từ `argv[optind]`.
* **Liên hệ Embedded Linux:** Cực kỳ hữu ích để xây dựng các công cụ cấu hình hoặc ứng dụng điều khiển trên thiết bị nhúng mà người dùng có thể tùy chỉnh hành vi thông qua các cờ lệnh.
* **Ví dụ (C++): `argopt.cpp` - Sử dụng `getopt()`**
  **C++**

  ```c
  #include <iostream>
  #include <string>
  #include <unistd.h> // For getopt
  #include <cstdlib>  // For EXIT_SUCCESS

  int main(int argc, char *argv[]) {
      int opt;
      // "if:lr" nghĩa là: -i, -l, -r là các cờ; -f yêu cầu một đối số.
      while ((opt = getopt(argc, argv, "if:lr")) != -1) {
          switch (opt) {
              case 'i':
              case 'l':
              case 'r':
                  std::cout << "INFO    : Option: " << (char)opt << std::endl;
                  break;
              case 'f':
                  std::cout << "INFO    : Filename: " << optarg << std::endl;
                  break;
              case ':': // Tùy chọn cần giá trị nhưng không có
                  std::cerr << "WARNING : Option needs a value (detected by ':'): " << (char)optopt << std::endl;
                  break;
              case '?': // Tùy chọn không xác định
                  std::cerr << "WARNING : Unknown option (detected by '?'): " << (char)optopt << std::endl;
                  break;
          }
      }
      // In các đối số không phải tùy chọn
      for (; optind < argc; optind++) {
          std::cout << "INFO    : Argument (non-option): " << argv[optind] << std::endl;
      }
      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ argopt.cpp -o argopt
    ./argopt -i -lr 'hi there' -f fred.c -q
    ./argopt -f # Sẽ báo lỗi tùy chọn cần giá trị
    ```

#### **1.3. `getopt_long()`: Hỗ trợ Tùy chọn Dài 📏**

* **Lý thuyết:** `getopt_long()` (một extension của GNU C Library, không phải POSIX chuẩn) cho phép bạn sử dụng các tùy chọn dài, có tên đầy đủ, bắt đầu bằng hai dấu gạch ngang (`--`), ví dụ: `--initialize`, `--file=filename.txt`.

  * Cần định nghĩa macro `_GNU_SOURCE` trước khi bao gồm `<getopt.h>`.

  **C++**

  ```c
  #define _GNU_SOURCE // Cần định nghĩa trước #include <getopt.h>
  #include <getopt.h> // For getopt_long, struct option
  // int getopt_long(int argc, char *const argv[], const char *optstring,
  //                 const struct option *longopts, int *longindex);
  ```

  * **`longopts`** : Một mảng các cấu trúc `struct option` mô tả các tùy chọn dài. Mảng này phải kết thúc bằng một cấu trúc chứa toàn số 0.
    **C++**

  ```cpp
  struct option {
      const char *name;  // Tên tùy chọn dài (ví dụ: "initialize")
      int has_arg;       // 0: no_argument; 1: required_argument; 2: optional_argument
      int *flag;         // Nếu NULL, getopt_long trả về 'val'. Nếu không NULL, gán 'val' vào *flag và trả về 0.
      int val;           // Giá trị trả về khi tùy chọn này được tìm thấy (hoặc gán vào *flag)
  };
  ```

  * **Ưu điểm:** Tùy chọn dễ đọc, dễ nhớ hơn cho người dùng. Có thể kết hợp cả tùy chọn ngắn và dài.
* **Liên hệ Embedded Linux:** Rất phù hợp cho các ứng dụng có nhiều tùy chọn cấu hình phức tạp, giúp các công cụ của bạn trở nên thân thiện và chuyên nghiệp hơn.
* **Ví dụ (C++): `longopt.cpp` - Sử dụng `getopt_long()`**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <unistd.h> // For getopt, optarg, optind, opterr, optopt
  #include <cstdlib>  // For EXIT_SUCCESS

  #define _GNU_SOURCE // Phải định nghĩa trước khi include getopt.h
  #include <getopt.h> // For getopt_long, struct option

  int main(int argc, char *argv[]) {
      int opt;
      // Định nghĩa các tùy chọn dài
      static struct option long_options[] = {
          // {tên_dài, có_đối_số_không, con_trỏ_cờ_ghi_vào_nếu_tìm_thấy, giá_trị_trả_về}
          {"initialize", no_argument,       nullptr, 'i'}, // --initialize
          {"file",       required_argument, nullptr, 'f'}, // --file=filename hoặc --file filename
          {"list",       no_argument,       nullptr, 'l'}, // --list
          {"restart",    no_argument,       nullptr, 'r'}, // --restart
          {nullptr, 0, nullptr, 0} // Dấu hiệu kết thúc mảng
      };

      int long_index = 0; // Biến để lưu chỉ số của tùy chọn dài được tìm thấy

      // "if:lr" vẫn xử lý các tùy chọn ngắn. getopt_long sẽ xử lý cả tùy chọn ngắn và dài.
      while ((opt = getopt_long(argc, argv, "if:lr", long_options, &long_index)) != -1) {
          switch (opt) {
              case 'i':
              case 'l':
              case 'r':
                  std::cout << "INFO    : Option: " << (char)opt << " (Long option: " << long_options[long_index].name << ")" << std::endl;
                  break;
              case 'f':
                  std::cout << "INFO    : Filename: " << optarg << std::endl;
                  break;
              case ':': // Tùy chọn cần giá trị nhưng không có
                  std::cerr << "WARNING : Option needs a value (detected by ':'): " << (char)optopt << std::endl;
                  break;
              case '?': // Tùy chọn không xác định
                  std::cerr << "WARNING : Unknown option (detected by '?'): " << (char)optopt << std::endl;
                  break;
              case 0: // Trường hợp khi long option set một cờ (flag)
                      // Ví dụ: {"verbose", no_argument, &verbose_flag, 1}
                  std::cout << "INFO    : Long option '" << long_options[long_index].name << "' set a flag." << std::endl;
                  break;
          }
      }
      // In các đối số không phải tùy chọn
      for (; optind < argc; optind++) {
          std::cout << "INFO    : Argument (non-option): " << argv[optind] << std::endl;
      }
      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ longopt.cpp -o longopt -D_GNU_SOURCE # -D_GNU_SOURCE là cần thiết để bật getopt_long
    ./longopt --initialize --list 'hi there' --file fred.c -q
    ./longopt --file=other.txt --restart
    ./longopt --init -l --file my_doc.txt
    ```

---

### **Câu hỏi Tự đánh giá Module 1 🤔**

1. Trong hàm `main(int argc, char *argv[])`, `argc` và `argv` có ý nghĩa gì? Điều gì luôn là giá trị của `argv[0]`?
2. Nếu bạn chạy một chương trình với lệnh `$ my_app -a -b val_b --verbose input.txt`, hãy liệt kê các giá trị của `argc`, `argv[0]`, `argv[1]`, `argv[2]`, `argv[3]`, `argv[4]` (nếu có) mà chương trình nhận được.
3. Giải thích sự khác biệt chính giữa `getopt()` và `getopt_long()`. Khi nào bạn sẽ chọn sử dụng `getopt_long()`?
4. Bạn muốn tạo một tùy chọn `--output <file>` (bắt buộc đối số) và một tùy chọn `--debug` (không có đối số). Viết đoạn code định nghĩa `struct option` và chuỗi `optstring` phù hợp cho `getopt_long()`.
5. Giải thích vai trò của các biến toàn cục `optarg`, `optind`, và `optopt` trong quá trình phân tích tùy chọn.

---

### **Bài tập Thực hành Module 1 ✍️**

1. **Chương trình Phân tích Cấu hình Đơn giản:**
   * Viết một chương trình C++ (`config_parser.cpp`) sử dụng `getopt_long()` để phân tích các tùy chọn dòng lệnh sau:
     * `-v` hoặc `--verbose`: Không có đối số. Bật chế độ chi tiết (in thêm thông báo debug).
     * `-o <file>` hoặc `--output <file>`: Bắt buộc đối số. Chỉ định đường dẫn file đầu ra.
     * `-p <port>` hoặc `--port <port>`: Bắt buộc đối số. Chỉ định số cổng (kiểu int).
     * `-h` hoặc `--help`: Không có đối số. In ra thông báo hướng dẫn sử dụng chương trình và thoát.
   * Chương trình nên kiểm tra và in ra giá trị của các tùy chọn được cung cấp.
   * Nếu có tùy chọn không xác định hoặc thiếu đối số, in ra thông báo lỗi thích hợp và thoát.
   * Sau khi phân tích tùy chọn, in ra bất kỳ đối số không phải tùy chọn nào còn lại.
   * **Thử thách:**
     * Chuyển đổi đối số `port` từ chuỗi sang số nguyên.
     * Đảm bảo chương trình tiếp tục chạy ngay cả khi có các đối số không phải tùy chọn sau các tùy chọn (hành vi mặc định của getopt trên Linux).
2. **Chương trình "Gạch đầu dòng":**
   * Viết một chương trình C++ (`bullet_liner.cpp`) nhận một file đầu vào và in ra từng dòng của file đó, có đánh số thứ tự hoặc gạch đầu dòng.
   * Chương trình hỗ trợ các tùy chọn:
     * `-n` hoặc `--numbered`: Đánh số thứ tự dòng (1., 2., ...).
     * `-b` hoặc `--bullet`: Dùng ký tự `* ` làm gạch đầu dòng (mặc định nếu không có `-n`).
     * Nếu không có tùy chọn nào, hoặc nếu có `-b`, dùng `* `.
   * Chương trình đọc từ `stdin` nếu không có file đầu vào được chỉ định (đối số không phải tùy chọn).
   * **Thử thách:** Xử lý trường hợp file không tồn tại hoặc không thể đọc.

---
