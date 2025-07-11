cmake_minimum_required(VERSION 3.15)

project(PRACTICE_LINUX VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(/usr/include/c++/13
                    /usr/include)
add_subdirectory(src_1)
add_subdirectory(src_2)
add_subdirectory(src_3)
add_subdirectory(src_4)
add_subdirectory(runProcess)
add_subdirectory(unixSocket)
add_subdirectory(internetSocket)

message(STATUS "📚 Include directories for target 'your_target_name': ${INCLUDE_DIRECTORIES}")

# ====================================================================
# PHẦN CẤU HÌNH CPPCHECK - PHIÊN BẢN HYBRID VỚI LOG TRACE
# ====================================================================
find_program(CPPCHECK_EXECUTABLE cppcheck REQUIRED)

# --- BƯỚC A: Lấy các đường dẫn include ngầm định của hệ thống ---
set(SYSTEM_INCLUDES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

# --- LOG TRACE 1: In ra các đường dẫn hệ thống mà CMake tìm thấy ---
message(STATUS "🔎 TRACE: Các đường dẫn include hệ thống ngầm định (raw):")
message(STATUS "  ==> ${SYSTEM_INCLUDES}")

# --- BƯỚC B: Chuyển các đường dẫn đó thành các cờ -I ---
set(CPPCHECK_SYS_INCLUDE_FLAGS "")
foreach(DIR ${SYSTEM_INCLUDES})
    list(APPEND CPPCHECK_SYS_INCLUDE_FLAGS "-I${DIR}")
endforeach()

# --- LOG TRACE 2: In ra danh sách cờ -I đã được tạo ---
message(STATUS "🛠️ TRACE: Các cờ -I hệ thống được tạo cho Cppcheck:")
message(STATUS "  ==> ${CPPCHECK_SYS_INCLUDE_FLAGS}")

# --- BƯỚC C: Tạo custom target kết hợp --project và -I hệ thống ---
add_custom_target(analyze
    COMMAND ${CPPCHECK_EXECUTABLE}
            --enable=all
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
            ${CPPCHECK_SYS_INCLUDE_FLAGS}
            --library=std.cfg
            --verbose # Giữ lại verbose để xem Cppcheck có nhận được không
    COMMENT "✅ Running Cppcheck analysis (Hybrid Mode)..."
)

message(STATUS "🎯 Đã cấu hình xong target 'analyze'. Chạy 'make analyze' để thực thi.")