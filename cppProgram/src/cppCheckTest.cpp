// buggy_code_for_cppcheck.cpp
#include <iostream>
#include <vector>   // For std::vector
#include <string>   // For std::string
#include <cstdlib>  // For malloc, free, rand, srand, EXIT_SUCCESS
#include <cstring>  // For strlen, strcpy, memset
#include <ctime>    // For time
#include <stdexcept> // For std::runtime_error (just to show throwing exceptions)

// Function 1: Contains memory leaks and uninitialized variable
void function_with_memory_issues(int value) {
    // BUG: Memory Leak (new without delete)
    int* leaked_int_ptr = new int;
    *leaked_int_ptr = value;
    // No 'delete leaked_int_ptr;' here.

    // BUG: Memory Leak (malloc without free)
    char* leaked_char_array = (char*)malloc(10);
    if (leaked_char_array != nullptr) { // Good practice to check, but still leaks
        std::strcpy(leaked_char_array, "leak");
    }
    // No 'free(leaked_char_array);' here.

    // BUG: Uninitialized variable usage
    int uninitialized_local_var; 
    if (value > 10) {
        uninitialized_local_var = 50;
    }
    // If 'value' is <= 10, 'uninitialized_local_var' is used uninitialized here.
    std::cout << "Debug: Value from uninitialized var path (if applicable): " << uninitialized_local_var << std::endl; 
}

// Function 2: Contains out-of-bounds access and C-style cast
void function_with_array_and_cast_issues() {
    char buffer[5]; // Buffer of size 5 (indices 0-4)
    
    // BUG: Out-of-bounds write
    // This attempts to write beyond the allocated size of 'buffer'.
    std::strcpy(buffer, "Too long string!"); // "Too long string!" is 16 chars + '\0' = 17 bytes
    std::cout << "Debug: Buffer content after OOB write: " << buffer << std::endl;

    // BUG: C-style cast (often flagged as style/portability issue)
    int int_val = 123;
    float float_val = (float)int_val; // C-style cast
    std::cout << "Debug: Float value: " << float_val << std::endl;

    // BUG: Redundant condition (always true)
    if (float_val == float_val) { // This condition is always true
        std::cout << "Debug: Float value is equal to itself." << std::endl;
    }
}

// Function 3: Contains null pointer dereference and double free
void function_with_pointer_abuse(int* ptr_param) {
    // BUG: Null pointer dereference
    // If ptr_param is nullptr, this will cause a crash (SIGSEGV)
    // Cppcheck might warn about this if it can infer ptr_param could be null.
    std::cout << "Debug: Dereferencing ptr_param: " << *ptr_param << std::endl;

    int* dynamic_array = new int[5];
    if (dynamic_array == nullptr) {
        std::cerr << "Error: Failed to allocate dynamic_array." << std::endl;
        return;
    }
    delete[] dynamic_array; // First free

    // BUG: Double Free
    // Calling delete[] again on the same pointer after it's been freed.
    delete[] dynamic_array; // Second free (causes undefined behavior/crash)
    std::cout << "Debug: Attempted double free." << std::endl;
}

// Function 4: Contains unused variable and missing break in switch
void function_with_style_issues(int choice) {
    int unused_variable; // BUG: Unused variable (if --enable=all or style is used)

    switch (choice) {
        case 1:
            std::cout << "Choice 1 selected." << std::endl;
            // BUG: Missing break (fall-through to case 2)
        case 2:
            std::cout << "Choice 2 selected (fall-through)." << std::endl;
            break;
        default:
            std::cout << "Default choice." << std::endl;
            break;
    }
}

// Function 5: Integer overflow and signed/unsigned comparison
void function_with_integer_issues() {
    unsigned int large_num = 4000000000U; // A large unsigned int
    int small_signed_num = 100;

    // BUG: Integer overflow (if result exceeds int max)
    // If int is 32-bit, 2000000000 * 2 will overflow.
    int result_overflow = 2000000000 * 2; 
    std::cout << "Debug: Result of potential overflow: " << result_overflow << std::endl;

    // BUG: Comparison of signed and unsigned (portability/style issue)
    if (small_signed_num < large_num) { // Comparing signed with unsigned
        std::cout << "Debug: Signed vs unsigned comparison." << std::endl;
    }
}


int main() {
    std::cout << "INFO: Program starting. Running various buggy functions." << std::endl;

    // Seed random for potential future use (not directly used for bugs here)
    std::srand(std::time(nullptr)); 

    // Call functions with intentional bugs
    function_with_memory_issues(5); // Leaks, uninitialized var (if value <= 10)
    function_with_memory_issues(15); // Leaks, uninitialized var (initialized path)

    function_with_array_and_cast_issues(); // OOB write, C-style cast, redundant condition

    // This call will likely cause a crash due to null pointer dereference or double free
    // Cppcheck should detect this even if it doesn't crash immediately.
    // To see the double free, you might need to comment out the null pointer dereference.
    // function_with_pointer_abuse(nullptr); // Will crash immediately
    int dummy_val = 1;
    function_with_pointer_abuse(&dummy_val); // Will cause double free (if not crashed by OOB already)

    function_with_style_issues(1); // Missing break
    function_with_style_issues(3); // Default case

    function_with_integer_issues(); // Integer overflow, signed/unsigned comparison

    std::cout << "INFO: Program finished. Check Cppcheck report for issues." << std::endl;

    return EXIT_SUCCESS;
}
