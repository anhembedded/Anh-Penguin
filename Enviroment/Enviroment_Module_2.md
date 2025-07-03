### **Module 2: Biến Môi trường (Environment Variables) 🌐**

#### **2.1. Biến Môi trường là gì? (What are Environment Variables?) 📖**

* **Lý thuyết:** **Biến môi trường** là các cặp **tên-giá trị (name=value)** được lưu trữ trong môi trường của một tiến trình. Chúng cung cấp một cách để cấu hình hành vi của chương trình hoặc hệ thống mà không cần sửa đổi mã nguồn hoặc các file cấu hình tĩnh.
  * Không giống như tham số chương trình (mà bạn vừa học), biến môi trường được các **tiến trình con kế thừa** từ tiến trình cha của chúng. Điều này có nghĩa là một chương trình có thể ảnh hưởng đến các chương trình khác mà nó khởi chạy bằng cách thiết lập các biến môi trường.
  * Chúng thường được sử dụng để lưu trữ thông tin cấu hình hệ thống, đường dẫn, cài đặt cục bộ cho người dùng, hoặc các tùy chọn cho các ứng dụng.
* **Cách truy cập trong Shell:**
  * Hiển thị giá trị của một biến: `echo $VARIABLE_NAME`
  * Thiết lập một biến (chỉ có hiệu lực trong shell hiện tại và các tiến trình con): `export VARIABLE_NAME=VALUE`
  * Liệt kê tất cả các biến môi trường: `env` hoặc `printenv`
* **Liên hệ Embedded Linux:** Cực kỳ quan trọng trong môi trường nhúng để:
  * Cấu hình đường dẫn cho các thư viện đặc biệt (`LD_LIBRARY_PATH`).
  * Đặt các thông số cấu hình chung cho các ứng dụng (ví dụ: cấp độ log, địa chỉ IP của server).
  * Giúp các shell script quản lý các đường dẫn và tùy chọn.

#### **2.2. Sử dụng Biến Môi trường trong C/C++ (`getenv()` và `putenv()`) 🛠️**

* **Lý thuyết:**

  * **`getenv()`:** Hàm dùng để **lấy giá trị** của một biến môi trường.
    **C++**

    ```cpp
    #include <stdlib.h> // For getenv
    char *getenv(const char *name);
    ```

    * `name`: Tên của biến môi trường cần lấy (ví dụ: "HOME", "PATH").
    * **Giá trị trả về:**
      * Con trỏ tới một chuỗi (kiểu `char*`) chứa giá trị của biến nếu tìm thấy.
      * `NULL` nếu biến không tồn tại.
    * **Lưu ý quan trọng:** Chuỗi trả về bởi `getenv()` nằm trong vùng nhớ tĩnh (static storage) do `getenv()` quản lý. Bạn **không được phép sửa đổi** chuỗi này và nó có thể bị ghi đè bởi các lệnh gọi `getenv()` tiếp theo. Nếu bạn cần lưu trữ giá trị, hãy sao chép nó vào bộ đệm của riêng bạn.
  * **`putenv()`:** Hàm dùng để **thiết lập hoặc thay đổi giá trị** của một biến môi trường trong môi trường của tiến trình hiện tại.
    **C++**

    ```cpp
    #include <stdlib.h> // For putenv
    int putenv(char *string);
    ```

    * `string`: Một chuỗi có định dạng `"NAME=VALUE"` (ví dụ: "MY_VAR=hello").
    * **Giá trị trả về:** `0` nếu thành công, `-1` nếu thất bại (và `errno` được thiết lập, ví dụ: `ENOMEM` nếu hết bộ nhớ).
    * **Lưu ý quan trọng:** Chuỗi bạn truyền vào `putenv()` (và vùng nhớ nó trỏ tới)  **phải tồn tại trong suốt vòng đời của biến môi trường đó** . Nếu bạn cấp phát động chuỗi này bằng `malloc()`, bạn **không được `free()`** nó sau khi gọi `putenv()`, vì `putenv()` có thể sử dụng chính con trỏ đó. Điều này đôi khi dẫn đến rò rỉ bộ nhớ hoặc lỗi nếu không cẩn thận.
  * **`setenv()` và `unsetenv()` (POSIX.1):** Thường được ưu tiên hơn `putenv()` vì an toàn hơn khi quản lý bộ nhớ.
    **C++**

    ```
    #include <stdlib.h> // For setenv, unsetenv
    int setenv(const char *name, const char *value, int overwrite); // overwrite: 1 ghi đè nếu tồn tại, 0 thì không
    int unsetenv(const char *name);
    ```

    * `setenv()`: Tạo hoặc thay đổi biến môi trường. Kernel tự quản lý bộ nhớ.
    * `unsetenv()`: Xóa biến môi trường.
* **Liên hệ Embedded Linux:** Rất hữu ích khi chương trình của bạn cần đọc các tham số cấu hình từ môi trường hoặc thiết lập các biến môi trường cho các chương trình con mà nó sẽ khởi chạy.
* **Ví dụ (C++): `environ_ops.cpp` - `getenv()` và `putenv()`**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <cstdlib>  // For getenv, putenv, EXIT_SUCCESS, EXIT_FAILURE
  #include <cstring>  // For strlen, strcpy, strcat
  #include <errno.h>  // For errno

  int main(int argc, char *argv[]) {
      if (argc == 1 || argc > 3) {
          std::cerr << "ERROR   : Usage: " << argv[0] << " <VAR_NAME> [VALUE]" << std::endl;
          return EXIT_FAILURE;
      }

      char *var_name = argv[1];
      char *var_value;

      // --- Lấy giá trị của biến môi trường ---
      std::cout << "INFO    : Attempting to get value for variable: " << var_name << std::endl;
      var_value = getenv(var_name);
      if (var_value) {
          std::cout << "SUCCESS : Variable " << var_name << " has value: '" << var_value << "'" << std::endl;
      } else {
          std::cout << "INFO    : Variable " << var_name << " has no value (or not set)." << std::endl;
      }

      // --- Thiết lập giá trị của biến môi trường (nếu có đối số thứ 2) ---
      if (argc == 3) {
          char *new_value_str = argv[2];
          // putenv yêu cầu chuỗi có dạng "NAME=VALUE"
          // Cấp phát động bộ nhớ cho chuỗi này
          char *env_string = new char[strlen(var_name) + strlen(new_value_str) + 2]; // +2 cho '=' và '\0'
          if (!env_string) {
              std::cerr << "CRITICAL: Out of memory!" << std::endl;
              return EXIT_FAILURE;
          }
          strcpy(env_string, var_name);
          strcat(env_string, "=");
          strcat(env_string, new_value_str);

          std::cout << "INFO    : Calling putenv with: '" << env_string << "'" << std::endl;
          if (putenv(env_string) != 0) {
              std::cerr << "ERROR   : putenv failed for '" << env_string << "': " << strerror(errno) << std::endl;
              // Nếu putenv thất bại, ta phải tự free() bộ nhớ đã cấp phát
              delete[] env_string;
              return EXIT_FAILURE;
          }
          std::cout << "SUCCESS : putenv successful." << std::endl;
          // Quan trọng: KHÔNG delete[] env_string ở đây, vì putenv có thể đang sử dụng nó.
          // Điều này là rủi ro của putenv(), nên setenv() thường được ưu tiên hơn.

          // --- Lấy lại giá trị để xác nhận ---
          std::cout << "INFO    : Getting new value for variable: " << var_name << std::endl;
          var_value = getenv(var_name);
          if (var_value) {
              std::cout << "SUCCESS : New value of " << var_name << " is: '" << var_value << "'" << std::endl;
          } else {
              std::cerr << "ERROR   : New value of " << var_name << " is null? Something went wrong." << std::endl;
          }
      }

      // Lưu ý: Các thay đổi biến môi trường chỉ có hiệu lực cục bộ trong chương trình này
      // và các tiến trình con mà nó tạo ra. Chúng không ảnh hưởng đến shell cha.
      std::cout << "INFO    : Program exiting. Check variable in shell after exit (it won't be changed)." << std::endl;

      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```bash
    g++ environ_ops.cpp -o environ_ops
    ./environ_ops HOME                   # Lấy giá trị của biến HOME
    ./environ_ops MY_TEST_VAR            # Lấy giá trị của biến chưa có
    ./environ_ops MY_TEST_VAR "Hello World" # Đặt giá trị cho biến
    ./environ_ops MY_TEST_VAR            # Lấy lại giá trị đã đặt
    echo $MY_TEST_VAR                    # Kiểm tra trong shell (sẽ thấy không thay đổi)
    MY_TEST_VAR="Another Value" ./environ_ops MY_TEST_VAR # Đặt biến tạm thời qua shell
    ```

#### **2.3. Biến `environ` (The `environ` Variable) 📚**

* **Lý thuyết:** Ngoài `getenv()`, thư viện C trên các hệ thống POSIX còn cung cấp một biến toàn cục đặc biệt tên là  **`environ`** . Biến này là một con trỏ tới một mảng các con trỏ `char` (kiểu `char **`), trong đó mỗi con trỏ trỏ tới một chuỗi có định dạng `"NAME=VALUE"`. Mảng này được kết thúc bằng một con trỏ `NULL`.
  **C++**

  ```cpp
  #include <unistd.h> // Cần thiết cho extern char **environ; trên một số hệ thống
  extern char **environ;
  ```

  * **Mục đích:** Cung cấp một cách cấp thấp (low-level) để truy cập toàn bộ danh sách các biến môi trường của tiến trình.
  * **Lưu ý:** Thường thì `getenv()` và `setenv()`/`unsetenv()` được ưu tiên sử dụng hơn vì an toàn và dễ quản lý hơn, đặc biệt khi sửa đổi môi trường. Trực tiếp thao tác với `environ` có thể phức tạp và dễ gây lỗi nếu không cẩn thận.
* **Liên hệ Embedded Linux:** Kiến thức này hữu ích cho việc gỡ lỗi hoặc khi cần hiểu cách các thư viện hoặc hệ thống nội bộ xử lý môi trường ở cấp độ rất thấp.
* **Ví dụ (C++): `show_env.cpp` - Duyệt qua `environ`**
  **C++**

  ```cpp
  #include <iostream>
  #include <string>
  #include <cstdlib>  // For EXIT_SUCCESS
  #include <unistd.h> // For extern char **environ; on some systems

  // Khai báo extern char **environ; để sử dụng biến toàn cục
  extern char **environ; 

  int main() {
      std::cout << "INFO    : Listing all environment variables using 'environ':" << std::endl;
      char **env_ptr = environ; // Lấy con trỏ đến đầu mảng

      // Duyệt qua mảng cho đến khi gặp NULL pointer
      while (*env_ptr != nullptr) {
          std::cout << "INFO    : " << *env_ptr << std::endl;
          env_ptr++; // Di chuyển đến chuỗi tiếp theo
      }
      return EXIT_SUCCESS;
  }
  ```

  * **Cách chạy:**
    **Bash**

    ```
    g++ show_env.cpp -o show_env
    ./show_env
    ```
  * **Phân tích:** Output sẽ hiển thị tất cả các biến môi trường của shell hiện tại, được kế thừa bởi chương trình này.

---

### **Câu hỏi Tự đánh giá Module 2 🤔**

1. Biến môi trường khác với tham số dòng lệnh như thế nào về cách chúng được truyền và phạm vi hiệu lực?
2. Bạn muốn lấy giá trị của biến môi trường `MY_CONFIG_PATH`. Viết đoạn code C++ sử dụng `getenv()` để lấy giá trị này. Nếu biến không tồn tại, in ra thông báo phù hợp.
3. Giải thích lý do tại sao việc sử dụng `putenv()` với một chuỗi được cấp phát động (`malloc()`) có thể gây rò rỉ bộ nhớ nếu không được xử lý cẩn thận.
4. Khi nào bạn nên dùng `setenv()` và `unsetenv()` thay vì `putenv()`?
5. Biến toàn cục `environ` được dùng để làm gì? Nêu một trường hợp bạn có thể cần truy cập nó thay vì chỉ dùng `getenv()`.

---

### **Bài tập Thực hành Module 2 ✍️**

1. **Chương trình Thử nghiệm Biến Môi trường:**
   * Viết một chương trình C++ (`env_tester.cpp`) mà:
     * Lấy và in ra giá trị của các biến môi trường `HOME`, `PATH`, `USER`, `LANG`.
     * Thử thiết lập một biến môi trường mới có tên `MY_CUSTOM_SETTING` với giá trị "active" bằng `setenv()`. In ra giá trị mới này.
     * Thử bỏ thiết lập một biến môi trường `MY_CUSTOM_SETTING` đó bằng `unsetenv()`.
     * Sau đó, thử khởi chạy một chương trình con (ví dụ: `printenv` của Linux) bằng `system()` hoặc `fork()`/`exec()` và quan sát xem biến `MY_CUSTOM_SETTING` có xuất hiện trong môi trường của chương trình con đó không.
   * **Thử thách:** Chạy chương trình của bạn và sau đó thử khởi chạy một chương trình con khác bằng `system()` hoặc `fork()`/`exec()` với một môi trường **tùy chỉnh** (chỉ định các biến riêng, không kế thừa từ cha) sử dụng `execle()` hoặc `execve()`.
2. **Chương trình `ls_env_sorted`:**
   * Viết một chương trình C++ (`ls_env_sorted.cpp`) mà:
     * Sử dụng biến toàn cục `environ` để duyệt qua tất cả các biến môi trường.
     * Lưu các biến này vào một `std::vector<std::string>`.
     * Sắp xếp các biến này theo thứ tự bảng chữ cái.
     * In ra danh sách các biến môi trường đã sắp xếp.
   * **Thử thách:** Thêm tùy chọn dòng lệnh `-r` hoặc `--reverse` để in ra theo thứ tự ngược lại.

---
