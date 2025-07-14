#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <memory> // Για std::unique_ptr
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>   // Για strdup
#include <cstdlib>

/**
 * @class Command
 * @brief Represents a parsed command, managing memory using RAII.
 * The destructor ensures that memory allocated by strdup is freed.
 */
class Command {
public:
    // Constructor parses a raw string into arguments
    explicit Command(const std::string& raw_command) {
        std::istringstream iss(raw_command);
        std::string token;
        while (iss >> token) {
            // Allocate memory and store the pointer
            char* arg = strdup(token.c_str());
            if (!arg) {
                throw std::bad_alloc();
            }
            m_args_ptr.push_back(std::unique_ptr<char[]>(arg));
        }
    }

    // Get arguments for execvp
    std::vector<char*> get_argv() const {
        std::vector<char*> argv;
        for (const auto& arg : m_args_ptr) {
            argv.push_back(arg.get());
        }
        argv.push_back(nullptr); // execvp requires a NULL terminator
        return argv;
    }

private:
    // Vector of smart pointers to manage the lifetime of C-style strings
    std::vector<std::unique_ptr<char[]>> m_args_ptr;
};

/**
 * @brief Launches each command in its own process and waits for completion.
 * Usage: ./launcher "ls -l" "echo Hello" ...
 */
int main(const int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " \"<command1>\" \"<command2>\" ...\n";
        return EXIT_FAILURE;
    }

    std::vector<pid_t> child_pids;

    // Launch all commands in parallel
    for (int i = 1; i < argc; ++i) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork failed");
            // Consider stopping here or trying to clean up already started processes
            continue; // Skip to the next command
        }

        if (pid == 0) { // Child process
            try {
                Command cmd(argv[i]);
                auto args = cmd.get_argv();
                std::cout << "[PID " << getpid() << "] Executing: " << argv[i] << std::endl;
                execvp(args[0], args.data());

                // execvp only returns on error
                perror("execvp failed");
                exit(EXIT_FAILURE); // Exit child if exec fails

            } catch (const std::bad_alloc& e) {
                std::cerr << "Memory allocation failed for command: " << argv[i] << std::endl;
                exit(EXIT_FAILURE);
            }
        } else { // Parent process
            std::cout << "[Parent] Launched child with PID " << pid << " for command: " << argv[i] << std::endl;
            child_pids.push_back(pid);
        }
    }

    // Wait for all child processes and check their status
    int total_failures = 0;
    for (const auto pid : child_pids) {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            total_failures++;
        } else {
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                if (exit_code != 0) {
                    std::cerr << "[Parent] Child PID " << pid << " exited with error code: " << exit_code << std::endl;
                    total_failures++;
                }
            } else {
                std::cerr << "[Parent] Child PID " << pid << " terminated abnormally." << std::endl;
                total_failures++;
            }
        }
    }

    if (total_failures > 0) {
        std::cerr << "\n[Launcher] " << total_failures << " command(s) failed to execute correctly.\n";
        return EXIT_FAILURE;
    }

    std::cout << "\n[Launcher] All commands completed successfully.\n";
    return EXIT_SUCCESS;
}