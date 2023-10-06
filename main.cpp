#include <iostream>
#include "ThreadPool.h"

int main() {
    ThreadPool pool(4);

    std::vector<std::future<int>> results;

//    extern template<typename T, typename ...Args>
//    auto ThreadPool::submit(T &&f, Args &&...args)->std::future<decltype(f(args...))>{}

    results.reserve(8);
    for(int i = 0; i < 8; i++){
        results.emplace_back(pool.submit([](int x){
            std::cout << "Hello from" << x << std::endl;
            return x;
        }, i));
    }

    for(auto &result: results){
        std::cout << result.get() << ' ';
    }
    std::cout << std::endl;

    pool.shutdown();

    return 0;
}