#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // Cho inet_addr
#include <unistd.h>

#include "internetSocket_common.hpp"


int main() {
    // 1. Tạo socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Chuẩn bị địa chỉ server
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    // inet_addr sẽ chuyển đổi địa chỉ IP dạng chuỗi sang dạng số và network byte order
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // 3. Kết nối đến server
    if (connect(client_fd, (const sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect"); return 1;
    }
    std::cout << "[Client] Connected to server." << std::endl;

    // 4. Giao tiếp và Đóng (logic không đổi)
    write(client_fd, "Hello from INET Client!", 24);
    char buffer[256] = {0};
    read(client_fd, buffer, sizeof(buffer));
    std::cout << "[Client] Received: " << buffer << std::endl;
    close(client_fd);

    return 0;
}