#include <pthread.h>
#include <iostream>
#include <list>
#include <forward_list>
#include <cstring>

pthread_t mainId {};
void *printIdSelfAndMain(void *arg)
{
    pthread_t selfID {};
    selfID = pthread_self();
    std::cout << "printIdSelfAndMain" << std::endl;
    std::cout << "selfID: " << selfID << std::endl;
    std::cout << "mainID: " << mainId << std::endl;
    return nullptr;
}
int main()
{
    mainId = pthread_self();
    std::cout << "mainId: " << mainId << std::endl;
    std::forward_list<pthread_t> threads {};
    for(auto i {0u} ; i <3 ; i++)
    {
        pthread_t thread {};
        int res = pthread_create(&thread, nullptr, printIdSelfAndMain, nullptr);
        if (res != 0)
        {
            std::cerr << "pthread_create failed: " << strerror(res) << std::endl;
            return EXIT_FAILURE;
        }
        threads.push_front(thread);
    }
    for( auto &i : threads)
    {
        auto res = pthread_join(i, nullptr);
        if (res != 0)
        {
            std::cerr << "pthread_join failed: " << strerror(res) << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}