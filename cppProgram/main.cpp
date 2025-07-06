#include <iostream>
#include <cstdio>
#include <fstream> // For std::ifstream
#include <vector>  // For std::vector
#include <cstdlib> // For exit()

using namespace std;
int main()
{
    // Using std::ifstream for safer and more portable file reading
    std::ifstream inputFile("./hello.txt", std::ios::binary); // Open in binary mode for raw byte reading
    if (!inputFile.is_open())
    {
        std::cerr << "Error: Could not open file ./hello.txt" << std::endl; // Use cerr for errors
        exit(1); // Indicate an error
    }
    else
    {
        // Read the entire file into a vector of chars
        std::vector<char> buffer((std::istreambuf_iterator<char>(inputFile)),
                                  std::istreambuf_iterator<char>());
        
        std::cout << "num read: " << buffer.size() << " bytes" << std::endl;
        // You can now process the content in 'buffer'
        // For example, print it (be careful with non-printable characters):
        // for (char c : buffer) { std::cout << c; }
    }
    std::cout << "Hello World!" << std::endl;
    exit(0);
    return 0;

}