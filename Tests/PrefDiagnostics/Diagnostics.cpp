#include "../../Carbon/Concurrency.hpp"
#include <iostream>
namespace CarbonPrefDiagnostics {
    constexpr size_t maxNum = 100000000;
    void concurrencyTp() {
        std::atomic_int result;
        Carbon::asyncForIntegerRange(Carbon::Threadpool::default(), 
            [&](int) { result = rand(); }, 0, maxNum).get();
    }
}

int main() {
    using namespace CarbonPrefDiagnostics;
    concurrencyTp();
    system("pause");
}
