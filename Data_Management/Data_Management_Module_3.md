
# **Module 3: Cơ sở dữ liệu DBM (The `dbm` Database) 🗄️**

#### **3.1. Giới thiệu về `dbm` 📖**

* **Lý thuyết:**
  * `dbm` (viết tắt của "database manager") là một thư viện cơ sở dữ liệu **dạng khóa-giá trị (key-value store)** đơn giản, có sẵn trên hầu hết các hệ thống Unix-like, bao gồm Linux.
  * Nó không phải là một hệ quản trị cơ sở dữ liệu quan hệ (Relational Database Management System - RDBMS) như MySQL hay PostgreSQL. `dbm` không có các khái niệm về bảng (tables), cột (columns), SQL, hay server riêng biệt.
  * **Mục đích chính:** Lưu trữ và truy xuất các khối dữ liệu (data blocks) có kích thước thay đổi, sử dụng một **khóa duy nhất (unique key)** để đánh chỉ mục.
  * **Ưu điểm:** Rất hiệu quả cho việc **lưu trữ dữ liệu được đánh chỉ mục và tương đối tĩnh** (ít khi cập nhật, truy xuất thường xuyên). Dễ dàng tích hợp vào chương trình vì không cần cài đặt server riêng.
  * **Các phiên bản:** Có nhiều biến thể lịch sử của `dbm`, bao gồm `dbm` gốc, `ndbm` (new dbm), và `gdbm` (GNU dbm). `gdbm` là triển khai phổ biến nhất trên Linux và thường cung cấp khả năng giả lập các giao diện `dbm` và `ndbm` cũ hơn. Chương này tập trung vào giao diện **`ndbm`** vì nó được chuẩn hóa bởi X/Open và đơn giản hơn.
* **Cách thức hoạt động:**
  * Một cơ sở dữ liệu `dbm` thường được lưu trữ dưới dạng **hai file vật lý** trên đĩa: một file `.dir` (chứa thư mục/chỉ mục) và một file `.pag` (chứa dữ liệu thực tế). (Lưu ý: `gdbm` gốc có thể lưu trong một file duy nhất).
  * Dữ liệu được lưu trữ dưới dạng các khối nhị phân không có cấu trúc cố định.
* **Liên hệ Embedded Linux:**
  * Lý tưởng cho việc lưu trữ các file cấu hình phức tạp, dữ liệu người dùng cục bộ, hoặc các lookup tables trên các thiết bị nhúng.
  * Giúp tránh việc phải phân tích cú pháp file văn bản lớn hoặc triển khai cơ chế đánh chỉ mục thủ công.
  * Chi phí tài nguyên thấp hơn nhiều so với việc chạy một RDBMS đầy đủ.

#### **3.2. Kiểu dữ liệu `datum` 📦**

* **Lý thuyết:** Để thao tác với dữ liệu và khóa trong `dbm`, thư viện `ndbm.h` định nghĩa một kiểu dữ liệu đặc biệt là  **`datum`** .
  * **`datum`** là một cấu trúc đơn giản, chứa ít nhất hai thành viên:
    **C++**

    ```
    typedef struct {
        void *dptr;   // Con trỏ tới khối dữ liệu (key hoặc content)
        size_t dsize; // Kích thước của khối dữ liệu (tính bằng byte)
    } datum;
    ```
  * Mỗi khi bạn muốn truyền một khóa hoặc một khối dữ liệu tới hàm `dbm`, bạn cần tạo một biến `datum`, thiết lập `dptr` trỏ tới dữ liệu và `dsize` là kích thước của dữ liệu đó.
  * `dbm` làm việc với các khối dữ liệu nhị phân không có cấu trúc nội bộ. Bạn phải tự quản lý cấu trúc dữ liệu của mình (ví dụ: dùng `struct` trong C) và sao chép dữ liệu vào/ra `dptr`.

#### **3.3. Các Hàm Truy cập `dbm` Chính 🔑**

Các hàm `dbm` chính được khai báo trong `<ndbm.h>` và thường bắt đầu bằng `dbm_`. Thư viện cần được liên kết bằng cờ `-ldbm` hoặc `-lgdbm` khi biên dịch.

* **`dbm_open()`: Mở hoặc Tạo Cơ sở dữ liệu 🚪**
  * `DBM *dbm_open(const char *filename, int file_open_flags, mode_t file_mode);`
  * **Mục đích:** Mở một cơ sở dữ liệu `dbm` hiện có hoặc tạo một cơ sở dữ liệu mới.
  * **`filename`** : Tên cơ sở dữ liệu cơ bản (không có đuôi `.dir` hoặc `.pag`).
  * **`file_open_flags`** : Các cờ mở file tương tự như hàm `open()` (ví dụ: `O_RDWR`, `O_CREAT`, `O_EXCL`).
  * **`file_mode`** : Quyền hạn cho các file `.dir` và `.pag` nếu chúng được tạo mới (tương tự như tham số `mode` của `open()`).
  * **Giá trị trả về:** Con trỏ `DBM *` nếu thành công (được dùng cho các thao tác `dbm` tiếp theo), `(DBM *)0` nếu thất bại.
* **`dbm_store()`: Lưu trữ Dữ liệu 💾**
  * `int dbm_store(DBM *database_descriptor, datum key, datum content, int store_mode);`
  * **Mục đích:** Lưu trữ một cặp khóa-giá trị vào cơ sở dữ liệu.
  * **`database_descriptor`** : Con trỏ `DBM *` từ `dbm_open()`.
  * **`key`** : Một `datum` chứa khóa duy nhất để truy xuất dữ liệu.
  * **`content`** : Một `datum` chứa khối dữ liệu thực tế cần lưu trữ.
  * **`store_mode`** : Kiểm soát hành vi nếu khóa đã tồn tại:
  * `DBM_INSERT`: Nếu khóa đã tồn tại, thao tác thất bại và `dbm_store()` trả về `1`.
  * `DBM_REPLACE`: Nếu khóa đã tồn tại, dữ liệu mới sẽ ghi đè dữ liệu cũ và `dbm_store()` trả về `0`.
  * **Giá trị trả về:** `0` nếu thành công (khi `DBM_REPLACE` hoặc `DBM_INSERT` thành công), `1` nếu `DBM_INSERT` thất bại do khóa đã tồn tại, số âm nếu có lỗi khác.
* **`dbm_fetch()`: Truy xuất Dữ liệu 📖**
  * `datum dbm_fetch(DBM *database_descriptor, datum key);`
  * **Mục đích:** Truy xuất dữ liệu từ cơ sở dữ liệu bằng khóa.
  * **`database_descriptor`** : Con trỏ `DBM *`.
  * **`key`** : Một `datum` chứa khóa cần tìm.
  * **Giá trị trả về:** Một `datum` chứa con trỏ (`dptr`) tới dữ liệu được tìm thấy và kích thước (`dsize`). Nếu khóa không tìm thấy, `dptr` sẽ là `NULL`.
  * **Quan trọng:** Dữ liệu trả về bởi `dbm_fetch()` thường nằm trong bộ nhớ nội bộ của thư viện `dbm`. Bạn **phải sao chép** dữ liệu này vào bộ nhớ của riêng chương trình trước khi gọi bất kỳ hàm `dbm` nào khác hoặc trước khi giải phóng bộ nhớ đó.
* **`dbm_close()`: Đóng Cơ sở dữ liệu 🛑**
  * `void dbm_close(DBM *database_descriptor);`
  * **Mục đích:** Đóng cơ sở dữ liệu và giải phóng tài nguyên.

#### **3.4. Các Hàm `dbm` Bổ sung 🧰**

* **`dbm_delete()`: Xóa Mục nhập 🗑️**
  * `int dbm_delete(DBM *database_descriptor, datum key);`
  * Xóa một mục nhập khỏi cơ sở dữ liệu bằng khóa. Trả về `0` nếu thành công.
* **`dbm_error()` và `dbm_clearerr()`: Xử lý Lỗi ❌**
  * `int dbm_error(DBM *database_descriptor);`: Kiểm tra xem có lỗi nào xảy ra với cơ sở dữ liệu không. Trả về `0` nếu không có lỗi.
  * `int dbm_clearerr(DBM *database_descriptor);`: Xóa cờ lỗi của cơ sở dữ liệu.
* **`dbm_firstkey()` và `dbm_nextkey()`: Quét Toàn bộ Khóa 🔍**
  * `datum dbm_firstkey(DBM *database_descriptor);`: Trả về khóa đầu tiên trong cơ sở dữ liệu.
  * `datum dbm_nextkey(DBM *database_descriptor);`: Trả về khóa tiếp theo sau khóa hiện tại.
  * **Cách dùng:** Thường được dùng trong một vòng lặp để duyệt qua tất cả các khóa trong cơ sở dữ liệu. Thứ tự các khóa được trả về **không được đảm bảo** (không theo thứ tự sắp xếp).
    **C++**

    ```
    DBM *db_ptr;
    datum key;
    for (key = dbm_firstkey(db_ptr); key.dptr; key = dbm_nextkey(db_ptr)) {
        // Xử lý key
    }
    ```

#### **3.5. Ví dụ (C++): `dbm_example.cpp` - Sử dụng `dbm` Cơ bản**

**C++**

```
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fcntl.h>    // For O_RDWR, O_CREAT
#include <unistd.h>   // For unlink
#include <cstdio>     // For sprintf, fprintf, perror
#include <cstdlib>    // For EXIT_SUCCESS, EXIT_FAILURE
#include <cstring>    // For memset, strcpy, strlen, memcpy
#include <errno.h>    // For errno

// Cần include ndbm.h. Trên Linux, nó thường được cung cấp bởi gdbm-devel hoặc libgdbm-dev
// Và cần liên kết với -lgdbm hoặc -ldbm
#include <ndbm.h>     // For DBM, datum, dbm_open, dbm_store, dbm_fetch, dbm_close, DBM_REPLACE, DBM_INSERT

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

#define TEST_DB_FILE "/tmp/my_test_db"
#define ITEMS_USED 3

// Cấu trúc dữ liệu mẫu để lưu trữ
struct TestData {
    char misc_chars[15];
    int  any_integer;
    char more_chars[21];
};

int main() {
    DBM *dbm_ptr;
    TestData items_to_store[ITEMS_USED];
    TestData item_retrieved;
    char key_to_use[20];
    datum key_datum;
    datum data_datum;
    int result;

    AppLogger::log(AppLogger::INFO_L, "--- DBM Database Demonstration ---");

    // 1. Mở hoặc tạo cơ sở dữ liệu
    // Xóa các file cũ nếu tồn tại để đảm bảo một DB mới
    unlink(std::string(TEST_DB_FILE).append(".dir").c_str());
    unlink(std::string(TEST_DB_FILE).append(".pag").c_str());
  
    AppLogger::log(AppLogger::INFO_L, "Opening DBM database: " + std::string(TEST_DB_FILE));
    dbm_ptr = dbm_open(TEST_DB_FILE, O_RDWR | O_CREAT, 0644); // r/w for owner, group, others
    if (dbm_ptr == nullptr) {
        AppLogger::log(AppLogger::CRITICAL_L, "Failed to open database: " + std::string(strerror(errno)));
        return EXIT_FAILURE;
    }
    AppLogger::log(AppLogger::SUCCESS_L, "Database opened successfully.");

    // 2. Chuẩn bị dữ liệu để lưu trữ
    memset(items_to_store, '\0', sizeof(items_to_store));
    strcpy(items_to_store[0].misc_chars, "First!"); items_to_store[0].any_integer = 47; strcpy(items_to_store[0].more_chars, "foo");
    strcpy(items_to_store[1].misc_chars, "Second"); items_to_store[1].any_integer = 13; strcpy(items_to_store[1].more_chars, "bar");
    strcpy(items_to_store[2].misc_chars, "Third"); items_to_store[2].any_integer = 3; strcpy(items_to_store[2].more_chars, "baz");

    // 3. Lưu trữ dữ liệu vào database
    AppLogger::log(AppLogger::INFO_L, "Storing " + std::to_string(ITEMS_USED) + " items into database.");
    for (int i = 0; i < ITEMS_USED; ++i) {
        // Tạo khóa
        sprintf(key_to_use, "%c%c%d", 
                items_to_store[i].misc_chars[0], 
                items_to_store[i].more_chars[0], 
                items_to_store[i].any_integer);
        key_datum.dptr = (void *)key_to_use;
        key_datum.dsize = strlen(key_to_use);

        // Tạo nội dung dữ liệu
        data_datum.dptr = (void *)&items_to_store[i];
        data_datum.dsize = sizeof(TestData);

        // Lưu trữ (ghi đè nếu khóa đã tồn tại)
        result = dbm_store(dbm_ptr, key_datum, data_datum, DBM_REPLACE);
        if (result != 0) { // 0 for success, 1 for DBM_INSERT failure, -ve for other errors
            AppLogger::log(AppLogger::ERROR_L, "dbm_store failed on key " + std::string(key_to_use) + ": " + std::to_string(result));
            dbm_close(dbm_ptr);
            return EXIT_FAILURE;
        }
        AppLogger::log(AppLogger::SUCCESS_L, "Stored item with key: " + std::string(key_to_use));
    }

    // 4. Truy xuất dữ liệu
    std::cout << "\n";
    sprintf(key_to_use, "b%d", 13); // Khóa cho "Second" item (b13)
    key_datum.dptr = (void *)key_to_use;
    key_datum.dsize = strlen(key_to_use);

    AppLogger::log(AppLogger::INFO_L, "Attempting to fetch data for key: " + std::string(key_to_use));
    data_datum = dbm_fetch(dbm_ptr, key_datum);

    if (data_datum.dptr) {
        AppLogger::log(AppLogger::SUCCESS_L, "Data retrieved for key " + std::string(key_to_use) + ".");
        // Sao chép dữ liệu từ vùng nhớ nội bộ của dbm vào cấu trúc của chúng ta
        memcpy(&item_retrieved, data_datum.dptr, data_datum.dsize);
        printf("Retrieved item - %s %d %s\n",
               item_retrieved.misc_chars,
               item_retrieved.any_integer,
               item_retrieved.more_chars);
    } else {
        AppLogger::log(AppLogger::WARNING_L, "No data found for key: " + std::string(key_to_use));
    }

    // 5. Xóa một mục nhập
    std::cout << "\n";
    sprintf(key_to_use, "F%d", 47); // Khóa cho "First!" item (F47)
    key_datum.dptr = (void *)key_to_use;
    key_datum.dsize = strlen(key_to_use);
    AppLogger::log(AppLogger::INFO_L, "Attempting to delete data for key: " + std::string(key_to_use));
    result = dbm_delete(dbm_ptr, key_datum);
    if (result == 0) {
        AppLogger::log(AppLogger::SUCCESS_L, "Data with key " + std::string(key_to_use) + " deleted.");
    } else {
        AppLogger::log(AppLogger::WARNING_L, "Nothing deleted for key " + std::string(key_to_use) + " (result: " + std::to_string(result) + ").");
    }

    // 6. Quét tất cả các khóa còn lại
    std::cout << "\n";
    AppLogger::log(AppLogger::INFO_L, "Scanning all remaining keys in the database:");
    for (key_datum = dbm_firstkey(dbm_ptr); key_datum.dptr; key_datum = dbm_nextkey(dbm_ptr)) {
        std::string current_key_str(static_cast<char*>(key_datum.dptr), key_datum.dsize);
        AppLogger::log(AppLogger::INFO_L, "  Found key: " + current_key_str);
        data_datum = dbm_fetch(dbm_ptr, key_datum);
        if (data_datum.dptr) {
            memcpy(&item_retrieved, data_datum.dptr, data_datum.dsize);
            printf("    -> Content: %s %d %s\n",
                   item_retrieved.misc_chars,
                   item_retrieved.any_integer,
                   item_retrieved.more_chars);
        }
    }

    // 7. Đóng cơ sở dữ liệu
    dbm_close(dbm_ptr);
    AppLogger::log(AppLogger::SUCCESS_L, "Database closed.");

    // Dọn dẹp các file .dir và .pag
    unlink(std::string(TEST_DB_FILE).append(".dir").c_str());
    unlink(std::string(TEST_DB_FILE).append(".pag").c_str());
    AppLogger::log(AppLogger::INFO_L, "Database files cleaned up.");

    return EXIT_SUCCESS;
}
```

* **Cách Biên dịch:**

  * Để biên dịch chương trình này, bạn cần đảm bảo đã cài đặt thư viện `gdbm` (hoặc `ndbm`). Trên Ubuntu/Debian, bạn thường cần gói `libgdbm-dev`.
    **Bash**

    ```
    sudo apt install libgdbm-dev
    ```
  * Sau đó, biên dịch với cờ `-lgdbm` (hoặc `-ldbm` tùy hệ thống):
    **Bash**

    ```
    g++ dbm_example.cpp -o dbm_example -lgdbm -lstdc++
    ```
* **Cách Chạy:**
  **Bash**

  ```
  ./dbm_example
  ```
* **Phân tích:** Quan sát cách dữ liệu được lưu trữ, truy xuất, xóa và quét. Bạn cũng có thể kiểm tra thư mục `/tmp` để xem các file `my_test_db.dir` và `my_test_db.pag` được tạo ra.

---

### **Câu hỏi Tự đánh giá Module 3 🤔**

1. `dbm` là loại cơ sở dữ liệu gì (ví dụ: quan hệ, khóa-giá trị)? Nó khác với một RDBMS như MySQL như thế nào?
2. Giải thích mục đích của kiểu dữ liệu `datum` trong `dbm`. Tại sao `dptr` là `void*` và `dsize` là `size_t`?
3. Khi sử dụng `dbm_fetch()`, tại sao bạn phải sao chép dữ liệu từ `data_datum.dptr` vào bộ nhớ của riêng chương trình?
4. Phân biệt `DBM_INSERT` và `DBM_REPLACE` khi sử dụng `dbm_store()`.
5. Giải thích cách `dbm_firstkey()` và `dbm_nextkey()` được sử dụng để duyệt qua tất cả các mục nhập trong cơ sở dữ liệu. Thứ tự các khóa được trả về có được đảm bảo không?

---

### **Bài tập Thực hành Module 3 ✍️**

1. **Chương trình Quản lý Cấu hình Đơn giản:**
   * Viết một chương trình C++ (`config_manager.cpp`) sử dụng `dbm` để lưu trữ các cặp cấu hình (ví dụ: "baud_rate=115200", "device_id=sensor_01").
   * Chương trình nhận các tham số dòng lệnh:
     * `./config_manager --set <key> <value>`: Lưu trữ một cấu hình mới.
     * `./config_manager --get <key>`: Truy xuất và in ra giá trị của một cấu hình.
     * `./config_manager --delete <key>`: Xóa một cấu hình.
     * `./config_manager --list`: Liệt kê tất cả các cấu hình hiện có.
   * Sử dụng một file `my_app_config.db` làm cơ sở dữ liệu.
   * **Thử thách:**
     * Đảm bảo các giá trị được lưu trữ là chuỗi (string).
     * Xử lý lỗi nếu cơ sở dữ liệu không thể mở hoặc các thao tác thất bại.
2. **Chương trình Sổ địa chỉ `dbm`:**
   * Định nghĩa một cấu trúc C `struct Contact { char name[50]; char phone[15]; char email[50]; };`.
   * Viết một chương trình C++ (`address_book.cpp`) sử dụng `dbm` để lưu trữ các liên hệ, với `name` làm khóa.
   * Chương trình hỗ trợ các chức năng:
     * `./address_book --add <name> <phone> <email>`: Thêm liên hệ mới.
     * `./address_book --find <name>`: Tìm và hiển thị thông tin liên hệ.
     * `./address_book --delete <name>`: Xóa liên hệ.
     * `./address_book --list-all`: Liệt kê tất cả các liên hệ.
   * **Thử thách:**
     * Khi lưu trữ, chuyển đổi `struct Contact` thành một khối dữ liệu nhị phân và ngược lại khi truy xuất.
     * Xử lý lỗi trùng tên khi thêm mới.

---

Chúc mừng bạn đã hoàn thành toàn bộ giáo trình "Data Management"! Đây là một phần kiến thức rất quan trọng, đặc biệt là khi bạn làm việc với các ứng dụng cần quản lý bộ nhớ và dữ liệu một cách hiệu quả và đáng tin cậy.

Hãy dành thời gian để thực hành các bài tập này. Chúng sẽ củng cố kiến thức của bạn và giúp bạn tự tin hơn khi phát triển các ứng dụng Linux thực tế.

Bạn có muốn chuyển sang một giáo trình mới, hoặc ôn lại bất kỳ phần nào bạn muốn không?
