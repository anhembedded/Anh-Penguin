#include <iostream>
#include <unistd.h> // For getpid(), getppid(), sleep()
#include <cstdlib>  // For EXIT_SUCCESS

int main() {
    std::cout << "[printName] Subprocess (PID: " << getpid() << ") started. PPID: " << getppid() << std::endl;
    for (int i = 0; i < 200; ++i) {
        std::cout << "[printName] Still running... (" << i+1 << "/200)" << std::endl;
        sleep(1);
    }
    std::cout << "[printName] Subprocess (PID: " << getpid() << ") finished." << std::endl;
    return EXIT_SUCCESS;
}