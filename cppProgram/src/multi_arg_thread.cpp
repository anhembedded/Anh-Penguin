#include <iostream>
#include <pthread.h>

struct ThreadArgs { int id; std::string message; };

void* threadFunc(void* args) {
    ThreadArgs* threadArgs = static_cast<ThreadArgs*>(args);
    std::cout << "Thread " << threadArgs->id << ": " << threadArgs->message << std::endl;
    delete threadArgs;
    return nullptr;
}

/**
 * @brief Main entry point of the program.
 *
 * This program creates two threads with different arguments, and waits for them
 * to finish before exiting.
 *
 * @return 0 if successful, non-zero otherwise.
 */
int main() {
    pthread_t thread1, thread2;
    ThreadArgs* args1 = new ThreadArgs{1, "Hello from thread 1"};
    ThreadArgs* args2 = new ThreadArgs{2, "Hello from thread 2"};
    pthread_create(&thread1, nullptr, threadFunc, args1);
    pthread_create(&thread2, nullptr, threadFunc, args2);
    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);
    return 0;
}
