#include <iostream>   // For std::cout, std::cerr, std::getline, std::cin
#include <cstdio>     // For popen, pclose, FILE, std::perror, std::fputs, EOF
#include <cstdlib>    // For EXIT_SUCCESS, EXIT_FAILURE
#include <string>     // For std::string
#include <sys/wait.h> // For WEXITSTATUS, WIFEXITED, WIFSIGNALED, WTERMSIG (macros for pclose status)
#include <cstring>    // For strerror (used by std::perror implicitly)

int main(int argc, char *argv[]) {
    // 1. Kiểm tra Tham số Dòng lệnh
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " \"<shell-command>\"\n";
        return EXIT_FAILURE;
    }

    // 2. Lấy Lệnh Shell từ Tham số Dòng lệnh
    std::string command = argv[1];

    // 3. Mở Pipe để Ghi dữ liệu vào Lệnh Shell
    std::cout << "INFO: Opening pipe to command: \"" << command << "\" in write mode." << std::endl;
    FILE* pipe_stream = popen(command.c_str(), "w");
    if (!pipe_stream) {
        std::perror("ERROR: popen failed"); // std::perror() tự động thêm thông báo lỗi từ errno
        return EXIT_FAILURE;
    }
    std::cout << "INFO: Pipe opened successfully." << std::endl;

    // 4. Đọc từ stdin của chương trình hiện tại và ghi vào pipe
    std::string line;
    std::cout << "INFO: Enter lines to send to the shell command. Press Ctrl+D to finish input." << std::endl;
    while (std::getline(std::cin, line)) {
        // std::getline() không bao gồm '\n', nên cần thêm vào khi ghi cho lệnh shell.
        // std::fputs() trả về EOF nếu lỗi, kiểm tra nó.
        if (std::fputs((line + "\n").c_str(), pipe_stream) == EOF) {
            std::cerr << "ERROR: fputs failed when writing to pipe. Possible pipe broken or command exited prematurely." << std::endl;
            // Không cần perror ở đây vì fputs đã báo lỗi qua EOF, không nhất thiết là lỗi hệ thống.
            pclose(pipe_stream); // Đóng pipe trước khi thoát
            return EXIT_FAILURE;
        }
        std::cout << "DEBUG: Sent line to pipe: \"" << line << "\"" << std::endl;
    }
    std::cout << "INFO: End of input detected. Closing input stream." << std::endl;

    // 5. Đóng Pipe và Thu nhận Mã thoát của Lệnh Shell
    std::cout << "INFO: Closing pipe and waiting for shell command to exit." << std::endl;
    int status = pclose(pipe_stream);
    if (status == -1) {
        std::perror("ERROR: pclose failed");
        return EXIT_FAILURE;
    }
    std::cout << "INFO: Pipe closed. Shell command exited." << std::endl;

    // 6. Diễn giải Mã thoát một cách an toàn
    if (WIFEXITED(status)) { // Kiểm tra nếu tiến trình con thoát bình thường
        int exit_code = WEXITSTATUS(status);
        std::cout << "SUCCESS: Shell command exited with code: " << exit_code << std::endl;
    } else if (WIFSIGNALED(status)) { // Kiểm tra nếu tiến trình con bị chấm dứt bởi tín hiệu
        std::cerr << "WARNING: Shell command terminated by signal: " << WTERMSIG(status) << std::endl;
        return EXIT_FAILURE; // Coi là thất bại nếu bị chấm dứt bởi tín hiệu
    } else if (WIFSTOPPED(status)) { // Kiểm tra nếu tiến trình con bị dừng
        std::cerr << "WARNING: Shell command stopped by signal: " << WSTOPSIG(status) << std::endl;
        // Có thể không cần thoát ở đây nếu muốn xử lý tiếp
        return EXIT_FAILURE;
    } else {
        std::cerr << "WARNING: Shell command terminated abnormally for unknown reason." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}