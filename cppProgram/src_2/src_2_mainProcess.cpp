#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstring>
#include <errno.h>

int main() {
    std::cout << "Anh-Penguin Main Process bắt đầu 🔧\n";

    constexpr auto subprocessCmd = "src_2_subProcess";

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // 👶 Tiến trình con
        close(pipe_fd[1]); // Không ghi
        dup2(pipe_fd[0], STDIN_FILENO); // stdin ← read-end
        close(pipe_fd[0]);

        execlp(subprocessCmd, subprocessCmd, nullptr);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // 👨 Tiến trình cha
        close(pipe_fd[0]); // Không đọc
        std::string input {};

        while (true) {
            std::cout << "Nhập lệnh (hoặc 'exit'): ";
            if (!std::getline(std::cin, input))
            {
                std::cout << "⛔ Không còn dữ liệu để đọc (EOF). Thoát.\n";
                break;
            }
            std::cout << "✅ Bạn vừa nhập: " << input << "\n";
            if (input == "exit")
            {
                break;
            }
            input += "\n"; // Thêm newline để con đọc được
            write(pipe_fd[1], input.c_str(), input.size());
        }

        close(pipe_fd[1]); // Gửi EOF
        wait(nullptr);
        std::cout << "⛔ Main kết thúc\n";
    }

    return 0;
}