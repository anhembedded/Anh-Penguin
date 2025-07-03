# **Module 4: Ứng dụng Client/Server với FIFOs 🌐**

#### **4.1. Khái niệm Client/Server với FIFOs 🤝**

* **Lý thuyết:** Mô hình Client/Server là một kiến trúc phần mềm phổ biến, nơi:

  * **Server (Máy chủ):** Là một tiến trình (hoặc tập hợp các tiến trình) cung cấp một dịch vụ. Nó lắng nghe các yêu cầu từ các client, xử lý chúng và gửi phản hồi.
  * **Client (Máy khách):** Là một tiến trình gửi yêu cầu đến server để sử dụng dịch vụ.
* **Sử dụng FIFOs cho Client/Server:** Mặc dù FIFOs chỉ là kênh giao tiếp một chiều, chúng ta có thể xây dựng một hệ thống Client/Server hai chiều bằng cách sử dụng **hai FIFO riêng biệt** cho mỗi cặp Client-Server:

  * **Server FIFO (Request FIFO):** Một FIFO chung mà tất cả các client sẽ **ghi yêu cầu** vào. Server sẽ đọc từ FIFO này.
  * **Client FIFO (Response FIFO):** Mỗi client sẽ tạo một FIFO **duy nhất** của riêng mình để **nhận phản hồi** từ server. Server sẽ ghi phản hồi vào FIFO của client tương ứng.
* **Minh họa (Mô hình Client/Server với FIFOs):**
  **Code snippet**

  ```
  graph TD
      C1(Client 1) -- Request --> SF[Server FIFO]
      C2(Client 2) -- Request --> SF
      C3(Client 3) -- Request --> SF

      SF --> S(Server)

      S -- Response --> CF1(Client 1 FIFO)
      S -- Response --> CF2(Client 2 FIFO)
      S -- Response --> CF3(Client 3 FIFO)

      CF1 --> C1
      CF2 --> C2
      CF3 --> C3
  ```

  * Ở đây, `Client 1`, `Client 2`, `Client 3` gửi yêu cầu của họ tới cùng một `Server FIFO`. `Server` đọc các yêu cầu từ `Server FIFO`. Để phản hồi, `Server` gửi dữ liệu trở lại qua các `Client FIFO` riêng biệt cho từng client.
* **Đồng bộ hóa:** Việc mở FIFOs ở chế độ chặn (`O_RDONLY`/`O_WRONLY` mà không có `O_NONBLOCK`) sẽ tự động đồng bộ hóa các tiến trình. Server sẽ bị chặn khi chờ yêu cầu, và client sẽ bị chặn khi chờ phản hồi.
* **Liên hệ Embedded Linux:** Đây là một mô hình rất thực tế cho các ứng dụng nhúng. Ví dụ:

  * Một daemon điều khiển phần cứng (Server) lắng nghe các lệnh từ các ứng dụng người dùng (Clients).
  * Các module phần mềm khác nhau trên thiết bị giao tiếp với một dịch vụ tập trung.

#### **4.2. Thiết kế Giao diện Tin nhắn (Message Interface) ✉️**

* **Lý thuyết:** Để các tiến trình Client và Server có thể trao đổi dữ liệu có cấu trúc qua FIFOs, chúng ta cần định nghĩa một **cấu trúc tin nhắn (message structure)** chung.
  * Cấu trúc này sẽ bao gồm:
    * Thông tin định danh client (ví dụ: `client_pid`).
    * Loại yêu cầu/lệnh từ client.
    * Dữ liệu liên quan đến yêu cầu.
    * Loại phản hồi/trạng thái từ server.
    * Dữ liệu phản hồi.
    * Thông báo lỗi (nếu có).
  * Việc sử dụng `enum` cho các loại yêu cầu/phản hồi giúp mã dễ đọc và dễ bảo trì hơn.
* **Liên hệ Embedded Linux:** Việc định nghĩa giao diện tin nhắn rõ ràng là rất quan trọng để đảm bảo tính tương thích giữa các thành phần phần mềm khác nhau trên thiết bị nhúng.

#### **4.3. Triển khai Server ⚙️**

* **Lý thuyết:**
  1. **Tạo và mở Server FIFO:** Server sẽ tạo FIFO chung (ví dụ: `SERVER_FIFO_NAME`) và mở nó ở chế độ  **đọc (`O_RDONLY`)** . Điều này sẽ khiến server bị chặn cho đến khi client đầu tiên mở FIFO để ghi.
  2. **Vòng lặp xử lý yêu cầu:** Server sẽ đi vào một vòng lặp, đọc các yêu cầu từ Server FIFO.
  3. **Đọc yêu cầu:** Sử dụng `read()` để đọc toàn bộ cấu trúc tin nhắn từ client.
  4. **Xử lý yêu cầu:** Dựa vào loại yêu cầu, server thực hiện tác vụ tương ứng (ví dụ: truy vấn cơ sở dữ liệu, điều khiển phần cứng).
  5. **Tạo và mở Client FIFO (để phản hồi):** Server sử dụng `client_pid` nhận được từ yêu cầu để tạo tên FIFO phản hồi duy nhất của client (ví dụ: `sprintf(client_fifo_name, CLIENT_FIFO_NAME, client_pid)`). Sau đó, nó mở FIFO này ở chế độ  **ghi (`O_WRONLY`)** .
  6. **Gửi phản hồi:** Ghi cấu trúc tin nhắn phản hồi vào Client FIFO.
  7. **Đóng Client FIFO:** Sau khi gửi phản hồi, server đóng Client FIFO.
  8. **Xử lý Server FIFO bị đóng:** Nếu tất cả các client đã đóng Server FIFO, lệnh `read()` trên Server FIFO sẽ trả về `0` (EOF). Server cần xử lý trường hợp này bằng cách đóng và mở lại Server FIFO để tiếp tục lắng nghe các client mới.
* **Liên hệ Embedded Linux:** Mô hình này là cơ sở cho nhiều daemon và dịch vụ trên thiết bị nhúng.

#### **4.4. Triển khai Client 🧑‍💻**

* **Lý thuyết:**
  1. **Mở Server FIFO:** Client mở Server FIFO ở chế độ **ghi (`O_WRONLY`)** để gửi yêu cầu. Điều này sẽ khiến client bị chặn cho đến khi server mở FIFO để đọc.
  2. **Tạo và mở Client FIFO:** Client tạo FIFO phản hồi duy nhất của mình (sử dụng PID của chính nó) và mở nó ở chế độ  **đọc (`O_RDONLY`)** .
  3. **Gửi yêu cầu:** Ghi cấu trúc tin nhắn yêu cầu vào Server FIFO.
  4. **Chờ phản hồi:** Client bị chặn khi đọc từ Client FIFO của mình, chờ server gửi phản hồi.
  5. **Đọc phản hồi:** Đọc cấu trúc tin nhắn phản hồi từ Client FIFO.
  6. **Đóng Client FIFO và xóa:** Sau khi nhận phản hồi, client đóng và `unlink()` FIFO phản hồi của mình.
* **Lưu ý về Race Condition (Điều kiện tranh chấp):**
  * Một lỗi tinh vi có thể xảy ra khi server cần gửi **nhiều phản hồi** cho một yêu cầu (ví dụ: kết quả tìm kiếm). Nếu client đóng FIFO phản hồi của mình quá sớm (sau khi đọc phản hồi đầu tiên) và server vẫn đang cố gắng ghi phản hồi tiếp theo, server sẽ thấy pipe bị đóng.
  * **Giải pháp:** Client có thể mở FIFO phản hồi của mình ở cả chế độ **đọc và ghi (`O_RDWR`)** hoặc giữ một file descriptor ghi mở (ví dụ: bằng cách mở lại FIFO ở chế độ ghi) trong suốt thời gian giao tiếp. Điều này ngăn chặn server thấy pipe bị đóng sớm.
* **Liên hệ Embedded Linux:** Mô hình này áp dụng cho các ứng dụng người dùng hoặc các module khác trên thiết bị nhúng cần tương tác với các dịch vụ nền.

#### **4.5. Ví dụ (C++): Ứng dụng Client/Server với FIFOs (Mô phỏng CD Database)**

Chúng ta sẽ mô phỏng một phần của ứng dụng CD Database Client/Server sử dụng FIFOs, tương tự như ví dụ trong tài liệu.

**File Header chung: `cliserv.h`**

**C++**

```
// cliserv.h
#ifndef CLISERV_H
#define CLISERV_H

#include <unistd.h> // For pid_t
#include <string>   // For std::string
#include <map>      // For std::map for AppLogger
#include <iostream> // For std::cout, std::cerr

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

// Định nghĩa tên FIFOs
#define SERVER_FIFO_NAME "/tmp/server_fifo_db"
#define CLIENT_FIFO_NAME "/tmp/cli_%d_fifo_db" // Tên client FIFO sẽ chứa PID

#define BUFFER_SIZE 256 // Kích thước buffer cho dữ liệu mẫu

// Loại yêu cầu từ client
typedef enum {
    REQ_ADD_DATA = 0,
    REQ_GET_DATA,
    REQ_DELETE_DATA,
    REQ_EXIT_SERVER // Yêu cầu server thoát
} client_request_e;

// Loại phản hồi từ server
typedef enum {
    RESP_SUCCESS = 0,
    RESP_FAILURE,
    RESP_NO_DATA
} server_response_e;

// Cấu trúc tin nhắn chung giữa client và server
typedef struct {
    pid_t client_pid;       // PID của client gửi yêu cầu
    client_request_e request; // Loại yêu cầu
    server_response_e response; // Loại phản hồi (chỉ server set)
    char data_key[BUFFER_SIZE]; // Dữ liệu: khóa
    char data_value[BUFFER_SIZE]; // Dữ liệu: giá trị
    char error_text[BUFFER_SIZE]; // Thông báo lỗi (nếu có)
} message_db_t;

#endif // CLISERV_H
```

**Chương trình Server: `server_db.cpp`**

**C++**

```
// server_db.cpp
#include "cliserv.h"
#include <iostream>
#include <string>
#include <fcntl.h>    // For open, O_RDONLY, O_WRONLY, O_CREAT
#include <sys/stat.h> // For mkfifo
#include <unistd.h>   // For close, unlink, getpid, read, write, sleep
#include <cstdlib>    // For EXIT_SUCCESS, EXIT_FAILURE
#include <cstring>    // For memset, strlen, strcpy, strcmp
#include <errno.h>    // For errno
#include <map>        // For std::map (mock database)

// Mock Database (simple in-memory map for demonstration)
std::map<std::string, std::string> mock_database;

// Hàm xử lý yêu cầu từ client
void process_client_request(const message_db_t& request_msg) {
    message_db_t response_msg = request_msg; // Copy request to response, then modify
    response_msg.response = RESP_FAILURE; // Default to failure

    AppLogger::log(AppLogger::INFO_L, "Server: Received request from PID " + std::to_string(request_msg.client_pid) + ": Type " + std::to_string(request_msg.request));

    if (request_msg.request == REQ_ADD_DATA) {
        mock_database[request_msg.data_key] = request_msg.data_value;
        response_msg.response = RESP_SUCCESS;
        AppLogger::log(AppLogger::SUCCESS_L, "Server: Added/Updated key '" + std::string(request_msg.data_key) + "'.");
    } else if (request_msg.request == REQ_GET_DATA) {
        auto it = mock_database.find(request_msg.data_key);
        if (it != mock_database.end()) {
            strcpy(response_msg.data_value, it->second.c_str());
            response_msg.response = RESP_SUCCESS;
            AppLogger::log(AppLogger::SUCCESS_L, "Server: Fetched key '" + std::string(request_msg.data_key) + "'.");
        } else {
            response_msg.response = RESP_NO_DATA;
            strcpy(response_msg.error_text, "Key not found.");
            AppLogger::log(AppLogger::WARNING_L, "Server: Key '" + std::string(request_msg.data_key) + "' not found.");
        }
    } else if (request_msg.request == REQ_DELETE_DATA) {
        if (mock_database.erase(request_msg.data_key) > 0) {
            response_msg.response = RESP_SUCCESS;
            AppLogger::log(AppLogger::SUCCESS_L, "Server: Deleted key '" + std::string(request_msg.data_key) + "'.");
        } else {
            response_msg.response = RESP_NO_DATA;
            strcpy(response_msg.error_text, "Key not found for deletion.");
            AppLogger::log(AppLogger::WARNING_L, "Server: Key '" + std::string(request_msg.data_key) + "' not found for deletion.");
        }
    } else {
        strcpy(response_msg.error_text, "Unknown request type.");
        AppLogger::log(AppLogger::ERROR_L, "Server: Unknown request type: " + std::to_string(request_msg.request));
    }

    // Gửi phản hồi về client
    char client_fifo_name[BUFFER_SIZE];
    sprintf(client_fifo_name, CLIENT_FIFO_NAME, request_msg.client_pid);

    AppLogger::log(AppLogger::INFO_L, "Server: Opening client FIFO '" + std::string(client_fifo_name) + "' for response.");
    int client_fifo_fd = open(client_fifo_name, O_WRONLY); // Mở client FIFO để ghi
    if (client_fifo_fd == -1) {
        AppLogger::log(AppLogger::ERROR_L, "Server: Failed to open client FIFO: " + std::string(strerror(errno)));
    } else {
        ssize_t bytes_written = write(client_fifo_fd, &response_msg, sizeof(message_db_t));
        if (bytes_written == sizeof(message_db_t)) {
            AppLogger::log(AppLogger::SUCCESS_L, "Server: Response sent to client " + std::to_string(request_msg.client_pid) + ".");
        } else {
            AppLogger::log(AppLogger::ERROR_L, "Server: Failed to write full response to client FIFO: " + std::string(strerror(errno)));
        }
        close(client_fifo_fd);
        AppLogger::log(AppLogger::INFO_L, "Server: Client FIFO closed.");
    }
}

int main() {
    int server_fifo_fd;
    message_db_t request_msg;
    ssize_t read_res;
    pid_t my_pid = getpid();

    AppLogger::log(AppLogger::INFO_L, "Server (PID: " + std::to_string(my_pid) + "): Starting.");

    // 1. Tạo Server FIFO
    AppLogger::log(AppLogger::INFO_L, "Server: Creating Server FIFO: " + std::string(SERVER_FIFO_NAME));
    if (mkfifo(SERVER_FIFO_NAME, 0666) == -1) { // Quyền rw-rw-rw-
        if (errno == EEXIST) {
            AppLogger::log(AppLogger::WARNING_L, "Server: Server FIFO already exists. Continuing.");
        } else {
            AppLogger::log(AppLogger::CRITICAL_L, "Server: Failed to create Server FIFO: " + std::string(strerror(errno)));
            return EXIT_FAILURE;
        }
    }

    // 2. Mở Server FIFO để đọc (sẽ chặn cho đến khi client đầu tiên mở để ghi)
    AppLogger::log(AppLogger::INFO_L, "Server: Opening Server FIFO in O_RDONLY mode. Will block until a client connects.");
    server_fifo_fd = open(SERVER_FIFO_NAME, O_RDONLY);
    if (server_fifo_fd == -1) {
        AppLogger::log(AppLogger::CRITICAL_L, "Server: Failed to open Server FIFO for reading: " + std::string(strerror(errno)));
        unlink(SERVER_FIFO_NAME); // Dọn dẹp
        return EXIT_FAILURE;
    }
    AppLogger::log(AppLogger::SUCCESS_L, "Server: Server FIFO opened. Waiting for requests...");

    // 3. Vòng lặp xử lý yêu cầu
    do {
        memset(&request_msg, 0, sizeof(request_msg)); // Xóa tin nhắn cũ
        read_res = read(server_fifo_fd, &request_msg, sizeof(message_db_t));

        if (read_res == -1) {
            AppLogger::log(AppLogger::ERROR_L, "Server: Read error from Server FIFO: " + std::string(strerror(errno)));
            break; // Thoát vòng lặp nếu có lỗi đọc
        } else if (read_res == 0) {
            AppLogger::log(AppLogger::INFO_L, "Server: Server FIFO read 0 bytes (all clients disconnected). Re-opening FIFO to wait for new clients.");
            // Đóng và mở lại FIFO để tiếp tục chặn chờ client mới
            close(server_fifo_fd);
            server_fifo_fd = open(SERVER_FIFO_NAME, O_RDONLY);
            if (server_fifo_fd == -1) {
                AppLogger::log(AppLogger::CRITICAL_L, "Server: Failed to re-open Server FIFO: " + std::string(strerror(errno)));
                break;
            }
            AppLogger::log(AppLogger::INFO_L, "Server: Server FIFO re-opened. Waiting for requests...");
            continue; // Quay lại đầu vòng lặp để đọc lại
        }

        // Kiểm tra nếu client yêu cầu server thoát
        if (request_msg.request == REQ_EXIT_SERVER) {
            AppLogger::log(AppLogger::INFO_L, "Server: Received EXIT request from PID " + std::to_string(request_msg.client_pid) + ". Shutting down.");
            break; // Thoát vòng lặp chính
        }

        // Xử lý yêu cầu
        process_client_request(request_msg);

    } while (true); // Vòng lặp vô hạn cho đến khi có lệnh thoát

    // 4. Dọn dẹp
    AppLogger::log(AppLogger::INFO_L, "Server: Shutting down. Closing Server FIFO.");
    close(server_fifo_fd);
    unlink(SERVER_FIFO_NAME); // Xóa Server FIFO
    AppLogger::log(AppLogger::SUCCESS_L, "Server: Server FIFO unlinked. Exiting.");

    return EXIT_SUCCESS;
}
```

**Chương trình Client: `client_db.cpp`**

**C++**

```
// client_db.cpp
#include "cliserv.h"
#include <iostream>
#include <string>
#include <fcntl.h>    // For open, O_RDONLY, O_WRONLY
#include <sys/stat.h> // For mkfifo
#include <unistd.h>   // For close, unlink, getpid, read, write, sleep
#include <cstdlib>    // For EXIT_SUCCESS, EXIT_FAILURE
#include <cstring>    // For memset, strlen, strcpy, sprintf, strcmp
#include <errno.h>    // For errno

// Hàm gửi yêu cầu và nhận phản hồi
bool send_request_and_get_response(client_request_e request_type, const std::string& key, const std::string& value, message_db_t& response_msg) {
    int server_fifo_fd, client_fifo_fd;

    AppLogger::log(AppLogger::INFO_L, "Client (PID: " + std::to_string(getpid()) + "): Sending request to server.");

    // 1. Mở Server FIFO để ghi
    server_fifo_fd = open(SERVER_FIFO_NAME, O_WRONLY);
    if (server_fifo_fd == -1) {
        if (errno == ENOENT) { // Server FIFO không tồn tại
            AppLogger::log(AppLogger::ERROR_L, "Client: Server is not running. Please start the server first.");
        } else {
            AppLogger::log(AppLogger::ERROR_L, "Client: Failed to open Server FIFO: " + std::string(strerror(errno)));
        }
        return false;
    }
    AppLogger::log(AppLogger::SUCCESS_L, "Client: Server FIFO opened for writing.");

    // 2. Tạo Client FIFO cho phản hồi (sẽ bị xóa khi kết thúc)
    char client_fifo_name[BUFFER_SIZE];
    pid_t my_pid = getpid();
    sprintf(client_fifo_name, CLIENT_FIFO_NAME, my_pid);

    AppLogger::log(AppLogger::INFO_L, "Client: Creating Client FIFO: " + std::string(client_fifo_name));
    if (mkfifo(client_fifo_name, 0666) == -1) {
        if (errno == EEXIST) {
            AppLogger::log(AppLogger::WARNING_L, "Client: Client FIFO already exists. Continuing.");
        } else {
            AppLogger::log(AppLogger::ERROR_L, "Client: Failed to create Client FIFO: " + std::string(strerror(errno)));
            close(server_fifo_fd);
            return false;
        }
    }
    AppLogger::log(AppLogger::SUCCESS_L, "Client: Client FIFO created.");

    // 3. Chuẩn bị tin nhắn yêu cầu
    message_db_t request_msg;
    memset(&request_msg, 0, sizeof(request_msg));
    request_msg.client_pid = my_pid;
    request_msg.request = request_type;
    strcpy(request_msg.data_key, key.c_str());
    strcpy(request_msg.data_value, value.c_str());

    // 4. Gửi yêu cầu tới Server FIFO
    ssize_t bytes_written = write(server_fifo_fd, &request_msg, sizeof(message_db_t));
    if (bytes_written != sizeof(message_db_t)) {
        AppLogger::log(AppLogger::ERROR_L, "Client: Failed to write full request to Server FIFO: " + std::string(strerror(errno)));
        close(server_fifo_fd);
        unlink(client_fifo_name);
        return false;
    }
    AppLogger::log(AppLogger::SUCCESS_L, "Client: Request sent to server.");

    // Đóng Server FIFO ở phía client sau khi gửi yêu cầu
    close(server_fifo_fd);

    // 5. Mở Client FIFO để đọc phản hồi
    // Quan trọng: Mở ở chế độ O_RDWR để tránh race condition khi server cần phản hồi nhiều lần
    // Hoặc nếu client đóng read end quá nhanh server sẽ thấy pipe bị đóng.
    AppLogger::log(AppLogger::INFO_L, "Client: Opening Client FIFO for reading response.");
    client_fifo_fd = open(client_fifo_name, O_RDWR); // O_RDWR để giữ writer end của client FIFO
    if (client_fifo_fd == -1) {
        AppLogger::log(AppLogger::ERROR_L, "Client: Failed to open Client FIFO for reading response: " + std::string(strerror(errno)));
        unlink(client_fifo_name);
        return false;
    }
    AppLogger::log(AppLogger::SUCCESS_L, "Client: Client FIFO opened for reading.");

    // 6. Đọc phản hồi từ Client FIFO
    ssize_t bytes_read = read(client_fifo_fd, &response_msg, sizeof(message_db_t));
    if (bytes_read == sizeof(message_db_t)) {
        AppLogger::log(AppLogger::SUCCESS_L, "Client: Received response from server.");
    } else if (bytes_read == 0) {
        AppLogger::log(AppLogger::WARNING_L, "Client: Received EOF from server (server closed pipe prematurely or sent no data).");
        unlink(client_fifo_name);
        close(client_fifo_fd);
        return false;
    } else {
        AppLogger::log(AppLogger::ERROR_L, "Client: Failed to read full response from Client FIFO: " + std::string(strerror(errno)));
        unlink(client_fifo_name);
        close(client_fifo_fd);
        return false;
    }

    // 7. Dọn dẹp Client FIFO
    close(client_fifo_fd);
    unlink(client_fifo_name); // Xóa client FIFO
    AppLogger::log(AppLogger::INFO_L, "Client: Client FIFO unlinked.");

    return true;
}

int main(int argc, char *argv[]) {
    // Basic argument parsing for client commands
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <command> [key] [value]" << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  set <key> <value>  - Add/Update data" << std::endl;
        std::cout << "  get <key>          - Retrieve data" << std::endl;
        std::cout << "  del <key>          - Delete data" << std::endl;
        std::cout << "  exit_server        - Request server to exit" << std::endl;
        return EXIT_FAILURE;
    }

    std::string command = argv[1];
    std::string key = (argc > 2) ? argv[2] : "";
    std::string value = (argc > 3) ? argv[3] : "";
  
    message_db_t response_msg; // For receiving server response

    if (command == "set") {
        if (key.empty() || value.empty()) { std::cerr << "Usage: " << argv[0] << " set <key> <value>" << std::endl; return EXIT_FAILURE; }
        if (send_request_and_get_response(REQ_ADD_DATA, key, value, response_msg)) {
            if (response_msg.response == RESP_SUCCESS) {
                AppLogger::log(AppLogger::SUCCESS_L, "Client: Data set successfully.");
            } else {
                AppLogger::log(AppLogger::ERROR_L, "Client: Failed to set data: " + std::string(response_msg.error_text));
            }
        }
    } else if (command == "get") {
        if (key.empty()) { std::cerr << "Usage: " << argv[0] << " get <key>" << std::endl; return EXIT_FAILURE; }
        if (send_request_and_get_response(REQ_GET_DATA, key, value, response_msg)) {
            if (response_msg.response == RESP_SUCCESS) {
                AppLogger::log(AppLogger::SUCCESS_L, "Client: Retrieved data for key '" + key + "': '" + std::string(response_msg.data_value) + "'");
            } else if (response_msg.response == RESP_NO_DATA) {
                 AppLogger::log(AppLogger::WARNING_L, "Client: Key '" + key + "' not found.");
            } else {
                AppLogger::log(AppLogger::ERROR_L, "Client: Failed to get data: " + std::string(response_msg.error_text));
            }
        }
    } else if (command == "del") {
        if (key.empty()) { std::cerr << "Usage: " << argv[0] << " del <key>" << std::endl; return EXIT_FAILURE; }
        if (send_request_and_get_response(REQ_DELETE_DATA, key, value, response_msg)) {
            if (response_msg.response == RESP_SUCCESS) {
                AppLogger::log(AppLogger::SUCCESS_L, "Client: Data deleted successfully for key '" + key + "'.");
            } else if (response_msg.response == RESP_NO_DATA) {
                AppLogger::log(AppLogger::WARNING_L, "Client: Key '" + key + "' not found for deletion.");
            } else {
                AppLogger::log(AppLogger::ERROR_L, "Client: Failed to delete data: " + std::string(response_msg.error_text));
            }
        }
    } else if (command == "exit_server") {
        if (send_request_and_get_response(REQ_EXIT_SERVER, "", "", response_msg)) {
            if (response_msg.response == RESP_SUCCESS) { // Server có thể không phản hồi cho lệnh exit
                AppLogger::log(AppLogger::SUCCESS_L, "Client: Exit server request sent.");
            } else {
                AppLogger::log(AppLogger::INFO_L, "Client: Exit server request sent (no explicit success response expected).");
            }
        }
    }
    else {
        std::cerr << "Client: Unknown command: " << command << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
```

* **Cách Biên dịch:**
  **Bash**

  ```
  g++ server_db.cpp -o server_db
  g++ client_db.cpp -o client_db
  ```
* **Cách Chạy và Kiểm tra:**

  1. **Khởi động Server (trong một terminal):**
     **Bash**

     ```
     ./server_db & # Chạy nền
     ```
  2. **Chạy Client (trong các terminal khác):**

     * **Thêm dữ liệu:**
       **Bash**

       ```
       ./client_db set my_key "Hello World"
       ./client_db set another_key "Linux is fun!"
       ```
     * **Lấy dữ liệu:**
       **Bash**

       ```
       ./client_db get my_key
       ./client_db get non_existent_key
       ```
     * **Xóa dữ liệu:**
       **Bash**

       ```
       ./client_db del my_key
       ./client_db get my_key
       ```
     * **Yêu cầu server thoát:**
       **Bash**

       ```
       ./client_db exit_server
       ```
  3. **Quan sát Output:** Theo dõi output của cả server và client để thấy các giao tiếp và xử lý.

---

### **Câu hỏi Tự đánh giá Module 4 🤔**

1. Mô tả mô hình Client/Server cơ bản khi sử dụng FIFOs. Tại sao lại cần hai FIFO cho mỗi cặp Client-Server?
2. Làm thế nào để Server biết gửi phản hồi về FIFO nào khi có nhiều client?
3. Trong ví dụ `client_db.cpp`, tại sao `client_fifo_fd` được mở ở chế độ `O_RDWR` thay vì chỉ `O_RDONLY` khi đọc phản hồi từ server? Điều này giải quyết vấn đề gì?
4. Khi server đọc từ Server FIFO và nhận được 0 bytes (`read_res == 0`), điều đó có ý nghĩa gì? Server trong ví dụ đã xử lý trường hợp này như thế nào?
5. Nêu một thách thức hoặc hạn chế của việc sử dụng FIFOs cho mô hình Client/Server so với việc sử dụng Sockets (mà chúng ta sẽ học sau).

---

### **Bài tập Thực hành Module 4 ✍️**

1. **Chương trình "FIFO Message Processor":**
   * Mở rộng ví dụ Client/Server:
   * **`server_processor.cpp`:**
     * Mở rộng hàm `process_client_request` để xử lý một yêu cầu mới: `REQ_PROCESS_TEXT`.
     * Khi nhận `REQ_PROCESS_TEXT`, server sẽ chuyển đổi chuỗi `data_value` thành chữ hoa (uppercase) và gửi lại client. (Bạn có thể dùng `std::toupper` từ `<cctype>` cho việc này).
   * **`client_processor.cpp`:**
     * Thêm một lệnh mới `--process <text>` vào giao diện dòng lệnh.
     * Khi lệnh này được gọi, client sẽ gửi `REQ_PROCESS_TEXT` với `<text>` trong `data_value`.
     * Nhận phản hồi và in ra chuỗi đã được xử lý.
   * **Mục tiêu:** Client gửi văn bản thường, server trả về văn bản chữ hoa.
2. **Chương trình "FIFO Broadcaster" (Nâng cao):**
   * Thiết kế một hệ thống nơi có một Server và nhiều Client, nhưng Client  **không cần nhận phản hồi cá nhân** . Server chỉ cần "phát sóng" (broadcast) thông báo cho tất cả các client đang lắng nghe.
   * **`broadcaster_server.cpp`:**
     * Tạo một FIFO chung (ví dụ: `/tmp/broadcast_fifo`).
     * Mở FIFO đó ở chế độ  **ghi (`O_WRONLY`)** . Server sẽ bị chặn cho đến khi ít nhất một client mở để đọc.
     * Trong vòng lặp, đọc một dòng từ `stdin` của server.
     * Ghi dòng đó vào FIFO.
   * **`broadcaster_client.cpp`:**
     * Mở FIFO chung ở chế độ  **đọc không chặn (`O_RDONLY | O_NONBLOCK`)** . Điều này quan trọng vì client không muốn bị chặn chờ dữ liệu nếu không có broadcast.
     * Trong vòng lặp, cứ mỗi 1 giây, cố gắng đọc từ FIFO.
     * Nếu có dữ liệu, in ra "Received broadcast: [data]".
     * Nếu không có dữ liệu (lỗi `EAGAIN`/`EWOULDBLOCK`), in ra "No new data...".
     * Nếu FIFO bị đóng (read 0 bytes), in ra "Writer disconnected. Exiting." và thoát.
   * **Mục tiêu:** Server gõ một tin nhắn, tất cả các client đang chạy sẽ nhận được tin nhắn đó.
3. **Chương trình "Shared Status via FIFO" (Nâng cao):**
   * Thiết kế một hệ thống nơi nhiều tiến trình ghi trạng thái nhỏ (ví dụ: nhiệt độ, mức pin) vào một FIFO chung. Một tiến trình khác (monitor) sẽ đọc và hiển thị các trạng thái này.
   * **`sensor_writer.cpp`:**
     * Nhận ID cảm biến và giá trị.
     * Mở FIFO chung (`O_WRONLY`).
     * Ghi một cấu trúc `struct SensorStatus { int id; double value; time_t timestamp; }` vào FIFO.
     * **Thử thách:** Đảm bảo `write()` là atomic (ghi cả cấu trúc hoặc không gì cả) bằng cách đảm bảo kích thước cấu trúc nhỏ hơn `PIPE_BUF` và mở FIFO ở chế độ chặn (không `O_NONBLOCK`).
   * **`monitor_reader.cpp`:**
     * Mở FIFO chung (`O_RDONLY`).
     * Trong vòng lặp, đọc các cấu trúc `SensorStatus`.
     * In ra trạng thái của từng cảm biến.
   * **Mục tiêu:** Cho nhiều `sensor_writer` chạy cùng lúc, `monitor_reader` hiển thị các bản cập nhật trạng thái.

---

Hãy dành thời gian để hiểu sâu lý thuyết và thực hành các bài tập này. Xây dựng ứng dụng Client/Server với FIFOs là một kỹ năng rất thực tế. Khi bạn đã sẵn sàng, hãy cho tôi biết để chuyển sang  **Module 5: Luyện tập Tổng hợp & Ứng dụng** !
