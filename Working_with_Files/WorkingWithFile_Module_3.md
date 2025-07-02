
### **Module 3: Thư viện I/O Chuẩn (Standard I/O Library - `stdio.h`) 📚**

Trong Module 2, bạn đã học cách tương tác trực tiếp với Kernel thông qua các System Call cấp thấp. Tuy nhiên, việc sử dụng các System Call này có thể kém hiệu quả cho các thao tác đọc/ghi nhỏ lẻ và đôi khi phức tạp. Đó là lý do thư viện I/O chuẩn (`stdio`) ra đời.

#### **3.1. `stdio`: Giao diện cấp cao hơn và Bộ đệm (Buffering) 💨**

* **Lý thuyết:**
  * Thư viện  **Standard I/O (stdio)** , được cung cấp qua header file  **`stdio.h`** , là một tập hợp các hàm thư viện chuẩn C. Chúng cung cấp một giao diện cấp cao hơn, thuận tiện hơn để làm việc với file và thiết bị I/O.
  * Điểm khác biệt lớn nhất và lợi thế chính của `stdio` so với System Calls là  **khả năng đệm (buffering)** . Thay vì thực hiện một System Call cho mỗi byte hoặc mỗi ký tự bạn đọc/ghi, `stdio` sẽ:
    * Tích lũy dữ liệu vào một **bộ đệm (buffer)** trong không gian người dùng (user space) khi bạn ghi. Khi bộ đệm đầy, hoặc khi bạn ép buộc, nó mới thực hiện một System Call lớn để ghi toàn bộ khối dữ liệu đó ra Kernel.
    * Khi đọc, nó đọc một khối dữ liệu lớn từ Kernel vào bộ đệm, sau đó trả về từng phần cho chương trình của bạn khi bạn yêu cầu.
  * Điều này giúp  **giảm đáng kể số lượng System Calls** , từ đó **cải thiện hiệu suất** đáng kể cho các thao tác I/O.
* **Streams (`FILE *`) vs. File Descriptors (`int`)** :
* Thay vì File Descriptor (số nguyên), `stdio` sử dụng  **Streams** . Một stream được biểu diễn bằng một con trỏ tới cấu trúc `FILE` (kiểu `FILE *`).
* Mỗi stream được liên kết với một File Descriptor cấp thấp bên dưới.
* **Streams chuẩn:** Giống như File Descriptors, có ba stream chuẩn được mở tự động khi chương trình bắt đầu:
  * `stdin`: Standard Input (liên kết với FD 0).
  * `stdout`: Standard Output (liên kết với FD 1).
  * `stderr`: Standard Error (liên kết với FD 2).
* **Liên hệ Embedded Linux:** `stdio` cực kỳ hữu ích cho việc ghi log, đọc/ghi file cấu hình, hoặc tương tác với console. Mặc dù nó có lớp trừu tượng và buffering, bạn vẫn cần hiểu System Calls để điều khiển các thiết bị đặc biệt hoặc khi cần hiệu suất I/O tuyệt đối không có buffering.

#### **3.2. Mở và Đóng File: `fopen()` & `fclose()` 🔓🛑**

* **Lý thuyết:**

  * **`fopen()`:** Hàm thư viện để mở một file và liên kết một stream với nó.
    **C**

    ```
    #include <stdio.h> // For fopen
    FILE *fopen(const char *filename, const char *mode);
    ```

    * `filename`: Đường dẫn đến file.
    * `mode`: Chuỗi ký tự (không phải ký tự đơn) chỉ định cách mở file.
      * `"r"` / `"rb"`: Mở để đọc (read only).
      * `"w"` / `"wb"`: Mở để ghi (write only), nếu file tồn tại sẽ **cắt bớt về 0 bytes** (truncate).
      * `"a"` / `"ab"`: Mở để ghi, **ghi tiếp vào cuối file** (append).
      * `"r+"` / `"rb+"`: Mở để đọc và ghi (update), không truncate.
      * `"w+"` / `"wb+"`: Mở để đọc và ghi, **cắt bớt về 0 bytes** nếu file tồn tại.
      * `"a+"` / `"ab+"`: Mở để đọc và ghi, ghi tiếp vào cuối file.
      * Chữ `b` (ví dụ: `"rb"`, `"wb"`) chỉ ra file nhị phân. Trên Linux, điều này không tạo ra sự khác biệt về cách xử lý nội dung file (Linux xem mọi file là nhị phân), nhưng nó có thể ảnh hưởng đến tính di động trên các hệ thống khác (như Windows, nơi có sự phân biệt rõ ràng giữa text và binary mode).
    * Trả về `FILE *` nếu thành công, `NULL` nếu thất bại.
  * **`fclose()`:** Hàm thư viện để đóng một stream.
    **C**

    ```
    #include <stdio.h> // For fclose
    int fclose(FILE *stream);
    ```

    * Đóng stream, **đảm bảo mọi dữ liệu còn trong bộ đệm được ghi ra file** trước khi giải phóng tài nguyên.
    * Trả về `0` nếu thành công, `EOF` nếu thất bại.
  * **Quan trọng:** `fclose()` được gọi tự động cho tất cả các stream đang mở khi chương trình kết thúc bình thường, nhưng việc gọi `fclose()` tường minh là thực hành tốt để kiểm soát khi nào dữ liệu được ghi ra đĩa và giải phóng tài nguyên.
* **Ví dụ (C): `fopen` và `fclose`**
  **C++**

  ```c
  #include <iostream>
  #include <string>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <errno.h>
  #include <unistd.h> // For unlink

  int main() {
      FILE *fp;
      const char *filename = "my_stdio_file.txt";

      // Mở file để ghi (tạo mới/ghi đè)
      std::cout << "INFO    : Attempting to open " << filename << " in 'w' mode." << std::endl;
      fp = fopen(filename, "w");
      if (fp == NULL) {
          std::cerr << "ERROR   : Failed to open " << filename << ": " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : Successfully opened " << filename << " for writing." << std::endl;
      fprintf(fp, "This is a test line written via stdio.\n");
      std::cout << "INFO    : Data written to file's buffer." << std::endl;

      // Đóng file
      if (fclose(fp) == EOF) {
          std::cerr << "ERROR   : Failed to close " << filename << ": " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : File " << filename << " closed successfully." << std::endl;

      // Mở file để đọc
      std::cout << "INFO    : Attempting to open " << filename << " in 'r' mode." << std::endl;
      fp = fopen(filename, "r");
      if (fp == NULL) {
          std::cerr << "ERROR   : Failed to open " << filename << " for reading: " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : Successfully opened " << filename << " for reading." << std::endl;

      char buffer[100];
      if (fgets(buffer, sizeof(buffer), fp) != NULL) {
          std::cout << "INFO    : Read from file: " << buffer; // fgets includes newline, no std::endl needed
      }

      if (fclose(fp) == EOF) {
          std::cerr << "ERROR   : Failed to close " << filename << ": " << strerror(errno) << std::endl;
          return EXIT_FAILURE;
      }
      std::cout << "SUCCESS : File " << filename << " closed successfully." << std::endl;

      // Dọn dẹp
      // unlink(filename); // Bỏ comment để tự động xóa file sau khi chạy
      return EXIT_SUCCESS;
  }
  ```

#### **3.3. Đọc và Ghi Dữ liệu: `fread()` & `fwrite()` 📖✍️**

* **Lý thuyết:** Các hàm này dùng để đọc và ghi các **khối dữ liệu (data records)** vào/từ một stream. Rất hiệu quả khi làm việc với dữ liệu nhị phân hoặc các cấu trúc.
  * **`fread()`:**
    **C**

    ```
    #include <stdio.h> // For fread
    size_t fread(void *ptr, size_t size, size_t nitems, FILE *stream);
    ```

    * Đọc `nitems` phần tử, mỗi phần tử `size` byte, từ `stream` vào bộ đệm `ptr`.
    * Trả về số lượng phần tử đã đọc thành công. Có thể nhỏ hơn `nitems` nếu gặp EOF hoặc lỗi.
  * **`fwrite()`:**
    **C**

    ```
    #include <stdio.h> // For fwrite
    size_t fwrite(const void *ptr, size_t size, size_t nitems, FILE *stream);
    ```

    * Ghi `nitems` phần tử, mỗi phần tử `size` byte, từ bộ đệm `ptr` vào `stream`.
    * Trả về số lượng phần tử đã ghi thành công.
* **Lưu ý:**
  * `fread`/`fwrite` làm việc với số lượng  **phần tử (items)** , không phải số lượng byte.
  * Bạn phải tự quản lý bộ đệm `ptr` (cấp phát đủ không gian).
  * **Không khuyến nghị** dùng `fread`/`fwrite` với dữ liệu có cấu trúc khi di chuyển giữa các kiến trúc máy khác nhau (vấn đề byte ordering, padding của struct). Tốt hơn nên tuần tự hóa (serialize) dữ liệu bằng cách ghi từng trường một hoặc chuyển đổi sang định dạng chuẩn.

#### **3.4. Ép buộc Ghi Bộ đệm: `fflush()` 💧**

* **Lý thuyết:** Hàm `fflush()` ép buộc mọi dữ liệu còn lại trong bộ đệm đầu ra của một stream phải được ghi ngay lập tức xuống file/thiết bị.
  **C**

  ```cpp
  #include <stdio.h> // For fflush
  int fflush(FILE *stream);
  ```
* **Khi dùng:**

  * Đảm bảo output đến terminal ngay lập tức (ví dụ: sau một lời nhắc nhở tương tác).
  * Đảm bảo dữ liệu quan trọng được ghi vào đĩa ngay lập tức (ví dụ: trong các ứng dụng ghi log quan trọng).
  * Khi gỡ lỗi, để xem output chương trình mà không cần đóng file.
* **Lưu ý:** `fflush(NULL)` sẽ flush tất cả các stream output đang mở. `fclose()` tự động gọi `fflush()` trước khi đóng.

#### **3.5. Di chuyển con trỏ Stream: `fseek()` ➡️**

* **Lý thuyết:** `fseek()` là hàm tương đương cấp cao của `lseek()` dành cho streams. Nó thay đổi vị trí hiện tại của con trỏ đọc/ghi trong một stream.
  

  ```c
  #include <stdio.h> // For fseek
  int fseek(FILE *stream, long int offset, int whence);
  ```

  * `stream`: Stream cần thay đổi vị trí.
  * `offset`: Giá trị offset (có thể âm, dương).
  * `whence`: Cách tính offset (`SEEK_SET`, `SEEK_CUR`, `SEEK_END`), giống `lseek()`.
* **Giá trị trả về:** `0` nếu thành công, `-1` nếu thất bại (và `errno` được thiết lập).
* **Các hàm liên quan:**

  * `long int ftell(FILE *stream);`: Trả về vị trí hiện tại của con trỏ stream.
  * `void rewind(FILE *stream);`: Đặt con trỏ stream về đầu file (`fseek(stream, 0L, SEEK_SET)` và xóa lỗi).

#### **3.6. I/O Ký tự: `fgetc`, `getc`, `getchar`, `fputc`, `putc`, `putchar` 🔡**

* **Lý thuyết:** Các hàm này dùng để đọc hoặc ghi từng ký tự một.
  * **Đọc:**
    * `int fgetc(FILE *stream);`: Đọc ký tự tiếp theo từ `stream`. Trả về ký tự (dạng `int`) hoặc `EOF` (khi hết file hoặc lỗi).
    * `int getc(FILE *stream);`: Tương tự `fgetc`, nhưng có thể được triển khai dưới dạng macro (nhanh hơn nhưng cẩn thận với side effects).
    * `int getchar();`: Tương đương `getc(stdin)`.
  * **Ghi:**
    * `int fputc(int c, FILE *stream);`: Ghi ký tự `c` vào `stream`. Trả về ký tự đã ghi hoặc `EOF` nếu lỗi.
    * `int putc(int c, FILE *stream);`: Tương tự `fputc`, có thể là macro.
    * `int putchar(int c);`: Tương đương `putc(c, stdout)`.
* **Lưu ý:** Các hàm này trả về/nhận ký tự dưới dạng `int` thay vì `char` để có thể biểu diễn giá trị `EOF` (`-1`), nằm ngoài phạm vi của các giá trị ký tự hợp lệ.

#### **3.7. I/O Chuỗi: `fgets()` & `gets()` (Cẩn thận!) 📝**

* **Lý thuyết:** Dùng để đọc chuỗi (một dòng văn bản).
  * **`char *fgets(char *s, int n, FILE *stream);`:**
    * Đọc tối đa `n-1` ký tự từ `stream` vào bộ đệm `s` hoặc cho đến khi gặp ký tự xuống dòng (`\n`) hoặc EOF.
    * Ký tự xuống dòng (`\n`) (nếu đọc được) **sẽ được bao gồm** trong chuỗi `s`.
    * Luôn thêm ký tự `\0` kết thúc chuỗi.
    * Trả về `s` nếu thành công, `NULL` nếu lỗi hoặc EOF.
    * **Là hàm an toàn** vì nó giới hạn số ký tự đọc.
  * **`char *gets(char *s);`:**
    * Đọc ký tự từ `stdin` vào bộ đệm `s` cho đến khi gặp ký tự xuống dòng hoặc EOF.
    * **Bỏ qua** ký tự xuống dòng (`\n`).
    * Luôn thêm `\0` kết thúc chuỗi.
    * **CỰC KỲ NGUY HIỂM!** Nó  **không giới hạn số ký tự đọc** , có thể dẫn đến **lỗi tràn bộ đệm (buffer overflow)** nếu input dài hơn `s`.
    * **Khuyến nghị:** **TUYỆT ĐỐI TRÁNH SỬ DỤNG `gets()`** trong các chương trình mới. Luôn dùng `fgets()` thay thế.

#### **3.8. I/O Định dạng: `printf`, `fprintf`, `sprintf`, `scanf`, `fscanf`, `sscanf` 🔢**

* **Lý thuyết:** Các hàm này cho phép bạn định dạng đầu ra hoặc phân tích cú pháp đầu vào bằng cách sử dụng các **chuỗi định dạng (format strings)** và **specifiers chuyển đổi (conversion specifiers)** (ví dụ: `%d`, `%s`).
* **Đầu ra Định dạng (`printf` family):**
  * **`int printf(const char *format, ...);`** : Ghi ra `stdout`.
  * **`int fprintf(FILE *stream, const char *format, ...);`** : Ghi ra một `stream` cụ thể.
  * **`int sprintf(char *s, const char *format, ...);`** : Ghi vào một chuỗi `s` (cần đảm bảo `s` đủ lớn).
  * **Conversion Specifiers:** `%d` (int thập phân), `%f` (float), `%s` (chuỗi), `%c` (char), `%x` (hex), v.v.
  * **Field Specifiers:** Kiểm soát khoảng trắng, căn chỉnh, số chữ số thập phân (ví dụ: `%10s`, `%.2f`).
  * **Quan trọng:** Đảm bảo số lượng và kiểu của đối số khớp chính xác với specifiers trong chuỗi định dạng để tránh lỗi runtime.
* **Đầu vào Định dạng (`scanf` family):**
  * **`int scanf(const char *format, ...);`** : Đọc từ `stdin`.
  * **`int fscanf(FILE *stream, const char *format, ...);`** : Đọc từ một `stream` cụ thể.
  * **`int sscanf(const char *s, const char *format, ...);`** : Đọc từ một chuỗi `s`.
  * **Lưu ý:** Các biến để lưu giá trị đọc được phải là **con trỏ** (`&num`, `&str_var`).
  * **Cẩn thận:**
    * **Không an toàn cho chuỗi:** Tương tự `gets()`, `%s` trong `scanf` không giới hạn số ký tự đọc và có thể gây tràn bộ đệm. **Luôn sử dụng field width specifier** (ví dụ: `%99s` để đọc tối đa 99 ký tự vào buffer 100 byte) hoặc kết hợp `fgets()` với `sscanf()`.
    * **Xử lý khoảng trắng:** `%d`, `%s` thường bỏ qua khoảng trắng đầu. `%c` thì không.
    * **Lỗi vòng lặp vô hạn:** Nếu có ký tự không hợp lệ trong input khi `scanf` mong đợi một kiểu dữ liệu cụ thể (ví dụ: một chữ cái khi mong đợi số), nó sẽ không đọc ký tự đó và có thể dẫn đến vòng lặp vô hạn nếu không có xử lý lỗi.
  * **Khuyến nghị:** Trong lập trình an toàn và mạnh mẽ, thường **không khuyến nghị sử dụng `scanf` family** để đọc input phức tạp từ người dùng. Thay vào đó, hãy đọc toàn bộ dòng bằng `fgets()` và sau đó phân tích cú pháp chuỗi đó bằng `sscanf()` hoặc các hàm xử lý chuỗi khác.

#### **3.9. Xử lý Lỗi Stream (`errno`, `ferror`, `feof`, `clearerr`) ❌**

* **Lý thuyết:** Khi các hàm `stdio` gặp lỗi hoặc đạt EOF, chúng thường trả về các giá trị đặc biệt (`NULL`, `EOF`) và/hoặc thiết lập biến toàn cục `errno`.
  * **`extern int errno;`** : Biến toàn cục này lưu trữ mã lỗi.  **Luôn kiểm tra `errno` ngay sau khi một hàm báo lỗi** , vì các hàm khác (kể cả `printf`) có thể thay đổi giá trị của nó.
  * **`int ferror(FILE *stream);`** : Kiểm tra chỉ báo lỗi của `stream`. Trả về khác `0` nếu có lỗi.
  * **`int feof(FILE *stream);`** : Kiểm tra chỉ báo End-Of-File của `stream`. Trả về khác `0` nếu đã đạt EOF.
  * **`void clearerr(FILE *stream);`** : Xóa cả chỉ báo lỗi và EOF cho `stream`. Hữu ích để khôi phục sau lỗi và tiếp tục thao tác.
* **Ví dụ (`copy_stdio.c`):** Đọc từng ký tự và ghi lại, minh họa hiệu suất khi dùng `stdio` (0.35 giây) so với System Calls từng ký tự (2.5 phút), nhưng vẫn chậm hơn System Calls đọc theo block (0.01 giây). Điều này cho thấy buffering của `stdio` hiệu quả hơn rất nhiều so với System Calls không đệm.

---

### **Câu hỏi Tự đánh giá Module 3 🤔**

1. Giải thích sự khác biệt cơ bản về cách hoạt động giữa một System Call (`read()`, `write()`) và một hàm thư viện `stdio` (`fread()`, `fwrite()`). Lợi ích chính của `stdio` là gì?
2. Phân biệt giữa `FILE *` (stream) và `int` (file descriptor). Làm thế nào để chuyển đổi giữa chúng trong chương trình C?
3. Khi nào bạn sẽ sử dụng `fopen()` với mode `"w"` so với `"a"`?
4. Viết một đoạn code C ngắn sử dụng `fopen()` và `fprintf()` để ghi 3 dòng văn bản vào một file `log.txt`, đảm bảo mỗi dòng được ghi ra đĩa ngay lập tức sau khi gọi `fprintf()`.
5. Tại sao việc sử dụng `gets()` được coi là không an toàn? Nêu một cách thay thế an toàn hơn.
6. Khi nào bạn cần sử dụng `fflush()`? `fflush(NULL)` có ý nghĩa gì?
7. Nêu hai lý do tại sao `scanf()` family không được khuyến nghị để đọc input từ người dùng trong các ứng dụng quan trọng.

---

### **Bài tập Thực hành Module 3 ✍️**

1. **Chương trình Đếm Từ và Ký tự:**
   * Viết một chương trình C (`word_char_counter.c`) nhận một tham số dòng lệnh là tên file.
   * Chương trình sẽ mở file đó bằng `fopen()` ở chế độ đọc.
   * Đọc nội dung file từng dòng (sử dụng `fgets()`).
   * Đối với mỗi dòng, đếm số ký tự và số từ.
   * Hiển thị tổng số dòng, tổng số ký tự, và tổng số từ trong file.
   * Sử dụng `feof()` và `ferror()` để kiểm tra cuối file hoặc lỗi khi đọc.
   * Nếu không có tham số dòng lệnh, đọc từ `stdin`.
   * **Thử thách:** Xử lý các ký tự xuống dòng và khoảng trắng thừa một cách chính xác trong việc đếm từ.
2. **Chương trình Ghi Log với `fflush()`:**
   * Viết một chương trình C (`event_logger.c`) có một hàm `log_event(const char *message)` nhận vào một chuỗi thông báo.
   * Hàm này sẽ mở một file `application.log` ở chế độ append (`"a"`).
   * Ghi thông báo cùng với dấu thời gian hiện tại vào file (sử dụng `fprintf` và các hàm thời gian như `time`, `localtime`, `strftime`).
   * Sau mỗi lần ghi, sử dụng `fflush()` để đảm bảo dữ liệu được ghi ngay lập tức xuống đĩa.
   * `main()` sẽ gọi `log_event()` nhiều lần với các thông báo khác nhau và kiểm tra nội dung file log sau đó.
   * **Thử thách:** Thay đổi `fflush()` bằng `fclose()` sau mỗi lần ghi và quan sát sự khác biệt về hiệu suất (sẽ chậm hơn).
3. **So sánh Hiệu suất `read`/`write` và `fread`/`fwrite` (Nâng cao):**
   * Dựa trên bài tập sao chép file từ Module 2 và `copy_stdio.c` trong bài học này, tạo hai phiên bản:
     * `copy_syscall.c`: Chỉ dùng `open`, `read`, `write`, `close`.
     * `copy_stdio.c`: Chỉ dùng `fopen`, `fread`, `fwrite`, `fclose`.
   * Sử dụng cùng kích thước buffer (ví dụ: 4KB hoặc 8KB) cho cả hai phiên bản.
   * Chạy cả hai chương trình để sao chép một file lớn (ví dụ: 10MB) và đo thời gian thực thi bằng lệnh `time` của shell.
   * So sánh kết quả và giải thích tại sao phiên bản `stdio` lại nhanh hơn phiên bản `syscall` khi đọc/ghi từng ký tự, nhưng lại có thể chậm hơn khi phiên bản `syscall` đọc/ghi theo khối lớn.

---
