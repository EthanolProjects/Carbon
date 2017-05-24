#include "Concurrency.hpp"

namespace Carbon {
    Threadpool::Threadpool() = default;
    Threadpool::~Threadpool() {}
    Work::Work() = default;
    Work::~Work() {}

    size_t Threadpool::getConcurrencyLevel() const {
        return std::thread::hardware_concurrency();
    }

}
