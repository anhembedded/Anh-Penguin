# src/CMakeLists.txt (nằm trong thư mục 'src/')

# Tạo executable cho từng file .cpp có hàm main()
add_executable(internetSocket_client internetSocket_client.cpp)
add_executable(internetSocket_server internetSocket_server.cpp)
add_executable(mainProcess mainProcess.cpp)
add_executable(cppCheckTest cppCheckTest.cpp)


# Nếu cần, liên kết thư viện cho từng executable
target_link_libraries(internetSocket_client PRIVATE pthread)
target_link_libraries(internetSocket_server PRIVATE pthread)
target_link_libraries(mainProcess PRIVATE pthread)
target_link_libraries(cppCheckTest PRIVATE pthread)
