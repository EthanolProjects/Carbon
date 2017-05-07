#include "../../Carbon/Concurrency.hpp"
namespace CarbonPrefDiagnostics {
    constexpr size_t maxNum = 100000000;
    void concurrencyTp() {
        std::vector<int> result(maxNum);
        auto pool = Carbon::Threadpool::create();
        auto func = [&](size_t i) { result[i] = []() { return rand(); }(); };
        auto future = Carbon::AsyncGroup(*pool, { 0,maxNum }, func);
        future->wait();
    }
}

int main() {
    using namespace CarbonPrefDiagnostics;
    concurrencyTp();
}
