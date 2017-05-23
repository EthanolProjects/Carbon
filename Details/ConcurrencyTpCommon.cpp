#include "Concurrency.hpp"

namespace Carbon {
    size_t Threadpool::getConcurrencyLevel() const {
        return std::thread::hardware_concurrency();
    }
}
