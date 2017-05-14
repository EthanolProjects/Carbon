#include "../../Carbon/Concurrency.hpp"
#include <iostream>
namespace CarbonPrefDiagnostics {
    constexpr size_t maxNum = 10000;
    void concurrencyTp() {
        std::atomic_int result;
        Carbon::asyncForIntegerRange(Carbon::Threadpool::getDefault(), 
            [&](int) { result = rand(); }, 0, maxNum).get();
    }

    void exceptionTest() {
        for (int i = 0; i < 1000; ++i){
            try {
                concurrencyTp();
            }
            catch (std::exception& e) {
                std::cout << e.what() << std::endl;
            }
        }
    }
}


int main() {
    using namespace CarbonPrefDiagnostics;
    exceptionTest();
    system("pause");
}
