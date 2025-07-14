#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h> // Cho sockaddr_in
#include <unistd.h>

#include "internetSocket_common.hpp"



int main() {
    // 1. Tạo socket với domain AF_INET
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { perror("socket"); return 1; }

    // 2. Chuẩn bị địa chỉ và `bind`
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    // INADDR_ANY: Chấp nhận kết nối từ bất kỳ địa chỉ IP nào của máy. Rất hữu dụng.
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT); // Chuyển port sang network byte order

    if (bind(server_fd, (const sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind"); return 1;
    }

    // 3. Listen và Accept (logic không đổi)
    listen(server_fd, 5);
    std::cout << "[Server] Waiting for connection on port " << PORT << std::endl;
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) { perror("accept"); return 1; }
    std::cout << "[Server] Client connected!" << std::endl;

    // 4. Giao tiếp và Đóng (logic không đổi)
    char buffer[256] = { 0 };
    read(client_fd, buffer, sizeof(buffer));
    std::cout << "[Server] Received: " << buffer << std::endl;
    write(client_fd, "Hello from INET Server!", 24);

    close(client_fd);
    close(server_fd);

    return 0;
}