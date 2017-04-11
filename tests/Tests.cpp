#include "ThreadPool.hpp"
#include <iostream>
bool testThreadPool() {
    using namespace Carbon;
    ThreadPool pool(1);
    std::atomic_int val = 0;
    auto res = Async(pool, [&val]() { ++val; return val.load(); });
    res.get();
    return true;
}

int main() {
    std::cout << (testThreadPool() ? 'Y' : 'N') << std::endl;
}