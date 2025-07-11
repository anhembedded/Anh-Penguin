# Danh sách các target bạn muốn Cppcheck phân tích
set(TARGETS_TO_ANALYZE
    src_1_mainProcess
    src_1_printName
)

add_executable(src_1_mainProcess src_1_mainProcess.cpp)
add_executable(src_1_printName src_1_printName.cpp)

# 🎯 CHỌN TARGET BẠN MUỐN XEM Ở ĐÂY
set(TARGET_TO_DEBUG src_1_mainProcess)

# Bắt đầu in thông tin
message(STATUS "========================================================================")
message(STATUS "🖨️  Đang in môi trường của target: ${TARGET_TO_DEBUG}")
message(STATUS "========================================================================")

# 1. In ra chuẩn C++
get_target_property(CXX_STANDARD ${TARGET_TO_DEBUG} CXX_STANDARD)
if(CXX_STANDARD)
    message(STATUS ">> Chuẩn C++ (để tạo cờ --std=c++XX):")
    message(STATUS "   ${CXX_STANDARD}")
endif()

# 2. In ra các file nguồn (đường dẫn tuyệt đối)
message(STATUS "\n>> Các file mã nguồn (để truyền vào cuối lệnh):")
get_target_property(SOURCES ${TARGET_TO_DEBUG} SOURCES)
foreach(ITEM ${SOURCES})
    get_filename_component(ABSOLUTE_PATH ${ITEM} ABSOLUTE)
    message(STATUS "   ${ABSOLUTE_PATH}")
endforeach()

# 3. In ra các đường dẫn include của DỰ ÁN
message(STATUS "\n>> Các đường dẫn include của DỰ ÁN (để tạo cờ -I):")
get_target_property(PROJECT_INCLUDES ${TARGET_TO_DEBUG} INCLUDE_DIRECTORIES)
if(PROJECT_INCLUDES AND NOT "${PROJECT_INCLUDES}" STREQUAL "PROJECT_INCLUDES-NOTFOUND")
    foreach(ITEM ${PROJECT_INCLUDES})
        message(STATUS "   -I${ITEM}")
    endforeach()
else()
    message(STATUS "   (Không có)")
endif()

# 4. In ra các đường dẫn include của HỆ THỐNG
message(STATUS "\n>> Các đường dẫn include của HỆ THỐNG (để tạo cờ -I):")
set(SYSTEM_INCLUDES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
foreach(ITEM ${SYSTEM_INCLUDES})
    message(STATUS "   -I${ITEM}")
endforeach()

# 5. In ra các định nghĩa preprocessor
message(STATUS "\n>> Các định nghĩa preprocessor (để tạo cờ -D):")
get_target_property(DEFINES ${TARGET_TO_DEBUG} COMPILE_DEFINITIONS)
if(DEFINES AND NOT "${DEFINES}" STREQUAL "DEFINES-NOTFOUND")
    foreach(ITEM ${DEFINES})
        message(STATUS "   -D${ITEM}")
    endforeach()
else()
    message(STATUS "   (Không có)")
endif()

message(STATUS "========================================================================")
message(STATUS "💡 Gợi ý: Copy các đường dẫn và file ở trên để tạo lệnh cppcheck thủ công.")
message(STATUS "========================================================================")
