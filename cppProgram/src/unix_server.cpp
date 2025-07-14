#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include "unixSocket_common.hpp"

int main() 
{
    // Đảm bảo socket cũ không tồn tại
    unlink(SOCKET_NAME.c_str());

    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("socket create");
        return EXIT_FAILURE;
    }

    sockaddr_un socket_addr {};
    socket_addr.sun_family = AF_UNIX;
    // Sử dụng strncpy để tránh buffer overflow
    strncpy(socket_addr.sun_path, SOCKET_NAME.c_str(), sizeof(socket_addr.sun_path) - 1);

    if(bind(sock_fd, reinterpret_cast<sockaddr*>(&socket_addr), sizeof(socket_addr)) < 0)
    {
        perror("bind");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    if(listen(sock_fd, 10) < 0)
    {
        perror("listen");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    std::cout << "[Server] Waiting for a client connection at " << SOCKET_NAME << "..." << std::endl;
    int client_fd = accept(sock_fd, nullptr, nullptr);
    if(client_fd < 0)
    {
        perror("accept");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    // --- PHẦN QUAN TRỌNG: CHUYỂN HƯỚNG STDOUT ---
    std::cout << "[Server] Client connected. Redirecting stdout to client socket." << std::endl;

    // STDOUT_FILENO là file descriptor chuẩn cho standard output (thường là 1)
    if (dup2(client_fd, STDOUT_FILENO) == -1) {
        perror("dup2 failed");
        close(client_fd);
        close(sock_fd);
        return EXIT_FAILURE;
    }
    
    // Sau khi dup2, client_fd gốc không còn cần thiết vì STDOUT_FILENO đã trỏ đến socket.
    // Đóng nó đi để tránh rò rỉ file descriptor.
    close(client_fd);

    // --- BÂY GIỜ, MỌI THỨ GHI RA `std::cout` SẼ ĐI VÀO SOCKET ---
    std::cout << "Hello from server! This message was sent via redirected stdout." << std::endl;
    std::cout << "Running 'ls -l' command and sending output:" << std::endl;
    
    // Dùng std::endl hoặc fflush để đảm bảo dữ liệu được gửi ngay lập tức
    fflush(stdout); 

    // Ví dụ: thực thi một lệnh và kết quả của nó sẽ tự động được gửi qua socket
    system("ls -l /");

    // Đóng listening socket và dọn dẹp file socket
    close(sock_fd);
    unlink(SOCKET_NAME.c_str());

    // Lưu ý: không cần `close(STDOUT_FILENO)` ở đây vì nó sẽ đóng stdout của chính terminal
    // và chương trình sẽ tự động đóng khi kết thúc.

    return EXIT_SUCCESS;
}