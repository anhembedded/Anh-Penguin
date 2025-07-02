#include <iostream>
#include <cstdio>
#include <>

using namespace std;
int main()
{
    char buffer [255] {};
    int numRead {};
    numRead = read("./hello.txt", buffer, 225);
    if(numRead == -1)
    {
        std::cout << "an error occurred" << std::endl;
    }
    else
    {
        std::cout << "num read: " << numRead << std::endl;
    }
    exit(0);
    
}