#include <iostream>   // For std::cout, std::cerr
#include <cstdio>     // For printf (dùng cho perror), std::perror
#include <cstdlib>    // For EXIT_SUCCESS, EXIT_FAILURE, exit()
#include <string>     // For std::string
#include <sys/wait.h> // For WEXITSTATUS, WIFEXITED, WIFSIGNALED, WTERMSIG
#include <cstring>    // For strerror
#include <unistd.h>  
#include <errno.h>    

std::string global_i_option_value = ""; 

// Hàm của tiến trình chính
void mainProcessHandler() {
    for (int i = 1; i <= 100; ++i) { // Vòng lặp chạy 100 lần
        std::cout << "[mainProcess] PID: " << getpid() << " - Iteration " << i << "/100" << std::endl;
        sleep(2);
    }
    std::cout << "[mainProcess] PID: " << getpid() << " - Handler finished after 100 iterations." << std::endl;
}

int main(int argc, char *argv[]) {
    int opt {};
    bool i_option_provided = false; 
    while ((opt = getopt(argc, argv, ":i:")) != -1) { // Thêm ":" ở đầu optstring để phân biệt lỗi thiếu đối số
        switch (opt) {
            case 'i':
                std::cout << "INFO    : Option -i provided with value: " << optarg << std::endl;
                global_i_option_value = optarg; 
                i_option_provided = true;     
                break;
            case ':':
                std::cerr << "ERROR   : Option -" << (char)optopt << " requires an argument." << std::endl;
                return EXIT_FAILURE;
            case '?':
                std::cerr << "ERROR   : Unknown option: -" << (char)optopt << std::endl;
                return EXIT_FAILURE;
            default:
                std::cerr << "ERROR   : Unexpected getopt return value: " << opt << std::endl;
                return EXIT_FAILURE;
        }
    }

    if (!i_option_provided) {
        std::cerr << "WARNING : Option -i is MANDATORY. Please provide it." << std::endl;
        std::cerr << "Usage   : " << argv[0] << " -i <string_value>\n";
        return EXIT_FAILURE;
    }

    constexpr auto cmdSubProcess{"src_1_printName"}; 
    const auto thisProcessPid{getpid()};     

    pid_t subprocess_pid{fork()}; 

    if (subprocess_pid == -1) {
        std::cerr << "[" << thisProcessPid << "] ERROR: Create subprocess failed: " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    } else if (subprocess_pid == 0) { 
        std::cout << "[" << getpid() << "] Child Process: About to replace image with '" << cmdSubProcess << "'" << std::endl;
        execl(cmdSubProcess, cmdSubProcess, global_i_option_value.c_str(), nullptr); 
        
        std::cerr << "[" << getpid() << "] CRITICAL: Replace image subprocess failed for '" << cmdSubProcess << "': " << strerror(errno) << std::endl;
        _exit(EXIT_FAILURE);
    } else { // PARENT PROCESS (subprocess_pid > 0)
        std::cout << "[" << thisProcessPid << "] Parent Process: Child created with PID " << subprocess_pid << "." << std::endl;
        std::cout << "[" << thisProcessPid << "] Parent Process: Value from -i option: " << global_i_option_value << std::endl;
        mainProcessHandler(); 
        int status{};
        waitpid(subprocess_pid, &status, 0); 
        std::cout << "[" << thisProcessPid << "] Parent Process: Child (PID " << subprocess_pid << ") finished." << std::endl;
        if (WIFEXITED(status)) {
            std::cout << "[" << thisProcessPid << "] Parent: Child exited with code: " << WEXITSTATUS(status) << std::endl;
        } else if (WIFSIGNALED(status)) {
            std::cout << "[" << thisProcessPid << "] Parent: Child terminated by signal: " << WTERMSIG(status) << std::endl;
        }
    }
    return EXIT_SUCCESS;
}