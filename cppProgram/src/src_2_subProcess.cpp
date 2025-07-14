#include <iostream>
#include <string>

int main() {
    std::cout << "Anh-Penguin Subprocess src2" << std::endl;
    std::string line;
    while (true)
    {
        std::getline(std::cin, line);
        std::cout << "📥 Reader nhận được: " << line << std::endl;
        if (!std::getline(std::cin, line)) 
        {
            if (std::cin.eof()) {
                std::cout << "📴 Đã đến EOF (stdin bị đóng)\n";
            } else if (std::cin.fail()) {
                std::cout << "⚠️ Lỗi đọc dòng (failbit set)\n";
            } else if (std::cin.bad()) {
                std::cout << "💥 Lỗi stream nghiêm trọng (badbit)\n";
            }
            break;
        }
    }
    return 0;
}