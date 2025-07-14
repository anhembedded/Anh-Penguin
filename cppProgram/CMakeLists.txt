# CMakeLists.txt (ở thư mục gốc của dự án)

cmake_minimum_required(VERSION 3.15)
project(PRACTICE_LINUX VERSION 1.0.0 LANGUAGES CXX) # Giả sử chỉ C++
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# === Thêm thư mục 'src' và định nghĩa các executable bên trong 'src/CMakeLists.txt' ===
add_subdirectory(src)

# === Cấu hình Cppcheck (Custom Target) ===
find_program(CPPCHECK_EXECUTABLE cppcheck REQUIRED)

if (CPPCHECK_EXECUTABLE)
    message(STATUS "Cppcheck found: ${CPPCHECK_EXECUTABLE}")

    add_custom_target(
        run_cppcheck
        COMMAND ${CPPCHECK_EXECUTABLE}
                --enable=all
                --platform=unix64
                --std=c++17
                #--xml-version=2
                #--output-file=${PROJECT_BINARY_DIR}/cppcheck_report.xml
                --project=${PROJECT_BINARY_DIR}/compile_commands.json # Dùng compile_commands.
                -v
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMENT "Running Cppcheck static analysis..."
        VERBATIM
    )
    # add_dependencies() ở đây sẽ phức tạp hơn nếu có nhiều executable.
    # Tốt nhất là chạy 'cmake --build . --target run_cppcheck' thủ công.
else ()
    message(WARNING "Cppcheck executable not found. Static analysis will be skipped.")
endif ()