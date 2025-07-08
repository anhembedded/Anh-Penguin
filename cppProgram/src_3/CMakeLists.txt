find_package(Threads REQUIRED)

add_executable(waiter waiter.cpp)
add_executable(worker worker.cpp)

target_link_libraries(waiter PRIVATE rt Threads::Threads)
target_link_libraries(worker PRIVATE rt Threads::Threads)