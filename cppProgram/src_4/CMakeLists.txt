find_package(Threads REQUIRED)

add_executable(src_4_reader reader.cpp)
add_executable(src_4_writer writer.cpp)

target_link_libraries(src_4_reader PRIVATE rt Threads::Threads)
target_link_libraries(src_4_writer PRIVATE rt Threads::Threads)